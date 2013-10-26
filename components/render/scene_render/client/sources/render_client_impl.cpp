#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ������� �����
*/

typedef xtl::array<render::scene::interchange::object_id_t, ObjectType_Num> IdArray;

struct ClientImpl::Impl
{
  client::Context*                    context;            //�������� (�������� ������ ����� ����� Context ())
  IdArray                             id_pool;            //��� ���������������
  interchange::PropertyMapAutoWriter  properties_writer;  //������������� ������� (������ �� ������)
  interchange::PropertyMapReader      properties_reader;  //������������� ������� (������ � �������)
  stl::auto_ptr<client::SceneManager> scene_manager;      //�������� ����

/// �����������
  Impl ()
    : context ()
  {
    id_pool.assign (0);
  }

/// ��������� ���������
  client::Context& Context ()
  {
    if (!context)
      throw xtl::format_operation_exception ("render::scene::client::ClientImpl::Impl::Context", "Context is null");

    return *context;
  }
};

/*
    ����������� / ����������
*/

ClientImpl::ClientImpl ()
  : impl (new Impl)
{
}

ClientImpl::~ClientImpl ()
{
}

/*
    ���������� � ����������
*/

void ClientImpl::SetContext (Context* context)
{
  try
  {
    impl->context = context;

    try
    {
      impl->scene_manager.reset ();

      if (context)
      {
        impl->scene_manager.reset (new client::SceneManager (*this, *context));
      }
    }
    catch (...)
    {
      impl->context = 0;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::SetContext");
    throw;
  }
}

/*
    ���������� ������������� � ��������
*/

void ClientImpl::Synchronize ()
{
  try
  {
    impl->properties_writer.Write (impl->Context ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::Synchronize");
    throw;
  }
}

/*
    ��������� ���������������
*/

render::scene::interchange::object_id_t ClientImpl::AllocateId (ObjectType type)
{
  static const char* METHOD_NAME = "render::scene::client::ClientImpl::AllocateId";

  if (type < 0 || type >= ObjectType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  render::scene::interchange::object_id_t& id = impl->id_pool [type];

  if (!(id + 1))
    throw xtl::format_operation_exception (METHOD_NAME, "ID pool is full for object type %d", type);

  return ++id;
}

void ClientImpl::DeallocateId (ObjectType type, render::scene::interchange::object_id_t id)
{
  if (type < 0 || type >= ObjectType_Num)
    return;
}

/*
    ������������� �������
*/

PropertyMapSynchronizer ClientImpl::CreateSynchronizer (const common::PropertyMap& properties)
{
  try
  {
    return impl->properties_writer.CreateSynchronizer (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::CreateSynchronizer");
    throw;
  }
}

common::PropertyMap ClientImpl::GetServerProperties (object_id_t id)
{
  try
  {
    return impl->properties_reader.GetProperties (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::GetServerProperties");
    throw;
  }
}

/*
    ����������� ������� �������
*/

void ClientImpl::RemovePropertyMap (object_id_t id)
{
  impl->properties_reader.RemoveProperties (id);
}

void ClientImpl::RemovePropertyLayout (object_id_t id)
{
  impl->properties_reader.RemoveLayout (id);
}

void ClientImpl::UpdatePropertyMap (render::scene::interchange::InputStream& stream)
{
  try
  {
    impl->properties_reader.Read (stream);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::ClientImpl::UpdatePropertyMap");
    throw;
  }
}

/*
    �������� �����
*/

SceneManager& ClientImpl::SceneManager ()
{
  if (!impl->context)
    throw xtl::format_operation_exception ("render::scene::client::ClientImpl::SceneManager", "Can't return SceneManger: context is null");

  return *impl->scene_manager;
}
