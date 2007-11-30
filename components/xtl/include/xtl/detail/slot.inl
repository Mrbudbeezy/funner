namespace detail
{

/*
    ���������� �����
*/

template <class Signature> class slot_impl: public connection_impl
{
  public:
    typedef xtl::function<Signature> function_type;
  
      //������������ / ����������
    slot_impl () : lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot = this;
      next_slot = this;
    }

    slot_impl (const function_type& in_fn) : fn (in_fn), lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot = next_slot = this;
    }
    
    slot_impl (const function_type& in_fn, slot_impl* in_next_slot) :
               fn (in_fn), next_slot (in_next_slot), prev_slot (in_next_slot->prev_slot), lock_count (0), wait_next_slot (0), is_blocked (false)
    {
      prev_slot->next_slot = next_slot->prev_slot = this;
      addref (); //���������� ���� � ������� �������������� ����������� ����� ������
    }

    ~slot_impl ()
    {
      next_slot->prev_slot = prev_slot;
      prev_slot->next_slot = next_slot;
    }

      //�������-���������� �������
    function_type& function () { return fn; }

      //���������� � ��������� ����
    slot_impl* previos () { return prev_slot; }
    slot_impl* next    () { return next_slot; }

      //���������� ������ �������
    void connect (slot_impl* new_next_slot)
    {
      if (new_next_slot == next_slot)
        return;

      if (lock_count > 0)
      {
        wait_next_slot = new_next_slot;
        return;
      }

        //����������� ����

      addref ();

        //����������� ���� �� ������� �������
    
      if (next_slot != this) //��������� ������������ ����� �� �������
      {
        prev_slot->next_slot = next_slot;
        next_slot->prev_slot = prev_slot;

        release (); //������������ ����� �� ������� �������������� ����������� ����� ������
      }

        //������������ ���� � ����� �������

      if (new_next_slot != this)
      {
        next_slot            = new_next_slot;
        prev_slot            = new_next_slot->prev_slot;
        next_slot->prev_slot = prev_slot->next_slot = this;        

        addref ();
      }
      else //disconnect
      {
        next_slot = prev_slot = this;        
      }

        //����������� ����

      release ();
    }
    
      //������������
    void disconnect ()
    {
      if (prev_slot == this)
        return;
        
      connect (this);        
    }

      //�������� ����������
    bool connected () { return prev_slot != this; }

      //������������ �� ����
    bool locked () { return lock_count != 0; }

      //���������� ����� �� ��������������
    void lock ()
    {
      lock_count++;
    } 
    
      //������ ���������� ����� �� ��������������
    void unlock ()
    {
      if (!--lock_count && wait_next_slot)
      {
        connect (wait_next_slot);
        wait_next_slot = 0;
      }
    }
    
      //���������� ����������
    void block ()
    {
      is_blocked = true;
    }
    
      //������ ���������� ����������
    void unblock ()
    {
      is_blocked = false;
    }
    
      //�������� ���������� ����������
    bool blocked () { return is_blocked; }

  private:
    slot_impl (const slot_impl&); //no impl
    slot_impl& operator = (const slot_impl&); //no impl

  private:
    function_type fn;
    slot_impl     *prev_slot, *next_slot;
    size_t        lock_count;
    slot_impl*    wait_next_slot;
    bool          is_blocked;
};

}

/*
    ����������� / ����������
*/

template <class Signature>
inline slot<Signature>::slot ()
  : impl (new detail::slot_impl<Signature> ())
{
}

template <class Signature>
inline slot<Signature>::slot (const function_type& fn)
  : impl (new detail::slot_impl<Signature> (fn))
{
}

template <class Signature>
inline slot<Signature>::slot (const slot& s)
  : impl (s.impl)
{
  impl->addref ();
}

template <class Signature>
inline slot<Signature>::slot (const function_type& fn, slot_impl* next_slot)
  : impl (new slot_impl (fn, next_slot))
{
}

template <class Signature>
inline slot<Signature>::~slot ()
{
  impl->release ();
}

/*
    ������������
*/

template <class Signature>
inline slot<Signature>& slot<Signature>::operator = (const slot& s)
{
  slot (s).swap (*this);
  return *this;
}

template <class Signature>
inline slot<Signature>& slot<Signature>::operator = (const function_type& fn)
{
  slot (fn).swap (*this);
  return *this;
}

/*
    �������-���������� �������
*/

template <class Signature>
inline const typename slot<Signature>::function_type& slot<Signature>::function () const
{
  return impl->function ();
}

/*
    ����������, ��������� �� ������
*/

template <class Signature>
inline xtl::connection slot<Signature>::connection () const
{
  return impl;
}

/*
    ���������� � ��������� ����
*/

template <class Signature>
inline slot<Signature> slot<Signature>::previos () const
{
  return slot (impl->previos ());
}

template <class Signature>
inline slot<Signature> slot<Signature>::next () const
{
  return slot (impl->next ());
}

/*
    ���������� ������
*/


template <class Signature>
inline void slot<Signature>::disconnect ()
{
  impl->disconnect ();
}

template <class Signature>
inline bool slot<Signature>::connected () const
{
  return impl->connected ();
}

/*
    �����
*/

template <class Signature>
inline void slot<Signature>::swap (slot& s)
{
  detail::slot_impl* tmp = s.impl;
  s.impl                 = impl;
  impl                   = tmp;
}

template <class Signature>
inline void swap (slot<Signature>& s1, slot<Signature>& s2)
{
  s1.swap (s2);
}
