#include "shared.h"

using namespace render::manager;

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
  FrameTime time_delay;    //�������� �� ������� �� �������� ������� �� ����
  FrameId   frame_delay;   //�������� �� ���������� ������ �� �������� ������� �� ����
  FrameId   current_frame; //������� ����
  FrameTime current_time;  //������� �����
  
  Impl ()
    : time_delay (DEFAULT_TIME_DELAY)
    , frame_delay (DEFAULT_FRAME_DELAY)
    , current_frame (1)
    , current_time ()
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

void CacheManager::SetTimeDelay (FrameTime milliseconds)
{
  impl->time_delay = milliseconds;
}

void CacheManager::SetFrameDelay (FrameId frames_count)
{
  impl->frame_delay = frames_count;
}

FrameTime CacheManager::TimeDelay ()
{
  return impl->time_delay;
}

FrameId CacheManager::FrameDelay ()
{
  return impl->frame_delay;
}

/*
    ������� ���� / ������� �����
*/

FrameTime CacheManager::CurrentTime ()
{
  return impl->current_time;
}

FrameId CacheManager::CurrentFrame ()
{
  return impl->current_frame - 1;
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
  : manager (in_manager)
{  
  if (manager)
  {
    current_frame = manager->impl->current_frame;
    current_time  = manager->impl->current_time;
  
    manager->AddCache (*this);
  }
}

Cache::~Cache ()
{
  if (manager)
    manager->RemoveCache (*this);
}

/*
    ��������� �����������
*/

FrameTime Cache::TimeDelay ()
{
  return manager ? manager->TimeDelay () : 0;
}

FrameId Cache::FrameDelay ()
{
  return manager ? manager->FrameDelay () : 0;
}
