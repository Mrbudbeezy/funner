#include "shared.h"

using namespace render::scene::client;

/*
    �������� ���������� ���� ����������
*/

struct RenderTargetImpl::Impl
{
  stl::string          name;       //��� ����
  ConnectionPtr        connection; //����������
  scene_graph::Screen* screen;     //�����  

/// �����������
  Impl (const ConnectionPtr& in_connection, const char* render_target_name)
    : connection (in_connection)
    , screen ()    
  {
    static const char* METHOD_NAME = "render::scene::client::RenderTargetImpl::RenderTargetImpl";

    if (!connection)
      throw xtl::make_null_argument_exception (METHOD_NAME, "connection");

    if (!render_target_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "render_target_name");

    name = render_target_name;
  }
};

/*
  ����������� / ����������
*/

RenderTargetImpl::RenderTargetImpl (const ConnectionPtr& connection, const char* render_target_name)
  : impl (new Impl (connection, render_target_name))
{
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    ����� (�������� �������� - weak-ref)
*/

void RenderTargetImpl::SetScreen (scene_graph::Screen* screen)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

scene_graph::Screen* RenderTargetImpl::Screen ()
{
  return impl->screen;
}

/*
    ����������
*/

void RenderTargetImpl::Update ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ ����������� (screen-shot)
*/

void RenderTargetImpl::CaptureImage (media::Image&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
