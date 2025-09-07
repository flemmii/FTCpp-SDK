#pragma once

#include <tuple>
#include <functional>
#include <chrono>
#include <queue>
#include <thread>

// !!! WARNING !!!
// Use shared_ptr and a copy instead of a reference when capturing a lambda function
// to ensure the variable is not destroyed before the lambda is finished executing
// !!! WARNING !!!
namespace task_scheduler
{
    struct Task
    {
        std::function<bool()> loop_function;
        std::function<void()> end_function;
        std::chrono::steady_clock::duration execution_interval;
        std::chrono::steady_clock::duration timer;

        Task(const std::function<bool()> &loop_function, const std::chrono::steady_clock::duration &execution_interval, const std::function<void()> &end_function) : loop_function(loop_function),
                                                                                                                                                                     execution_interval(execution_interval),
                                                                                                                                                                     end_function(end_function),
                                                                                                                                                                     timer(execution_interval) {}
    };

    void register_task(std::unique_ptr<Task> task, const bool execute_once_directly = false);

    void register_task(const std::function<bool()> &loop_function, const std::chrono::steady_clock::duration &execution_interval, const bool execute_once_directly = false, const std::function<void()> &end_function = [] {});

    void register_task(const std::function<void()> &loop_function, const std::chrono::steady_clock::duration &execution_interval, const bool execute_once_directly = false);

    void register_function_sleep_chain(
        const std::vector<std::tuple<std::chrono::steady_clock::duration, std::function<void()>>> &chain);

    void register_task_dependency_chain(const std::vector<std::unique_ptr<Task>> &chain);

    void
    run_after(const std::function<void()> &function,
              const std::chrono::steady_clock::duration &delay);

    void clear_tasks();

    void execute();

    void register_thread(const std::function<void()> &function);

    void terminate_all_threads();
}