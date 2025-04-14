#pragma once

// CThreadPool 명령 대상

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

#ifdef THREADPOOL_EXPORTS
#define THREADPOOL_API __declspec(dllexport)
#else
#define THREADPOOL_API __declspec(dllimport)
#endif

class THREADPOOL_API CThreadPool {
public:
	CThreadPool(size_t threads);
	~CThreadPool();

	void enqueue(std::function<void()> task);

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop = false;
};