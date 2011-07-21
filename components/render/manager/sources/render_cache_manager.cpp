#include "shared.h"

using namespace render;

/*
    ���������
*/

namespace
{

const size_t DEFAULT_TIME_DELAY  = 3600 * 1000; //�������� �� ������� �� �������� ������� �� ���� �� ���������
const size_t DEFAULT_FRAME_DELAY = ~0u - 1;     //�������� �� ���������� ������ �� �������� ������� �� ���� �� ���������

}

/*
===================================================================================================
    CacheManager
===================================================================================================
*/

/*
    �������� ���������� ����
*/

typedef stl::list<Cache*> CacheList;

struct CacheManager::Impl
{
  CacheList caches;        //������ �����
  size_t    time_delay;    //�������� �� ������� �� �������� ������� �� ����
  size_t    frame_delay;   //�������� �� ���������� ������ �� �������� ������� �� ����
  size_t    current_frame; //������� ����
  size_t    current_time;  //������� �����
  
  Impl ()
    : time_delay (DEFAULT_TIME_DELAY)
    , frame_delay (DEFAULT_FRAME_DELAY)
    , current_frame (0)
    , current_time (0)
  {
  }
};

/*
    ����������� / ����������
*/

CacheManager::CacheManager ()
  : impl (new Impl)
{
}

CacheManager::~CacheManager ()
{
}

/*
    ��������� �����������
*/

void CacheManager::SetTimeDelay (size_t milliseconds)
{
  impl->time_delay = milliseconds;
}

void CacheManager::SetFrameDelay (size_t frames_count)
{
  impl->frame_delay = frames_count;
}

size_t CacheManager::TimeDelay ()
{
  return impl->time_delay;
}

size_t CacheManager::FrameDelay ()
{
  return impl->frame_delay;
}

/*
    ���������� �������� �������
*/

void CacheManager::UpdateMarkers ()
{
  impl->current_time = common::milliseconds ();

  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
  {
    (*iter)->current_frame = impl->current_frame;
    (*iter)->current_time  = impl->current_time;
  }
  
  impl->current_frame++;
}

/*
    ����� �����
*/

void CacheManager::FlushCaches ()
{
  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
    (*iter)->FlushCache ();
}

/*
    ���������� / �������� ����
*/

void CacheManager::AddCache (Cache& cache)
{
  RemoveCache (cache);
  
  impl->caches.push_back (&cache);
}

void CacheManager::RemoveCache (Cache& cache)
{
  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
    if (*iter == &cache)
    {
      impl->caches.erase (iter);
      return;
    }
}

/*
===================================================================================================
    Cache
===================================================================================================
*/

/*
    ����������� / ����������
*/

Cache::Cache (const CacheManagerPtr& in_manager)
{
  if (!in_manager)
    throw xtl::make_null_argument_exception ("render::Cache::Cache", "manager");
    
  manager       = in_manager;
  current_frame = manager->impl->current_frame;
  current_time  = manager->impl->current_time;
  
  manager->AddCache (*this);
}

Cache::~Cache ()
{
  manager->RemoveCache (*this);
}

/*
    ��������� �����������
*/

size_t Cache::TimeDelay ()
{
  return manager->TimeDelay ();
}

size_t Cache::FrameDelay ()
{
  return manager->FrameDelay ();
}
