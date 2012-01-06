#ifndef SOCIAL_USER_HEADER
#define SOCIAL_USER_HEADER

#include <xtl/functional_fwd>

#include <social/common.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace social
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class User
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    User ();
    User (const void* handle);
    User (const User&);
    ~User ();

    User& operator = (const User&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*                Id            () const;
    void                       SetId         (const char* id);
    const char*                Nickname      () const;
    void                       SetNickname   (const char* nickname);
    bool                       IsFriend      () const;
    void                       SetFriend     (bool is_friend);
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
///�������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IUserManager: public virtual ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const User& user, OperationStatus status, const char* error)> LoadUserCallback;

    virtual void LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const media::Image& picture, OperationStatus status, const char* error)> LoadPictureCallback;

    virtual void LoadUserPicture (const User& user, const LoadPictureCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (size_t count, const char** users, OperationStatus status, const char* error)> LoadFriendsIdsCallback;
    typedef xtl::function<void (size_t count, User* users, OperationStatus status, const char* error)>        LoadFriendsCallback;

    virtual void LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LoadFriends    (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties) = 0;
};

}

#endif
