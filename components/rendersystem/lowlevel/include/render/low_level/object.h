#ifndef RENDER_LOW_LEVEL_OBJECT_HEADER
#define RENDER_LOW_LEVEL_OBJECT_HEADER

namespace render
{

namespace low_level
{

//forward declaration
class IDevice;
class IPropertyList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��������� �������������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IPropertyList* GetProperties () = 0;

  protected:
    virtual ~IObject () {}
    
  private:
    IObject& operator = (const IObject&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������, ��������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDeviceObject: virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ���������� ���������, �������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IDevice* GetDevice () = 0;
};

}

}

#endif
