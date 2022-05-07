#ifndef _SAFE_QUEUE_H_
#define _SAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class TestQueue
{
public:
	TestQueue();
	~TestQueue();
private:
	std::queue<T> q;
};

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
	SafeQueue(void);
	~SafeQueue(void);

	void enqueue(T);
	T dequeue(void);
	bool empty(void);
private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
#endif

