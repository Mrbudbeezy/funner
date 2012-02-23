#include "shared.h"

using namespace scene_graph;
using detail::ISceneAttachment;

/*
    �������� ���������� ���� ������� �����
*/

namespace
{

typedef xtl::shared_ptr<ISceneAttachment> AttachmentPtr;

struct CacheValue: public xtl::reference_counter
{
  AttachmentPtr     attachment;
  common::ParseNode decl;

  CacheValue (const AttachmentPtr& in_attachment, const common::ParseNode& in_decl) : attachment (in_attachment), decl (in_decl) {}
};

typedef xtl::intrusive_ptr<CacheValue>       CacheValuePtr;
typedef stl::hash_map<size_t, CacheValuePtr> CacheMap;

}

struct SceneParserCache::Impl: public xtl::reference_counter
{
  CacheMap cache; //��� ��������
};

/*
    ������������ / ���������� / ������������
*/

SceneParserCache::SceneParserCache ()
  : impl (new Impl)
{
}

SceneParserCache::SceneParserCache (const SceneParserCache& cache)
  : impl (cache.impl)
{
  addref (impl);
}

SceneParserCache::~SceneParserCache ()
{
  release (impl);
}

SceneParserCache& SceneParserCache::operator = (const SceneParserCache& cache)
{
  SceneParserCache (cache).Swap (*this);
  
  return *this;
}

/*
    ��������� ��������
*/

void SceneParserCache::SetValueCore (const common::ParseNode& decl, detail::ISceneAttachment* in_attachment)
{
  if (!in_attachment)
    throw xtl::make_null_argument_exception ("scene_graph::SceneParserCache::SetValue", "attachment");

  AttachmentPtr attachment (in_attachment);
  
  CacheMap::iterator iter = impl->cache.find (decl.Id ());
  
  if (iter != impl->cache.end ())
  {
    iter->second->attachment = attachment;
    
    return;
  }
  
  CacheValuePtr value (new CacheValue (attachment, decl), false);
  
  impl->cache.insert_pair (decl.Id (), value);
}

/*
    �������� ��������
*/

void SceneParserCache::ResetValue (const common::ParseNode& decl)
{
  impl->cache.erase (decl.Id ());
}

/*
    ����� ��������
*/

ISceneAttachment* SceneParserCache::FindValueCore (const common::ParseNode& decl) const
{
  CacheMap::iterator iter = impl->cache.find (decl.Id ());
  
  if (iter == impl->cache.end ())
    return 0;
    
  return &*iter->second->attachment;
}

/*
    �������
*/

void SceneParserCache::Clear ()
{
  impl->cache.clear ();
}

/*
    �����
*/

void SceneParserCache::Swap (SceneParserCache& cache)
{
  stl::swap (impl, cache.impl);
}

namespace scene_graph
{

void swap (SceneParserCache& cache1, SceneParserCache& cache2)
{
  cache1.Swap (cache2);
}

}
