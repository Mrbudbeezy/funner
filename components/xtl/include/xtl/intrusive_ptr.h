#ifndef XTL_INTRUSIVE_POINTER_HEADER
#define XTL_INTRUSIVE_POINTER_HEADER

#include <cstddef>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������� ������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct default_intrusive_strategy
{
  static T*   clone   (T* ptr) { intrusive_ptr_add_ref (ptr); return ptr; }
  static void release (T* ptr) { intrusive_ptr_release (ptr); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������� ������ ������� �������� COM-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct com_intrusive_strategy
{
  static T*   clone   (T* ptr) { ptr->AddRef (); return ptr; }
  static void release (T* ptr) { ptr->Release (); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ��������� �� ������ ��������� ������ ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, template <class > class Strategy=default_intrusive_strategy>
class intrusive_ptr
{
  typedef T* (intrusive_ptr::*unspecified_bool_type)() const;
  typedef Strategy<T> strategy_type;
  public:
    typedef T             element_type;
    typedef element_type* pointer;
    typedef element_type& reference;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    intrusive_ptr  ();
    intrusive_ptr  (element_type*, bool addref = true);
    intrusive_ptr  (const intrusive_ptr&);
    ~intrusive_ptr ();

    template <class T1> intrusive_ptr (const intrusive_ptr<T1, Strategy>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    intrusive_ptr& operator = (element_type*);
    intrusive_ptr& operator = (const intrusive_ptr&);

    template <class T1> intrusive_ptr& operator = (const intrusive_ptr<T1, Strategy>&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    pointer   get         () const;
    reference operator *  () const;
    pointer   operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    operator unspecified_bool_type () const; //������� ���������� ���� ��� �������� if (p)
    bool     operator !            () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (intrusive_ptr&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T1, template <class> class Strategy1> bool operator == (const intrusive_ptr<T1, Strategy1>&) const;
    template <class T1, template <class> class Strategy1> bool operator != (const intrusive_ptr<T1, Strategy1>&) const;
    template <class T1, template <class> class Strategy1> bool operator <  (const intrusive_ptr<T1, Strategy1>&) const;
    template <class T1, template <class> class Strategy1> bool operator >  (const intrusive_ptr<T1, Strategy1>&) const;
    template <class T1, template <class> class Strategy1> bool operator <= (const intrusive_ptr<T1, Strategy1>&) const;
    template <class T1, template <class> class Strategy1> bool operator >= (const intrusive_ptr<T1, Strategy1>&) const;

    template <class T1> bool operator == (const T1*) const;
    template <class T1> bool operator != (const T1*) const;
    template <class T1> bool operator <  (const T1*) const;
    template <class T1> bool operator >  (const T1*) const;
    template <class T1> bool operator <= (const T1*) const;
    template <class T1> bool operator >= (const T1*) const;
    
  private:
    pointer ptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1, class T2, template <class> class Strategy> bool operator == (const T1*, const intrusive_ptr<T2, Strategy>&);
template <class T1, class T2, template <class> class Strategy> bool operator != (const T1*, const intrusive_ptr<T2, Strategy>&);
template <class T1, class T2, template <class> class Strategy> bool operator <  (const T1*, const intrusive_ptr<T2, Strategy>&);
template <class T1, class T2, template <class> class Strategy> bool operator >  (const T1*, const intrusive_ptr<T2, Strategy>&);
template <class T1, class T2, template <class> class Strategy> bool operator <= (const T1*, const intrusive_ptr<T2, Strategy>&);
template <class T1, class T2, template <class> class Strategy> bool operator >= (const T1*, const intrusive_ptr<T2, Strategy>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1, class T2, template <class> class Strategy>
intrusive_ptr<T1, Strategy> static_pointer_cast (const intrusive_ptr<T2, Strategy>&);

template <class T1, class T2, template <class> class Strategy>
intrusive_ptr<T1, Strategy> const_pointer_cast (const intrusive_ptr<T2, Strategy>&);

template <class T1, class T2, template <class> class Strategy>
intrusive_ptr<T1, Strategy> dynamic_pointer_cast (const intrusive_ptr<T2, Strategy>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, template <class> class Strategy>
void swap (intrusive_ptr<T, Strategy>&, intrusive_ptr<T, Strategy>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� plain-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, template <class> class Strategy> T* get_pointer (const intrusive_ptr<T, Strategy>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������ ������� COM-��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class com_ptr: public intrusive_ptr<T, com_intrusive_strategy>
{
  typedef intrusive_ptr<T, com_intrusive_strategy> base;
  public:
    typedef typename base::element_type element_type;

    com_ptr ();
    com_ptr (element_type*, bool addref = true);
    com_ptr (const com_ptr&);

    template <class T1> com_ptr (const com_ptr<T1>&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void swap (com_ptr<T>&, com_ptr<T>&);

#include <xtl/detail/intrusive_ptr.inl>

}

#endif
