#ifndef IMAGELIB_SHARED_HEADER
#define IMAGELIB_SHARED_HEADER

#include <stdarg.h>
#include <il/il.h>
#include <il/ilu.h>
#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>
#include <xtl/function.h>
#include <common/exception.h>
#include <common/singleton.h>
#include <common/heap.h>
#include <common/file.h>
#include <media/image.h>
#include <shared/resource_manager.h>

namespace medialib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageImpl
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
    virtual size_t Width  () = 0;
    virtual size_t Height () = 0;
    virtual size_t Depth  () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Resize (size_t width, size_t height, size_t depth) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* Bitmap (size_t z) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ � �������������� ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data) = 0;
    virtual void GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data) = 0;

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
ImageImpl* create_bitmap_image (size_t width, size_t height, size_t depth, PixelFormat format, const void* data);
ImageImpl* create_bitmap_image (const char* file_name);
ImageImpl* create_multilayer_image (size_t count, Image* images, LayersCloneMode clone_mode);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageSystemImpl: public ResourceManager<ImageSystem::LoadHandler, ImageSystem::SaveHandler>
{
  public:
    ImageSystemImpl ();
};

typedef common::Singleton<ImageSystemImpl> ImageSystemSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������� DevIL
///////////////////////////////////////////////////////////////////////////////////////////////////
void devil_init ();

}

#endif
