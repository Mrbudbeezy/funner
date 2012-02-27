#include "shared.h"

using namespace scene_graph;

namespace
{

const float  DEFAULT_CORNER_SHADOW_OFFSET              = 15.f;
const float  DEFAULT_GRID_SIZE                         = 100.f;
const float  DEFAULT_SHADOW_WIDTH                      = 0.25f;
const float  DEFAULT_SHADOW_DENSITY                    = 0.4f;
const float  DEFAULT_SHADOW_GROW_POWER                 = 0.25f;
const float  DEFAULT_OPPOSITE_CORNER_SHADOW_GROW_POWER = 15.f;
const size_t DEFAULT_FIND_BEST_CURL_STEPS              = 1000;
const float  DEFAULT_BINDING_MISMATCH_WEIGHT           = 10.f;

}

/*
    �������� ���������� �������������� ��������
*/

struct PageCurl::Impl
{
  PageCurlMode   mode;                               //����� �������
  stl::string    materials [PageCurlPageType_Num];   //���������
  stl::string    shadow_material;                    //�������� ����
  math::vec2f    size;                               //������ ������ �������
  PageCurlCorner corner;                             //����
  math::vec2f    corner_position;                    //������� ����
  float          curl_radius;                        //������ ������
  float          minimum_curl_radius;                //����������� ������ ������
  math::vec2ui   grid_size;                          //���������� ��������� �����
  math::vec4f    color;                              //���� ��������
  float          corner_shadow_offset;               //�������� ���� �� ����
  float          shadow_width;                       //������ ����
  float          shadow_density;                     //��������� ����
  float          shadow_grow_power;                  //������� ���������� ���� ��� ���������� ������
  float          opposite_corner_shadow_grow_power;  //������� ���������� ���� ��� ���������� ������ ��� �������� ��������������� ���� ��������
  size_t         find_best_curl_steps;               //���������� �������� ������ ��������� ������� ������
  float          binding_mismatch_weight;            //��� ���������� ������� ����� �������� ��� ������ ��������� ������� ������

  Impl ()
    : mode (PageCurlMode_SinglePage)
    , size (1.f)
    , corner (PageCurlCorner_LeftTop)
    , curl_radius (0.f)
    , minimum_curl_radius (0.f)
    , grid_size (DEFAULT_GRID_SIZE)
    , color (1.f)
    , corner_shadow_offset (DEFAULT_CORNER_SHADOW_OFFSET)
    , shadow_width (DEFAULT_SHADOW_WIDTH)
    , shadow_density (DEFAULT_SHADOW_DENSITY)
    , shadow_grow_power (DEFAULT_SHADOW_GROW_POWER)
    , opposite_corner_shadow_grow_power (DEFAULT_OPPOSITE_CORNER_SHADOW_GROW_POWER)
    , find_best_curl_steps (DEFAULT_FIND_BEST_CURL_STEPS)
    , binding_mismatch_weight (DEFAULT_BINDING_MISMATCH_WEIGHT)
    {}
};

/*
    ����������� / ����������
*/

PageCurl::PageCurl ()
  : impl (new Impl)
{
  UpdateBoundsNotify ();
}

PageCurl::~PageCurl ()
{
  delete impl;
}

/*
    ��������
*/

PageCurl::Pointer PageCurl::Create ()
{
  return Pointer (new PageCurl, false);
}

/*
   ��������� ������ �������
*/

void PageCurl::SetMode (PageCurlMode mode)
{
  switch (mode)
  {
    case PageCurlMode_SinglePage:
    case PageCurlMode_DoublePageSingleMaterial:
    case PageCurlMode_DoublePageDoubleMaterial:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::PageCurl::SetMode", "mode", mode);
  }

  impl->mode = mode;

  UpdateNotify ();
}

PageCurlMode PageCurl::Mode () const
{
  return impl->mode;
}

/*
   ��������� ����������
*/

void PageCurl::SetPageMaterial (PageCurlPageType type, const char* name)
{
  static const char* METHOD_NAME = "scene_graph::PageCurl::SetPageMaterial";

  if (type >= PageCurlPageType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  impl->materials [type] = name;

  UpdateNotify ();
}

void PageCurl::SetShadowMaterial (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::PageCurl::SetShadowMaterial", "name");

  impl->shadow_material = name;

  UpdateNotify ();
}

const char* PageCurl::ShadowMaterial () const
{
  return impl->shadow_material.c_str ();
}

const char* PageCurl::PageMaterial (PageCurlPageType type) const
{
  if (type >= PageCurlPageType_Num)
    throw xtl::make_argument_exception ("scene_graph::PageCurl::PageMaterial", "type", type);

  return impl->materials [type].c_str ();
}

/*
   ��������� ������� ������� �������
*/

void PageCurl::SetSize (const math::vec2f& size)
{
  impl->size = size;

  UpdateNotify ();
  UpdateBoundsNotify ();
}

void PageCurl::SetSize (float width, float height)
{
  SetSize (math::vec2f (width, height));
}

const math::vec2f& PageCurl::Size () const
{
  return impl->size;
}

/*
   ��������� ���� ��������
*/

void PageCurl::SetCurlCorner (PageCurlCorner corner)
{
  switch (corner)
  {
    case PageCurlCorner_LeftTop:
    case PageCurlCorner_LeftBottom:
    case PageCurlCorner_RightTop:
    case PageCurlCorner_RightBottom:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::CurlCurl::SetPageCorner", "corner", corner);
  }

  impl->corner = corner;

  UpdateNotify ();
}

PageCurlCorner PageCurl::CurlCorner () const
{
  return impl->corner;
}

void PageCurl::SetCornerPosition (const math::vec2f& position)
{
  impl->corner_position = position;

  UpdateNotify ();
}

void PageCurl::SetCornerPosition (float x, float y)
{
  SetCornerPosition (math::vec2f (x, y));
}

const math::vec2f& PageCurl::CornerPosition () const
{
  return impl->corner_position;
}

/*
   ��������� ������
*/

void PageCurl::SetCurlRadius (float curl_radius)
{
  impl->curl_radius = curl_radius;

  UpdateNotify ();
  UpdateBoundsNotify ();
}

float PageCurl::CurlRadius () const
{
  return impl->curl_radius;
}

void PageCurl::SetMinimumCurlRadius (float minimum_curl_radius)
{
  impl->minimum_curl_radius = minimum_curl_radius;

  UpdateNotify ();
}

float PageCurl::MinimumCurlRadius () const
{
  return impl->minimum_curl_radius;
}

/*
   ������������ ���������
*/

void PageCurl::SetGridSize (const math::vec2ui& size)
{
  if (size.x < 2 || size.y < 2)
    throw xtl::format_operation_exception ("scene_graph::PageCurl::SetGridSize", "Invalid size %ux%u, has to be greater than 2 for both axises", size.x, size.y);

  impl->grid_size = size;

  UpdateNotify ();
}

void PageCurl::SetGridSize (size_t x, size_t y)
{
  SetGridSize (math::vec2ui (x, y));
}

const math::vec2ui& PageCurl::GridSize () const
{
  return impl->grid_size;
}

void PageCurl::SetPageColor (const math::vec4f& color)
{
  impl->color = color;

  UpdateNotify ();
}

void PageCurl::SetPageColor (float red, float green, float blue, float alpha)
{
  SetPageColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& PageCurl::PageColor () const
{
  return impl->color;
}

void PageCurl::SetCornerShadowOffset (float shadow_offset)
{
  impl->corner_shadow_offset = shadow_offset;

  UpdateNotify ();
}

float PageCurl::CornerShadowOffset () const
{
  return impl->corner_shadow_offset;
}

void PageCurl::SetShadowWidth (float width)
{
  impl->shadow_width = width;

  UpdateNotify ();
}

float PageCurl::ShadowWidth () const
{
  return impl->shadow_width;
}

void PageCurl::SetShadowDensity (float density)
{
  impl->shadow_density = density;

  UpdateNotify ();
}

float PageCurl::ShadowDensity () const
{
  return impl->shadow_density;
}

void PageCurl::SetShadowGrowPower (float power)
{
  impl->shadow_grow_power = power;

  UpdateNotify ();
}

float PageCurl::ShadowGrowPower () const
{
  return impl->shadow_grow_power;
}

void PageCurl::SetOppositeCornerShadowGrowPower (float power)
{
  impl->opposite_corner_shadow_grow_power = power;

  UpdateNotify ();
}

float PageCurl::OppositeCornerShadowGrowPower () const
{
  return impl->opposite_corner_shadow_grow_power;
}

void PageCurl::SetFindBestCurlSteps (size_t count)
{
  impl->find_best_curl_steps = count;

  UpdateNotify ();
}

size_t PageCurl::FindBestCurlSteps () const
{
  return impl->find_best_curl_steps;
}

void PageCurl::SetBindingMismatchWeight (float weight)
{
  impl->binding_mismatch_weight = weight;

  UpdateNotify ();
}

float PageCurl::BindingMismatchWeight () const
{
  return impl->binding_mismatch_weight;
}

/*
   ������� ��������������� ������
*/

void PageCurl::UpdateBoundsCore ()
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-impl->size.x / 2, -impl->size.y / 2, -impl->curl_radius, impl->size.x / 2, impl->size.y / 2, 0));
}

/*
    �����, ���������� ��� ��������� ������� �������
*/

void PageCurl::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);  
}
