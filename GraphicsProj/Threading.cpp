#include "Threading.h"

using namespace perf;
using namespace std;

#define THREAD_NUM 4

#ifdef __CPP11

#include <mutex>

ThreadPool * ThreadPool::sInstance = new ThreadPool();

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
		mThreads.push_back(thread(&perf::ThreadPool::wait, this, i));
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
	mutex lck;
	unique_lock<mutex> ulck(lck);
	bool& work = mWorkAvailable;

	while (!mThreadExit)
	{
		mWorkMutex.lock();
		mWorkMutex.unlock();

		mAtomicCount++;

		mCondition.wait(ulck, [&work]() { return work; });

		if (mThreadExit)
		{
			return;
		}

		(*mFunc)(threadNum, mThreadNum, mArg1, mArg2, mArg3, mArg4);

		mAtomicCount++;
	}
}

void ThreadPool::doWorkInst(void (*func)(uchar, uchar, void*, void*, void*, void*), void * arg1, void * arg2, void * arg3, void * arg4)
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

	while (mAtomicCount < avail_threads)
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