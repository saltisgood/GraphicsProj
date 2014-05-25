#ifndef THREADING_H_
#define THREADING_H_

#ifdef __CPP11

#include <thread>
#include <condition_variable>
#include <atomic>

#include "Util.h"

namespace perf
{
	class ThreadPool
	{
	public:
		virtual ~ThreadPool();

		static void doWork(void (*func)(uchar, uchar, void*, void*, void*, void*), void *arg1 = NULL, void *arg2 = NULL, void *arg3 = NULL, void *arg4 = NULL)
		{
			sInstance->doWorkInst(func, arg1, arg2, arg3, arg4);
		}

	private:
		ThreadPool();

		void doWorkInst(void (*func)(uchar, uchar, void*, void*, void*, void*), void *, void *, void *, void *);
		bool isWorkAvailable() const { mWorkAvailable; }
		void wait(uchar);

		const uchar mThreadNum;
		std::vector<std::thread> mThreads;
		std::condition_variable mCondition;
		std::mutex mMutex;
		std::mutex mWorkMutex;
		std::atomic<uchar> mAtomicCount;
		bool mThreadExit;
		bool mWorkAvailable;
		void (*mFunc)(uchar, uchar, void*, void*, void*, void*);
		void * mArg1;
		void * mArg2;
		void * mArg3;
		void * mArg4;

		static ThreadPool * sInstance;

		//friend bool isWorkAvailable();
	};

	//bool isWorkAvailable() { return ThreadPool::sInstance->mWorkAvailable; }
}

#endif

#endif