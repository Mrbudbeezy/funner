#include <xtl/reference_counter.h>
#include <xtl/uninitialized_storage.h>

#include <math/spline.h>

using namespace math;

/*
    �������� ���������� �������
*/

typedef xtl::uninitialized_storage<char> Buffer;

struct basic_spline::implementation
{
  detail::spline_operations& operations;     //��������� ���������� �������� ��� �������
  spline_type                type;           //��� �������
  size_t                     key_size;       //������ �����
  spline_wrap                begin_wrap;     //��������� �� ����� ������� �������
  spline_wrap                end_wrap;       //��������� �� ������ ������� �������
  Buffer                     buffer;         //����� �������� �����
  size_t                     keys_count;     //���������� ������
  bool                       need_recompute; //��������� �������� �������������
  
///�����������
  implementation (detail::spline_operations& in_operations, spline_type in_type, size_t in_key_size)
    : operations (in_operations)
    , type (in_type)
    , key_size (in_key_size)
    , begin_wrap (spline_wrap_default)
    , end_wrap (spline_wrap_default)
    , keys_count (0)
    , need_recompute (false)
  {
    switch (type)
    {
      case spline_type_tcb:
      case spline_type_bezier:
        break;
      default:
        throw xtl::make_argument_exception ("math::basic_spline::implementation::implementation", "type", type);
    }
  }
  
///�����������
  implementation (const implementation& impl)
    : operations (impl.operations)
    , type (impl.type)
    , key_size (impl.key_size)
    , begin_wrap (impl.begin_wrap)
    , end_wrap (impl.end_wrap)
    , keys_count (impl.keys_count)
    , need_recompute (impl.need_recompute)
  {
    buffer.resize (impl.buffer.size ());

    operations.initialize_keys (keys_count, impl.buffer.data (), buffer.data ());
  }
};

/*
    ������������ / ���������� / ������������
*/

basic_spline::basic_spline (detail::spline_operations& operations, spline_type type, size_t key_size)
  : impl (new implementation (operations, type, key_size))
{
}

basic_spline::basic_spline (const basic_spline& s)
  : impl (s.impl)
{
  addref (impl);
}

basic_spline::~basic_spline ()
{
  release (impl);
}

basic_spline& basic_spline::operator = (const basic_spline& s)
{
  basic_spline (s).swap (*this);

  return *this;
}

/*
    ��������� �� �������
*/

void basic_spline::wrap (spline_wrap begin_wrap, spline_wrap end_wrap)
{
  switch (begin_wrap)
  {
    case spline_wrap_clamp:
    case spline_wrap_repeat:
    case spline_wrap_mirror:
      break;
    default:
      throw xtl::make_argument_exception ("math::basic_spline::wrap", "begin_wrap", begin_wrap);
  }
  
  switch (end_wrap)
  {
    case spline_wrap_clamp:
    case spline_wrap_repeat:
    case spline_wrap_mirror:
      break;
    default:
      throw xtl::make_argument_exception ("math::basic_spline::wrap", "end_wrap", end_wrap);
  }

  impl->begin_wrap = begin_wrap;
  impl->end_wrap   = end_wrap;
}

spline_wrap basic_spline::begin_wrap () const
{
  return impl->begin_wrap;
}

spline_wrap basic_spline::end_wrap () const
{
  return impl->end_wrap;
}

/*
    ���������� ������ / �������� �� �������
*/

size_t basic_spline::keys_count () const
{
  return impl->keys_count;
}

bool basic_spline::empty () const
{
  return impl->keys_count == 0;
}

/*
    �����������
*/

basic_spline basic_spline::clone () const
{
  return basic_spline (new implementation (*impl));
}

/*
    �������������� ������
*/

void basic_spline::reserve (size_t keys_count)
{
  Buffer new_buffer;
  
  new_buffer.resize (keys_count * impl->key_size, false);
  
  if (impl->keys_count)
    impl->operations.initialize_keys (impl->keys_count, impl->buffer.data (), new_buffer.data ());

  impl->operations.destroy_keys (impl->keys_count, impl->buffer.data ());

  new_buffer.swap (impl->buffer);
}

size_t basic_spline::capacity () const
{
  return impl->buffer.capacity () / impl->key_size;
}

/*
    �������� ������
*/

void basic_spline::remove_key (size_t key_index)
{
  //?????
}

void basic_spline::clear ()
{
  impl->operations.destroy_keys (impl->keys_count, impl->buffer.data ());

  impl->keys_count = 0;
}

/*
    ���������� ������ ��� ��������� ������
*/

void basic_spline::sort ()
{
    //????
}

/*
    ��������� � ���������� ������
*/

const void* basic_spline::get_keys () const
{
  return impl->buffer.data ();
}

void* basic_spline::get_keys ()
{
  return impl->buffer.data ();
}

void basic_spline::update_key (size_t)
{
  impl->need_recompute = true;
}

/*
    ���������� �����
*/

size_t basic_spline::add_keys (size_t keys_count, const void* source_data)
{
  reserve (keys_count + impl->keys_count)

  impl->operations.initialize_keys (keys_count, source_data, impl->buffer.data ());

  size_t result = impl->keys_count;

  impl->need_recompute  = true;
  impl->keys_count     += keys_count;

  return result;
}

/*
    ������ �������� � ������������� �����
*/

void basic_spline::eval (const time_type& time, void* uninitialized_result) const
{
  //??????
}

/*
    �����
*/

void basic_spline::swap (basic_spline& s)
{
  stl::swap (impl, s.impl);
}
