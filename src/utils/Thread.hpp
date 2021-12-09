/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** thread
*/

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include "../Common.hpp"

namespace es {
    namespace thread {
        class ThreadPool {
            public:
                class Task {
                    public:
                        Task();
                        Task(const Task &t) {};
                        Task(std::function<void(std::shared_ptr<void>)> callback_function, std::shared_ptr<void> data);

                        void await_completion();

                    protected:
                        std::mutex _mutex;
                        std::condition_variable _is_complete;
                        std::atomic<bool> _is_dead;
                        std::function<void(std::shared_ptr<void>)> _callback;
                        std::shared_ptr<void> _data;

                    friend class es::thread::ThreadPool;
                };

                ThreadPool(std::size_t size);
                ThreadPool(const ThreadPool &tp) {};
                ~ThreadPool();

                std::shared_ptr<es::thread::ThreadPool::Task> push(std::function<void(std::shared_ptr<void>)> callback_function,
                                                                    std::shared_ptr<void> data);

                void pop(std::shared_ptr<es::thread::ThreadPool::Task> work);

            private:
                void work();
                
                std::list<std::thread> _workers;
                std::atomic<bool> _worker_stop;
                std::atomic<uint32_t> _worker_idx;
                std::list<std::shared_ptr<::es::thread::ThreadPool::Task>> _tasks;
                std::mutex _tasks_lock;
                std::condition_variable _tasks_cv;
        };
    }
}

#endif /* !THREAD_HPP_ */