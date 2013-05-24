#ifndef STORE_TRANSACTION_HEADER
#define STORE_TRANSACTION_HEADER

#include <cstddef>
#include <xtl/functional_fwd>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace store
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TransactionState
{
  TransactionState_New,         //���������� �� ������������ � �������
  TransactionState_Purchasing,  //���������� ���������� � �������
  TransactionState_Purchased,   //���������� ������� ���������
  TransactionState_Failed,      //���������� ��������� � �������
  TransactionState_Restored     //���������� �������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Transaction
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (bool success, const char* status)> OnFinishedCallback;
    typedef xtl::function <void (const Transaction&, const OnFinishedCallback&)> FinishTransactionHandler;

    Transaction  (const FinishTransactionHandler& finish_handler = FinishTransactionHandler ());
    Transaction  (const Transaction&);
    ~Transaction ();

    Transaction& operator = (const Transaction&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TransactionState State    () const;
    void             SetState (TransactionState state);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Status    () const;
    void        SetStatus (const char* status);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ProductId    () const;
    void        SetProductId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Quantity    () const;
    void   SetQuantity (size_t quantity);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      ReceiptSize   () const;
    const void* ReceiptData   () const;
    const char* ReceiptBase64 () const;
    void        SetReceipt    (size_t size, const void* data);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const common::PropertyMap& Properties    () const;
          common::PropertyMap& Properties    ();
    void                       SetProperties (const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (const void*)> ReleaseHandleFunction;

    const void* Handle    () const;
    void        SetHandle (const void* handle, const ReleaseHandleFunction& release_function = ReleaseHandleFunction ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� (�������� ����� ��������� ��������� ���������� ��� ��������� Purchased, Failed � Restored)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Finish (const OnFinishedCallback& callback = OnFinishedCallback ());

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Transaction&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Transaction&, Transaction&);

}

#endif
