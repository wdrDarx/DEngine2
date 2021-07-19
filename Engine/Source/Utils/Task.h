#pragma once
#include "Core/Core.h"

class DENGINE_API ThreadPool
{
public:

	ThreadPool(int threads) : shutdown_(false)
	{
		// Create the specified number of threads
		threads_.reserve(threads);
		for (int i = 0; i < threads; ++i)
			threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
	}

	~ThreadPool()
	{
		{
			// Unblock any threads and tell them to stop
			std::unique_lock <std::mutex> l(lock_);

			shutdown_ = true;
			condVar_.notify_all();
		}

		// Wait for all threads to stop
		//std::cerr << "Joining threads" << std::endl;
		for (auto& thread : threads_)
			thread.join();
	}

	void doJob(std::function <void(void)> func)
	{
		// Place a job on the queue and unblock a thread
		std::unique_lock <std::mutex> l(lock_);

		jobs_.emplace(std::move(func));
		condVar_.notify_one();
	}

protected:

	void threadEntry(int i)
	{
		std::function <void(void)> job;

		while (1)
		{
			{
				std::unique_lock <std::mutex> l(lock_);

				while (!shutdown_ && jobs_.empty())
				{ 
					//m_Finished = true;
					condVar_.wait(l);
					//m_FinishedCondVar.notify_all();
				}

			//	m_Finished = false;

				if (jobs_.empty())
				{
					// No jobs to do and we are shutting down
					//std::cerr << "Thread " << i << " terminates" << std::endl;
					return;
				}

				//std::cerr << "Thread " << i << " does a job" << std::endl;
				job = std::move(jobs_.front());
				jobs_.pop();
			}

			// Do the job without holding any locks
			job();
		}

		m_FinishedCondVar.notify_all();
	}
public:
	bool HasFinished() 
	{
		std::unique_lock <std::mutex> l(lock_);
		m_FinishedCondVar.wait(l);
	}

	void Shutdown()
	{
		{
			// Unblock any threads and tell them to stop
			std::unique_lock <std::mutex> l(lock_);

			shutdown_ = true;
			condVar_.notify_all();
		}

		// Wait for all threads to stop
		//std::cerr << "Joining threads" << std::endl;
		for (auto& thread : threads_)
			thread.join();
	}

public:
	std::atomic<bool> m_Finished;

protected:
	std::mutex lock_;
	std::condition_variable condVar_;
	std::condition_variable m_FinishedCondVar;
	bool shutdown_;
	std::queue <std::function <void(void)>> jobs_;
	std::vector <std::thread> threads_;
};

// Fixed size very simple thread safe ring buffer
template <typename T, size_t capacity>
class DENGINE_API ThreadSafeRingBuffer
{
public:
	// Push an item to the end if there is free space
	//  Returns true if succesful
	//  Returns false if there is not enough space
	inline bool push_back(const T& item)
	{
		bool result = false;
		lock.lock();
		size_t next = (head + 1) % capacity;
		if (next != tail)
		{
			data[head] = item;
			head = next;
			result = true;
		}
		lock.unlock();
		return result;
	}

	// Get an item if there are any
	//  Returns true if succesful
	//  Returns false if there are no items
	inline bool pop_front(T& item)
	{
		bool result = false;
		lock.lock();
		if (tail != head)
		{
			item = data[tail];
			tail = (tail + 1) % capacity;
			result = true;
		}
		lock.unlock();
		return result;
	}

private:
	T data[capacity];
	size_t head = 0;
	size_t tail = 0;
	std::mutex lock; // this just works better than a spinlock here (on windows)
};
// A Dispatched job will receive this as function argument:
struct DENGINE_API JobDispatchArgs
{
	uint32_t jobIndex;
	uint32_t groupIndex;
};

class DENGINE_API JobPool
{
public:
	JobPool()
	{
		Initialize();
	}

	// Create the internal resources such as worker threads, etc. Call it once when initializing the application.
	void Initialize();

	// Add a job to execute asynchronously. Any idle thread will execute this job.
	void Execute(const std::function<void()>& job);

	// Divide a job onto multiple jobs and execute in parallel.
	//  jobCount    : how many jobs to generate for this task.
	//  groupSize   : how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
	//  func        : receives a JobDispatchArgs as parameter
	void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job);

	// Check if any threads are working currently or not
	bool IsBusy();

	inline void poll()
	{
		wakeCondition.notify_one(); // wake one worker thread
		std::this_thread::yield(); // allow this thread to be rescheduled
	}

	// Wait until all threads become idle
	void Wait();

	uint32_t numThreads = 0;    // number of worker threads, it will be initialized in the Initialize() function
	ThreadSafeRingBuffer<std::function<void()>, 256> jobPool;    // a thread safe queue to put pending jobs onto the end (with a capacity of 256 jobs). A worker thread can grab a job from the beginning
	std::condition_variable wakeCondition;    // used in conjunction with the wakeMutex below. Worker threads just sleep when there is no job, and the main thread can wake them up
	std::mutex wakeMutex;    // used in conjunction with the wakeCondition above
	uint64_t currentLabel = 0;    // tracks the state of execution of the main thread
	std::atomic<uint64_t> finishedLabel;
};