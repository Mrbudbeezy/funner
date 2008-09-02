#ifndef COMMONLIB_VAR_REGISTRY_HEADER
#define COMMONLIB_VAR_REGISTRY_HEADER

#include <xtl/functional_fwd>

namespace xtl
{

//forward declarations
template <class T> class iterator;
class any;

}

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum VarRegistryEvent
{
  VarRegistryEvent_OnCreateVar, //��������� ����� ����������
  VarRegistryEvent_OnDeleteVar, //�������� ����������
  VarRegistryEvent_OnChangeVar, //��������� �������� ����������

  VarRegistryEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomVarRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::any GetValue (const char* var_name) = 0;
    virtual void     SetValue (const char* var_name, const xtl::any& value) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool HasVariable (const char* var_name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* var_name)> EnumHandler;

    virtual void EnumerateVars (const EnumHandler& handler) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ����������/���������/�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (const char* var_name, VarRegistryEvent event)> EventHandler;

    virtual void                SetEventHandler (const EventHandler& handler) = 0;
    virtual const EventHandler& GetEventHandler () = 0;

  protected:
    virtual ~ICustomVarRegistry () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class VarRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VarRegistry ();
    VarRegistry (const char* branch_name);
    VarRegistry (const VarRegistry& source);
    ~VarRegistry ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VarRegistry& operator = (const VarRegistry& source);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::any GetValue (const char* var_name) const;
    void     SetValue (const char* var_name, const xtl::any& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasVariable (const char* var_name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* BranchName () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsOpened () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Open  (const char* branch_name);
    void Close ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���� ���������� / ���������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef ICustomVarRegistry::EnumHandler EnumHandler;

    void EnumerateVars (const EnumHandler& handler) const;
    void EnumerateVars (const char* var_name_mask, const EnumHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    class ISubLevel
    {
      public:
        virtual const char* Name      () = 0; //��� ���������
        virtual size_t      VarsCount () = 0; //����� ���������� ���������� ���������

      protected:
        virtual ~ISubLevel () {}
    };

    typedef xtl::iterator<ISubLevel> Iterator;

    Iterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ����������/���������/�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (const char* var_name)> EventHandler;

    xtl::connection RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (VarRegistry& source);

  private:
    class Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (VarRegistry& source1, VarRegistry& source2);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class VarRegistrySystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Link   (const char* link_name, const char* source);
    static void Unlink (const char* link_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������/������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Mount      (const char* branch_name, ICustomVarRegistry* registry);
    static void Unmount    (const char* branch_name, ICustomVarRegistry* registry);
    static void Unmount    (const char* branch_name);
    static void UnmountAll (ICustomVarRegistry* registry);
    static void UnmountAll ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �� XML �����
///////////////////////////////////////////////////////////////////////////////////////////////////
void load_xml_configuration (VarRegistry& registry, const char* file_mask);
void load_xml_configuration (VarRegistry& registry, const char* file_mask, const char* root_name_mask);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����� ������� � ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* BranchName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void LoadConfiguration (const char* file_name_mask);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void UnloadAll ();
};

}

#endif
