//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2020 Media Design School
//
// File Name    |	ThreadPool.h
// Author(s)    |	Provided and updated by Michael Jordan (Michael.jor8834@mediadesign.com)
// Class(es)	|	ThreadPool
// Description:
//		Thread pool to hold tasks and distribute them to working threads.
//

//Library Includes
#include <iostream>
#include <thread>
#include <functional>
#include <Chrono>

//Local Includes
#include "WorkQueue.h"
#include "Task.h"
#include "IniParser.h"

//This Include
#include "ThreadPool.h"

//Static Variables
ThreadPool* ThreadPool::s_pThreadPool = nullptr;

//Default Constructor
ThreadPool::ThreadPool()
{
	m_iNumberOfThreads = std::thread::hardware_concurrency();
}

//Constructor
ThreadPool::ThreadPool(unsigned int _size)
{
	//Create a pool of threads equal to specified size
	m_iNumberOfThreads = _size;
}

//Destructor
ThreadPool::~ThreadPool()
{
	m_bStop = true;
	for (unsigned int i = 0; i < m_iNumberOfThreads; i++)
	{
		m_workerThreads[i].join();
	}

	if (m_pWorkQueue != 0)
	{
		delete m_pWorkQueue;
		m_pWorkQueue = 0;
	}
}

//Singleton GetInstance
ThreadPool& ThreadPool::GetInstance()
{
	if (s_pThreadPool == nullptr)
	{
		int count = IniParser::GetInstance().GetValueAsInt("Tread", "ThreadCount");
		
		if (count > 0)
		{
			s_pThreadPool = new ThreadPool(count);
		}
		else
		{
			s_pThreadPool = new ThreadPool();
		}
		
	}
	return (*s_pThreadPool);
}

//Singleton Destroy Instance
void ThreadPool::DestroyInstance()
{
	if (s_pThreadPool != nullptr)
	{
		delete s_pThreadPool;
	}
	s_pThreadPool = nullptr;
}

//	Initialize( )
//
//	Author: Provided
//	Access: public
//	Description:
//		Initialise the threadpool variables.
//		
//	Param:
//		- n/a	|
//
//	Return: n/a		|	
//
void ThreadPool::Initialize()
{
	//Create a new Work Queue
	m_pWorkQueue = new CWorkQueue<CTask>();
}

//	Start( )
//
//	Author: Provided
//	Access: public
//	Description:
//		Add N threads to the vector of threads.
//		
//	Param:
//		- n/a	|
//
//	Return: n/a		|	
//
void ThreadPool::Start()
{
	for (unsigned int i = 0; i < m_iNumberOfThreads; i++)
	{
		m_workerThreads.push_back(std::thread(&ThreadPool::DoWork, this));
	}
}

//	Submit( _fItem )
//
//	Author: Provided
//	Access: public
//	Description:
//		Adds a task to the back of the work queue.
//		
//	Param:
//		- CTask		|	Task to add to the list of tasks.
//
//	Return: n/a		|	
//
void ThreadPool::Submit(CTask _fItem)
{
	m_pWorkQueue->push(_fItem);
	m_TotalItems++;
}

//	Stop( )
//
//	Author: Provided
//	Access: public
//	Description:
//		Prompt all working threads to stop looping.
//		
//	Param:
//		- n/a	|
//
//	Return: n/a		|	
//
void ThreadPool::Stop()
{
	m_bStop = true;
}

//	HasItemsRemaining( )
//
//	Author:	Michael Jordan
//	Access: public
//	Description:
//		Compares the amount of tasks submitted to the amount of tasks completed.
//		
//	Param:
//		- n/a	|
//
//	Return: bool	|	True if there are tasks yet to be completed.
//
bool ThreadPool::HasItemsRemaining()
{
	return (m_TotalItems > m_aiItemsProcessed);
}

//	DoWork( )
//
//	Author:	Provided
//	Access: public
//	Description:
//		Main function for the worker thread.
//		
//	Param:
//		- n/a	|
//
//	Return: n/a		|	
//
void ThreadPool::DoWork()
{
	//Entry point of  a thread.
	std::cout << std::endl << "Thread with id [" << std::this_thread::get_id() << "] starting........" << std::endl;
	while (!m_bStop)
	{
		CTask WorkItem;
		//If there is an item in the queue to be processed; just take it off the q and process it
		//m_pWorkQueue->blocking_pop(WorkItem);
		if (m_pWorkQueue->blocking_pop(WorkItem, 500))
		{
			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
			WorkItem();
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

			auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Thread [" << std::this_thread::get_id() << "] finished: " << WorkItem.ToString() << "in " << time << "microseconds." << std::endl;
			m_aiItemsProcessed++;
		}
		//Else just continue back to the beginning of the while loop.
		else
		{
			continue;
		}
	}
}

//	getItemsProcessed( )
//
//	Author:	Provided
//	Access: public
//	Description:
//		Returns the amount of items currently completed.
//		
//	Param:
//		- n/a	|
//
//	Return: std::atomic_int&	|	Amount of items processed
//
std::atomic_int& ThreadPool::getItemsProcessed()
{
	return m_aiItemsProcessed;
}

