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
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Achievement
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Achievement ();
    Achievement (const void* handle);
    Achievement (const Achievement&);
    ~Achievement ();

    Achievement& operator = (const Achievement&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                Id            () const;
    void                       SetId         (const char* id);
    const char*                Title         () const;
    void                       SetTitle      (const char* title);
    bool                       IsHidden      () const;
    void                       SetHidden     (bool hidden);
    double                     Progress      () const;
    void                       SetProgress   (double progress);
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle () const;

  private:
    struct Impl;
    Impl* impl;
};

//forward declaration
class ISessionManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IAchievementManager: public virtual ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Collection<Achievement>                                                                                   AchievementCollection;
    typedef xtl::function<void (const AchievementCollection& achievements, OperationStatus status, const char* text)> LoadAchievementsCallback;

    virtual void LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const media::Image& picture, OperationStatus status, const char* text)> LoadPictureCallback;

    virtual void LoadAchievementPicture (const Achievement& achievement, const LoadPictureCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (OperationStatus status, const char* text)> ReportCallback;

    virtual void ReportAchievement (const Achievement& achievement, const ReportCallback& callback, const common::PropertyMap& properties) = 0;
};

}

#endif
