#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

View::View (const ContextManager& context_manager, ITexture* in_texture, const ViewDesc& in_desc)
  : ContextObject (context_manager),
    texture (in_texture),
    type (in_texture ? typeid (*in_texture) : typeid (void))
{
  static const char* METHOD_NAME = "render::low_level::opengl::View::View";

    //�������� ������������ �������� � ������������� � ������� ����������

  if (!texture)
    RaiseNullArgument (METHOD_NAME, "texture");

  cast_object<ContextObject> (GetContextManager (), in_texture, METHOD_NAME, "texture");

     //�������� ������������ �����������

  TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  if (in_desc.layer >= texture_desc.layers)
    RaiseOutOfRange (METHOD_NAME, "desc.layer", in_desc.layer, texture_desc.layers);
    
    //����������� ������ ��������
    
  bind_flags = texture_desc.bind_flags;
    
    //��������� �����������

  desc = in_desc;
}

/*
    ��������� �����������
*/

void View::GetDesc (ViewDesc& out_desc)
{
  out_desc = desc;
}

/*
    ��������� ������� ��������
*/

ITexture* View::GetTexture ()
{
  return texture.get ();
}
