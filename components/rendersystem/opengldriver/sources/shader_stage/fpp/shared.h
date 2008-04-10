#ifndef RENDER_GL_DRIVER_FPP_SHADER_MANAGER_SHARED_HEADER
#define RENDER_GL_DRIVER_FPP_SHADER_MANAGER_SHARED_HEADER

#include "../shared.h"

#include <stl/string>
#include <stl/hash_map>

#include <common/parser.h>
#include <common/strlib.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t FPP_MAX_LIGHTS_COUNT = 8; //������������ ���������� ���������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightType
{
  LightType_Point,  //�������� �������� �����
  LightType_Remote, //�������� ������������ �������� �����
  LightType_Spot,   //�������������� ������������ �������� �����
  
  LightType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightDesc
{
  LightType type;                  //��� ��������� �����
  int       enable;                //��������� ��������� �����: �������/��������
  float     position [3];          //��������� ��������� �����
  float     direction [3];         //����������� ��������� �����
  Color4f   ambient_color;         //���� ����������� ������������ �����
  Color4f   diffuse_color;         //���� ������������ ������������ �����
  Color4f   specular_color;        //���� ��������� ������������ �����
  float     angle;                 //���� ���������
  float     exponent;              //���������� ��������� ��������� �� ����
  float     constant_attenuation;  //����������� ��������� �� ����������
  float     linear_attenuation;    //�������� ��������� �� ����������
  float     quadratic_attenuation; //������������ ��������� �� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TexcoordSource
{
  TexcoordSource_Explicit,      //���������� ���������� �������� ����
  TexcoordSource_SphereMap,     //��������� ���������� ���������: ����������� �����
  TexcoordSource_ReflectionMap, //��������� ���������� ���������: ����� ���������
  TexcoordSource_ObjectSpace,   //��������� ���������� ���������: texmap.transform * position
  TexcoordSource_ViewerSpace,   //��������� ���������� ���������: texmap.transform * modelviewprojection * position
  
  TexcoordSource_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ��� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureBlend
{
  TextureBlend_Replace,  //������ ������������ ����� ������ ��������
  TextureBlend_Modulate, //��������� ������������ ����� �� ���� ��������
  TextureBlend_Blend,    //����������

  TextureBlend_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TexmapDesc
{
  TextureBlend   blend;     //���������� ������
  Matrix4f       transform; //������� �������������� ���������� ���������
  Matrix4f       texgen;    //������� ���������� ��������� ���������� ���������
  TexcoordSource source_u;  //�������� ���������� ��������� ��� U-����������
  TexcoordSource source_v;  //�������� ���������� ��������� ��� V-����������
  TexcoordSource source_w;  //�������� ���������� ��������� ��� W-����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ColorMaterial
{
  ColorMaterial_Explicit,          //���� ��������� ������� ����������� ���������
  ColorMaterial_Emission,          //���� ��������� ����� emission-������������ ���������
  ColorMaterial_Ambient,           //���� ��������� ����� ambient-������������ ���������
  ColorMaterial_Diffuse,           //���� ��������� ����� diffuse-������������ ���������
  ColorMaterial_Specular,          //���� ��������� ����� specular-������������ ���������
  ColorMaterial_AmbientAndDiffuse, //���� ��������� ����� ambient � diffuse ������������ ���������  
  
  ColorMaterial_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FppState
{
  Matrix4f      projection_matrix;                  //������� �������������
  Matrix4f      view_matrix;                        //������� ����
  Matrix4f      object_matrix;                      //������� �������������� �������
  LightDesc     lights [FPP_MAX_LIGHTS_COUNT];      //��������� ���������� ���������
  Color4f       emission_color;                     //���� ���������
  Color4f       ambient_color;                      //���� ����������
  Color4f       diffuse_color;                      //���� �����������
  Color4f       specular_color;                     //���� ���������
  float         shininess;                          //"�������������"
  ColorMaterial color_material;                     //��������� �������� ����� ���������
  CompareMode   alpha_compare_mode;                 //����� ����� �����
  float         alpha_reference;                    //��������� ����� �����
  bool          normalize;                          //���������� �� ��������������� �������
  TexmapDesc    maps [DEVICE_SAMPLER_SLOTS_COUNT];  //���������� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FppDynamicParameterType
{
  FppDynamicParameterType_Int,   //����� �����
  FppDynamicParameterType_Float, //������������ �����
  
  FppDynamicParameterType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ����� ��������� FppState, ����������� ������ � ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef stl::vector<size_t> FppFieldOffsetArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������� fpp-������� (������ uniform-var � glsl)
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FppDynamicParameter
{
  FppDynamicParameterType type;          //��� ���������
  size_t                  count;         //���������� ���������
  FppFieldOffsetArray     field_offsets; //�������� ����� � ��������� FppState, ����������� ��� ��������� ���������
};

typedef stl::hash_map<stl::hash_key<const char*>, FppDynamicParameter> FppDynamicParameterMap;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Fixed-pipeline-program ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppShader: virtual public Shader, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppShader  (const ShaderDesc& shader_desc, const LogFunction& error_log);
    ~FppShader ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FppState& GetBaseState () const { return base_state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FppDynamicParameter* FindDynamicParameter (const char* name) const;

  private:
    FppState               base_state;         //������� ��������� ������������� ��������� ��������
    FppDynamicParameterMap dynamic_parameters; //������ ������������ ����������
};

typedef xtl::com_ptr<FppShader> FppShaderPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Fixed-pipeline-program
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppProgram : public Program
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppProgram (const ContextManager& manager, size_t shaders_count, ShaderPtr* shaders);
    ~FppProgram ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ConstantBufferPtr* constant_buffers, ProgramParametersLayout* parameters_layout);
    
  private:
    struct LayoutCacheEntry;  

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LayoutCacheEntry& GetLayout (ProgramParametersLayout*);

  private:
    typedef stl::vector<LayoutCacheEntry> LayoutsCache;
    typedef xtl::com_ptr<FppShader>       FppShaderPtr;

    FppShaderPtr  shader;         //������������ ������
    LayoutsCache  layouts_cache;  //��� ���������� ���������
    size_t        current_time;   //������� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� fixed-pipeline-program ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppShaderManager : virtual public ShaderManager, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppShaderManager (const ContextManager& manager);
    ~FppShaderManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetProfilesCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProfile (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shader*  CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log);
    Program* CreateProgram (size_t shaders_count, ShaderPtr* shaders, const LogFunction& error_log);
};

}

}

}

#endif
