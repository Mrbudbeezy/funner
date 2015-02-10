#ifndef IMAGELIB_SHARED_HEADER
#define IMAGELIB_SHARED_HEADER

#include <stdarg.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/common_exceptions.h>

#include <common/heap.h>
#include <common/file.h>
#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/image.h>
#include <media/compressed_image.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageImpl: public xtl::reference_counter
{
  public:
    virtual ~ImageImpl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ImageImpl* Clone () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   ();
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual PixelFormat Format () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Convert (PixelFormat new_format) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int Width  () = 0;
    virtual unsigned int Height () = 0;
    virtual unsigned int Depth  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Resize (unsigned int width, unsigned int height, unsigned int depth) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* Bitmap (unsigned int z) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ � �������������� ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data) = 0;
    virtual void GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Save (const char* file_name) = 0;

  protected:
    ImageImpl  ();
    ImageImpl  (const ImageImpl&);

  private:
    ImageImpl& operator = (const ImageImpl&); //no impl

  private:
    stl::string name; //��� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
ImageImpl* create_bitmap_image ();
ImageImpl* create_bitmap_image (unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data);
ImageImpl* create_bitmap_image (const char* file_name);
ImageImpl* create_null_image ();

ImageImpl* create_multilayer_image (unsigned int count, Image* images, LayersCloneMode clone_mode);
ImageImpl* create_cubemap_image    (const char* file_name);
ImageImpl* create_skybox_image     (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
ICustomCompressedImage* create_compressed_image (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
void fill (PixelFormat format, const void* data, Image& image);

}

#endif
