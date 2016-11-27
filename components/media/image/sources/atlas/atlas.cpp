#include "shared.h"

using namespace media;
using namespace common;

/*
    Реализация карты картинок
*/

struct Atlas::Impl
{
  public:
///Конструктор
    Impl ()
    {
    }

///Деструктор
    Impl (const Impl& source)
    {
      tiles.reserve (source.tiles.size ());

      for (TileArray::const_iterator iter=source.tiles.begin (), end=source.tiles.end (); iter!=end; ++iter)
        tiles.push_back (TilePtr (new TileEntry (**iter), false));
    }

///Количество тайлов
    unsigned int TilesCount ()
    {
      return (unsigned int)tiles.size ();
    }
    
    unsigned int ImageTilesCount (unsigned int image_index)
    {
      if (image_index >= images.size ())
        return 0;
        
      return (unsigned int)images [image_index]->tiles.size ();
    }

    unsigned int ImageTilesCount (const char* image_name)
    {
      if (!image_name)
        return 0;
        
      long long image_index = FindImageIndex (image_name);
        
      if (image_index == -1)
        return 0;

      return (unsigned int)images [(unsigned int)image_index]->tiles.size ();
    }
    
///Количество изображений
    unsigned int ImagesCount () const
    {
      return (unsigned int)images.size ();
    }    

///Поиск
    const media::Tile* Find (const char* name)
    {
      if (!name)
        return 0;

      TileArray::iterator iter = FindTile (name);

      if (iter == tiles.end ())
        return 0;

      return &(*iter)->tile;
    }
    
///Получение тайла
    const media::Tile& Tile (unsigned int index)
    {
      if (index >= TilesCount ())
        throw xtl::make_range_exception ("media::Atlas::Tile(unsigned int)", "index", index, TilesCount ());

      return tiles [index]->tile;
    }

    const media::Tile& Tile (const char* name)
    {
      static const char* METHOD_NAME = "media::Atlas::Tile(const char*)";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      const media::Tile* tile = Find (name);

      if (!tile)
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "No tile with this name");

      return *tile;
    }
    
    const media::Tile& ImageTile (unsigned int image_index, unsigned int index)
    {
      static const char* METHOD_NAME = "media::Atlas::ImageTile(unsigned int, unsigned int)";
      
      if (image_index >= images.size ())
        throw xtl::make_range_exception (METHOD_NAME, "image_index", image_index, images.size ());

      TileImage& image = *images [image_index];
        
      if (index >= image.tiles.size ())
        throw xtl::make_range_exception (METHOD_NAME, "index", index, image.tiles.size ());

      return image.tiles [index]->tile;      
    }
    
    const media::Tile& ImageTile (const char* image_name, unsigned int index)
    {
      static const char* METHOD_NAME = "media::Atlas::ImageTile(const char*, unsigned int)";

      if (!image_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "image_name");

      long long image_index = FindImageIndex (image_name);

      if (image_index == -1)
        throw xtl::make_argument_exception (METHOD_NAME, "image_name", image_name, "Image not found");
        
      return ImageTile ((unsigned int)image_index, index);
    }
    
///Добавление тайла
    unsigned int Insert (const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag)
    {
      static const char* METHOD_NAME = "media::Atlas::Insert";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!image)
        throw xtl::make_null_argument_exception (METHOD_NAME, "image");

      if (tiles.size () >= (unsigned int)-1)
        throw xtl::format_operation_exception (METHOD_NAME, "Tiles count limit exceeded");

      TilePtr tile (new TileEntry (name, image, origin, size, tag), false);

      tiles.push_back (tile);
      
      try
      {
        UpdateImageReference (tile);
      }
      catch (...)
      {
        tiles.pop_back ();
        throw;
      }

      return (unsigned int)tiles.size () - 1;
    }

///Изменение тайла
    void Set (unsigned int tile_index, const media::Tile& new_tile)
    {
      Set (tile_index, new_tile.name, new_tile.image, new_tile.origin, new_tile.size, new_tile.tag);
    }

    void Set (unsigned int tile_index, const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag)
    {
      static const char* METHOD_NAME = "media::Atlas::Set";

      if (tile_index >= TilesCount ())
        throw xtl::make_range_exception (METHOD_NAME, "tile_index", tile_index, TilesCount ());

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!image)
        throw xtl::make_null_argument_exception (METHOD_NAME, "image");

      TileEntry& tile              = *tiles [tile_index];
      bool       need_update_image = tile.image != image;

      if (need_update_image)
        RemoveImageReference (tiles [tile_index]);

      tile.name        = name;
      tile.image       = image;
      tile.tile.name   = tile.name.c_str ();
      tile.tile.image  = tile.image.c_str ();
      tile.tile.origin = origin;
      tile.tile.size   = size;      
      tile.tile.tag    = tag;

      if (need_update_image)
      {
        try
        {
          UpdateImageReference (tiles [tile_index]);
        }
        catch (...)
        {
          tiles.erase (tiles.begin () + tile_index);
          throw;
        }
      }
    }
    
///Удаление тайла
    void Remove (unsigned int tile_index)
    {
      if (tile_index >= TilesCount ())
        return;
        
      RemoveImageReference (tiles [tile_index]);

      tiles.erase (tiles.begin () + tile_index);
    }

    void Remove (const char* name)
    {
      if (!name)
        return;

      TileArray::iterator remove_iter = FindTile (name);

      if (remove_iter != tiles.end ())
        Remove ((unsigned int)(remove_iter - tiles.begin ()));
    }

    void RemoveAllTiles ()
    {
      tiles.clear ();
    }
    
///Получение имени изображения
    const char* ImageName (unsigned int image_index)
    {
      if (image_index >= images.size ())
        throw xtl::make_range_exception ("media::Atlas::ImageName", "image_index", image_index, images.size ());
        
      return images [image_index]->name.c_str ();
    }
    
///Поиск изображения по индексу
    long long FindImageIndex (const char* name)
    {
      if (!name)
        return -1;
        
      for (ImageArray::iterator iter=images.begin (), end=images.end (); iter!=end; ++iter)
        if ((*iter)->name == name)
          return iter - images.begin ();
          
      return -1;
    }    
    
///Установка размеров изображения
    void SetImageSize (const char* image_name, const math::vec2ui& size)
    {
      long long image_index = FindImageIndex (image_name);
      
      if (image_index != -1)
      {
        SetImageSize ((unsigned int)image_index, size);
        return;
      }
      
      ImagePtr image (new TileImage (image_name), false);
      
      image->size = size;

      images.push_back (image);
    }

    void SetImageSize (unsigned int index, const math::vec2ui& size)
    {      
      if (index >= images.size ())
        throw xtl::make_range_exception ("media::Atlas::SetImageSize(unsigned int,const math::vec2ui&)", "index", index, images.size ());

      images [index]->size = size;
    }
    
    const math::vec2ui& ImageSize (unsigned int index)
    {
      if (index >= images.size ())
      {
        static math::vec2ui empty_size;
        
        return empty_size;
      }
      
      return images [index]->size;
    }        
    
///Удаление пустых изображений
    void RemoveEmptyImages ()
    {
      for (ImageArray::iterator iter=images.begin (); iter!=images.end ();)
        if ((*iter)->tiles.empty ()) images.erase (iter);
        else                         ++iter;
    }    

  private:
    struct TileEntry: public xtl::reference_counter
    {
      media::Tile tile;
      stl::string name;
      stl::string image;

      TileEntry (const char* in_name, const char* in_image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag)
        : name (in_name)
        , image (in_image)
        {
          tile.origin = origin;
          tile.size   = size;
          tile.name   = name.c_str ();
          tile.image  = image.c_str ();
          tile.tag    = tag;
        }
    };
    
    typedef xtl::intrusive_ptr<TileEntry> TilePtr;
    typedef stl::vector<TilePtr>          TileArray;
    
    struct TileImage: public xtl::reference_counter
    {
      stl::string  name;
      math::vec2ui size;
      TileArray    tiles;
      
      TileImage (const char* in_name)
        : name (in_name)
        {}
    };
    
    typedef xtl::intrusive_ptr<TileImage> ImagePtr;
    typedef stl::vector<ImagePtr>         ImageArray;

  private:
    TileArray::iterator FindTile (const char* name)
    {
      for (TileArray::iterator iter = tiles.begin (), end = tiles.end (); iter != end; ++iter)
      {
        if ((*iter)->name == name)
          return iter;
      }

      return tiles.end ();
    }    
    
    void UpdateImageReference (const TilePtr& tile)
    {
      long long image_index = FindImageIndex (tile->image.c_str ());

      if (image_index != -1)
      {
        images [(unsigned int)image_index]->tiles.push_back (tile);
      }
      else
      {
        ImagePtr image (new TileImage (tile->image.c_str ()), false);

        image->tiles.push_back (tile);

        images.push_back (image);
      }
    }
    
    void RemoveImageReference (const TilePtr& tile)
    {
      long long image_index = FindImageIndex (tile->image.c_str ());
      
      if (image_index == -1)
        return;

      TileImage& image = *images [(unsigned int)image_index];

      for (TileArray::iterator iter=image.tiles.begin (), end=image.tiles.end (); iter!=end; ++iter)
        if (*iter == tile)
        {
          image.tiles.erase (iter);
          break;
        }
    }    
    
    Impl& operator = (const Impl& source); //no impl

  private:    
    TileArray  tiles;
    ImageArray images;
};


/*
   Конструкторы / деструктор
*/

Atlas::Atlas ()
  : impl (new Impl)
{
}

Atlas::Atlas (const char* file_name)
  : impl (new Impl)
{
  static const char* METHOD_NAME = "media::Atlas::Atlas";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  try
  {
    static ComponentLoader loader ("media.image.atlas.loaders.*");

    AtlasManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

Atlas::Atlas (const Atlas& source)
  : impl (new Impl (*source.impl))
{
}

Atlas::~Atlas ()
{
}

/*
   Присваивание
*/

Atlas& Atlas::operator = (const Atlas& source)
{
  if (this == &source)
    return *this;

  Atlas (source).Swap (*this);

  return *this; 
}

/*
   Количество тайлов и изображений
*/

unsigned int Atlas::TilesCount () const
{
  return impl->TilesCount ();
}

unsigned int Atlas::ImagesCount () const
{
  return impl->ImagesCount ();
}

unsigned int Atlas::ImageTilesCount (const char* image_name) const
{
  return impl->ImageTilesCount (image_name);
}

unsigned int Atlas::ImageTilesCount (unsigned int image_index) const
{
  return impl->ImageTilesCount (image_index);
}

/*
   Поиск
*/

const media::Tile* Atlas::Find (const char* name) const
{
  return impl->Find (name);
}

/*
   Получение тайла
*/

const media::Tile& Atlas::Tile (unsigned int index) const
{
  return impl->Tile (index);
}

const media::Tile& Atlas::Tile (const char* name) const
{
  return impl->Tile (name);
}

const media::Tile& Atlas::ImageTile (unsigned int image_index, unsigned int index) const
{
  return impl->ImageTile (image_index, index);
}

const media::Tile& Atlas::ImageTile (const char* image_name, unsigned int index) const
{
  return impl->ImageTile (image_name, index);
}
    
/*
   Добавление тайла
*/

unsigned int Atlas::Insert (const media::Tile& new_tile)
{
  return impl->Insert (new_tile.name, new_tile.image, new_tile.origin, new_tile.size, new_tile.tag);
}

unsigned int Atlas::Insert (const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag)
{
  return impl->Insert (name, image, origin, size, tag);
}

/*
   Изменение тайла
*/

void Atlas::Set (unsigned int tile_index, const media::Tile& new_tile)
{
  impl->Set (tile_index, new_tile);
}

void Atlas::Set (unsigned int tile_index, const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag)
{
  impl->Set (tile_index, name, image, origin, size, tag);
}
    
/*
   Удаление тайла
*/

void Atlas::Remove (unsigned int tile_index)
{
  impl->Remove (tile_index);
}

void Atlas::Remove (const char* name)
{
  impl->Remove (name);
}

void Atlas::RemoveAllTiles ()
{
  impl->RemoveAllTiles ();
}

/*
    Получение имени изображения
*/

const char* Atlas::ImageName (unsigned int image_index) const
{
  return impl->ImageName (image_index);
}

/*
    Установка / получение размеров картинок
*/

void Atlas::SetImageSize (unsigned int image_index, unsigned int width, unsigned int height)
{
  SetImageSize (image_index, math::vec2ui (width, height));
}

void Atlas::SetImageSize (unsigned int image_index, const math::vec2ui& size)
{
  impl->SetImageSize (image_index, size);
}

const math::vec2ui& Atlas::ImageSize (unsigned int image_index) const
{
  return impl->ImageSize (image_index);
}

void Atlas::SetImageSize (const char* image_name, unsigned int width, unsigned int height)
{
  SetImageSize (image_name, math::vec2ui (width, height));
}

void Atlas::SetImageSize (const char* image_name, const math::vec2ui& size)
{
  static const char* METHOD_NAME = "media::Atlas::SetImageSize(const char*, const math::vec2ui&)";

  if (!image_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "image_name");
    
  impl->SetImageSize (image_name, size);
}

const math::vec2ui& Atlas::ImageSize (const char* image_name) const
{
  return impl->ImageSize ((unsigned int)impl->FindImageIndex (image_name));
}

/*
    Удаление пустых изображений
*/

void Atlas::RemoveEmptyImages ()
{
  impl->RemoveEmptyImages ();
}

/*
   Загрузка / сохранение
*/

void Atlas::Load (const char* file_name)
{
  try
  {
    Atlas (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Atlas::Load");
    throw;
  }
}

void Atlas::Save (const char* file_name)
{
  static const char* METHOD_NAME = "media::Atlas::Save";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");
    
  try
  {
    static ComponentLoader loader ("media.image.atlas.savers.*");

    AtlasManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
   Обмен
*/

void Atlas::Swap (Atlas& atlas)
{
  swap (impl, atlas.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (Atlas& atlas1, Atlas& atlas2)
{
  atlas1.Swap (atlas2);
}

}
