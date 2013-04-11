#ifndef RENDER_DX11_DEVICE_MANAGER_HEADER
#define RENDER_DX11_DEVICE_MANAGER_HEADER

#include <D3D11.h>

#include <xtl/intrusive_ptr.h>

#include <shared/object.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DeviceManager
{
  public:
    typedef xtl::com_ptr<ID3D11Device> DxDevicePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceManager  (const DxDevicePtr& device);
    DeviceManager  (const DeviceManager&);
    ~DeviceManager ();

    DeviceManager& operator = (const DeviceManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11Device& GetDevice () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetFamilySubId () const { return (size_t)impl; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (DeviceManager&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DeviceObject: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceObject (const DeviceManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const DeviceManager& GetDeviceManager () const { return manager; }
          DeviceManager& GetDeviceManager ()       { return manager; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetFamily (ObjectFamily& family, void*& root);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11Device& GetDevice () const;

  private:
    DeviceManager manager; //�������� ���������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� �������� � ��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DstT, class SrcT>
DstT* cast_object (const DeviceManager& owner, SrcT* ptr, const char* source, const char* argument_name);

template <class DstT, class SrcT>
DstT* cast_object (const DeviceObject& owner, SrcT* ptr, const char* source, const char* argument_name);

#include <shared/detail/device_manager.inl>

}

}

}

#endif

