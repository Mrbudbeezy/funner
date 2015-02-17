#ifndef RENDER_LOW_LEVEL_TEXTURE_HEADER
#define RENDER_LOW_LEVEL_TEXTURE_HEADER

#include <render/low_level/common.h>

namespace render
{

namespace low_level
{

//forwards
class IDeviceContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///  (��� DXT �������� ������ SetData/GetData �������� �� ������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_RGB8,        //red:8, green:8, blue:8
  PixelFormat_RGBA8,       //red:8, green:8, blue:8, alpha:8
  PixelFormat_L8,          //luminance:8
  PixelFormat_A8,          //alpha:8
  PixelFormat_LA8,         //luminance:8, alpha:8
  PixelFormat_DXT1,        //red, green, blue, alpha - compressed
  PixelFormat_DXT3,        //red, green, blue, alpha - compressed  
  PixelFormat_DXT5,        //red, green, blue, alpha - compressed
  PixelFormat_RGB_PVRTC2,  //red, green, blue - compressed
  PixelFormat_RGB_PVRTC4,  //red, green, blue - compressed
  PixelFormat_RGBA_PVRTC2, //red, green, blue, alpha - compressed
  PixelFormat_RGBA_PVRTC4, //red, green, blue, alpha - compressed
  PixelFormat_ATC_RGB_AMD, //red, green, blue - compressed
  PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD,     //red, green, blue, alpha - compressed
  PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD, //red, green, blue, alpha - compressed
  PixelFormat_D16,         //depth:16
  PixelFormat_D24X8,       //depth:24, x:8
  PixelFormat_D24S8,       //depth:24, stencil:8
  PixelFormat_D32,         //depth:32
  PixelFormat_S8,          //stencil:8

  PixelFormat_Num,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureDimension
{
  TextureDimension_1D,      //���������� ��������
  TextureDimension_2D,      //��������� ��������
  TextureDimension_3D,      //��������� ��������
  TextureDimension_Cubemap, //���������� ��������
  
  TextureDimension_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureDesc
{
  TextureDimension dimension;             //����������� ��������
  unsigned int     width;                 //������ ��������
  unsigned int     height;                //������ ��������
  unsigned int     layers;                //���������� ���� (��� ���������� �������), ������� (��� ��������� �������)
  PixelFormat      format;                //������ ��������
  bool             generate_mips_enable;  //��������� �� ��������� mip-�������
  unsigned int     access_flags;          //����� ������� � ������
  unsigned int     bind_flags;            //����� �������� ������
  UsageMode        usage_mode;            //����� ������������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureData
{
  const void*         data;  //���������� ������
  const unsigned int* sizes; //������� ���-������� (���� ����� 0, �� ������������� �������������)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ITexture : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (TextureDesc&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat source_format, const void* buffer, IDeviceContext* context = 0) = 0;
    virtual void GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext* context = 0) = 0;
};

}

}

#endif
