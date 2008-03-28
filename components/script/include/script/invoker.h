#ifndef SCRIPTLIB_INVOKER_HEADER
#define SCRIPTLIB_INVOKER_HEADER

#include <xtl/functional_fwd>
#include <stl/auto_ptr.h>
#include <script/stack.h>

namespace xtl
{

//forward declarations
template <class T> class iterator;

}

namespace script
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������-���� ����� ���������� ������� � C++ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::function<size_t (IStack&)> Invoker; //return results count in stack

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InvokerRegistryEvent
{
  InvokerRegistryEvent_OnRegisterInvoker,    //����������� ��� ���������� �����
  InvokerRegistryEvent_OnUnregisterInvoker,  //����������� ��� �������� �����
  
  InvokerRegistryEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InvokerRegistry
{
  public:
    typedef xtl::iterator<Invoker>       Iterator;
    typedef xtl::iterator<const Invoker> ConstIterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    InvokerRegistry  ();
    InvokerRegistry  (const InvokerRegistry&);
    ~InvokerRegistry ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    InvokerRegistry& operator = (const InvokerRegistry&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Invoker* Find (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* InvokerId (const ConstIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Register   (const char* name, const Invoker& invoker);       
    void Register   (const InvokerRegistry& registry); //����������� ����� ������ registry        
    void Register   (const char* name, const char* source_name); //����������� �����
    void Register   (const char* name, const InvokerRegistry& source_registry, const char* source_name); //����������� �����
    void Unregister (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (InvokerRegistryEvent, const char* invoker_name, Invoker& invoker)> EventHandler;

    xtl::connection RegisterEventHandler (InvokerRegistryEvent event_id, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (InvokerRegistry&);
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (InvokerRegistry&, InvokerRegistry&);

}

#endif
