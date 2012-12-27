#ifndef STORE_CORE_SHARED_HEADER
#define STORE_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <store/store.h>

namespace store
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StoreManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StoreManagerImpl  ();
    ~StoreManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterStore       (const char* id, const char* store_name_mask, const store::StoreManager::CreateStoreHandler& handler);
    void UnregisterStore     (const char* id);
    void UnregisterAllStores ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsStoreRegistered (const char* store_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    store::IStore* CreateStore (const char* store_name);

  private:
    StoreManagerImpl (const StoreManagerImpl&);               //no impl
    StoreManagerImpl& operator = (const StoreManagerImpl&);   //no impl

  private:
    struct Impl;
    Impl* impl;
};

typedef common::Singleton<StoreManagerImpl> StoreManagerSingleton;

}

#endif
