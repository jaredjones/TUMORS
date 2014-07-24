//
//  LockedQueue.h
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#ifndef TUMORS_LockedQueue_h
#define TUMORS_LockedQueue_h

#include <deque>
#include <mutex>

template <class T, typename StorageType = std::deque<T> >
class LockedQueue
{
    //! Lock access to the queue.
    std::mutex _lock;
    
    //! Storage backing the queue.
    StorageType _queue;
    
    //! Cancellation flag.
    volatile bool _canceled;
    
public:
    
    //! Create a LockedQueue.
    LockedQueue()
    : _canceled(false)
    {
    }
    
    //! Destroy a LockedQueue.
    virtual ~LockedQueue()
    {
    }
    
    //! Adds an item to the queue.
    void add(const T& item)
    {
        lock();
        
        _queue.push_back(item);
        
        unlock();
    }
    
    //! Gets the next result in the queue, if any.
    bool next(T& result)
    {
        std::lock_guard<std::mutex> lock(_lock);
        
        if (_queue.empty())
            return false;
        
        result = _queue.front();
        _queue.pop_front();
        
        return true;
    }
    
    template<class Checker>
    bool next(T& result, Checker& check)
    {
        std::lock_guard<std::mutex> lock(_lock);
        
        if (_queue.empty())
            return false;
        
        result = _queue.front();
        if (!check.Process(result))
            return false;
        
        _queue.pop_front();
        return true;
    }
    
    //! Peeks at the top of the queue. Check if the queue is empty before calling! Remember to unlock after use if autoUnlock == false.
    T& peek(bool autoUnlock = false)
    {
        lock();
        
        T& result = _queue.front();
        
        if (autoUnlock)
            unlock();
        
        return result;
    }
    
    //! Cancels the queue.
    void cancel()
    {
        std::lock_guard<std::mutex> lock(_lock);
        
        _canceled = true;
    }
    
    //! Checks if the queue is cancelled.
    bool cancelled()
    {
        std::lock_guard<std::mutex> lock(_lock);
        return _canceled;
    }
    
    //! Locks the queue for access.
    void lock()
    {
        this->_lock.lock();
    }
    
    //! Unlocks the queue.
    void unlock()
    {
        this->_lock.unlock();
    }
    
    ///! Calls pop_front of the queue
    void pop_front()
    {
        std::lock_guard<std::mutex> lock(_lock);
        _queue.pop_front();
    }
    
    ///! Checks if we're empty or not with locks held
    bool empty()
    {
        std::lock_guard<std::mutex> lock(_lock);
        return _queue.empty();
    }
};

#endif
