//
// Created by 王筱童 on 2021/9/1.
//

#ifndef UNTITLED3_BLOCKQUEUE_H
#define UNTITLED3_BLOCKQUEUE_H
#include <boost/noncopyable.hpp>
#include <condition_variable>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <cassert>
namespace rondo{
    template<typename T>
    class BlockQueue: public boost::noncopyable{
    private:
        typedef T value_type;
        mutable std::mutex mutex;
        std::queue<T> queue;
        std::condition_variable isempty;
    public:
        BlockQueue():mutex(),queue(),isempty(){}
        void push(const value_type &val){
            std::unique_lock<std::mutex> lock(mutex);
            queue.push(val);
            isempty.notify_one();
        }
        T& front(){
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty()){
                isempty.wait(lock);
            }
            assert(!queue.empty());
            return queue.front();
        }
        const T& front(){
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty()){
                isempty.wait(lock);
            }
            assert(!queue.empty());
            return queue.front();
        }
        void pop(){
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty()){
                isempty.wait(lock);
            }
            assert(!queue.empty());
            return queue.pop();
        }
        T& take(){
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty()){
                isempty.wait(lock);
            }
            assert(!queue.empty());
            T res=queue.front();
            queue.pop();
            return res;
        }
    };
}
#endif //UNTITLED3_BLOCKQUEUE_H
