#ifndef RENDER_LOW_LEVEL_OBJECT_HEADER
#define RENDER_LOW_LEVEL_OBJECT_HEADER

#include <xtl/functional_fwd>

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
    typedef xtl::slot<void ()>     DestroyHandlerSlot;
    typedef xtl::function<void ()> DestroyHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IPropertyList* GetProperties () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� trackable
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::trackable& GetTrackable () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ ������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::connection RegisterDestroyHandler (DestroyHandlerSlot& handler) = 0;
    virtual xtl::connection RegisterDestroyHandler (const DestroyHandler& handler) = 0;
    virtual xtl::connection RegisterDestroyHandler (const DestroyHandler& handler, xtl::trackable& trackable) = 0;

  protected:
    virtual ~IObject () {}

  private:
    IObject& operator = (const IObject&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� trackable
///////////////////////////////////////////////////////////////////////////////////////////////////
inline xtl::trackable& get_trackable (IObject& object)
{
  return object.GetTrackable ();
}

}

}

#endif
