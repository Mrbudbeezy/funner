#ifndef MEDIALIB_ADOBE_XFL_RESOURCE_HEADER
#define MEDIALIB_ADOBE_XFL_RESOURCE_HEADER

namespace media
{

namespace adobe
{

namespace xfl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ресурс
///////////////////////////////////////////////////////////////////////////////////////////////////
class Resource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Resource  ();
    Resource  (const Resource&);
    ~Resource ();

    Resource& operator = (const Resource&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Путь к ресурсу
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Path    () const;
    void        SetPath (const char* path);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Resource&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Resource&, Resource&);

}

}

}

#endif
