#ifndef MEDIALIB_COMPRESSED_IMAGE_HEADER
#define MEDIALIB_COMPRESSED_IMAGE_HEADER

#include <cstddef>

#include <common/serializer_manager.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание блока данных карты сжатой картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CompressedImageBlockDesc
{
  unsigned int offset; //смещение от начала буфера данных
  unsigned int size;   //размер блока в байтах
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сжатая картинка
///////////////////////////////////////////////////////////////////////////////////////////////////
class CompressedImage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    CompressedImage  ();
    CompressedImage  (const char* file_name);
    CompressedImage  (const CompressedImage&);
    ~CompressedImage ();

    CompressedImage& operator = (const CompressedImage&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры картинки в пикселях
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  () const;
    unsigned int Height () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество слоёв / mip-уровней / блоков
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int LayersCount () const;
    unsigned int MipsCount   () const;
    unsigned int BlocksCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Формат - платформо-зависим, предполагается непосредственная загрузка сжатых образов в видеопамять
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Format () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void*                     Data   () const;
    const CompressedImageBlockDesc* Blocks () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение битовой карты
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void*  Bitmap     (unsigned int layer, unsigned int mip_level) const;
    unsigned int BitmapSize (unsigned int layer, unsigned int mip_level) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (CompressedImage&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация сериализаторов по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterDefaultLoaders ();    

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (CompressedImage&, CompressedImage&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс реализации сжатой картинки
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomCompressedImage
{
  public:
    virtual ~ICustomCompressedImage () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о картинке
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int Width       () = 0;
    virtual unsigned int Height      () = 0;
    virtual unsigned int LayersCount () = 0;
    virtual unsigned int MipsCount   () = 0;
    virtual const char*  Format      () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const void*                     Data   () = 0;
    virtual const CompressedImageBlockDesc* Blocks () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления картинками
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<ICustomCompressedImage* (const char* file_name), void ()> CompressedImageManager;

}

#endif
