#ifndef RENDER_SCENE_INTERCHANGE_TYPES_HEADER
#define RENDER_SCENE_INTERCHANGE_TYPES_HEADER

#include <math/angle.h>
#include <math/vector.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef   signed long long int64;
typedef unsigned long long uint64;
typedef   signed int       int32;
typedef unsigned int       uint32;
typedef   signed short     int16;
typedef unsigned short     uint16;
typedef   signed char      int8;
typedef unsigned char      uint8;
typedef float              float32;
typedef int8               bool8;

typedef uint32             command_id_t;
typedef uint64             object_id_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Command
{
  command_id_t command_id;   //������������� �������
  uint32       command_size; //������ �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeType
{
  NodeType_Node,        //����
  NodeType_Entity,      //��������
  NodeType_Scissor,     //������� ���������
  NodeType_VisualModel, //������������ ������
  NodeType_SpriteModel, //�������
  NodeType_StaticMesh,  //����������� ���
  NodeType_PointLight,  //�������� �������� �����
  NodeType_SpotLight,   //�������������� �������� �����
  NodeType_DirectLight, //������������ �������� �����
  NodeType_PageCurl,    //�������� �����

  NodeType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightParams
{
  math::vec3f  color;       //����
  float        intensity;   //�������������
  math::vec3f  attenuation; //���������
  float        range;       //���������� ��������
  float        exponent;    //���������� ����������� ����� (��� spot-light)
  math::anglef angle;       //���� �������� (��� spot-light)
  float        radius;      //������ �������� (��� direct-light)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PageCurlParams
{
  math::vec4f  page_color;                    //���� ��������
  math::vec2f  size;                          //������ ������� �������
  math::vec2f  curl_point;                    //����� ��������������
  math::vec2f  curl_point_position;           //������� ��������� ����� ��������������
  math::vec2ui grid_size;                     //����������� ��������� �����
  size_t       find_best_curl_steps;          //���������� �������� ������ ��������� ������� ������
  int          mode;                          //����� (PageCurlMode)
  float        curl_radius;                   //��������� ������
  float        minimum_curl_radius;           //��������� ������
  float        corner_shadow_offset;          //�������� ���� �� ����
  float        shadow_width;                  //������ ����
  float        shadow_log_base;               //��������� ��������� ��������� ����
  float        shadow_min_log_value;          //����������� �������� ���� ��� ����������������
  float        binding_mismatch_weight;       //��� ���������� ������� ����� �������� ��� ������ ��������� ������� ������
  float        rigid_page_perspective_factor; //����������� ���������� ���� ������� �������� ��� ��������� �����������
  bool         is_rigid_page;                 //�������� �� �������� �������
};

}

}

}

#endif
