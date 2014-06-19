#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    ���������
*/

const size_t DEFAULT_BATCH_VERTICES_COUNT = 8192;                             //������������� ���������� ������ � ������
const size_t DEFAULT_BATCH_INDICES_COUNT  = DEFAULT_BATCH_VERTICES_COUNT * 2; //������������� ���������� �������� � ������

/*
    �������� ���������� ��������� �������������
*/

namespace
{

/// ������ �������������
struct BatchGroup: public xtl::reference_counter
{
  stl::string wildcard;       //����� �����
  size_t      vertices_count; //���������� ������
  size_t      indices_count;  //���������� ��������

  BatchGroup (const char* in_wildcard, size_t in_vertices_count, size_t in_indices_count)
    : wildcard (in_wildcard)
    , vertices_count (in_vertices_count)
    , indices_count (in_indices_count)
  {
  }
};

typedef xtl::intrusive_ptr<BatchGroup>                                               BatchGroupPtr;
typedef stl::list<BatchGroupPtr>                                                     BatchGroupList;
typedef stl::hash_map<stl::hash_key<const char*>, render::manager::PrimitiveBuffers> BatchMap;

}

struct BatchingManager::Impl
{
  manager::RenderManager render_manager; //�������� ����������
  common::Log            log;            //����� ����������� ����������������
  BatchMap               batches;        //������
  BatchGroupList         batch_groups;   //������ �������

/// �����������
  Impl (const manager::RenderManager& in_render_manager, const common::Log& in_log)
    : render_manager (in_render_manager)
    , log (in_log)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

BatchingManager::BatchingManager (const render::manager::RenderManager& manager, const common::Log& log)
  : impl (new Impl (manager, log))
{
}

BatchingManager::~BatchingManager ()
{
}

/*
    ���������� ����� �������������
*/

void BatchingManager::AddBatchGroup (const char* group_wildcard, size_t vertices_count, size_t indices_count)
{
  try
  {
    if (!group_wildcard)
      throw xtl::make_null_argument_exception ("", "group_wildcard");

    for (BatchGroupList::iterator iter=impl->batch_groups.begin (), end=impl->batch_groups.end (); iter!=end; ++iter)
      if ((*iter)->wildcard == group_wildcard)
        throw xtl::make_argument_exception ("", "Batch group '%s' has been already defined", group_wildcard);

    BatchGroupPtr group (new BatchGroup (group_wildcard, vertices_count, indices_count), false);

    impl->batch_groups.push_front (group);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BatchingManager::AddBatchGroup");
    throw;
  }
}

void BatchingManager::RemoveAllBatchGroups ()
{
  impl->log.Printf ("Remove all batch groups");

  impl->batch_groups.clear ();
}

/*
    ��������� ������ ����������
*/

render::manager::PrimitiveBuffers BatchingManager::GetBatch (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    BatchMap::iterator iter = impl->batches.find (name);

    if (iter != impl->batches.end ())
      return iter->second;

    for (BatchGroupList::reverse_iterator iter=impl->batch_groups.rbegin (), end=impl->batch_groups.rend (); iter!=end; ++iter)
    {
      BatchGroup& group = **iter;

      if (!common::wcmatch (name, group.wildcard.c_str ()))
        continue;

      impl->log.Printf ("Create batch '%s' according to batch group '%s' (%u vertices, %u indices)", name, group.wildcard.c_str (), group.vertices_count, group.indices_count);

      render::manager::PrimitiveBuffers buffers = impl->render_manager.CreatePrimitiveBuffers ();

      buffers.ReserveDynamicBuffers (group.vertices_count, group.indices_count);

      impl->batches.insert_pair (name, buffers);

      return buffers;
    }

    throw xtl::format_operation_exception ("", "Can't create batch '%s' according to existing batch groups list", name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BatchingManager::GetBatch");
    throw;
  }
}

/*
    �������� ���� �������
*/

void BatchingManager::RemoveAllBatches ()
{
  impl->log.Printf ("Remove all batches");

  impl->batches.clear ();
}

/*
    �������
*/

void BatchingManager::Clear ()
{
  RemoveAllBatchGroups ();
  RemoveAllBatches ();
}

/*
    ������������ ������������
*/

void BatchingManager::ReloadConfiguration (const common::ParseNode& node)
{
  try
  {
    for (common::ParseNamesakeIterator iter=node.First ("batch"); iter; ++iter)
    {
      common::Parser::AttributeIterator attr_iter = make_attribute_iterator (*iter);

      const char* wildcard    = xtl::io::get<const char*> (attr_iter);
      size_t      verts_count = xtl::io::get<size_t> (attr_iter, DEFAULT_BATCH_VERTICES_COUNT),
                  inds_count  = xtl::io::get<size_t> (attr_iter, DEFAULT_BATCH_INDICES_COUNT);

      AddBatchGroup (wildcard, verts_count, inds_count);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BatchingManager::ReloadConfiguration");
    throw;
  }
}
