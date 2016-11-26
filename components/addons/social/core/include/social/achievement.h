#ifndef SOCIAL_ACHIEVEMENT_HEADER
#define SOCIAL_ACHIEVEMENT_HEADER

#include <xtl/functional_fwd>

#include <social/collection.h>
#include <social/common.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace media
{

//forward declaration
class Image;

}

namespace social
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Достижение
///////////////////////////////////////////////////////////////////////////////////////////////////
class Achievement
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор / копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    Achievement  ();
    Achievement  (const Achievement&);
    ~Achievement ();

    Achievement& operator = (const Achievement&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Id    () const;
    void        SetId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Название
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Title    () const;
    void        SetTitle (const char* title);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Является ли скрытым для пользователя
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsHidden  () const;
    void SetHidden (bool hidden);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прогресс выполнения
///////////////////////////////////////////////////////////////////////////////////////////////////
    double Progress    () const;
    void   SetProgress (double progress);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Другие свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle    () const;
    void        SetHandle (const void* handle, const ReleaseHandleFunction& release_function = ReleaseHandleFunction ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Achievement&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Achievement&, Achievement&);

typedef Collection<Achievement> AchievementList;

typedef xtl::function<void (const AchievementList& achievements, OperationStatus status, const char* text)> LoadAchievementsCallback;
typedef xtl::function<void (const media::Image& picture, OperationStatus status, const char* text)>         LoadAchievementPictureCallback;
typedef xtl::function<void (OperationStatus status, const char* text)>                                      SendAchievementCallback;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер достижений
///////////////////////////////////////////////////////////////////////////////////////////////////
class IAchievementManager: public virtual ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Достижения
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Иконка
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties) = 0;
};

}

#endif
