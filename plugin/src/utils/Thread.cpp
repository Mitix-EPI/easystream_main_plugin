/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** thread
*/

#include "Thread.hpp"

es::thread::ThreadPool::ThreadPool(std::size_t size)
	: _workers(), _worker_stop(false), _worker_idx(0), _tasks(), _tasks_lock(), _tasks_cv()
{
	for (std::size_t n = 0; n < size; n++) {
		_workers.emplace_back(std::bind(&es::thread::ThreadPool::work, this));
        _workers.back().detach();
	}
}

es::thread::ThreadPool::~ThreadPool()
{
	_worker_stop = true;
	_tasks_cv.notify_all();
	for (auto& thread : _workers) {
		_tasks_cv.notify_all();
		if (thread.joinable()) {
			thread.join();
		}
	}
}

std::shared_ptr<es::thread::ThreadPool::Task> es::thread::ThreadPool::push(std::function<void(std::shared_ptr<void>)> c_fn,
																		    std::shared_ptr<void> data)
{
	auto task = std::make_shared<es::thread::ThreadPool::Task>(c_fn, data);

	// Append the task to the queue.
	std::unique_lock<std::mutex> lock(_tasks_lock);
	_tasks.emplace_back(task);
	_tasks_cv.notify_one();

	return task;
}

void es::thread::ThreadPool::pop(std::shared_ptr<es::thread::ThreadPool::Task> work)
{
	if (work) {
		{
			std::unique_lock<std::mutex> lock(work->_mutex);
			work->_is_dead = true;
		}
		work->_is_complete.notify_all();
	}
}

void es::thread::ThreadPool::work()
{
	std::shared_ptr<es::thread::ThreadPool::Task> local_work{};
	uint32_t local_number = _worker_idx.fetch_add(1);

	while (!_worker_stop) {
		// Wait for more work, or immediately continue if there is still work to do.
		{
			// Lock the tasks mutex to check for work.
			std::unique_lock<std::mutex> lock(_tasks_lock);

			// If there are currently no tasks queued, wait on the condition variable.
			// This temporarily unlocks the mutex until it is woken up.
			if (_tasks.size() == 0) {
				_tasks_cv.wait(lock, [this]() { return _worker_stop || _tasks.size() > 0; });
			}

			// If there is either no tasks or we were asked to stop, skip everything.
			if (_worker_stop || (_tasks.size() == 0)) {
				continue;
			}

			// Grab the latest task and immediately remove it from the queue.
			local_work = _tasks.front();
			_tasks.pop_front();
		}

		// If the task was killed, skip everything again.
		if (local_work->_is_dead.load()) {
			continue;
		}

		// Try to execute work, but don't crash on catchable exceptions.
		if (local_work->_callback) {
			try {
				local_work->_callback(local_work->_data);
			} catch (std::exception const& ex) {
				blog(LOG_WARNING, "Worker %" PRIx32 " caught exception from task (%" PRIxPTR ", %" PRIxPTR
                                    ") with message: %s",
                                    local_number, reinterpret_cast<ptrdiff_t>(local_work->_callback.target<void>()),
                                    reinterpret_cast<ptrdiff_t>(local_work->_data.get()), ex.what());
			} catch (...) {
				blog(LOG_WARNING, "Worker %" PRIx32 " caught exception of unknown type from task (%" PRIxPTR ", %" PRIxPTR
                                    ").",
                                    local_number, reinterpret_cast<ptrdiff_t>(local_work->_callback.target<void>()),
                                    reinterpret_cast<ptrdiff_t>(local_work->_data.get()));
			}
			{
				std::unique_lock<std::mutex> lock(local_work->_mutex);
				local_work->_is_dead.store(true);
			}
			local_work->_is_complete.notify_all();
		}

		// Remove our reference to the work unit.
		local_work.reset();
	}

	_worker_idx.fetch_sub(1);
}

es::thread::ThreadPool::Task::Task() {}

es::thread::ThreadPool::Task::Task(std::function<void(std::shared_ptr<void>)> fn, std::shared_ptr<void> dt)
	: _mutex(), _is_complete(), _is_dead(false), _callback(fn), _data(dt)
{}

void es::thread::ThreadPool::Task::await_completion()
{
	if (!_is_dead) {
		std::unique_lock<std::mutex> lock(_mutex);
		_is_complete.wait(lock, [this]() { return this->_is_dead.load(); });
	}
}
