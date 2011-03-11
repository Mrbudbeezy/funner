#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    �������� ���������� ��������� ������� �������
*/

struct EntityTextureStorage::Impl
{
  EntityImpl& owner; //�������� ��������� �������
  
///�����������
  Impl (EntityImpl& in_owner) : owner (in_owner) {}
};

/*
    ����������� / ����������
*/

EntityTextureStorage::EntityTextureStorage (EntityImpl& entity)
  : impl (new Impl (entity))
{
}

EntityTextureStorage::~EntityTextureStorage ()
{
}

/*
    ������ � ����� ����������
*/

void EntityTextureStorage::BeginUpdate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void EntityTextureStorage::EndUpdate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ������� ��� ���������
*/

size_t EntityTextureStorage::GetTextures (const MaterialPtr& material, TexturePtr textures [low_level::DEVICE_SAMPLER_SLOTS_COUNT])
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������� �������
*/

void EntityTextureStorage::Update (const FramePtr& frame)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
