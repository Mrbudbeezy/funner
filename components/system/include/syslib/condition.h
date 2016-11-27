#ifndef SYSLIB_CONDITION_HEADER
#define SYSLIB_CONDITION_HEADER

#include <stl/auto_ptr.h>

#include <syslib/mutex.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Условие работы кода нити
///////////////////////////////////////////////////////////////////////////////////////////////////
class Condition
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Condition  ();
    ~Condition ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Посылка оповещения об изменении условия
///////////////////////////////////////////////////////////////////////////////////////////////////
    void NotifyOne ();
    void NotifyAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание изменения условия
///////////////////////////////////////////////////////////////////////////////////////////////////
                          void Wait    (Mutex& lock);
                          void Wait    (Mutex& lock, size_t wait_in_milliseconds);
    template <class Pred> void Wait    (Mutex& lock, Pred pred);
    template <class Pred> void Wait    (Mutex& lock, Pred pred, size_t wait_in_milliseconds); 
                          bool TryWait (Mutex& lock);
                          bool TryWait (Mutex& lock, size_t wait_in_milliseconds);
    template <class Pred> bool TryWait (Mutex& lock, Pred pred, size_t wait_in_milliseconds);     

  private:
    Condition  (const Condition&); //no impl
    Condition& operator = (const Condition&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

#include <syslib/detail/condition.inl>

}

#endif
