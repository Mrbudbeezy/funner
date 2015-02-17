#ifndef RENDER_LOW_LEVEL_UTILITIES_HEADER
#define RENDER_LOW_LEVEL_UTILITIES_HEADER

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������� ��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_name (CompareMode);
const char* get_name (UsageMode);
const char* get_name (BindFlag);
const char* get_name (AccessFlag);
const char* get_name (PixelFormat);
const char* get_name (TextureDimension);
const char* get_name (VertexAttributeSemantic);
const char* get_name (InputDataType);
const char* get_name (InputDataFormat);
const char* get_name (TexMinFilter);
const char* get_name (TexMagFilter);
const char* get_name (TexcoordWrap);
const char* get_name (ProgramParameterType);
const char* get_name (FillMode);
const char* get_name (CullMode);
const char* get_name (StencilOperation);
const char* get_name (FaceMode);
const char* get_name (BlendOperation);
const char* get_name (BlendArgument);
const char* get_name (ColorWriteFlag);
const char* get_name (SwapMethod);
const char* get_name (ClearFlag);
const char* get_name (PrimitiveType);
const char* get_name (QueryType);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////

//��������� �������� ��������� ������� � ������� ����������� � ��������� �������
unsigned int get_texel_size (PixelFormat format);
unsigned int get_image_size (unsigned int width, PixelFormat format);
unsigned int get_image_size (unsigned int width, unsigned int height, PixelFormat format);
unsigned int get_image_size (unsigned int width, unsigned int height, unsigned int depth, PixelFormat format);

//��������� �������� �������������� ������� � ������� ����������� ����� ����������
unsigned int get_uncompressed_texel_size (PixelFormat format);
unsigned int get_uncompressed_image_size (unsigned int width, PixelFormat format);
unsigned int get_uncompressed_image_size (unsigned int width, unsigned int height, PixelFormat format);
unsigned int get_uncompressed_image_size (unsigned int width, unsigned int height, unsigned int depth, PixelFormat format);

bool is_compressed    (PixelFormat); //�������� �� ������ ������
bool is_uncompressed  (PixelFormat); //�������� �� ������ ��������
bool is_depth_stencil (PixelFormat); //�������� �� ������� ������ �������-��������
bool is_color         (PixelFormat); //�������� �� ������ "��������"

PixelFormat get_uncompressed_format (PixelFormat); //���������� ������������� ���������� ����������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� mip-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int get_mips_count (unsigned int size);
unsigned int get_mips_count (unsigned int width, unsigned int height);
unsigned int get_mips_count (unsigned int width, unsigned int height, unsigned int depth);

}

}

#endif
