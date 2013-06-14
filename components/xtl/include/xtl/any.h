#ifndef XTL_ANY_HEADER
#define XTL_ANY_HEADER

#include <xtl/custom_cast.h>
#include <xtl/lexical_cast.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/type.h>
#include <xtl/type_traits> //for is_polymorphic, remove_reference

namespace stl
{

//forward declarations
template <class T> class auto_ptr;

}

namespace xtl
{

namespace detail
{

//implementation forwards
struct any_holder;

}

//forward declaration
template <class T> class shared_ptr;
template <class T> class com_ptr;
template <class T> class trackable_ptr;

template <class T, template <class > class Strategy>  class intrusive_ptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� �������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class any
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    any  ();
    any  (const any&);
    ~any ();
                       
    template <class T> explicit any (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    any& operator = (const any&);
    
    template <class T> any& operator = (const T& value);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    any clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� / �������� �� ��������� 0 (���� castable_type - ������ ��� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;
    bool null  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& type () const;

    template <class T>       T* content ();
    template <class T> const T* content () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> const T cast () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void to_string (stl::string& buffer) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    any& swap (any&);
    
  private:
    any (detail::any_holder*);

  private: 
    detail::any_holder* content_ptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (any&, any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>       T* any_cast (any*);
template <class T> const T* any_cast (const any*);
template <class T>       T  any_cast (any&);
template <class T> const T  any_cast (const any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������� �����:
/// - ������� custom_cast ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> const T any_multicast (const any&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ��������. ������������ ��� ������� ��� ������ any_multicast
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T& get_castable_value (T&);
template <class T> T* get_castable_value (T*);
template <class T> T* get_castable_value (stl::auto_ptr<T>&);
template <class T> T* get_castable_value (shared_ptr<T>&);
template <class T> T* get_castable_value (com_ptr<T>&);
template <class T> T* get_castable_value (reference_wrapper<T>&);
template <class T> T* get_castable_value (trackable_ptr<T>&);

template <class T, template <class > class Strategy>
T* get_castable_value (intrusive_ptr<T, Strategy>&);

char*    get_castable_value (char*);
wchar_t* get_castable_value (wchar_t*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
void to_string (stl::string& buffer, const volatile any& value);

#include <xtl/detail/any.inl>

}

#endif
