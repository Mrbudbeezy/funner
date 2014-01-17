#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    BatchStateBlock
===================================================================================================
*/

/*
    �������� ���������� ����� ��������� ������
*/

struct BatchStateBlock::Impl
{
};

/*
    ����������� / ����������
*/

BatchStateBlock::BatchStateBlock (BatchingManager& batching_manager, MaterialImpl& material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

BatchStateBlock::~BatchStateBlock ()
{
}

/*
    ���� ���������
*/

LowLevelStateBlockPtr BatchStateBlock::StateBlock ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������� ������������
*/

void BatchStateBlock::UpdateCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchStateBlock::ResetCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    BatchingManager
===================================================================================================
*/

/*
    �������� ���������� ��������� �������������
*/

struct BatchingManager::Impl
{
  DeviceManagerPtr                      device_manager;       //�������� ���������� ���������
  size_t                                lines_capacity;       //������������� ���������� �����
  size_t                                sprites_capacity;     //������������� ���������� ��������
  render::manager::DynamicVertexBuffer  dynamic_vb;           //������������ ��������� �����
  render::manager::DynamicIndexBuffer   dynamic_ib;           //������������ ��������� �����

  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , lines_capacity ()
    , sprites_capacity ()
    , dynamic_vb (render::low_level::UsageMode_Stream, render::low_level::BindFlag_VertexBuffer)
    , dynamic_ib (render::low_level::UsageMode_Stream, render::low_level::BindFlag_IndexBuffer)
  {
  }

  void ReserveDynamicPrimitives (size_t sprites_count, size_t lines_count)
  {
    try
    {
      size_t vertices_count = 4 * sprites_count + 2 * lines_count,
             indices_count  = 6 * sprites_count + 2 * lines_count;

      dynamic_vb.Reserve (vertices_count);
      dynamic_ib.Reserve (indices_count);

      dynamic_vb.SyncBuffers (device_manager->Device ());
      dynamic_ib.SyncBuffers (device_manager->Device ());

      sprites_capacity = sprites_count;
      lines_capacity   = lines_capacity;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::BatchingManager::ReserveDynamicPrimitives");
      throw;
    }
  }
};

/*
    ����������� / ����������
*/

BatchingManager::BatchingManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

BatchingManager::~BatchingManager ()
{
}

/*
    ������ �� �������� ����������
*/

render::manager::DeviceManager& BatchingManager::DeviceManager ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �������������� ��������������� ����������
*/

void BatchingManager::ReserveDynamicPrimitives (size_t sprites_count, size_t lines_count)
{
  try
  {
    impl->ReserveDynamicPrimitives (sprites_count, lines_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::manager::BatchingManager::ReserveDynamicPrimitives");
    throw;
  }
}

size_t BatchingManager::LinesCapacity ()
{
  return impl->lines_capacity;
}

size_t BatchingManager::SpritesCapacity ()
{
  return impl->sprites_capacity;
}

/*
    ������������ ������
*/

render::manager::DynamicVertexBuffer& BatchingManager::DynamicVertexBuffer ()
{
  return impl->dynamic_vb;
}

render::manager::DynamicIndexBuffer& BatchingManager::DynamicIndexBuffer ()
{
  return impl->dynamic_ib;
}

/*
    ��������� ������ � ��������
*/

size_t BatchingManager::AllocateDynamicVertices (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t BatchingManager::AllocateDynamicIndices (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchingManager::DeallocateDynamicVertices (size_t first, size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchingManager::DeallocateDynamicIndices (size_t first, size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ����� ��������� �� ���������
*/

BatchStateBlockPtr BatchingManager::GetStateBlock (MaterialImpl* material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
