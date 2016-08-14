#include "shared.h"

using namespace scene_graph;
using detail::ISceneAttachment;

/*
    �������� ���������� ���� ������� �����
*/

namespace
{

typedef xtl::shared_ptr<ISceneAttachment> AttachmentPtr;

struct CacheValue: public xtl::reference_counter, public xtl::instance_counter<CacheValue>
{
  AttachmentPtr         attachment;
  const std::type_info* type;
  common::ParseNode     decl;

  CacheValue (const AttachmentPtr& in_attachment, const common::ParseNode& in_decl)
    : attachment (in_attachment)
    , type (&in_attachment->ValueType ())
    , decl (in_decl) 
  {
  }
};

typedef xtl::intrusive_ptr<CacheValue>            CacheValuePtr;
typedef stl::hash_multimap<size_t, CacheValuePtr> CacheMap;

}

struct SceneParserCache::Impl: public xtl::reference_counter, public xtl::instance_counter<SceneParserCache>
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
  
  const std::type_info* type = &attachment->ValueType ();
  
  stl::pair<CacheMap::iterator, CacheMap::iterator> range = impl->cache.equal_range (decl.Id ());
  
  for (;range.first != range.second; ++range.first)
    if (range.first->second->type == type)
    {
      range.first->second->attachment = attachment;

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

ISceneAttachment* SceneParserCache::FindValueCore (const common::ParseNode& decl, const std::type_info& type) const
{
  stl::pair<CacheMap::iterator, CacheMap::iterator> range = impl->cache.equal_range (decl.Id ());
  
  for (;range.first != range.second; ++range.first)
    if (range.first->second->type == &type)
      return range.first->second->attachment.get ();

  return 0;
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
