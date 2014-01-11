#ifndef STORE_ANDROID_STORE_SHARED_HEADER
#define STORE_ANDROID_STORE_SHARED_HEADER

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/signal.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/platform/android.h>

#include <store/store.h>

namespace store
{

namespace android_store
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StoreImpl : public IStore
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StoreImpl  ();
    ~StoreImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize (const IStore::OnInitializedCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool CanBuyProducts ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ������� (products_ids - ����������� ��������� ������ ��������������� ���������,
///products ������ ����� ��������� �� ��� ����������� ��������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadProducts (const char* product_ids, const Store::LoadProductsCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� / �������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterTransactionUpdateHandler (const Store::PurchaseCallback&);
    void            RestorePurchases                 ();
    Transaction     BuyProduct                       (const char* product_id, size_t count, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� java-��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InitJavaBindings (JNIEnv* env);

  private:
    xtl::auto_connection on_initialized_connection;
};

}

}

#endif
