#ifndef NETWORK_URL_CONNECTION_HEADER
#define NETWORK_URL_CONNECTION_HEADER

#include <network/url.h>

namespace network
{

//+callbacks
//+abstract implementation registry for: curl, bada, npapi

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��� ������ URL ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class UrlConnection
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    UrlConnection  ();
    UrlConnection  (const network::Url&);
    UrlConnection  (const UrlConnection&);
    ~UrlConnection ();
    
    UrlConnection& operator = (const UrlConnection&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const network::Url& Url () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close    ();
    bool IsClosed () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      ContentLength   () const;
    const char* ContentEncoding () const;
    const char* ContentType     () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Receive (void* buffer, size_t size, size_t timeout_in_milliseconds = 0);
    size_t Send    (const void* buffer, size_t size, size_t timeout_in_milliseconds = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� ��������� ��� ������ ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t ReceiveAvailable () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (UrlConnection&);
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (UrlConnection&, UrlConnection&);


}

#endif
