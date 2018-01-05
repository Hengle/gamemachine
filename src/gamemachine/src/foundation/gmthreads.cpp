﻿#include "stdafx.h"
#include "gmthreads.h"
#if GM_WINDOWS
#	include <process.h>
#endif
#include <mutex>
#include <thread>

#if GM_USE_PTHREAD
void* pthreadCallback(void* thread)
{
	GMThread* t = (GMThread*)(thread);
	GMThread::Data* d = t->threadData();

	d->state = Running;
	if (d->callback)
		d->callback->beforeRun(t);
	t->run();
	if (d->callback)
		d->callback->afterRun(t);
	d->state = Finished;
	d->event.set();
	d->done = true;
}
#endif // GM_USE_PTHREAD

GMThread::GMThread()
{
	D(d);
	d->state = NotRunning;
	d->callback = nullptr;
	d->event.set();
}

void GMThread::threadCallback()
{
	D(d);
	d->state = Running;
	if (d->callback)
		d->callback->beforeRun(this);
	run();
	if (d->callback)
		d->callback->afterRun(this);
	d->state = Finished;
	d->event.set();
	d->done = true;
}

void GMThread::start()
{
	D(d);
#if GM_USE_PTHREAD
	pthread_create(&d->handle.thread, nullptr, ::pthreadCallback, this);
#else
	d->handle = GMThreadHandle(&GMThread::threadCallback, this);
#endif // GM_USE_PTHREAD

	d->event.reset();
	d->done = false;
	if (d->callback)
		d->callback->onCreateThread(this);
}

void GMThread::wait(GMuint milliseconds)
{
	D(d);
	// 等待线程结束，并重置事件
	d->event.wait();
}

void GMThread::setCallback(IThreadCallback* callback)
{
	D(d);
	d->callback = callback;
}

void GMThread::terminate()
{
	D(d);
#if GM_USE_PTHREAD
	pthread_exit(nullptr);
#else
	HANDLE* handle = (HANDLE*)d->handle.native_handle();
	::TerminateThread(handle, 0);
#endif
}

void GMThread::detach()
{
	D(d);
#if GM_USE_PTHREAD
	pthread_detach(pthread_self());
#else
	if (d->handle.joinable())
		d->handle.detach();
#endif
}

bool GMThread::join()
{
	D(d);
#if GM_USE_PTHREAD
	pthread_join(pthread_self(), nullptr);
	return true;
#else
	if (d->handle.joinable())
	{
		d->handle.join();
		return true;
	}
	return false;
#endif
}

GMThreadHandle::id GMThread::getThreadId()
{
	D(d);
#if GM_USE_PTHREAD
    ///todo pthread
    GM_ASSERT(false);
	//return (unsigned long int)d->handle.thread;
	return 0;
#else
	return d->handle.get_id();
#endif
}

GMThreadHandle::id GMThread::getCurrentThreadId()
{
#if GM_USE_PTHREAD
    ///todo pthread
    GM_ASSERT(false);
	//return (unsigned long int)pthread_self();
	return 0;
#else
	return std::this_thread::get_id();
#endif
}

void GMThread::sleep(GMint milliseconds)
{
#if GM_USE_PTHREAD
	GM_ASSERT(false);
	//::sleep(milliseconds / 1000.f);
#else
	std::chrono::duration<GMint, std::milli> milli(milliseconds);
	std::this_thread::sleep_for(milli);
#endif
}