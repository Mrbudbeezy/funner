#include "shared.h"

using namespace store;
using namespace store::amazon_store;

namespace
{

const char* COMPONENT_NAME              = "store.stores.amazon_store";                        //��� ����������
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.amazon_store"; //��� ���������� ����������� �������� ����������
const char* LOAD_HANDLER_ID             = "AmazonStore";                                      //������������� ����������� �������� ����������
const char* STORE_ID                    = "AmazonStore";                                      //������������� ��������
const char* STORE_NAME_MASK             = "AmazonStore";                                      //����� ���� ���������

}

/*
   ��������� ��������
*/

namespace components
{

namespace amazon_store
{

class AmazonStoreComponent
{
  public:
    AmazonStoreComponent ()
    {
      StoreManager::RegisterStore (STORE_ID, STORE_NAME_MASK, xtl::bind (&AmazonStoreComponent::CreateStore, this, _1));
    }

  private:
    IStore* CreateStore (const char* store_name)
    {
      static const char* METHOD_NAME = "store::android_store::AmazonStoreComponent::CreateStore";

      if (xtl::xstrcmp (store_name, STORE_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "store_name", store_name);

      return new StoreImpl ();
    }
};

class AmazonStoreLoadHandlerComponent
{
  public:
    AmazonStoreLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&AmazonStoreLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      StoreImpl::InitJavaBindings (env);
    }
};

extern "C"
{

common::ComponentRegistrator<AmazonStoreComponent> AmazonStore (COMPONENT_NAME);
common::ComponentRegistrator<AmazonStoreLoadHandlerComponent> AmazonStoreLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
