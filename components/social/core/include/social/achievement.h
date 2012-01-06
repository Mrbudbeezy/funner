#ifndef SOCIAL_ACHIEVEMENT_HEADER
#define SOCIAL_ACHIEVEMENT_HEADER

#include <social/session_manager.h>

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
  //non virtual, with internal reference counter
class Achievement
{
  public:
    typedef xtl::com_ptr<Achievement> Pointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char*                Id            () = 0;
    virtual const char*                Title         () = 0;
    virtual bool                       IsHidden      () = 0;
    virtual double                     Progress      () = 0;
    virtual void                       SetProgress   (double progress) = 0;
    virtual const common::PropertyMap& Properties    () = 0;
    virtual void                       SetProperties (const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const media::Image& picture, OperationStatus status, const char* error)> LoadPictureCallback;

    virtual void LoadPicture (const LoadPictureCallback& callback) = 0; //move to AchievementManager

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (OperationStatus status, const char* error)> ReportCallback;

    virtual void Report (const ReportCallback& callback) = 0; //move to AchievementManager

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
//    virtual void AddRef  () = 0;
//    virtual void Release () = 0;
    
  protected:
    virtual ~IUser () {}
    
  private:
    AchievementImpl* impl;
};



}

#endif
