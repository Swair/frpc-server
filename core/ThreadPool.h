#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <functional>
#include <semaphore.h>
#include "MemBlock.h"


class ThreadPool
{
    private:
        struct Thrd
        {
            std::thread worker;
            std::queue<std::function<void()> > tasks;
            sem_t queue_sem;
        };
    public:
        inline ThreadPool(size_t threads) : stop(false), ri(0) {
            for(size_t i = 0; i < threads; ++i) {
                Thrd* thrd = New(Thrd);
                sem_init(&thrd->queue_sem, 0, 0);
                thrd->worker = std::move(std::thread([=]{
                            while(true) {
                            std::function<void()> task;
                            {
                            sem_wait(&thrd->queue_sem);
                            if(this->stop && thrd->tasks.empty())
                            	return;
                            task = std::move(thrd->tasks.front());
                            thrd->tasks.pop();
                            }
                            task();
                            }}));
                thrds.emplace_back(thrd);
            }
        }
        
        template<typename F> auto enqueue(F&& f) -> std::future<decltype(f())>
        {
            Thrd* thrd;
            auto task = std::make_shared< std::packaged_task<decltype(f())()> > (f);
            std::future<decltype(f())> res = task->get_future();
            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            thrd = thrds[ri];
            thrd->tasks.emplace([task](){ (*task)(); });
            ri = (ri + 1) % thrds.size();
            sem_post(&thrd->queue_sem);
            return res;
        }
       
        inline ~ThreadPool()
        {
            stop = true;
            for(auto&& thrd : thrds)
            {
                sem_post(&thrd->queue_sem);
                thrd->worker.join();
                sem_destroy(&thrd->queue_sem);
                Delete(Thrd, thrd);
            }
        }      
    private:
        std::vector<Thrd*> thrds;
        std::atomic<bool> stop;
        int ri;
};

#endif



