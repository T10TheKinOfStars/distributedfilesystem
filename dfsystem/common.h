#ifndef COMMON_H
#define COMMON_H 
#include <mutex> 

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

class AutoLock {
    public:
        AutoLock(std::mutex *vmutex):mylock(*vmutex) {mylock.lock();}
        AutoLock(std::mutex &vmutex):mylock(vmutex) {mylock.lock();}
        ~AutoLock() {mylock.unlock();}
    private:
        std::mutex &mylock;
};
#endif
