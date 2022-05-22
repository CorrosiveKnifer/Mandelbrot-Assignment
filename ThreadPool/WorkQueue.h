/*********A very basic Work Queue class***************/

#ifndef __WORKQUEUE_H__
#define __WORKQUEUE_H__

#include <queue>
#include <mutex>


template<typename T>
class CWorkQueue
{
public:
	CWorkQueue() {}

	//Insert an item at the back of the queue and signal any thread that might be waiting for the q to be populated
	void push(const T& item)
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		workQ.push(std::move(item));
		m_WorkQCondition.notify_one();
	}

	//Attempt to get a workitem from the queue
	//If the Q is empty just return false; 
	bool nonblocking_pop(T& _workItem)
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		//If the queue is empty return false
		if (workQ.empty())
		{
			return false;
		}
		_workItem = std::move(workQ.front());
		workQ.pop();
		return true;
	}

	//Attempt to get a workitem from the queue
	//If the Q is empty block the thread until a notification is sent to it. 
	bool blocking_pop(T& _workItem, unsigned int time = 0)
	{
		std::unique_lock<std::mutex> _lock(m_WorkQMutex);
		//If the queue is empty block the thread from running until a work item becomes available
		if (time == 0)
		{
			m_WorkQCondition.wait(_lock, [this] {return !workQ.empty(); });
		}
		// A retun value of false from wait_for means "timeout has elapsed".
		if (!m_WorkQCondition.wait_for(_lock, std::chrono::milliseconds(time), [this] {return !workQ.empty(); }))
		{
			return false;
		}
		_workItem = std::move(workQ.front());
		workQ.pop();
		return true;
	}

	//Checking if the queue is empty or not
	bool empty() const
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		return workQ.empty();
	}

	
private:
	std::queue<T> workQ;
	mutable std::mutex m_WorkQMutex;
	std::condition_variable m_WorkQCondition;

};
#endif
