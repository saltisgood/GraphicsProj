#include "Threading.h"

using namespace perf;
using namespace std;

#define THREAD_NUM 8

#ifdef __CPP11

#include <mutex>

ThreadPool::ThreadPool() : 
	mThreadNum(THREAD_NUM),
	mThreads(),
	mCondition(),
	mMutex(),
	mWorkMutex(),
	mAtomicCount(0),
	mThreadExit(false),
	mWorkAvailable(false),
	mFunc(NULL),
	mArg1(NULL),
	mArg2(NULL),
	mArg3(NULL),
	mArg4(NULL)
{
	for (uchar i = 0; i < mThreadNum; i++)
	{
		mThreads.push_back(thread(wait, i));
	}
}

ThreadPool::~ThreadPool()
{
	mThreadExit = true;
	mWorkAvailable = true;
	mCondition.notify_all();
}

void ThreadPool::wait(uchar threadNum)
{
	unique_lock<mutex> ulck(mMutex, defer_lock);

	while (!mThreadExit)
	{
		mWorkMutex.lock();
		ulck.lock();
		mWorkMutex.unlock();

		mAtomicCount++;

		mCondition.wait(ulck, isWorkAvailable);

		if (mThreadExit)
		{
			return;
		}

		(*mFunc)(threadNum, mThreadNum, mArg1, mArg2, mArg3, mArg4);

		mAtomicCount++;
	}
}

void ThreadPool::doWork(void (*func)(uchar, uchar, void*, void*, void*, void*), void * arg1, void * arg2, void * arg3, void * arg4)
{
	unique_lock<mutex> ulck(mWorkMutex);

	mFunc = func;
	mArg1 = arg1;
	mArg2 = arg2;
	mArg3 = arg3; 
	mArg4 = arg4;

	uchar avail_threads = mAtomicCount;
	mAtomicCount = 0;

	mWorkAvailable = true;
	
	mCondition.notify_all();

	while (mAtomicCount < mThreadNum)
	{
		this_thread::yield();
	}

	mAtomicCount = 0;
	mFunc = NULL;
	mArg1 = NULL;
	mArg2 = NULL;
	mArg3 = NULL;
	mArg4 = NULL;

	mWorkAvailable = false;
}

#endif