#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

/*
    ���������
*/

namespace
{

const size_t PRIMITIVE_ARRAY_RESERVE_SIZE = 4096; //������������� ���������� ����������

}

/*
    ����������� / ����������
*/

Frame::Frame ()
{
  alpha_reference = 0;

  primitives.reserve (PRIMITIVE_ARRAY_RESERVE_SIZE);
  
  log.Printf ("Create frame2d (id=%u)", Id ());
}

Frame::~Frame ()
{
  try
  {
    log.Printf ("Destroy frame2d (id=%u)", Id ());
  }
  catch (...)
  {
    //���������� ����������
  }
}

/*
    ������� ���� / ������� ��������������
*/

void Frame::SetViewPoint (const math::vec3f& point)
{
  view_point = point;
}

void Frame::SetProjection (const math::mat4f& tm)
{
  proj_tm = tm;
}

void Frame::GetViewPoint (math::vec3f& point)
{
  point = view_point;
}

void Frame::GetProjection (math::mat4f& tm)
{
  tm = proj_tm;
}

/*
    ��������� ��������� ��� ������ �����-�����
*/

void Frame::SetAlphaReference (float ref)
{
  alpha_reference = ref;
}

float Frame::GetAlphaReference ()
{
  return alpha_reference;
}

/*
    ���������
*/

//���������� ���������� � �����
size_t Frame::PrimitivesCount ()
{
  return primitives.size ();
}

//���������� ���������
void Frame::AddPrimitive (IPrimitive* in_primitive)
{
  static const char* METHOD_NAME = "render::mid_level::debug::renderer2d::Frame::AddPrimitive";

  if (!in_primitive)
    throw xtl::make_null_argument_exception (METHOD_NAME, "primitive");
    
  Primitive* casted_primitive = dynamic_cast<Primitive*> (in_primitive);
  
  if (!casted_primitive)
    throw xtl::make_argument_exception (METHOD_NAME, "primitive", typeid (in_primitive).name (),
      "Primitive type incompatible with render::mid_level::debug::renderer2d::Primitive");

  primitives.push_back (casted_primitive);
}

//������� �����
void Frame::Clear ()
{
  primitives.clear ();
}

/*
    ���������� ������������
*/

void Frame::DrawCore ()
{
    //��������� ������� ������

  BasicFrame::BindViewport ();

  /*
    � ���������� ���������� � ���� ������ ������ ���� ����������� ���������� �������� �� �������� �� �����������
  */

  log.Printf ("Projection matrix=[[%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f]]",
    proj_tm [0][0], proj_tm [0][1], proj_tm [0][2], proj_tm [0][3],
    proj_tm [1][0], proj_tm [1][1], proj_tm [1][2], proj_tm [1][3],
    proj_tm [2][0], proj_tm [2][1], proj_tm [2][2], proj_tm [2][3],
    proj_tm [3][0], proj_tm [3][1], proj_tm [3][2], proj_tm [3][3]);

  log.Printf ("View point=[%.2f %.2f %.2f]", view_point.x, view_point.y, view_point.z);
  
  for (PrimitiveArray::iterator iter=primitives.begin (), end=primitives.end (); iter!=end; ++iter)
  {
    Primitive& primitive = **iter;
   
    log.Printf ("Draw primitive (id=%u)", primitive.Id ());
    
    if (primitive.GetTexture ()) log.Printf ("  texture_id=%u", primitive.GetTextureId ());
    else                         log.Printf ("  texture_id=none");
    
    switch (primitive.GetBlendMode ())
    {
      case BlendMode_None:
        log.Printf ("  blend=none");
        break;
      case BlendMode_Translucent:
        log.Printf ("  blend=translucent");
        break;
      case BlendMode_Mask:
        log.Printf ("  blend=mask");
        break;
      case BlendMode_Additive:
        log.Printf ("  blend=additive");
        break;
      case BlendMode_AlphaClamp:
        log.Printf ("  blend=alpha_clamp");
        break;
      default:
        break;
    }
    
    math::mat4f object_tm;
    
    primitive.GetTransform (object_tm);

    log.Printf ("  transform=[[%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f] [%.2f %.2f %.2f]]",
      object_tm [0][0], object_tm [0][1], object_tm [0][2], object_tm [0][3],
      object_tm [1][0], object_tm [1][1], object_tm [1][2], object_tm [1][3],
      object_tm [2][0], object_tm [2][1], object_tm [2][2], object_tm [2][3],
      object_tm [3][0], object_tm [3][1], object_tm [3][2], object_tm [3][3]);
      
    size_t sprites_count=primitive.GetSpritesCount ();

    log.Printf ("  sprites (count=%u):", sprites_count);

    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite s;
      
      primitive.GetSprite (i, s);
           
      log.Printf ("    sprite #%u: position=[%.2f %.2f %.2f] size=[%.2f %.2f] color=[%.2f %.2f %.2f %.2f] tex_offset=[%.2f %.2f] tex_size=[%.2f %.2f]",
        i, s.position.x, s.position.y, s.position.z, s.size.x, s.size.y, s.color.x, s.color.y, s.color.z, s.color.w, 
        s.tex_offset.x, s.tex_offset.y, s.tex_size.x, s.tex_size.y);
    }
  }
}
