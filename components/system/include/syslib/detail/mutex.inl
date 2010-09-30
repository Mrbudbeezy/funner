/*
    �����஢�� � �।���� �����    
*/

inline Lock::Lock (Mutex& in_Mutex)
  : mutex (in_Mutex)
{
  mutex.Lock ();
}

inline Lock::~Lock ()
{
  mutex.Unlock ();
}

/*
    �㭪樨 ��� ����������⢨� � XTL
*/

inline void lock (Mutex& mutex)
{
  mutex.Lock ();
}

inline void unlock (Mutex& mutex)
{
  mutex.Unlock ();
}
