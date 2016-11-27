#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

namespace
{

const char* SKYBOX_SUFFIX  = ".skybox";
const char* CUBEMAP_SUFFIX = ".cubemap";
const char* DDS_SUFFIX     = ".dds";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Многослойная картинка
///////////////////////////////////////////////////////////////////////////////////////////////////
class MultilayerImageImpl: public ImageImpl
{
  public:
    MultilayerImageImpl (unsigned int count, Image* images, LayersCloneMode clone_mode);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  () { return layers_width;   }
    unsigned int Height () { return layers_height;  }
    unsigned int Depth  () { return (unsigned int)layers.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat Format () { return layers_format; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageImpl* Clone ()
    {
      return new MultilayerImageImpl ((unsigned int)layers.size (), &layers [0], LayersCloneMode_Copy);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразование формата
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (PixelFormat new_format);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize (unsigned int width, unsigned int height, unsigned int depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с образом картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Bitmap (unsigned int z);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование образа с автоматическим преобразованием формата
///////////////////////////////////////////////////////////////////////////////////////////////////
   void PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data);
   void GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Save (const char*);

  private:
    void SaveSixLayersImage (const char* file_name, const char* suffixes [6]);
    void SaveCubemap        (const char* file_name);
    void SaveSkyBox         (const char* file_name);
    void SaveDDS            (const char* file_name);

  private:
    typedef stl::vector<Image> ImageArray;

  private:
    ImageArray   layers;
    unsigned int layers_width, layers_height;
    PixelFormat  layers_format;
};

}

/*
    Конструкторы
*/



MultilayerImageImpl::MultilayerImageImpl (unsigned int count, Image* images, LayersCloneMode clone_mode)
{
  try
  {
    if (!count)
      throw xtl::make_null_argument_exception ("", "count");
      
    if (!images)
      throw xtl::make_null_argument_exception ("", "images");

    layers_width  = images [0].Width ();
    layers_height = images [0].Height ();
    layers_format = images [0].Format ();    
    
    switch (clone_mode)
    {
      case LayersCloneMode_Capture:
        layers.assign (images, images + count);
        break;
      case LayersCloneMode_Copy:
      {
        layers.reserve (count);

        for (unsigned int i=0; i<count; i++)
          layers.push_back (images [i].Clone ());

        break;
      }
      default:
        throw xtl::make_argument_exception ("media::MultilayerImageImpl::MultilayerImageImpl", "clone_mode", clone_mode);
    }

    for (unsigned int i=1; i<count; i++)
    {
      layers [i].Resize (layers_width, layers_height, layers [i].Depth ());
      layers [i].Convert (layers_format);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::MultilayerImageImpl::MultilayerImageImpl");
    throw;
  }
}

/*
    Работа с образом
*/

void* MultilayerImageImpl::Bitmap (unsigned int z)
{
  return layers [z].Bitmap ();
}

void MultilayerImageImpl::PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data)
{
  unsigned int last_z = z + depth;
  
  if (last_z > layers.size ())
    last_z = (unsigned int)layers.size ();
    
  const char* source     = (const char*)data;
  size_t      layer_size = width * height * get_bytes_per_pixel (format);

  for (; z<last_z; z++, source += layer_size)
    layers [z].PutImage (x, y, 0, width, height, 1, format, source);
}

void MultilayerImageImpl::GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data)
{
  unsigned int last_z = z + depth;
  
  if (last_z > layers.size ())
    last_z = (unsigned int)layers.size ();
    
  char*  destination = (char*)data;
  size_t layer_size  = width * height * get_bytes_per_pixel (format);

  for (; z<last_z; z++, destination += layer_size)
    layers [z].GetImage (x, y, 0, width, height, 1, format, destination);
}

/*
    Изменение размера
*/

void MultilayerImageImpl::Resize (unsigned int width, unsigned int height, unsigned int new_layers_count)
{
  ImageArray new_layers;
  
  new_layers.reserve (new_layers_count);  
  new_layers.assign  (layers.begin (), layers.begin () + (new_layers_count < layers.size () ? new_layers_count : layers.size ()));

  if (width != layers_width || height != layers_height)
  {
    for (ImageArray::iterator i=new_layers.begin (), end=new_layers.end (); i!=end; ++i)
      i->Resize (width, height, i->Depth ());      
  }
  
  if (new_layers.size () < new_layers_count)
    new_layers.resize (new_layers_count, Image (width, height, 1, layers_format));

  layers_width  = width;
  layers_height = height;

  layers.swap (new_layers);
}

/*
    Преобразование формата
*/

void MultilayerImageImpl::Convert (PixelFormat new_format)
{
  ImageArray new_layers;

  new_layers.reserve (12);

  for (ImageArray::iterator i=layers.begin (), end=layers.end (); i!=end; ++i)
  {
    Image new_image (i->Clone (), new_format);

    new_layers.push_back (new_image);
  }

  layers_format = new_format;

  layers.swap (new_layers);
}

/*
    Сохранение
*/

void MultilayerImageImpl::SaveSixLayersImage (const char* file_name, const char* suffixes [6])
{
  if (layers.size () < 6)
    throw xtl::format_not_supported_exception ("media::MultilayerImageImpl::SaveSixLayerImage", "Can't save image '%s', depth=%d<6.", Name (), layers.size ());
    
  string basename1 = common::basename (file_name),
         basename2 = common::basename (basename1),
         suffix    = common::suffix (basename1);
    
  for (unsigned char i=0; i<6; i++)
    layers [i].Save ((basename2 + suffixes [i] + suffix).c_str ());
}

void MultilayerImageImpl::SaveCubemap (const char* file_name)
{
  static const char* suffixes [6] = {"_px", "_nx", "_py", "_ny", "_pz", "_nz"};  

  SaveSixLayersImage (file_name, suffixes);
}

void MultilayerImageImpl::SaveSkyBox (const char* file_name)
{
  static const char* suffixes [6] = {"_up", "_down", "_left", "_right", "_front", "_back"};

  SaveSixLayersImage (file_name, suffixes);
}

void MultilayerImageImpl::SaveDDS (const char* file_name)
{
  Image save_image (layers_width, layers_height, (unsigned int)layers.size (), layers_format);

  for (unsigned int i=0; i<layers.size (); i++)
    save_image.PutImage (0, 0, i, layers_width, layers_height, 1, layers_format, layers [i].Bitmap ());

  save_image.Save (file_name);
}

void MultilayerImageImpl::Save (const char* file_name)
{
  string suffix = common::suffix (file_name);

  if      (!::strcmp (CUBEMAP_SUFFIX, suffix.c_str ())) SaveCubemap (file_name);
  else if (!::strcmp (SKYBOX_SUFFIX,  suffix.c_str ())) SaveSkyBox  (file_name);
  else if (!::strcmp (DDS_SUFFIX,     suffix.c_str ())) SaveDDS     (file_name);
  else
  {
    throw xtl::format_not_supported_exception ("media::MultilayerImageImpl::Save", "Can't save image '%s' in file '%s. Unknown extension '%s'",
                       Name (), file_name, suffix.c_str ());
  }
}

/*
    Загрузка
*/

namespace media
{

/*
    Создание реализации
*/

ImageImpl* create_multilayer_image (unsigned int count, Image* images, LayersCloneMode clone_mode)
{
  return new MultilayerImageImpl (count, images, clone_mode);
}

void load_image_array (const char* file_name, unsigned int count, const char** suffixes, Image* images)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::load_image_array", "file_name");

  string basename1 = common::basename (file_name),
         basename2 = common::basename (basename1),
         suffix    = common::suffix   (basename1);         

  try
  {
    for (unsigned char i=0; i<6; i++)
      images [i].Load ((basename2 + suffixes [i] + suffix).c_str ());      
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::load_image_array");
    throw;
  }
}

ImageImpl* create_cubemap_image (const char* file_name)
{
  static const char* suffixes [6] = {"_px", "_nx", "_py", "_ny", "_pz", "_nz"};

  Image images [6];
  
  load_image_array (file_name, 6, suffixes, images);

  return new MultilayerImageImpl (6, images, LayersCloneMode_Capture);
}

ImageImpl* create_skybox_image (const char* file_name)
{
  static const char* suffixes [6] = {"_right", "_left", "_down", "_up", "_front", "_back"};

  Image images [6];
  
  load_image_array (file_name, 6, suffixes, images);

  return new MultilayerImageImpl (6, images, LayersCloneMode_Capture);
}

}
