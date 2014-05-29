#ifndef __GAMERUNNABLE_H
#define __GAMERUNNABLE_H

#include <thread>
#include <atomic>

class Runnable
{
public:
    Runnable() : m_stop(), m_thread() { }
    virtual ~Runnable() { try { stop(); } catch(...) { /*??*/ } }
    
    Runnable(Runnable const&) = delete;
    Runnable& operator =(Runnable const&) = delete;
    
    void stop() { m_stop = true; /*m_thread.join();*/ }
    void start() { m_thread = std::thread(&Runnable::run, this); }
    void join() { m_thread.join(); }
protected:
    virtual void run() = 0;
    std::atomic_bool m_stop;
    
private:
    std::thread m_thread;
};

/// Heartbeat thread for the Game
class GameRunnable : public Runnable
{
    protected:
        void run();
};

#endif

