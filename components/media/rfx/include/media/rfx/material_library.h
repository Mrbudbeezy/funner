#ifndef MEDIALIB_RFX_MATERIAL_LIBRARY_HEADER
#define MEDIALIB_RFX_MATERIAL_LIBRARY_HEADER

#include <stl/auto_ptr.h>

#include <common/serializer_manager.h>

#include <media/rfx/material.h>

namespace xtl
{

//forward decalration
template <class T> class iterator;

}

namespace media
{

namespace rfx
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialLibrary
{
  public:
    typedef xtl::iterator<Material>       Iterator;
    typedef xtl::iterator<const Material> ConstIterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialLibrary  ();
    MaterialLibrary  (const char* file_name);
    MaterialLibrary  (const MaterialLibrary&);
    ~MaterialLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialLibrary& operator = (const MaterialLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество материалов в библиотеке / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора материала
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск
///////////////////////////////////////////////////////////////////////////////////////////////////
          Material* Find (const char* name);
    const Material* Find (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach    (const char* id, const Material& material);
    void Detach    (const char* id); //no throw
    void DetachAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (MaterialLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация сериализаторов по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterDefaultSavers  ();
    static void RegisterDefaultLoaders ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (MaterialLibrary&, MaterialLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер библиотек материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager
<
  void (const char* file_name, MaterialLibrary& library),
  void (const char* file_name, const MaterialLibrary& library)
> MaterialLibraryManager;

}

}

#endif
