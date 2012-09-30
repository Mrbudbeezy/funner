#include "shared.h"

using namespace store;

/*
   ���������� ����������
*/

struct Transaction::Impl : xtl::reference_counter
{
  FinishTransactionHandler         finish_handler;              //���������� ���������� ����������
  TransactionState                 state;                       //���������
  stl::string                      status;                      //�������� ���������
  stl::string                      product_id;                  //������������� ��������
  size_t                           quantity;                    //����������
  xtl::uninitialized_storage<char> receipt;                     //������ ����
  stl::string                      receipt_base64;              //base 64 ������ ����
  bool                             receipt_base64_needs_update; //����� �� ��������� ������ ���� � base64
  common::PropertyMap              properties;                  //������ ��������
  const void*                      handle;                      //�������������� ����������
  ReleaseHandleFunction            handle_release_function;     //�������, ���������� ��� ������������ �����������

  Impl (const FinishTransactionHandler& in_finish_handler)
    : finish_handler (in_finish_handler)
    , state (TransactionState_New)
    , quantity (0)
    , receipt_base64_needs_update (false)
    , handle (0)
    {}

  ~Impl ()
  {
    if (handle_release_function)
      handle_release_function (handle);
  }
};

/*
   ����������� / ���������� / �����������
*/

Transaction::Transaction (const FinishTransactionHandler& finish_handler)
  : impl (new Impl (finish_handler))
  {}

Transaction::Transaction (const Transaction& source)
  : impl (source.impl)
{
  addref (impl);
}

Transaction::~Transaction ()
{
  release (impl);
}

Transaction& Transaction::operator = (const Transaction& source)
{
  Transaction(source).Swap (*this);

  return *this;
}

/*
   ������������� ����������
*/

size_t Transaction::Id () const
{
  return (size_t)impl;
}

/*
   ���������
*/

TransactionState Transaction::State () const
{
  return impl->state;
}

void Transaction::SetState (TransactionState state)
{
  impl->state = state;
}

/*
   ������
*/

const char* Transaction::Status () const
{
  return impl->status.c_str ();
}

void Transaction::SetStatus (const char* status)
{
  if (!status)
    throw xtl::make_null_argument_exception ("store::Transaction::SetStatus", "status");

  impl->status = status;
}

/*
   ������������� ��������
*/

const char* Transaction::ProductId () const
{
  return impl->product_id.c_str ();
}

void Transaction::SetProductId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("store::Transaction::SetProductId", "id");

  impl->product_id = id;
}

/*
   ���������� ���������� ����������� ��������
*/

size_t Transaction::Quantity () const
{
  return impl->quantity;
}

void Transaction::SetQuantity (size_t quantity)
{
  impl->quantity = quantity;
}

/*
   ��� �������
*/

size_t Transaction::ReceiptSize () const
{
  return impl->receipt.size ();
}

const void* Transaction::ReceiptData () const
{
  return impl->receipt.data ();
}

const char* Transaction::ReceiptBase64 () const
{
  if (impl->receipt_base64_needs_update)
  {
    common::encode_base64 (impl->receipt.size (), impl->receipt.data (), impl->receipt_base64);

    impl->receipt_base64_needs_update = false;
  }

  return impl->receipt_base64.c_str ();
}

void Transaction::SetReceipt (size_t size, const void* data)
{
  if (size && !data)
    throw xtl::make_null_argument_exception ("store::Transaction::SetReceipt", "data");

  impl->receipt.resize (size, false);

  memcpy (impl->receipt.data (), data, size);

  impl->receipt_base64_needs_update = true;
}

/*
   ������ ��������
*/

const common::PropertyMap& Transaction::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Transaction::Properties ()
{
  return impl->properties;
}

void Transaction::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   �������������� �����������
*/

const void* Transaction::Handle () const
{
  return impl->handle;
}

void Transaction::SetHandle (const void* handle, const ReleaseHandleFunction& release_function)
{
  if (impl->handle_release_function)
    impl->handle_release_function (impl->handle);

  impl->handle = handle;
  impl->handle_release_function = release_function;
}

/*
   ���������� ���������� (�������� ����� ��������� ��������� ���������� ��� ��������� Purchased, Failed � Restored)
*/

void Transaction::Finish ()
{
  impl->finish_handler (*this);
}

/*
   �����
*/

void Transaction::Swap (Transaction& source)
{
  stl::swap (impl, source.impl);
}

namespace store
{

/*
   �����
*/

void swap (Transaction& transaction1, Transaction& transaction2)
{
  transaction1.Swap (transaction2);
}

}
