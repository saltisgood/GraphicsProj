#ifndef THREADING_H_
#define THREADING_H_

#ifdef __CPP11

#include <thread>
#include <condition_variable>
#include <atomic>

#include "Util.h"

#define WORKER_ARGS(a,b,c,d,e,f) (uchar a, uchar b, void * c, void * d, void * e, void * f)

namespace perf
{
	class ThreadPool
	{
	public:
		virtual ~ThreadPool();

		static void doWork(void (*func) WORKER_ARGS(), void *arg1 = NULL, void *arg2 = NULL, void *arg3 = NULL, void *arg4 = NULL)
		{
			sInstance->doWorkInst(func, arg1, arg2, arg3, arg4);
		}

	private:
		ThreadPool();

		void doWorkInst(void (*func) WORKER_ARGS(), void *, void *, void *, void *);
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
		void (*mFunc) WORKER_ARGS();
		void * mArg1;
		void * mArg2;
		void * mArg3;
		void * mArg4;

		static ThreadPool * sInstance;
	};
}

#else

namespace perf
{
	class ThreadPool
	{
		static void doWork(void (*func) WORKER_ARGS(), void *arg1 = NULL, void *arg2 = NULL, void *arg3 = NULL, void *arg4 = NULL)
		{
			(*func)(0, 1, arg1, arg2, arg3, arg4);
		}
	};
}

#endif

#endif