#ifndef RENDER_LOW_LEVEL_TEXTURE_HEADER
#define RENDER_LOW_LEVEL_TEXTURE_HEADER

#include <render/low_level/common.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///  (��� DXT �������� ������ SetData/GetData �������� �� ������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PixelFormat
{
  PixelFormat_RGB8,  //red:8, green:8, blue:8
  PixelFormat_RGBA8, //red:8, green:8, blue:8, alpha:8
  PixelFormat_L8,    //luminance:8
  PixelFormat_A8,    //alpha:8
  PixelFormat_LA8,   //luminance:8, alpha:8
  PixelFormat_DXT1,  //red, green, blue, alpha - compressed
  PixelFormat_DXT3,  //red, green, blue, alpha - compressed  
  PixelFormat_DXT5,  //red, green, blue, alpha - compressed
  PixelFormat_D16,   //depth:16
  PixelFormat_D24,   //depth:24
  PixelFormat_D32,   //depth:32
  PixelFormat_S8,    //stencil:8

  PixelFormat_Num
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
  size_t           width;                 //������ ��������
  size_t           height;                //������ ��������
  size_t           layers;                //���������� ���� (��� ���������� �������), ������� (��� ��������� �������)
  PixelFormat      format;                //������ ��������
  bool             generate_mips_enable;  //��������� �� ��������� mip-�������
  size_t           access_flags;          //����� ������� � ������
  size_t           bind_flags;            //����� �������� ������
  UsageMode        usage_mode;            //����� ������������� ������
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
    virtual void SetData (size_t layer, size_t mip_level, size_t offset, size_t size, const void* buffer) = 0;
    virtual void GetData (size_t layer, size_t mip_level, size_t offset, size_t size, void* buffer) = 0;
};

}

}

#endif
