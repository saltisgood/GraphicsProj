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
	mConditions(),
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
		mConditions.push_back(new condition_variable());
		mThreads.push_back(thread(&perf::ThreadPool::wait, this, i));
	}
}

ThreadPool::~ThreadPool()
{
	mThreadExit = true;
	mWorkAvailable = true;

	for (auto& cond : mConditions)
	{
		cond->notify_one();
		delete cond;
	}
}

void ThreadPool::wait(uchar threadNum)
{
	mutex lck;
	unique_lock<mutex> ulck(lck);
	unique_lock<mutex> worklck(mWorkMutex, std::defer_lock);
	bool& work = mWorkAvailable;

	condition_variable* cond = mConditions.at(threadNum);

	while (!mThreadExit)
	{
		worklck.lock();
		worklck.unlock();

		mAtomicCount++;

		cond->wait(ulck, [&work]() { return work; });

		if (mThreadExit)
		{
			return;
		}

		(*mFunc)(threadNum, mThreadNum, mArg1, mArg2, mArg3, mArg4);

		mAtomicCount++;
	}
}

void ThreadPool::doWorkInst(void (*func) WORKER_ARGS(,,,,,), void * arg1, void * arg2, void * arg3, void * arg4)
{
	while (mAtomicCount < mThreadNum)
	{
		this_thread::yield();
	}

	unique_lock<mutex> ulck(mWorkMutex);

	mFunc = func;
	mArg1 = arg1;
	mArg2 = arg2;
	mArg3 = arg3; 
	mArg4 = arg4;

	uchar avail_threads = mAtomicCount;
	mAtomicCount = 0;

	mWorkAvailable = true;
	
	for (auto& cond : mConditions)
	{
		cond->notify_one();
	}

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