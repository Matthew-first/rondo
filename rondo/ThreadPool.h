//
// Created by 王筱童 on 2021/9/1.
//

#ifndef UNTITLED3_THREADPOOL_H
#define UNTITLED3_THREADPOOL_H

#include <functional>
#include <thread>
#include <boost/noncopyable.hpp>
#include "BlockQueue.h"
#include <future>
#include <vector>
#include <memory>
#include <atomic>

namespace rondo {

    class ThreadPool:boost::noncopyable{
    private:
        typedef std::function<void(void)> Task;
        typedef std::shared_ptr<std::thread> ThreadPtr;

        [[maybe_unused]] std::mutex mutex;
        [[maybe_unused]] std::condition_variable cond;
        BlockQueue<Task> tasks;

        std::vector<ThreadPtr> threadList;
        atomic<bool> running{false};
        atomic<bool> stoped{true};

        template<typename Fun,typename ...Args>
        auto submit(Fun &&fun,Args && ...args)-> std::future<decltype(fun(args...))>{
            using ResType= decltype(fun(args...));
            typedef std::packaged_task<ResType()> FutureTask;

            std::shared_ptr<FutureTask> task= std::make_shared<FutureTask>([Func = std::forward<Fun>(fun)] { return Func(); });
            tasks.push([task](){
                (*task)();
            });

            return task->get_future();
        }

        Task getTask(){
            return tasks.take();
        }

        void taskRun(){
            while (running.load()){
                Task && task=getTask();
                task();
            }
        }
    public:
        ThreadPool()= default;

        template<typename Fun,typename ...Args>
        auto add(Fun &&fun,Args && ...args){
            return submit<Fun,Args...>(std::forward<Fun>(fun),std::forward<Args>(args)...);
        }
        void start(int threadNum){
            //std::unique_lock<std::mutex> lock;
            if(running.load()){
                return;
            }
            for(int i=0;i<threadNum;i++){
                threadList.emplace_back(make_shared<std::thread>(&ThreadPool::taskRun,this));
            }
            running.store(true);
            stoped.store(false);
        }
        void stop(){
            //std::unique_lock<std::mutex> lock;
            if(stoped.load()){
                return;
            }
            running.store(false);
            for(int i=0;i<threadList.size();i++){
                add([](){});
            }
            for(auto & i : threadList){
                i->join();
            }
            threadList.clear();
            stoped.store(true);
        }
    };

}

#endif //UNTITLED3_THREADPOOL_H
