namespace detail
{

/*
    Функтор, сбрасывающий значение указателя
*/

template <class T>
struct trackable_ptr_reseter
{
  trackable_ptr_reseter (trackable_ptr<T>& in_ptr) : ptr (in_ptr) {}

  void operator () () const { ptr.reset (); }

  trackable_ptr<T>& ptr;
};

}

/*
    Конструкторы
*/

#ifdef _MSC_VER
  #pragma warning (push) //'this' : used in base member initializer list
  #pragma warning (disable : 4355)
#endif

template <class T>
inline trackable_ptr<T>::trackable_ptr ()
  : ptr (0), on_destroy (detail::trackable_ptr_reseter<T> (*this))
{
}

template <class T>
inline trackable_ptr<T>::trackable_ptr (T* in_ptr)
  : ptr (in_ptr), on_destroy (detail::trackable_ptr_reseter<T> (*this))
{
  update ();
}

template <class T>
inline trackable_ptr<T>::trackable_ptr (const trackable_ptr<T>& p)
  : ptr (p.ptr), on_destroy (detail::trackable_ptr_reseter<T> (*this))
{
  update ();
}

template <class T> template <class T1>
inline trackable_ptr<T>::trackable_ptr (T1* in_ptr)
  : ptr (static_cast<pointer> (in_ptr)), on_destroy (detail::trackable_ptr_reseter<T> (*this))
{
  update ();
}

template <class T> template <class T1>
inline trackable_ptr<T>::trackable_ptr (const trackable_ptr<T1>& p)
  : ptr (static_cast<pointer> (p.get ())), on_destroy (detail::trackable_ptr_reseter<T> (*this))
{
  update ();
}

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

/*
    Получение trackable-объекта
*/

inline trackable& get_trackable (trackable& t)
{
  return t;
}

inline trackable& get_trackable (const trackable& t)
{
  return const_cast<trackable&> (t);
}

inline trackable& get_trackable (const volatile trackable& t)
{
  return const_cast<trackable&> (t);
}

inline trackable& get_trackable (volatile trackable& t)
{
  return const_cast<trackable&> (t);
}

inline int& get_trackable (const default_cast_type&)
{
  static int dummy = 0;
  return dummy;
}

/*
    Регистрация обработчика удаления хранимого объекта
*/

namespace detail
{

inline void register_handler (trackable::slot_type& slot, trackable* ptr)
{
  ptr->connect_tracker (slot);
}

inline void register_handler (trackable::slot_type&, ...)
{
}

}

template <class T>
void trackable_ptr<T>::update ()
{
  on_destroy.disconnect ();

  if (ptr)
    detail::register_handler (on_destroy, &get_trackable (*ptr));

  try
  {
    notification ();
  }
  catch (...)
  {
  }
}

/*
    Присваивание
*/

template <class T>
inline trackable_ptr<T>& trackable_ptr<T>::operator = (T* in_ptr)
{
  reset (in_ptr);
  return *this;
}

template <class T>
inline trackable_ptr<T>& trackable_ptr<T>::operator = (const trackable_ptr& p)
{
  reset (p.get ());
  return *this;
}

template <class T> template <class T1>
inline trackable_ptr<T>& trackable_ptr<T>::operator = (T1* in_ptr)
{
  reset (in_ptr);
  return *this;
}

template <class T> template <class T1>
inline trackable_ptr<T>& trackable_ptr<T>::operator = (const trackable_ptr<T1>& p)
{
  reset (p.get ());
  return *this;
}

template <class T>
inline void trackable_ptr<T>::reset ()
{
  ptr = 0;

  update ();
}

template <class T>
inline void trackable_ptr<T>::reset (T* in_ptr)
{
  ptr = in_ptr;

  update ();
}

template <class T> template <class T1>
inline void trackable_ptr<T>::reset (T1* in_ptr)
{
  ptr = static_cast<pointer> (in_ptr);

  update ();
}

/*
    Операции доступа
*/

template <class T>
inline T* trackable_ptr<T>::get () const
{
  return ptr;
}

template <class T>
inline T& trackable_ptr<T>::operator * () const
{
  if (!ptr)
    throw bad_trackable_ptr ();

  return *ptr;
}

template <class T>
inline T* trackable_ptr<T>::operator -> () const
{
  if (!ptr)
    throw bad_trackable_ptr ();

  return ptr;
}

template <class T>
inline T* get_pointer (trackable_ptr<T>& p)
{
  return p.get ();
}

/*
    Информация о состоянии указателя
*/

template <class T>
inline bool trackable_ptr<T>::operator ! () const
{
  return ptr == 0;
}

/*
    Добавление/удаление обработчиков изменения состояния указателя
*/

template <class T>
inline connection trackable_ptr<T>::connect (const function_type& fn)
{
  return notification.connect (fn);
}

template <class T>
inline connection trackable_ptr<T>::connect (slot_type& s)
{
  return notification.connect (s);
}

template <class T> template <class Fn>
inline void trackable_ptr<T>::disconnect (Fn fn)
{
  notification.disconnect (fn);
}

template <class T>
inline void trackable_ptr<T>::disconnect_all ()
{
  notification.disconnect_all ();
}

/*
    Сравнение
*/

template <class T> template <class T1>
inline bool trackable_ptr<T>::operator == (const trackable_ptr<T1>& p) const
{
  return ptr == p.ptr;
}

template <class T> template <class T1>
inline bool trackable_ptr<T>::operator == (const T1* in_ptr) const
{
  return ptr == in_ptr;
}

template <class T> template <class T1>
inline bool trackable_ptr<T>::operator != (const trackable_ptr<T1>& p) const
{
  return ptr != p.ptr;
}

template <class T> template <class T1>
inline bool trackable_ptr<T>::operator != (const T1* in_ptr) const
{
  return ptr != in_ptr;
}

template <class T1, class T2>
inline bool operator == (const T1* p1, const trackable_ptr<T2>& p2)
{
  return p1 == p2.get ();
}

template <class T1, class T2>
inline bool operator != (const T1* p1, const trackable_ptr<T2>& p2)
{
  return p1 != p2.get ();
}
