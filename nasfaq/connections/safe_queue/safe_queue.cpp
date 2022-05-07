#include "safe_queue.h"

template <typename T>
TestQueue<T>::TestQueue(void)
	: q()
{}

template <typename T>
TestQueue<T>::~TestQueue(void)
{}

template <typename T>
SafeQueue<T>::SafeQueue(void)
	: q()
	, m()
	, c()
{}

template <typename T>
SafeQueue<T>::~SafeQueue(void)
{}

// Add an element to the queue.
template <typename T>
void SafeQueue<T>::enqueue(T t)
{
	std::lock_guard<std::mutex> lock(m);
	q.push(t);
	c.notify_one();
}

// Get the "front"-element.
// If the queue is empty, wait till a element is avaiable.
template <typename T>
T SafeQueue<T>::dequeue(void) {
	std::unique_lock<std::mutex> lock(m);
	while(q.empty())
	{
		// release lock as long as the wait and reaquire it afterwards.
		c.wait(lock);
	}
	T val = q.front();
	q.pop();
	return val;
}

template <typename T>
bool SafeQueue<T>::empty(void) {
	return q.empty();
}
