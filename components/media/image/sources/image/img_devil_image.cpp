#include "shared.h"

using namespace media;
using namespace common;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///DevIL ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DevILImageImpl: public ImageImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DevILImageImpl  ();
    DevILImageImpl  (const DevILImageImpl&);
    DevILImageImpl  (const char* file_name);
    DevILImageImpl  (unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data);
    ~DevILImageImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageImpl* Clone ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format  () { return format; }
    void        Convert (PixelFormat new_format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� / ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  () { return width; }
    unsigned int Height () { return height; }
    unsigned int Depth  () { return depth; }
    void         Resize (unsigned int width, unsigned int height, unsigned int depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Bitmap   (unsigned int z);
    void  PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data);
    void  GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Save (const char* file_name);

  private:
    static void LoadComponent ()
    {
      static ComponentLoader loader ("media.image.DevIL");
    }

  private:
    ILuint      il_image;             //������������� �������� � DevIL
    ILuint      width, height, depth; //���������� ������� ��������
    PixelFormat format;               //���������� ������ ��������
};

//��������� DevIL ����������
void raise_devil_exception (const char* source, const char* format, ...)
{
  va_list list;

  va_start (list, format);

  throw xtl::vformat_operation_exception (source, format, list);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� DevIL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DevILPixelFormat
{
  ILenum        format, type;
  unsigned char components;

  DevILPixelFormat (ILenum in_format, ILenum in_type, unsigned char in_components) :
     format (in_format), type (in_type), components (in_components) {}
};

//�������������� PixelFormat -> DevILPixelFormat
DevILPixelFormat get_devil_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:   return DevILPixelFormat (IL_RGB,  IL_UNSIGNED_BYTE, 3);
    case PixelFormat_RGB16:  return DevILPixelFormat (IL_RGB,  IL_UNSIGNED_SHORT, 3);
    case PixelFormat_RGBA8:  return DevILPixelFormat (IL_RGBA, IL_UNSIGNED_BYTE, 4);
    case PixelFormat_RGBA16: return DevILPixelFormat (IL_RGBA, IL_UNSIGNED_SHORT, 4);
    case PixelFormat_BGR8:   return DevILPixelFormat (IL_BGR,  IL_UNSIGNED_BYTE, 3);
    case PixelFormat_BGRA8:  return DevILPixelFormat (IL_BGRA, IL_UNSIGNED_BYTE, 4);
    case PixelFormat_L8:     return DevILPixelFormat (IL_LUMINANCE, IL_UNSIGNED_BYTE, 1);
    case PixelFormat_LA8:    return DevILPixelFormat (IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, 2);
    case PixelFormat_A8:     throw xtl::make_not_implemented_exception ("media::get_devil_format(PixelFormat_A8)"); break;
    default:                 throw xtl::make_argument_exception ("media::get_devil_format", "format", format); break;
  }

  return DevILPixelFormat (0, 0, 0); //��� ������ �������������� �����������, ������ ����� �����������
}

//�������� DevIL ������
void check_devil_errors (const char* source, const char* sub_source)
{
  ILenum error = ilGetError ();

  if (error != IL_NO_ERROR)
    raise_devil_exception (source, "Internal DevIL error: %s (after call %s)", iluErrorString (error), sub_source);
}

//��������� ����� DevIL �������
const char* get_devil_format_name (ILenum format)
{
  switch (format)
  {
    case IL_COLOUR_INDEX:     return "IL_COLOR_INDEX";
    case IL_RGB:              return "IL_RGB";
    case IL_RGBA:             return "IL_RGBA";
    case IL_BGR:              return "IL_BGR";
    case IL_BGRA:             return "IL_BGRA";
    case IL_LUMINANCE:        return "IL_LUMINANCE";
    case IL_LUMINANCE_ALPHA:  return "IL_LUMINANCE_ALPHA";
    default:                  throw xtl::make_argument_exception ("media::get_devil_format_name", "format", (int)format);
  }

  return "";
}

const char* get_devil_type_name (ILenum type)
{
  switch (type)
  {
    case IL_BYTE:           return "IL_BYTE";
    case IL_UNSIGNED_BYTE:  return "IL_UNSIGNED_BYTE";
    case IL_SHORT:          return "IL_SHORT";
    case IL_UNSIGNED_SHORT: return "IL_UNSIGNED_SHORT";
    case IL_INT:            return "IL_INT";
    case IL_UNSIGNED_INT:   return "IL_UNSIGNED_INT";
    case IL_FLOAT:          return "IL_FLOAT";
    case IL_DOUBLE:         return "IL_DOUBLE";
    default:                throw xtl::make_argument_exception ("media::get_devil_type_name", "type", (int)type);
  }

  return "";
}

/*
  Singleton for locking DevIL
*/

struct DevILLock
{
};

typedef common::Singleton<DevILLock> DevILLockSingleton;

/*
    DevILImageImpl
*/

/*
    ������������ / ����������
*/

DevILImageImpl::DevILImageImpl ()
  : width (0), height (0), depth (0), format (PixelFormat_Default)
{
  DevILLockSingleton::Instance instance;

  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl()", "ilGenImages");
}

DevILImageImpl::DevILImageImpl (const DevILImageImpl& source)
  : ImageImpl (source)
  , width (source.width)
  , height (source.height)
  , depth (source.depth)
  , format (source.format)
{
  DevILLockSingleton::Instance instance;

  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "ilGenImages");

  try
  {
    ilBindImage        (il_image);
    check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "ilBindImage");
    ilCopyImage        (source.il_image);

    ILenum error = ilGetError ();

    if (error != IL_NO_ERROR)
      raise_devil_exception ("media::DevILImageImpl::DevILImageImpl(const DevILImageImpl&)", "Can't copy image '%s'. %s.",
                             const_cast<DevILImageImpl&> (source).Name (), iluErrorString (error));                             
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::DevILImageImpl (const char* file_name)
  : width (0), height (0), depth (0), format (PixelFormat_Default)
{
  DevILLockSingleton::Instance instance;

  LoadComponent ();

  ilGenImages        (1, &il_image);
  check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const char*)", "ilGenImages");

  try
  {
    ilBindImage        (il_image);
    check_devil_errors ("media::DevILImageImpl::DevILImageImpl(const char*)", "ilBindImage");
    ilLoadImage        (const_cast<ILstring> (file_name));

    ILenum error = ilGetError();

    if (error != IL_NO_ERROR)
      raise_devil_exception ("DevILImageImpl::DevILImageImpl", "Can't create image from file '%s'. %s.", file_name, iluErrorString (error));

    int devil_format = ilGetInteger (IL_IMAGE_FORMAT), devil_type = ilGetInteger (IL_IMAGE_TYPE);

    switch (devil_format)
    {
      case IL_RGB:
        switch (devil_type)
        {
          case IL_UNSIGNED_BYTE:
            format = PixelFormat_RGB8;
            break;
          case IL_UNSIGNED_SHORT:
            format = PixelFormat_RGB16;
            break;
        }
        break;
      case IL_RGBA:
        switch (devil_type)
        {
          case IL_UNSIGNED_BYTE:
            format = PixelFormat_RGBA8;
            break;
          case IL_UNSIGNED_SHORT:
            format = PixelFormat_RGBA16;
            break;
        }
        break;
      case IL_BGR:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_BGR8;

        break;
      case IL_BGRA:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_BGRA8;

        break;
      case IL_LUMINANCE:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_L8;

        break;
      case IL_LUMINANCE_ALPHA:
        if (devil_type == IL_UNSIGNED_BYTE)
          format = PixelFormat_LA8;

       break;
    }

    if (format == PixelFormat_Default)
      raise_devil_exception ("media::DevILImageImpl::DevILImageImpl(const char*)", "Image '%s' has unsupported format (%s, %s)",
                             file_name, get_devil_format_name (devil_format), get_devil_type_name (devil_type));

    width  = ilGetInteger (IL_IMAGE_WIDTH);
    height = ilGetInteger (IL_IMAGE_HEIGHT);
    depth  = ilGetInteger (IL_IMAGE_DEPTH);
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::DevILImageImpl (unsigned int in_width, unsigned int in_height, unsigned int in_depth, PixelFormat in_format, const void* in_data)
  : width (in_width), height (in_height), depth (in_depth), format (in_format)
{
  static const char* METHOD_NAME = "media::DevILImageImpl::DevILImageImpl(unsigned int,unsigned int,unsigned int,PixelFormat,const void*)";

  DevILLockSingleton::Instance instance;

  LoadComponent ();

  ilGenImages (1, &il_image);
  
  if (!il_image) //workaround for il_image = 0
  {
    ilDeleteImages (1, &il_image);
    ilGenImages    (1, &il_image);    
  }
  
  check_devil_errors (METHOD_NAME, "ilGenImages");

  try
  {
    ilBindImage        (il_image);
    check_devil_errors (METHOD_NAME, "ilBindImage");

    DevILPixelFormat devil_format = get_devil_format (format);

    ilTexImage (width, height, depth, devil_format.components, devil_format.format, devil_format.type, const_cast<void*> (in_data));

    ILenum error = ilGetError();

    if (error != IL_NO_ERROR)
      raise_devil_exception (METHOD_NAME, "Can't create image %dx%dx%d with format=%s. %s", width, height, depth, get_format_name (format),
                             iluErrorString (error));
  }
  catch (...)
  {
    ilDeleteImages (1, &il_image);
    throw;
  }
}

DevILImageImpl::~DevILImageImpl ()
{
  DevILLockSingleton::Instance instance;

  ilDeleteImages (1, &il_image);
}

/*
    �����������
*/

ImageImpl* DevILImageImpl::Clone ()
{
  return new DevILImageImpl (*this);
}

/*
    ������
*/

void DevILImageImpl::Convert (PixelFormat new_format)
{
  DevILLockSingleton::Instance instance;

  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Convert", "ilBindImage");

  DevILPixelFormat devil_format = get_devil_format (new_format);

  ilConvertImage (devil_format.format, devil_format.type);

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Convert", "Can't convert image '%s' to format=%s. %s", Name (),
                           get_format_name (new_format), iluErrorString (error));

  format = new_format;
}

/*
    ������� ��������
*/

void DevILImageImpl::Resize (unsigned int in_width, unsigned int in_height, unsigned int in_depth)
{
  DevILLockSingleton::Instance instance;

  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Resize", "ilBindImage");
  iluScale           (in_width, in_height, in_depth);

  ILenum error = ilGetError();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Resize", "Can't resize image '%s' to %ux%ux%u. %s.",
                           Name (), in_width, in_height, in_depth, iluErrorString (error));

  width  = in_width;
  height = in_height;
  depth  = in_depth;
}

/*
    ������ � �������
*/

void* DevILImageImpl::Bitmap (unsigned int z)
{
  DevILLockSingleton::Instance instance;

  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Bitmap", "ilBindImage");

  size_t offset = get_bytes_per_pixel (format) * width * height * z;
  void*  data   = ilGetData () + offset;

  check_devil_errors ("media::DevILImageImpl::Bitmap", "ilGetData");

  return data;
}

void DevILImageImpl::PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data)
{
  DevILLockSingleton::Instance instance;

  if (format == PixelFormat_Default)
    format = this->format;

  ilBindImage (il_image);
  check_devil_errors ("media::DevILImageImpl::PutImage", "ilBindImage");

  DevILPixelFormat devil_format = get_devil_format (format);

  ilSetPixels (x, y, z, width, height, depth, devil_format.format, devil_format.type, const_cast<void*> (data));
  check_devil_errors ("media::DevILImageImpl::PutImage", "ilSetPixels");
}

void DevILImageImpl::GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data)
{
  DevILLockSingleton::Instance instance;

  if (format == PixelFormat_Default)
    format = this->format;

  ilBindImage (il_image);
  check_devil_errors ("media::DevILImageImpl::GetImage", "ilBindImage");

  DevILPixelFormat devil_format = get_devil_format (format);

  ilCopyPixels (x, y, z, width, height, depth, devil_format.format, devil_format.type, data);
  check_devil_errors ("media::DevILImageImpl::GetImage", "ilCopyPixels");
}

/*
    ����������
*/

void DevILImageImpl::Save (const char* file_name)
{
  DevILLockSingleton::Instance instance;

  ilBindImage        (il_image);
  check_devil_errors ("media::DevILImageImpl::Save", "ilBindImage");
  ilSaveImage        (const_cast<ILstring> (file_name));

  ILenum error = ilGetError ();

  if (error != IL_NO_ERROR)
    raise_devil_exception ("media::DevILImageImpl::Save", "Can't save image '%s' to file '%s'. %s",
                           Name (), file_name, iluErrorString (error));
}

}

namespace media
{

/*
    �������� ���������� ��������
*/

ImageImpl* create_bitmap_image ()
{
  return new DevILImageImpl;
}

ImageImpl* create_bitmap_image (unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data)
{
  return new DevILImageImpl (width, height, depth, format, data);
}

ImageImpl* create_bitmap_image (const char* file_name)
{
  return new DevILImageImpl (file_name);
}

}
