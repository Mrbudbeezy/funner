#include "shared.h"

using namespace syslib;

namespace
{

/*
    ��������� ���������� ����
*/

struct TlsValueImpl
{
  void*                   data;
  IThreadCleanupCallback* cleanup;

  TlsValueImpl (IThreadCleanupCallback* in_cleanup) : data (0), cleanup (in_cleanup) {}  
  
  static void Cleanup (void* data)
  {
    TlsValueImpl* value = (TlsValueImpl*)data;
    
    if (!value || !value->cleanup)
      return;

    value->cleanup->Cleanup (value->data);
  }
};

/*
    ���� ���������� ����
*/

struct TlsKeyImpl
{
  pthread_key_t           key;
  IThreadCleanupCallback* cleanup;
  
  TlsKeyImpl (IThreadCleanupCallback* in_cleanup) : cleanup (in_cleanup)
  {
    int status = pthread_key_create (&key, &TlsValueImpl::Cleanup);
    
    if (status)
      pthread_raise_error ("::pthread_create", status);
  }
  
  ~TlsKeyImpl ()
  {
    pthread_key_delete (key);
  }
  
  TlsValueImpl& GetValue ()
  {
    TlsValueImpl* value = (TlsValueImpl*)pthread_getspecific (key);

    if (value)
      return *value;

    stl::auto_ptr<TlsValueImpl> new_value (new TlsValueImpl (cleanup));

    int status = pthread_setspecific (key, new_value.get ());

    if (status)
      pthread_raise_error ("::pthread_setspecific", status);

    return *new_value.release ();
  }
};

}

tls_t PThreadManager::CreateTls (IThreadCleanupCallback* cleanup)
{
  try
  {
    thread_init ();              
    
    return (tls_t)new TlsKeyImpl (cleanup);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::CreateTls");
    throw;
  }
}

void PThreadManager::DestroyTls (tls_t tls)
{
  try
  {
    if (!tls)
      return;
      
    TlsKeyImpl* handle = (TlsKeyImpl*)tls;
      
    delete handle;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform");
    throw;
  }
}

void PThreadManager::SetTls (tls_t tls, void* data)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");
    
    TlsKeyImpl* key = (TlsKeyImpl*)tls;

    key->GetValue ().data = data;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::SetTls");
    throw;
  }
}

void* PThreadManager::GetTls (tls_t tls)
{
  try
  {
    if (!tls)
      throw xtl::make_null_argument_exception ("", "tls");
    
    TlsKeyImpl* key = (TlsKeyImpl*)tls;

    return key->GetValue ().data;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::GetTls");
    throw;
  }
}
