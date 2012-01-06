#ifndef SOCIAL_SESSION_MANAGER_HEADER
#define SOCIAL_SESSION_MANAGER_HEADER

#include <cstddef>

#include <xtl/functional_fwd>

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

enum OperationStatus
{
  OperationStatus_Success,
  OperationStatus_Failure
};

//forward declaration
class Achievement;
class Leaderboard;
class User;

/*///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct RequestResultType       { typedef T&   Type; };
template <>        struct RequestResultType<void> { typedef void Type; };

/*struct RequestResult
{
  const std::type_info* type;

  RequestResult (const std::type_info& in_type)
    : type (&in_type)
    {}
};*/

/*///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RequestState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T>
    RequestState  (const T& result);
    RequestState  (OperationStatus status, const char* error);
    RequestState  (const RequestState&);
    ~RequestState ();

    RequestState& operator = (const RequestState&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ret> typename RequestResultType<Ret>::Type Value () const;

    xtl::any AnyValue () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OperationStatus Status () const;
    const char*     Error  () const;

  private:
    struct Impl;
    Impl* impl;
};*/

/*template <class T>
class RequestStateImpl : private RequestResult, public RequestState
{
  public:
    RequestStateImpl (const T& in_result)
      : RequestResult (typeid (T))
      , RequestState (*this, OperationStatus_Success, "")
      , result (in_result)
    {
    }

    RequestStateImpl (OperationStatus status, const char* error);

    xtl::any AnyValue () const
    {
      return xtl::any (result, true);
    }

  private:
    T result;
};*/

//typedef xtl::function<void (const RequestState& state)> RequestCallback;

class ISessionManager
{
  public:
    virtual ~ISessionManager () {}
};

class ILeaderboardManager: public virtual ISessionManager
{
  public:
    ////.........
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Session
{
  public:
    typedef xtl::com_ptr<Session> Pointer;    
    
    Session (xtl::function<ISessionManager* (const common::PropertyMap& config)>& creator)
    {
      impl->leaderboard_manager = dynamic_cast<ILeaderboardManager*> (manager);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetDescription () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void LogIn      (const common::PropertyMap& config) = 0;
    virtual void LogOut     () = 0;
    virtual bool IsLoggedIn () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ShowWindow (const char* window_name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual User& CurrentUser () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const User& user, OperationStatus status, const char* error)> LoadUserCallback;

    virtual void LoadUser (const char* id, const LoadUserCallback& callback) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const media::Image& picture, OperationStatus status, const char* error)> LoadPictureCallback;

    virtual void LoadUserPicture (const char* user, const LoadPictureCallback& callback) = 0;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (size_t count, const char** users, OperationStatus status, const char* error)> LoadFriendsIdsCallback;
    typedef xtl::function<void (size_t count, User* users, OperationStatus status, const char* error)>       LoadFriendsCallback; //?????
    
    virtual void LoadFriendsIds (const char* id, const LoadFriendsIdsCallback& callback) = 0; //TODO: change param type User&
    virtual void LoadFriends    (const char* id, const LoadFriendsCallback& callback) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (size_t count, Achievement** achievements, OperationStatus status, const char* error)> LoadAchievementsCallback; //???????

    virtual void LoadAchievements (const LoadAchievementsCallback& callback) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (size_t count, const char** leaderboards, OperationStatus status, const char* error)> LoadLeaderboardsCallback;
    typedef xtl::function<void (const Leaderboard& leaderboard, OperationStatus status, const char* error)> LoadLeaderboardCallback;

    void LoadLeaderboardsIds (const LoadLeaderboardsCallback& callback)
    {
      try
      {
        if (!impl->leaderboard_manager)
          throw ???;
          
        impl->leaderboard_manager->LoadLeaderboardIds (callback);
      }
      catch (xtl::exception& e)
      {
        e.touch ("....");
        throw;
      }
    }
    
    virtual void LoadLeaderboard     (const char* leaderboard_id, const LoadLeaderboardCallback& callback) = 0;
    virtual void LoadLeaderboard     (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
//    virtual void AddRef  () = 0;
//    virtual void Release () = 0;

  protected:
    virtual ~ISession () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<Session::Pointer (const char* name)> CreateSessionHandler;

    static void RegisterSession       (const char* name, const CreateSessionHandler& handler);
    static void UnregisterSession     (const char* name);
    static void UnregisterAllSessions ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsSessionRegistered (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Session::Pointer CreateSession (const char* name);
};

}

#endif
