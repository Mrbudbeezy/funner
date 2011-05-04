#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    ���������
*/

const char* COMPRESSED_IMAGE_COMPONENT_MASK = "media.compressed_image.*"; //����� ��� ����������� �������� ������ �����������

/*
    ������ ����������� �� ���������
*/

class DefaultCompressedImage: public ICustomCompressedImage
{
  public:
    size_t      Width       () { return 0; }
    size_t      Height      () { return 0; }
    size_t      LayersCount () { return 0; }
    size_t      MipsCount   () { return 0; }
    const char* Format      () { return ""; }
    const void* Data        () { return 0;  }

    const CompressedImageBlockDesc* Blocks ()
    {
      return 0;
    }
};

}

/*
    �������� ���������� CompressedImage
*/

typedef stl::auto_ptr<ICustomCompressedImage> CompressedImagePtr;

struct CompressedImage::Impl: public xtl::reference_counter
{
  CompressedImagePtr image; //���������� ������� �����������
};

/*
    ������������ / ���������� / ������������
*/

CompressedImage::CompressedImage ()
  : impl (new Impl)
{
  try
  {
    impl->image = CompressedImagePtr (new DefaultCompressedImage);    
  }
  catch (xtl::exception& e)
  {
    delete impl;

    e.touch ("media::CompressedImage::CompressedImage()");

    throw;
  }
}

CompressedImage::CompressedImage (const char* file_name)
  : impl (new Impl)
{
  try
  {
    static ComponentLoader loader (COMPRESSED_IMAGE_COMPONENT_MASK);
    
    impl->image = CompressedImagePtr (CompressedImageManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name));

    if (!impl->image.get ())
      throw xtl::format_operation_exception ("", "Error at load image '%s'. Internal loader error" , file_name);
  }
  catch (xtl::exception& exception)
  {
    delete impl;

    exception.touch ("media::CompressedImage::CompressedImage(const char*)");

    throw;
  }
}

CompressedImage::CompressedImage (const CompressedImage& image)
  : impl (image.impl)
{
  addref (impl);
}

CompressedImage::~CompressedImage ()
{
  release (impl);
}

CompressedImage& CompressedImage::operator = (const CompressedImage& image)
{
  CompressedImage (image).Swap (*this);

  return *this;
}

/*
    ������� �������� � ��������
*/

size_t CompressedImage::Width () const
{
  try
  {
    return impl->image->Width ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Width");
    throw;
  }
}

size_t CompressedImage::Height () const
{
  try
  {
    return impl->image->Height ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Height");
    throw;
  }
}

/*
    ���������� ���� / mip-������� / ������
*/

size_t CompressedImage::LayersCount () const
{
  try
  {
    return impl->image->LayersCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::LayersCount");
    throw;
  }
}

size_t CompressedImage::MipsCount () const
{
  try
  {
    return impl->image->MipsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::MipsCount");
    throw;
  }
}

size_t CompressedImage::BlocksCount () const
{
  try
  {
    return impl->image->LayersCount () * impl->image->MipsCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::BlocksCount");
    throw;
  }
}

/*
    ������
*/

const char* CompressedImage::Format () const
{
  try
  {
    return impl->image->Format ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Format");
    throw;
  }
}

/*
    ��������� ������
*/

const void* CompressedImage::Data () const
{
  try
  {
    return impl->image->Data ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Data");
    throw;
  }  
}

const CompressedImageBlockDesc* CompressedImage::Blocks () const
{
  try
  {
    return impl->image->Blocks ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Blocks");
    throw;
  }  
}

/*
    ������ ������ / ��������� ������� �����
*/

size_t CompressedImage::BitmapSize (size_t layer, size_t mip_level) const
{
  try
  {
    size_t layers_count = LayersCount (),
           mips_count   = MipsCount ();
    
    if (layer >= layers_count)
      throw xtl::make_range_exception ("", "layer", layer, layers_count);

    if (mip_level >= mips_count)
      throw xtl::make_range_exception ("", "mip_level", mip_level, mips_count);

    const CompressedImageBlockDesc* blocks = Blocks ();
    size_t                          index  = layers_count * mip_level + layer;
    
    return blocks [index].size;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::BitmapSize");
    throw;
  }
}

const void* CompressedImage::Bitmap (size_t layer, size_t mip_level) const
{
  try
  {
    size_t layers_count = LayersCount (),
           mips_count   = MipsCount ();

    if (layer >= layers_count)
      throw xtl::make_range_exception ("", "layer", layer, layers_count);

    if (mip_level >= mips_count)
      throw xtl::make_range_exception ("", "mip_level", mip_level, mips_count);

    const CompressedImageBlockDesc* blocks = Blocks ();
    size_t                          index  = layers_count * mip_level + layer;

    return (char*)impl->image->Data () + blocks [index].offset;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Bitmap");
    throw;
  }
}

/*
    ��������
*/

void CompressedImage::Load (const char* file_name)
{
  try
  {
    CompressedImage (file_name).Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Load");
    throw;
  }
}

/*
    �����
*/

void CompressedImage::Swap (CompressedImage& image)
{
  stl::swap (impl, image.impl);
}

namespace media
{

void swap (CompressedImage& image1, CompressedImage& image2)
{
  image1.Swap (image2);
}

}

/*
    ����������� �������������� �� ���������
*/

void CompressedImage::RegisterDefaultLoaders ()
{
  static ComponentLoader loader ("media.compressed_image.loaders.*");
}
