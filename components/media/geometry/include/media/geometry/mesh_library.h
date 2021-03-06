#ifndef MEDIALIB_GEOMETRY_MESH_LIBRARY_HEADER
#define MEDIALIB_GEOMETRY_MESH_LIBRARY_HEADER

#include <media/geometry/mesh.h>
#include <common/serializer_manager.h>
#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>

namespace xtl
{

//forward declarations
template <class T> class iterator;

}

namespace media
{

namespace collada
{

//forward declarations
class Model;

}

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека мешей
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshLibrary
{
  public:
    typedef xtl::iterator<Mesh>       Iterator;
    typedef xtl::iterator<const Mesh> ConstIterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MeshLibrary  ();
    MeshLibrary  (const char* file_name);
    MeshLibrary  (const MeshLibrary&);
    ~MeshLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    MeshLibrary& operator = (const MeshLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char*);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество мешей / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Size    () const;
    bool     IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор меша в библиотеке
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск мешей
///////////////////////////////////////////////////////////////////////////////////////////////////
          Mesh* Find (const char* name);
    const Mesh* Find (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение мешей
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach    (const char* name, Mesh& mesh);
    void Detach    (const char* name); //no throw
    void DetachAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка (аддитивная) / сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (MeshLibrary&);
    
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
void swap (MeshLibrary&, MeshLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер библиотек мешей
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char*, MeshLibrary&),
                                          void (const char*, const MeshLibrary&)> MeshLibraryManager;

}

}

#endif
