#include "shared.h"

using namespace stl;
using namespace common;
using namespace media;

/*
    ������������ / ����������
*/

Image::Image (ImageImpl* in_impl)
  : impl (in_impl)
{
}

Image::Image ()
{
  try
  {
    impl = create_bitmap_image ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::Image::Image()");
    throw;
  }
}

Image::Image (const Image& source)
  : impl (source.impl)
{
  addref (impl);  
}

Image::Image (const Image& source, PixelFormat format)
  : impl (source.impl)
{
  try
  {    
    if (format == PixelFormat_Default)
    {
      addref (impl);
      return;
    }
    
    impl = source.impl->Clone ();

    try
    {
      Convert (format);      
    }
    catch (...)
    {
      release (impl);
      throw;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::Image(const Image&,PixelFormat)");
    throw;
  }
}

Image::Image (const char* file_name, PixelFormat format)
  : impl (create_null_image ())
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
        
    static ComponentLoader loader ("media.image.loaders.*");

    ImageManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Error at load image '%s' (format=%s)", file_name, get_format_name (format));

    Rename  (file_name);
    Convert (format);
  }
  catch (xtl::exception& exception)
  {
    release (impl);    
    exception.touch ("media::Image::Image(const char*,PixelFormat)");
    throw;
  }
}

Image::Image (size_t layers_count, Image* layers, LayersCloneMode clone_mode)
{
  try
  {
    if (!layers_count)
      throw xtl::make_null_argument_exception ("", "layers_count");
      
    if (!layers)
      throw xtl::make_null_argument_exception ("", "layers");
      
    switch (clone_mode)
    {
      case LayersCloneMode_Copy:
      case LayersCloneMode_Capture:
        break;
      default:
        throw xtl::make_argument_exception ("", "clone_mode", clone_mode);
    }

    impl = create_multilayer_image (layers_count, layers, clone_mode);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::Image::Image(size_t,Image*,LayersCloneMode)");
    throw;
  }
}

Image::Image (size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  try
  {
    impl = create_bitmap_image (width, height, depth, format, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::Image(size_t,size_t,size_t,PixelFormat,const void*)");
    throw;
  }
}

Image::~Image ()
{
  release (impl);
}

/*
    ������������
*/

Image& Image::operator = (const Image& source)
{
  Image (source).Swap (*this);

  return *this; 
}

/*
    ������������
*/

Image Image::Clone () const
{
  try
  {
    return Image (impl->Clone ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::Image::Clone");
    throw;
  }
}

/*
    ��� ��������
*/

const char* Image::Name () const
{
  return impl->Name ();
}

void Image::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::Image::Rename", "new_name");

  impl->Rename (new_name);
}

/*
    ������ ��������
*/

PixelFormat Image::Format () const
{
  return impl->Format ();
}

void Image::Convert (PixelFormat new_format)
{
  if (new_format == impl->Format () || new_format == PixelFormat_Default)
    return;

  if (new_format < 0 || new_format >= PixelFormat_Num)
    throw xtl::make_argument_exception ("media::Image::Convert", "new_format", new_format);

  impl->Convert (new_format);
}

size_t Image::Width () const
{
  return impl->Width ();
}

size_t Image::Height () const
{
  return impl->Height ();
}

size_t Image::Depth () const
{
  return impl->Depth ();
}

void Image::Resize (size_t width, size_t height, size_t depth)
{
  if (width == impl->Width () && height == impl->Height () && depth == impl->Depth ())
    return;

  if (!width)  throw xtl::make_argument_exception ("media::Image::Resize", "width", width);
  if (!height) throw xtl::make_argument_exception ("media::Image::Resize", "height", height);
  if (!depth)  throw xtl::make_argument_exception ("media::Image::Resize", "depth", depth);

  impl->Resize (width, height, depth);
}

/*
    ���������� ������������ ���������
*/

void Image::Fill (PixelFormat format, const void* data)
{
  static const char* METHOD_NAME = "media::Image::Fill";

  if (!data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

  switch (format)
  {
    case PixelFormat_Default:
      format = Format ();
      break;
    case PixelFormat_RGB8:   
    case PixelFormat_RGB16:  
    case PixelFormat_BGR8:   
    case PixelFormat_RGBA8:  
    case PixelFormat_RGBA16: 
    case PixelFormat_BGRA8:  
    case PixelFormat_L8:     
    case PixelFormat_A8:     
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }

  fill (format, data, *this);
}

/*
    ������ � �������
*/

const void* Image::Bitmap (size_t z) const
{
  if (z >= impl->Depth ())
    throw xtl::make_range_exception ("media::Image::Bitmap", "z", z, impl->Depth ());

  return impl->Bitmap (z);
}

void* Image::Bitmap (size_t z)
{
  return const_cast<void*> (const_cast<const Image&> (*this).Bitmap (z));
}

void Image::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
{
  if (!data)
    throw xtl::make_null_argument_exception ("media::Image::PutImage", "data");

  impl->PutImage (x, y, z, width, height, depth, format, data);
}

void Image::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data) const
{
  if (!data)
    throw xtl::make_null_argument_exception ("media::Image::GetImage", "data");

  impl->GetImage (x, y, z, width, height, depth, format, data);
}

namespace
{

template <class T, class Img1, class Img2, class Fn>
void blit (Img1& img1, size_t img1_x, size_t img1_y, size_t img1_z, size_t width, size_t height, size_t depth, Img2& img2, size_t img2_x, size_t img2_y, size_t img2_z, Fn fn)
{
  if (img2_x >= img2.Width ())  return;
  if (img2_y >= img2.Height ()) return;
  if (img2_z >= img2.Depth ())  return;

  if (img2_x + width > img2.Width ())   width  = img2.Width () - img2_x;
  if (img2_y + height > img2.Height ()) height = img2.Height () - img2_y;
  if (img2_z + depth > img2.Depth ())   depth  = img2.Depth () - img2_z;  

  if (!width || !height || !depth)
    return;

  PixelFormat img2_format     = img2.Format ();
  size_t      bytes_per_pixel = get_bytes_per_pixel (img2_format),
              offset          = bytes_per_pixel * (img2_z * img2.Width () * img2.Height () + img2_y * img2.Width () + img2_x);                       

  if (width == img2.Width () && height == img2.Height ())
  {
    T* src = reinterpret_cast<T*> (img2.Bitmap ()) + offset;
    
    (img1.*fn) (img1_x, img1_y, img1_z, width, height, depth, img2_format, src);
  }
  else
  {
    size_t step_y       = bytes_per_pixel * img2.Width (),
           step_z       = step_y * img2.Height ();
    bool   layered_copy = width == img2.Width ();

    for (size_t i=0; i<depth; i++, offset += step_z)
    {
      T* src = reinterpret_cast<T*> (img2.Bitmap ()) + offset;
      
      if (layered_copy)
      {
        (img1.*fn) (img1_x, img1_y, img1_z + i, width, height, 1, img2_format, src);
      }
      else
      {
        for (size_t j=0; j<height; j++, src += step_y)
          (img1.*fn) (img1_x, img1_y + j, img1_z + i, width, 1, 1, img2_format, src);
      }      
    }
  }
}

}

void Image::PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, const Image& source_image, size_t source_x, size_t source_y, size_t source_z)
{
  blit<const char> (*this, x, y, z, width, height, depth, source_image, source_x, source_y, source_z, (void (Image::*)(size_t, size_t, size_t, size_t, size_t, size_t, PixelFormat, const void*))&Image::PutImage);
}

void Image::GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, Image& target_image, size_t target_x, size_t target_y, size_t target_z) const
{
  blit<char> (*this, x, y, z, width, height, depth, target_image, target_x, target_y, target_z, (void (Image::*)(size_t, size_t, size_t, size_t, size_t, size_t, PixelFormat, void*) const)&Image::GetImage);
}

/*
    �������� / ����������
*/

void Image::Load (const char* file_name)
{
  try
  {
    Image (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Image::Load");
    throw;
  }
}

void Image::Save (const char* file_name, PixelFormat recommended_format, const char* format_specific_flags)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::Image::Save", "file_name");
    
  if (!format_specific_flags)
    throw xtl::make_null_argument_exception ("media::Image::Save", "format_specific_flags");

  if (recommended_format < 0 || recommended_format >= PixelFormat_Num)
    throw xtl::make_argument_exception ("media::Image::Save", "recommended_format", recommended_format);

  if (recommended_format != PixelFormat_Default && recommended_format != Format ())
  {  
    Image (*this, recommended_format).Save (file_name, PixelFormat_Default, format_specific_flags);
    return;
  }

  try
  {
    static ComponentLoader loader ("media.image.savers.*");

    ImageManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this, format_specific_flags);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Image::Save");
    throw;
  }
}

void Image::DefaultLoader (const char* file_name, Image& image)
{
  image = Image (create_bitmap_image (file_name));
}

void Image::CubemapLoader (const char* file_name, Image& image)
{
  image = Image (create_cubemap_image (file_name));
}

void Image::SkyBoxLoader (const char* file_name, Image& image)
{
  image = Image (create_skybox_image (file_name));
}

void Image::DefaultSaver (const char* file_name, const Image& image, const char*)
{
  image.impl->Save (file_name);
}

/*
    �����
*/

void Image::Swap (Image& image)
{
  stl::swap (impl, image.impl);
}

namespace media
{

void swap (Image& image1,Image& image2)
{
  image1.Swap (image2);
}

}

/*
    ����������� �������������� �� ���������
*/

void Image::RegisterDefaultSavers ()
{
  static ComponentLoader loader ("media.image.savers.*");
}

void Image::RegisterDefaultLoaders ()
{
  static ComponentLoader loader ("media.image.loaders.*");
}

/*
    ImageImpl
*/

ImageImpl::ImageImpl ()
{
}

ImageImpl::ImageImpl (const ImageImpl& impl)
  : reference_counter (impl)
  , name (impl.name)
{
}

const char* ImageImpl::Name ()
{
  return name.c_str ();
}

void ImageImpl::Rename (const char* new_name)
{
  name = new_name;
}
