#ifndef MEDIA_RMS_BINDING_HEADER
#define MEDIA_RMS_BINDING_HEADER

namespace xtl
{

//forward declaration
class trackable;

}

namespace media
{

namespace rms
{

//forward declaration
class ICustomBinding;
class AsyncOperation;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Событие прогресса загрузки ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ProgressState
{
  ProgressState_Processing, //операция в процессе выполнения
  ProgressState_Processed,  //операция выполнена успешно
  ProgressState_Failed,     //операция не выполнена
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прогресс выполнения операций загрузки / выгрузки
///////////////////////////////////////////////////////////////////////////////////////////////////
class Progress
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Progress  (AsyncOperation*);
    Progress  (const Progress&);
    ~Progress ();
    
    Progress& operator = (const Progress&);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние загрузки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetState (ProgressState);
    ProgressState State    () const;
    float         Value    () const; //[0;1]
    const char*   Resource () const; //имя текущего обрабатываемого ресурса
    const char*   Stage    () const; //этап выполнения операции
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Progress&);

  private:
    AsyncOperation* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Progress&, Progress&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание
///////////////////////////////////////////////////////////////////////////////////////////////////
class Binding
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Binding  ();
    Binding  (ICustomBinding* binding); //binding автоматически удаляется в деструкторе или при возникновении исключения
    Binding  (const Binding&);
    ~Binding ();

    Binding& operator = (const Binding&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронная загрузка и выгрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load   ();
    void Unload ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Асинхронная загрузка и выгрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Progress&)> ProgressHandler;

    void AsyncLoad   ();
    void AsyncLoad   (const ProgressHandler&);
    void AsyncLoad   (AsyncOperation&);
    void AsyncUnload ();
    void AsyncUnload (const ProgressHandler&);
    void AsyncUnload (AsyncOperation&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Binding&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Binding&, Binding&);

}

}

#endif
