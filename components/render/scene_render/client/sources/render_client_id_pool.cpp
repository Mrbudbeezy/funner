#include "shared.h"

using namespace render::scene::client;

/*
    ���������
*/

const size_t MAX_DEALLOCATIONS_COUNT = 10000; //���������� ������������, ����� ������� ����������� ����������
const size_t MAX_ID_SET_SIZE         = 1000;  //������ ��������� ��������, ����� ������� ������������ ����������

/*
    �������� ���������� ���� ���������������
*/

typedef stl::vector<size_t> IdSet;

struct IdPool::Impl: public xtl::reference_counter
{
  size_t current_id;          //������� �������������
  IdSet  freed_ids;           //������������� ��������������
  size_t deallocations_count; //���������� ������������� ������������

/// �����������
  Impl () : current_id (1) {}
};

/*
    ������������ / ���������� / ������������
*/

IdPool::IdPool (size_t reserve_size)
  : impl (new Impl)
{
  impl->freed_ids.reserve (reserve_size);
}

IdPool::IdPool (const IdPool& pool)
  : impl (pool.impl)
{
  addref (impl);
}

IdPool::~IdPool ()
{
  release (impl);
}

IdPool& IdPool::operator = (const IdPool& pool)
{
  IdPool tmp (pool);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��������� �������������� / ������������ ��������������
*/

size_t IdPool::Allocate ()
{
  if (impl->freed_ids.empty ())
    return impl->current_id++;

  size_t id = impl->freed_ids.back ();

  impl->freed_ids.pop_back ();

  return id;
}

void IdPool::Deallocate (size_t id)
{
  if (id == impl->current_id - 1)
  {
    impl->current_id--;

    return;
  }

  impl->deallocations_count++;

  impl->freed_ids.push_back (id);

  if (impl->deallocations_count >= MAX_DEALLOCATIONS_COUNT && impl->freed_ids.size () >= MAX_ID_SET_SIZE)
  {
      //����������

    stl::sort (impl->freed_ids.begin (), impl->freed_ids.end ());

    size_t count = 0;

printf ("?? %u/%u\n", impl->freed_ids.back (), impl->current_id);

    while (!impl->freed_ids.empty () && impl->freed_ids.back () == impl->current_id - 1)
      count++;
    
    if (count)
      impl->freed_ids.erase (impl->freed_ids.end () - count, impl->freed_ids.end ());
printf ("!!!\n");
    impl->deallocations_count = 0;
  }
}
