#include "task_scheduler.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <atomic>
#include <iostream>
#include <sstream>
#include <random>

using namespace std;
using namespace chrono;
using namespace task_scheduler;

steady_clock::duration generate_random_duration(steady_clock::duration min, steady_clock::duration max)
{
    // Seed with a real random value, if available
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(duration_cast<milliseconds>(min).count(), duration_cast<milliseconds>(max).count()); // Milliseconds range

    return milliseconds(dist(gen));
}

TEST(task_scheduler_test, single_tasks_execution)
{
    clear_tasks(); // Ensure scheduler is empty
    atomic<bool> task_executed = false;

    // Aufgabe registrieren
    register_task(
        static_cast<function<bool()>>(
            [&task_executed]()
            {
                task_executed = true;
                return true; // Aufgabe wird beendet
            }),
        10ms);

    // Scheduler ausführen
    execute();

    // Überprüfen, ob die Aufgabe ausgeführt wurde
    ASSERT_TRUE(task_executed);
}

TEST(task_scheduler_test, multiple_tasks_execution)
{
    clear_tasks(); // Ensure scheduler is empty
    atomic<int> task1_count = 0;
    atomic<int> task2_count = 0;

    // Redirect log output
    std::ostringstream log_stream;
    std::streambuf *original_cout = std::cout.rdbuf(log_stream.rdbuf());

    register_task(
        static_cast<function<bool()>>(
            [&task1_count]()
            {
                task1_count++;
                return false;
            }),
        10ms);

    register_task(
        static_cast<function<bool()>>(
            [&task2_count]()
            {
                task2_count++;
                return false;
            }),
        25ms);

    steady_clock::time_point start_time = steady_clock::now();
    // Execute scheduler
    while (steady_clock::now() - start_time < 35ms)
        execute();

    // Restore original log output
    std::cout.rdbuf(original_cout);

    // Verify task execution counts
    ASSERT_TRUE(task1_count > task2_count);

    // Count occurrences of "get bulk data" in the log
    std::string log_content = log_stream.str();
    size_t bulk_data_count = 0;
    size_t pos = log_content.find("get bulk data");
    while (pos != std::string::npos)
    {
        bulk_data_count++;
        pos = log_content.find("get bulk data", pos + 1);
    }

    // Verify "get bulk data" appears as often as task1 was executed
    ASSERT_EQ(bulk_data_count, task1_count);
}

TEST(task_scheduler_test, task_with_delayed_start)
{
    clear_tasks(); // Ensure scheduler is empty
    atomic<bool> task_executed = false;

    run_after(
        [&task_executed]()
        {
            task_executed = true;
        },
        50ms);

    // Wait for 30ms and execute scheduler
    steady_clock::time_point start_time = steady_clock::now();
    // Execute scheduler
    while (steady_clock::now() - start_time < 30ms)
        execute();

    // Verify task has not executed yet
    ASSERT_FALSE(task_executed);

    // Wait for another 30ms and execute scheduler
    start_time = steady_clock::now();
    // Execute scheduler
    while (steady_clock::now() - start_time < 35ms)
        execute();

    // Verify task has executed
    ASSERT_TRUE(task_executed);
}

TEST(task_scheduler_test, periodic_task_execution)
{
    clear_tasks(); // Ensure scheduler is empty
    atomic<int> execution_count = 0;

    register_task(
        static_cast<function<bool()>>(
            [&execution_count]()
            {
                execution_count++;
                return false; // Task does not stop
            }),
        10ms);

    // Execute scheduler multiple times
    for (int i = 0; i < 5; ++i)
    {
        this_thread::sleep_for(10ms);
        execute();
    }

    // Verify task executed 5 times
    ASSERT_EQ(execution_count, 5);
}

TEST(task_scheduler_test, complex_tasks_execution)
{
    clear_tasks(); // Ensure scheduler is empty
    atomic<int> task1_count = 0;
    atomic<int> task2_count = 0;

    atomic<bool> timed_task1_executed = false;
    atomic<bool> timed_task2_executed = false;

    constexpr steady_clock::duration execution_time = 1s;
    const steady_clock::duration execution_interval_task1 = generate_random_duration(1ms, execution_time);
    const steady_clock::duration execution_interval_task2 = generate_random_duration(1ms, execution_time);
    const steady_clock::duration delay_timed_task1 = generate_random_duration(1ms, execution_time);
    const steady_clock::duration delay_timed_task2 = generate_random_duration(1ms, execution_time);

    cout << "execution interval task1: " << chrono::duration_cast<chrono::milliseconds>(execution_interval_task1).count() << "ms" << endl;
    cout << "execution interval task2: " << chrono::duration_cast<chrono::milliseconds>(execution_interval_task2).count() << "ms" << endl;
    cout << "delay timed task1: " << chrono::duration_cast<chrono::milliseconds>(delay_timed_task1).count() << "ms" << endl;
    cout << "delay timed task2: " << chrono::duration_cast<chrono::milliseconds>(delay_timed_task2).count() << "ms" << endl;

    // Redirect log output
    std::ostringstream log_stream;
    std::streambuf *original_cout = std::cout.rdbuf(log_stream.rdbuf());

    register_task(
        static_cast<function<bool()>>(
            [&task1_count]()
            {
                task1_count++;
                return false;
            }),
        execution_interval_task1);

    register_task(
        static_cast<function<bool()>>(
            [&task2_count]()
            {
                task2_count++;
                return false;
            }),
        execution_interval_task2);

    run_after(
        [&timed_task1_executed]()
        {
            timed_task1_executed = true;
        },
        delay_timed_task1);

    run_after(
        [&timed_task2_executed]()
        {
            timed_task2_executed = true;
        },
        delay_timed_task2);

    steady_clock::time_point start_time = steady_clock::now();
    // Execute scheduler
    while (steady_clock::now() - start_time < 35ms)
        execute();

    // Restore original log output
    std::cout.rdbuf(original_cout);

    // Verify task execution counts
    ASSERT_TRUE((task1_count > task2_count && execution_interval_task1 < execution_interval_task2) ||
                (task1_count < task2_count && execution_interval_task1 > execution_interval_task2));

    // Count occurrences of "get bulk data" in the log
    std::string log_content = log_stream.str();
    size_t bulk_data_count = 0;
    size_t pos = log_content.find("get bulk data");
    while (pos != std::string::npos)
    {
        bulk_data_count++;
        pos = log_content.find("get bulk data", pos + 1);
    }

    // Verify "get bulk data" appears as often as task1 was executed
    ASSERT_EQ(bulk_data_count, execution_interval_task1 < execution_interval_task2 ? task1_count : task2_count);
}