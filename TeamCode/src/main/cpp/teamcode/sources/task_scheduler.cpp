#ifdef SOFTWARE_TEST_MODE
#include <iostream>
#include <android/log.h>
#else
#include "utils.hpp"
#include "robot.hpp"
using namespace sdk;
#endif

#include "task_scheduler.hpp"
#include <mutex>
#include <csignal>

using namespace std;

namespace task_scheduler
{
#ifdef SOFTWARE_TEST_MODE
    inline std::ostream &log(int errorLevel, const std::string &tag)
    {
        return std::cout << "[" << tag << "] ";
    }
#else
    inline LogcatStream log(int errorLevel, const std::string &tag)
    {
        return ::log(errorLevel, tag);
    }
#endif

    struct TaskComparator
    {
        bool operator()(const unique_ptr<Task> &lhs, const unique_ptr<Task> &rhs) const
        {
            return lhs->timer > rhs->timer; // Min-Heap: Aufgabe mit dem kleinsten timer an der Spitze
        }
    };

    struct TimedTaskComparator
    {
        bool operator()(const unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &lhs,
                        const unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &rhs) const
        {
            return get<1>(*lhs) > get<1>(*rhs); // Min-Heap: Aufgabe mit der kleinsten Verzögerung an der Spitze
        }
    };

    namespace awaiting
    {
        std::mutex mutex;
        vector<unique_ptr<Task>> loop_tasks;
        priority_queue<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>,
                       vector<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>>,
                       TimedTaskComparator>
            timed_tasks;
    }

    void register_task(unique_ptr<Task> task, const bool execute_once_directly)
    {
        if (task->execution_interval <= 0ms)
        {
            task_scheduler::log(ANDROID_LOG_WARN, "task_scheduler") << "Blocked registering task with execution interval: " << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(task->execution_interval).count()) << " ms execution interval is <= 0" << endl;
            return;
        }

        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "registered task, execution interval: " << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(task->execution_interval).count()) << " ms" << endl;

        if (execute_once_directly)
        {
            if (task->loop_function())
            {
                task->end_function();
                return;
            }
        }

        unique_lock<mutex> lock(awaiting::mutex);
        awaiting::loop_tasks.push_back(move(task));
        lock.unlock();
    }

    void register_task(const function<bool()> &loop_function, const chrono::steady_clock::duration &execution_interval, const bool execute_once_directly, const function<void()> &end_function)
    {
        register_task(make_unique<Task>(loop_function, execution_interval, end_function), execute_once_directly);
    }

    void register_task(const function<void()> &loop_function, const chrono::steady_clock::duration &execution_interval, const bool execute_once_directly)
    {
        register_task((function<bool()>)[loop_function]() {
            loop_function();
            return false; },
                      execution_interval, execute_once_directly, [] {});
    }

    void register_function_sleep_chain(const vector<tuple<chrono::steady_clock::duration, function<void()>>> &chain)
    {
        auto task = make_unique<Task>([chain]()
                                      {
        get<1>(chain.back())();
        return true; }, get<0>(chain.back()), []() {});
        for (size_t i = chain.size() - 1; i-- > 0;)
        {
            task = make_unique<Task>(
                [chain, i]()
                {
                    get<1>(chain[i])();
                    return true;
                },
                get<0>(chain[i]),
                [task = task.get()]() mutable
                { task_scheduler::register_task(make_unique<Task>(move(*task))); });
        }
        register_task(move(task));
    }

    void register_task_dependency_chain(const vector<unique_ptr<Task>> &chain)
    {
        auto task = make_unique<Task>(*chain.back());
        for (size_t i = chain.size() - 1; i-- > 0;)
        {
            task = make_unique<Task>(
                chain[i]->loop_function,
                chain[i]->execution_interval,
                [task = task.get(), &chain, i]() mutable
                {
                    chain[i]->end_function();
                    task_scheduler::register_task(make_unique<Task>(move(*task)));
                });
        }
        register_task(move(task));
    }

    void run_after(const function<void()> &function, const chrono::steady_clock::duration &delay)
    {
        if (delay <= 0ms)
        {
            task_scheduler::log(ANDROID_LOG_WARN, "task_scheduler") << "Blocked registering delay task with delay: " << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(delay).count()) << " ms delay is <= 0" << endl;
            return;
        }

        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "registered delayed task, delay: " << static_cast<double>(chrono::duration_cast<chrono::milliseconds>(delay).count()) << " ms" << endl;
        unique_lock<mutex> lock(awaiting::mutex);
        awaiting::timed_tasks.push(make_unique<tuple<std::function<void()>, chrono::steady_clock::duration>>(function, delay));
        lock.unlock();
    }

    vector<unique_ptr<Task>> loop_tasks;
    priority_queue<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>,
                   vector<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>>,
                   TimedTaskComparator>
        timed_tasks;
    unique_ptr<Task> smallest_execution_interval_task = nullptr;

    void clear_tasks()
    {
        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "cleared tasks" << endl;
        loop_tasks.clear();
        while (!timed_tasks.empty())
            timed_tasks.pop();
        smallest_execution_interval_task.reset();
    }

    void push_awaiting_tasks()
    {
        unique_lock<mutex> lock(awaiting::mutex);

        // Push all awaiting loop tasks into the main loop_tasks
        for (auto &task : awaiting::loop_tasks)
        {
            if (!smallest_execution_interval_task || task->execution_interval < smallest_execution_interval_task->execution_interval)
            {
                if (smallest_execution_interval_task)
                {
                    loop_tasks.push_back(move(smallest_execution_interval_task));
                    push_heap(loop_tasks.begin(), loop_tasks.end(), TaskComparator()); // Restore heap property
                }
                smallest_execution_interval_task = move(task);
            }
            else
            {
                loop_tasks.push_back(move(task));
                push_heap(loop_tasks.begin(), loop_tasks.end(), TaskComparator()); // Restore heap property
            }
        }
        awaiting::loop_tasks.clear();

        // Push all awaiting timed tasks into the main timed_tasks
        while (!awaiting::timed_tasks.empty())
        {
            timed_tasks.push(move(const_cast<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &>(awaiting::timed_tasks.top())));
            awaiting::timed_tasks.pop();
        }

        lock.unlock();
    }

    void get_bulk_data()
    {
#ifdef SOFTWARE_TEST_MODE
        std::cout << "get bulk data" << endl;
#else
        robot::control_hub.get_bulk_data();
        robot::expansion_hub.get_bulk_data();
#endif
    }

    void execute()
    {
        static chrono::steady_clock::time_point last_timestamp = chrono::steady_clock::now();

        push_awaiting_tasks();

        const auto current_time = chrono::steady_clock::now();
        const chrono::steady_clock::duration passed_time = current_time - last_timestamp;
        last_timestamp = current_time;

        bool already_retrieved_data = false;
        if (!timed_tasks.empty())
        {
            auto timed_task = move(const_cast<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &>(timed_tasks.top()));
            timed_tasks.pop();

            bool leave_task_queue_empty = false;

            if (get<1>(*timed_task) <= 0ms)
            {
                already_retrieved_data = true;
                get_bulk_data();
            }

            while (get<1>(*timed_task) <= 0ms)
            {
                get<0> (*timed_task)();
                if (timed_tasks.empty())
                {
                    leave_task_queue_empty = true;
                    break;
                }

                timed_task = move(const_cast<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &>(timed_tasks.top()));
                timed_tasks.pop();
            }

            if (!leave_task_queue_empty)
            {
                timed_tasks.push(move(timed_task)); // Ensure the unique_ptr is moved back into the queue
            }

            priority_queue<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>,
                           vector<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>>>,
                           TimedTaskComparator>
                temp_tasks;

            while (!timed_tasks.empty())
            {
                auto task = move(const_cast<unique_ptr<tuple<function<void()>, chrono::steady_clock::duration>> &>(timed_tasks.top()));
                timed_tasks.pop();
                get<1>(*task) -= passed_time;
                temp_tasks.push(move(task));
            }
            timed_tasks = move(temp_tasks); // Properly reassign the updated queue
        }

        if (loop_tasks.empty() && !smallest_execution_interval_task)
            return;

        unique_ptr<Task> current_task;

        if (loop_tasks.empty() || (smallest_execution_interval_task && smallest_execution_interval_task->timer <= loop_tasks.front()->timer))
        {
            current_task = move(smallest_execution_interval_task);
            smallest_execution_interval_task = nullptr;

            if (!already_retrieved_data)
            {
                get_bulk_data();
            }
        }
        else
        {
            pop_heap(loop_tasks.begin(), loop_tasks.end(), TaskComparator()); // Entfernt das kleinste Element und verschiebt es an das Ende
            current_task = move(loop_tasks.back());                           // Das kleinste Element ist jetzt am Ende des Vektors
            loop_tasks.pop_back();                                            // Entfernt das kleinste Element aus dem Vektor
        }

        const bool task_status = current_task->loop_function();
        if (task_status)
            current_task->end_function();

        for (auto &task : loop_tasks)
            task->timer -= passed_time;

        if (smallest_execution_interval_task)
            smallest_execution_interval_task->timer -= passed_time;

        if (!task_status)
        {
            current_task->timer = current_task->execution_interval;
            if (!smallest_execution_interval_task)
            {
                smallest_execution_interval_task = move(current_task);
            }
            else
            {
                loop_tasks.push_back(move(current_task));
                push_heap(loop_tasks.begin(), loop_tasks.end(), TaskComparator()); // Wiederherstellen der Heapeigenschaft
            }
        }
        else if (!smallest_execution_interval_task)
        {
            // Durchsuchen der loop_tasks nach der Aufgabe mit dem kleinsten execution_interval
            auto min_task_it = min_element(loop_tasks.begin(), loop_tasks.end(), [](const unique_ptr<Task> &lhs, const unique_ptr<Task> &rhs)
                                           { return lhs->execution_interval < rhs->execution_interval; });

            if (min_task_it != loop_tasks.end())
            {
                smallest_execution_interval_task = move(*min_task_it);
                loop_tasks.erase(min_task_it);
                make_heap(loop_tasks.begin(), loop_tasks.end(), TaskComparator()); // Wiederherstellen der Heapeigenschaft
            }
        }
    }

    vector<thread> threads;
    bool terminating_threads = false;
    mutex thread_mutex;

    void register_thread(const function<void()> &function)
    {
        if (terminating_threads)
        {
            task_scheduler::log(ANDROID_LOG_WARN, "task_scheduler") << "Blocked registering thread, terminating threads" << endl;
            return;
        }

        unique_lock<mutex> lock(thread_mutex);

        thread t([function]()
                 {
                     // Set up signal handling for the thread
                     signal(SIGTERM, [](int)
                            {
                                task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "thread " << this_thread::get_id() << " terminated due to signal" << endl;
                                pthread_exit(nullptr); // Safely exit the thread
                            });

                     function();

                    // Avoiding threads that end while the terminate_all_threads() function is running
                    while (terminating_threads)
                        ;
                            
                     task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "thread " << this_thread::get_id() << " ended" << endl; });

        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "registered thread " << t.get_id() << endl;
        threads.push_back(move(t)); // Store the thread in the vector
    }

    void terminate_all_threads()
    {
        terminating_threads = true;
        unique_lock<mutex> lock(thread_mutex);

        // Send SIGTERM to all threads in the process
        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "Terminating all threads" << endl;
        for (auto &thread : threads)
        {
            if (thread.joinable())
            {
                pthread_kill(thread.native_handle(), SIGTERM);
                thread.join(); // Wait for the thread to finish
            }
        }
        threads.clear();
        task_scheduler::log(ANDROID_LOG_INFO, "task_scheduler") << "All threads terminated" << endl;

        terminating_threads = false;
    }
}