#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPRESSED_IMAGE_COMPONENT_MASK = "media.compressed_image.*"; //маска имён компонентов загрузки сжатых изображений

/*
    Сжатое изображение по умолчанию
*/

class DefaultCompressedImage: public ICustomCompressedImage
{
  public:
    unsigned int Width       () { return 0; }
    unsigned int Height      () { return 0; }
    unsigned int LayersCount () { return 0; }
    unsigned int MipsCount   () { return 0; }
    const char*  Format      () { return ""; }
    const void*  Data        () { return 0;  }

    const CompressedImageBlockDesc* Blocks ()
    {
      return 0;
    }
};

}

/*
    Описание реализации CompressedImage
*/

typedef stl::auto_ptr<ICustomCompressedImage> CompressedImagePtr;

struct CompressedImage::Impl: public xtl::reference_counter
{
  CompressedImagePtr image; //реализация сжатого изображения
};

/*
    Конструкторы / деструктор / присваивание
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
    Размеры картинки в пикселях
*/

unsigned int CompressedImage::Width () const
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

unsigned int CompressedImage::Height () const
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
    Количество слоёв / mip-уровней / блоков
*/

unsigned int CompressedImage::LayersCount () const
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

unsigned int CompressedImage::MipsCount () const
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

unsigned int CompressedImage::BlocksCount () const
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
    Формат
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
    Получение данных
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
    Размер образа / получение битовой карты
*/

unsigned int CompressedImage::BitmapSize (unsigned int layer, unsigned int mip_level) const
{
  try
  {
    unsigned int layers_count = LayersCount (),
                 mips_count   = MipsCount ();
    
    if (layer >= layers_count)
      throw xtl::make_range_exception ("", "layer", layer, layers_count);

    if (mip_level >= mips_count)
      throw xtl::make_range_exception ("", "mip_level", mip_level, mips_count);

    const CompressedImageBlockDesc* blocks = Blocks ();
    unsigned int                    index  = layers_count * mip_level + layer;
    
    return blocks [index].size;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::BitmapSize");
    throw;
  }
}

const void* CompressedImage::Bitmap (unsigned int layer, unsigned int mip_level) const
{
  try
  {
    unsigned int layers_count = LayersCount (),
                 mips_count   = MipsCount ();

    if (layer >= layers_count)
      throw xtl::make_range_exception ("", "layer", layer, layers_count);

    if (mip_level >= mips_count)
      throw xtl::make_range_exception ("", "mip_level", mip_level, mips_count);

    const CompressedImageBlockDesc* blocks = Blocks ();
    unsigned int                    index  = layers_count * mip_level + layer;

    return (char*)impl->image->Data () + blocks [index].offset;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::CompressedImage::Bitmap");
    throw;
  }
}

/*
    Загрузка
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
    Обмен
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
    Регистрация сериализаторов по умолчанию
*/

void CompressedImage::RegisterDefaultLoaders ()
{
  static ComponentLoader loader ("media.compressed_image.*");
}
