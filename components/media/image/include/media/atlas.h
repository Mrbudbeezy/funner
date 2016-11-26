#ifndef MEDIALIB_IMAGEMAP_HEADER
#define MEDIALIB_IMAGEMAP_HEADER

#include <stl/auto_ptr.h>

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

#include <math/vector.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Элемент карты картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Tile
{
  const char*  name;
  const char*  image;
  math::vec2ui origin;
  math::vec2ui size;
  unsigned int tag;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Карта картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
class Atlas
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Atlas  ();
    Atlas  (const char* file_name);
    Atlas  (const Atlas&);
    ~Atlas ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Atlas& operator = (const Atlas&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество тайлов и изображений
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int TilesCount      () const;
    unsigned int ImagesCount     () const;
    unsigned int ImageTilesCount (const char* image_name) const;
    unsigned int ImageTilesCount (unsigned int image_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::Tile* Find (const char* name) const; //no throw

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::Tile& Tile      (unsigned int index) const;
    const media::Tile& Tile      (const char* name) const;
    const media::Tile& ImageTile (unsigned int image_index, unsigned int index) const;
    const media::Tile& ImageTile (const char* image_name, unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Insert (const media::Tile& new_tile);
    unsigned int Insert (const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Set (unsigned int tile_index, const media::Tile& new_tile);
    void Set (unsigned int tile_index, const char* name, const char* image, const math::vec2ui& origin, const math::vec2ui& size, unsigned int tag = 0);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление тайла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove         (unsigned int tile_index);
    void Remove         (const char* name);
    void RemoveAllTiles ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени изображения
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ImageName (unsigned int index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка / получение размеров картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetImageSize (const char* image_name, unsigned int width, unsigned int height);
    void                SetImageSize (const char* image_name, const math::vec2ui& size);
    void                SetImageSize (unsigned int index, unsigned int width, unsigned int height);
    void                SetImageSize (unsigned int index, const math::vec2ui& size);
    const math::vec2ui& ImageSize    (const char* image_name) const; //no throw
    const math::vec2ui& ImageSize    (unsigned int index) const; //no throw
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление пустых изображений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveEmptyImages ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Atlas&);
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Atlas&, Atlas&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления картами картинок
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char* file_name, Atlas& image_map), void (const char* file_name, const Atlas& image_map)> AtlasManager;

}

#endif
