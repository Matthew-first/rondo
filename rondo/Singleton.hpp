//
// Created by 王筱童 on 2021/8/31.
//

#ifndef UNTITLED3_SINGLETON_HPP
#define UNTITLED3_SINGLETON_HPP

#include <mutex>

namespace rondo{
    using std::once_flag;
    using std::call_once;
    template<class T>
    class Singleton {
    private:
        static once_flag once;
        static T *instance;

        template<typename ...Args>
        static void init(Args &&...args) {
            instance = new T(std::forward<Args>(args)...);
        }

    public:
        Singleton() = delete;

        template<typename ...Args>
        static T &getInstance(Args &&...args) {
            call_once(once, init<Args...>, std::forward<Args>(args)...);
            return *Singleton<T>::instance;
        }
    };

    template<class T>
    once_flag Singleton<T>::once;
    template<class T>
    T *Singleton<T>::instance = nullptr;
}


#endif //UNTITLED3_SINGLETON_HPP
