#ifndef SOCIAL_COMMON_HEADER
#define SOCIAL_COMMON_HEADER

#include <xtl/functional_fwd>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace social
{

//forward declaration
class User;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Результат операции
///////////////////////////////////////////////////////////////////////////////////////////////////
enum OperationStatus
{
  OperationStatus_Success,  //Операция завершена успешно
  OperationStatus_Canceled, //Операция была отменена
  OperationStatus_Failure   //В ходе выполнения операции произошла ошибка
};

typedef xtl::function <void (const void*)> ReleaseHandleFunction;

typedef xtl::function<void (const char* respone, OperationStatus status, const char* error)> RequestCallback;
typedef xtl::function<void (OperationStatus status, const char* error)> LoginCallback;
typedef xtl::function<void (OperationStatus status, const char* error)> WindowCallback;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сессии
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISessionManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ISessionManager () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание сессии
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetDescription () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Показ стандартных окон
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ShowWindow (const char* window_name, const WindowCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Логин/логаут
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void LogIn          (const LoginCallback& callback, const common::PropertyMap& properties) = 0;
    virtual void LogOut         () = 0;
    virtual bool IsUserLoggedIn () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка произвольных запросов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void PerformRequest (const char* request, const RequestCallback& callback, const common::PropertyMap& properties) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение залогиненного пользователя
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual User& CurrentUser () = 0;
};

}

#endif
