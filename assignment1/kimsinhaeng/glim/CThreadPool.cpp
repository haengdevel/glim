// CThreadPool.cpp: 구현 파일
//

#include "pch.h"
#include "glim.h"
#include "CThreadPool.h"


// CThreadPool
using namespace std;
CThreadPool::CThreadPool(size_t numThreads)
{
	for (size_t i = 0; i < numThreads; ++i) {
		workers.emplace_back([this]() {
			while (true) {
				function<void()> task;
				{
					unique_lock<mutex> lock(this->queue_mutex);
					this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

					if (this->stop && this->tasks.empty())
						return;

					task = move(this->tasks.front());
					this->tasks.pop();
				}
				task();
			}
			});
	}
}

CThreadPool::~CThreadPool() {
	{
		unique_lock<mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();

	for (thread& worker : workers)
		worker.join();
}

void CThreadPool::enqueue(function<void()> task) {
	{
		unique_lock<mutex> lock(queue_mutex);
		tasks.push(move(task));
	}
	condition.notify_one();
}

