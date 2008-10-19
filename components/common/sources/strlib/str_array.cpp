#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/string.h>

using namespace common;

namespace
{

/*
    ���������
*/

const size_t STRING_OFFSETS_RESERVE_SIZE = 16;  //������������� ��� ������������� ����� �����
const size_t STRING_BUFFER_RESERVE_SIZE  = 256; //������������� ������ ������ �����

}

/*
    ���������� ������� �����
*/

template <class Char> class BasicStringArray<Char>::Impl: public xtl::reference_counter
{
  public:
///�����������
    Impl () : need_update_pointers (false)
    {
      offsets.reserve (STRING_OFFSETS_RESERVE_SIZE);
      string_buffer.reserve (STRING_BUFFER_RESERVE_SIZE);
    }

///�����������
    Impl (const Impl& buffer) :
      string_buffer (buffer.string_buffer),
      offsets (buffer.offsets),
      need_update_pointers (true) { }      

///���������� �����
    size_t Size () { return offsets.size (); }

///����������������� ����� �����
    size_t Capacity () { return offsets.capacity (); }

///������ ������ �����
    size_t BufferSize () { return string_buffer.capacity (); }

///���������� �����
    size_t Add (size_t count, const Char** strings)
    {
      if (count && !strings)
        throw xtl::make_null_argument_exception ("", "string_array");
        
      volatile size_t old_buffer_size   = string_buffer.size (),
                      old_strings_count = offsets.size ();

      if (!count)
        return old_strings_count;

      need_update_pointers = true;

      try
      {
        offsets.reserve (offsets.size () + count);

        for (size_t i=0; i<count; i++)
        {
          const Char* string = strings [i];

          size_t offs = string_buffer.size ();

          if (string)
            string_buffer.append (string);            

          static Char null_buffer [] = {0};            

          string_buffer.append (null_buffer, 1);
          offsets.push_back    (offs);
        }

        return old_strings_count;
      }
      catch (...)
      {
        string_buffer.erase (old_buffer_size);
        offsets.erase (offsets.begin () + old_strings_count, offsets.end ());
        throw;
      }
    }    
    
///�������� �����
    void Remove (size_t first, size_t count)
    {
      if (first >= offsets.size () || !offsets.size ())
        return;

      size_t last = first + count;

      if (last > offsets.size ())
        last = offsets.size () - 1;

      offsets.erase (offsets.begin () + first, offsets.begin () + last);
    }
    
///�������
    void Clear ()
    {
      string_buffer.clear ();
      offsets.clear ();
      pointers.clear ();
      
      need_update_pointers = false;
    }
    
///��������� ������
    const Char* Item (size_t index)
    {
      if (index >= offsets.size ())
        throw xtl::make_range_exception ("common::BasicStringArray<Char>::operator []", "index", index, offsets.size ());

      return string_buffer.c_str () + offsets [index];
    }
    
///��������� ������� �����
   const Char** Items ()
   {
     if (offsets.empty ())
     {
       static Char        null_string [1] = {0};
       static const Char* null_array [1]  = {&null_string [0]};
      
       return null_array;
     }
    
     if (need_update_pointers)
     {
       pointers.clear ();
       pointers.resize (offsets.size ());

       typename PointerBuffer::iterator ptr_iter = pointers.begin ();
       const Char*                      base     = string_buffer.c_str ();
      
       for (typename OffsetBuffer::iterator iter=offsets.begin (), end=offsets.end (); iter!=end; ++iter, ++ptr_iter)
         *ptr_iter = base + *iter;

       need_update_pointers = false;
     }

     return &pointers [0];
   }

///�������������� ����� �����
    void Reserve (size_t string_count)
    {
      size_t* old_start = &offsets [0];
      
      offsets.reserve (string_count);
      
      if (old_start != &offsets [0])
        need_update_pointers = true;
    }

///�������������� ����� � ������ �����
    void ReserveBuffer (size_t size)
    {
      const Char* old_start = string_buffer.c_str ();
      
      string_buffer.reserve (size);
      
      if (old_start != string_buffer.c_str ())
        need_update_pointers = true;
    }
    
  private:
    typedef stl::vector<size_t>      OffsetBuffer;
    typedef stl::vector<const Char*> PointerBuffer;

  private:
    stl::basic_string<Char> string_buffer;        //����� �������� �����
    OffsetBuffer            offsets;              //�������� ����� � ������
    PointerBuffer           pointers;             //������ ���������� �� ������
    bool                    need_update_pointers; //����� �������� ������ ���������� �� ������
};

/*
    ������������ / ����������
*/

template <class Char>
BasicStringArray<Char>::BasicStringArray ()
  : impl (new Impl)
{
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (Impl* in_impl)
  : impl (in_impl)
{
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (const BasicStringArray& sa)
  : impl (sa.impl)
{
  addref (impl);
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (size_t string_count, const value_type** string_array)
  : impl (new Impl)
{
  Add (string_count, string_array);
}

template <class Char>
BasicStringArray<Char>::~BasicStringArray ()
{
  release (impl);
}

/*
    �����������
*/

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::Clone () const
{
  return BasicStringArray (new Impl (*impl));
}

/*
    ������������
*/

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator = (const BasicStringArray<Char>& sa)
{
  BasicStringArray (sa).Swap (*this);
  
  return *this;
}

/*
    ���������� ����� / �������� �� �������
*/

template <class Char>
size_t BasicStringArray<Char>::Size () const
{
  return impl->Size ();
}

template <class Char>
bool BasicStringArray<Char>::IsEmpty () const
{
  return impl->Size () == 0;
}

template <class Char>
size_t BasicStringArray<Char>::Capacity () const
{
  return impl->Capacity ();
}

template <class Char>
size_t BasicStringArray<Char>::BufferSize () const
{
  return impl->BufferSize ();
}

/*
    �������������� ����� ����� / �������������� ����� � ������ �����
*/

template <class Char>
void BasicStringArray<Char>::Reserve (size_t string_count)
{
  impl->Reserve (string_count);
}

template <class Char>
void BasicStringArray<Char>::ReserveBuffer (size_t buffer_size)
{
  impl->ReserveBuffer (buffer_size);
}

/*
    ��������� ������ / ������� �����
*/

template <class Char>
const typename BasicStringArray<Char>::value_type* BasicStringArray<Char>::operator [] (size_t index) const
{
  return impl->Item (index);
}

template <class Char>
const typename BasicStringArray<Char>::value_type** BasicStringArray<Char>::Data () const
{
  return impl->Items ();
}

/*
    ���������� �����
*/

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator += (const value_type* s)
{
  Add (1, &s);
  
  return *this;
}

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator += (const BasicStringArray& sa)
{
  Add (sa);

  return *this;
}

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::operator + (const value_type* s) const
{
  BasicStringArray res = Clone ();

  return res += s;
}

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::operator + (const BasicStringArray& sa) const
{
  BasicStringArray res = Clone ();

  return res += sa;
}

template <class Char>
size_t BasicStringArray<Char>::Add (const value_type* string)
{
  return Add (1, &string);
}

template <class Char>
size_t BasicStringArray<Char>::Add (const BasicStringArray& sa)
{
  return Add (sa.Size (), sa.Data ());
}

template <class Char>
size_t BasicStringArray<Char>::Add (size_t string_count, const value_type** string_array)
{
  try
  {
    return impl->Add (string_count, string_array);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BasicStringArray<Char>::Add(size_t,const value_type**)");
    throw;
  }
}

/*
    �������� �����
*/

template <class Char>
void BasicStringArray<Char>::Remove (size_t first, size_t count)
{
  impl->Remove (first, count);
}

template <class Char>
void BasicStringArray<Char>::Remove (size_t index)
{
  impl->Remove (index, 1);
}

template <class Char>
void BasicStringArray<Char>::Clear ()
{
  impl->Clear ();
}

/*
    �����
*/

template <class Char>
void BasicStringArray<Char>::Swap (BasicStringArray& sa)
{
  stl::swap (impl, sa.impl);
}

namespace common
{

void swap (StringArray& sa1, StringArray& sa2)
{
  sa1.Swap (sa2);
}

void swap (WStringArray& sa1, WStringArray& sa2)
{
  sa1.Swap (sa2);
}

/*
    �������������� ��������� ���� ��� StringArray, WStringArray
*/

template class BasicStringArray<char>;
template class BasicStringArray<wchar_t>;

}
