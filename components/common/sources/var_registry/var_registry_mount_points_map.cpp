#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

/*
   ���������
*/

const char* REGISTRY_COMPONENTS_MASK = "common.var_registries.*";

//��������� ��������
void remove_suffix (stl::string& s)
{
  size_t pos = s.rfind ('.');
  
  s.erase (pos != stl::string::npos ? pos : 0);
}

void dummy_event_handler (const char*, VarRegistryEvent)
{
}

/*
   ������ �� ������ ����������
*/
class VarRegistryLink : public ICustomVarRegistry, public xtl::reference_counter
{
  public:
    VarRegistryLink (const char* link_name, const char* in_source_name)
      : source_registry (in_source_name), source_name (in_source_name), handler (&dummy_event_handler)
    {
      for (size_t i = 0; i < VarRegistryEvent_Num; i++)
        event_coonection[i] = source_registry.RegisterEventHandler ("*", (VarRegistryEvent)i, xtl::bind (&VarRegistryLink::RegistryEventHandler, this, _1, (VarRegistryEvent)i));
    }

/*
   ���������/��������� ������
*/
    xtl::any GetValue (const char* var_name)
    {
      return source_registry.GetValue (var_name);
    }

    void SetValue (const char* var_name, const xtl::any& value)
    {
      source_registry.SetValue (var_name, value);
    }

/*
   �������� ������� ����������
*/
    bool HasVariable (const char* var_name)
    {
      return source_registry.HasVariable (var_name);
    }

/*
   ����� ���� ����������
*/
    void EnumerateVars (const EnumHandler& handler)
    {
      source_registry.EnumerateVars (handler);
    }

/*
   ����������/�������� ������
*/
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

/*
   �������� �� ����������/���������/�������� ����������
*/
    void SetEventHandler (const EventHandler& in_handler)
    {
      handler = in_handler;
    }

    const EventHandler& GetEventHandler ()
    {
      return handler;
    }

  private:
    void RegistryEventHandler (const char* var_name, VarRegistryEvent event)
    {
      handler (var_name, event);
    }

  private:
    VarRegistry          source_registry;
    xtl::auto_connection event_coonection[VarRegistryEvent_Num];
    stl::string          source_name;
    EventHandler         handler;
};

}

/*
   ����������
*/

MountPointsMap::~MountPointsMap ()
{
  UnmountAll ();
}

/*
   ���������/�������� ������
*/

void MountPointsMap::Link (const char* link_name, const char* source)
{
  static const char* METHOD_NAME = "common::MountPointsMap::Link";

  if (!link_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "link_name");

  if (!source)
    throw xtl::make_null_argument_exception (METHOD_NAME, "source");

  xtl::com_ptr<VarRegistryLink> link (new VarRegistryLink (link_name, source), false);

  Mount (link_name, link.get ());
}

void MountPointsMap::Unlink (const char* link_name)
{
  if (!link_name)
    return;

    //����� ����� ������������ � ��������� ������

  MountMap::iterator iter = mount_points_map.find (link_name);      

  if (iter == mount_points_map.end ())
    return;
    
  if (dynamic_cast<VarRegistryLink*> (iter->second->Registry ()))
    Unmount (link_name);
}

/*
   ������������ ��������
*/

void MountPointsMap::Mount (const char* branch_name, ICustomVarRegistry* registry)
{
  static const char* METHOD_NAME = "common::MountPointsMap::Mount";

  if (!branch_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "branch_name");

  if (!registry)
    throw xtl::make_null_argument_exception (METHOD_NAME, "registry");

    //�������� ���������� � ������ ����� ������������

  for (stl::string parent_branch=branch_name; !parent_branch.empty (); )
  {
    MountMap::iterator iter = mount_points_map.find (parent_branch.c_str ());

    if (iter != mount_points_map.end ())
      throw format_operation_exception ("common::MountPointsMap::Mount",
        "Can't mount branch '%s', because it conflicts with already mounted branch '%s'",
        branch_name, parent_branch.c_str ());
        
      //��������� ��������

    remove_suffix (parent_branch);
  }
  
    //���������� ����� ����� ������������

  MountPointPtr new_mount_point (new MountPoint (branch_name, registry), false);

  mount_points_map.insert_pair (branch_name, new_mount_point);
  
    //���������� � ��������� ����� ����� ������������

  for (ListenersList::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
  {
    IListener& listener = **iter;
    
    try
    {
      listener.OnMount (new_mount_point.get ());
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
}

void MountPointsMap::OnUnmount (MountPoint* mount_point)
{
  for (ListenersList::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
  {
    IListener& listener = **iter;
    
    try
    {
      listener.OnUnmount (mount_point);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
}

void MountPointsMap::Unmount (const char* branch_name, ICustomVarRegistry* registry)
{
  if (!branch_name)
    return;

    //����� ����� ������������ � ��������� ������

  MountMap::iterator iter = mount_points_map.find (branch_name);      

  if (iter == mount_points_map.end ())
    return;
    
  MountPoint* mount_point = iter->second.get ();
    
  if (mount_point->Registry () != registry)
    return;

    //���������� �� �������� ����� ������������
  
  OnUnmount (mount_point);

   //�������� ����� ������������

  mount_points_map.erase (iter);
}

void MountPointsMap::Unmount (const char* branch_name)
{
  if (!branch_name)
    return;

    //����� ����� ������������ � ��������� ������

  MountMap::iterator iter = mount_points_map.find (branch_name);      

  if (iter == mount_points_map.end ())
    return;
    
  MountPoint* mount_point = iter->second.get ();
    
    //���������� �� �������� ����� ������������
  
  OnUnmount (mount_point);

   //�������� ����� ������������

  mount_points_map.erase (iter);
}

void MountPointsMap::UnmountAll (ICustomVarRegistry* registry)
{
  for (MountMap::iterator map_iter = mount_points_map.begin (), map_end = mount_points_map.end (); map_iter != map_end;)
  {
    MountPoint* mount_point = map_iter->second.get ();
      
    if (mount_point->Registry () == registry)
    {
        //���������� �� �������� ����� ������������
      
      OnUnmount (mount_point);

       //�������� ����� ������������

      MountMap::iterator next = map_iter;

      ++next;

      mount_points_map.erase (map_iter);

      map_iter = next;
    }
    else ++map_iter;
  }
}

void MountPointsMap::UnmountAll ()
{
    //���������� �� �������� ���� ����� ������������

  for (MountMap::iterator map_iter = mount_points_map.begin (), map_end = mount_points_map.end (); map_iter != map_end; ++map_iter)
    OnUnmount (map_iter->second.get ());
    
    //�������� ���� ����� ������������

  mount_points_map.clear ();
}

/*
   ����� ����� ������������  
     ������ ����� ����� ������������ � ���������������� ���������� ��������� (������ ��� a.b.c.d)
       1) search: a.b.c.d var_sub_name = ''
       2) search: a.b.c   var_sub_name = 'd'
       3) search: a.b     var_sub_name = 'c.d'
       4) search: a       var_sub_name = 'b.c.d'
       5) return 0
*/

MountPoint* MountPointsMap::FindMountPoint (const char* full_var_name, stl::string& var_sub_name)
{
  static ComponentLoader loader (REGISTRY_COMPONENTS_MASK);

  for (stl::string mount_point_name = full_var_name; !mount_point_name.empty (); )
  {
    MountMap::iterator iter = mount_points_map.find (mount_point_name.c_str ());

    if (iter != mount_points_map.end ())
    {
      var_sub_name = full_var_name + mount_point_name.size () + 1;

      return iter->second.get ();
    }
    
      //��������� ��������
      
    remove_suffix (mount_point_name);
  }

  return 0;
}

MountPoint* MountPointsMap::FindMountPoint (const char* branch_name, const char* var_name, stl::string& var_sub_name)
{
  stl::string full_name (branch_name);
  
  if (*branch_name)
    full_name += '.';
  
  full_name += var_name;
  
  return FindMountPoint (full_name.c_str (), var_sub_name);
}

/*
   �������� �� ������� �����
*/

void MountPointsMap::RegisterListener (IListener* listener)
{
  if (!listener)
    return;

  listeners.push_back (listener);

  for (MountMap::iterator iter = mount_points_map.begin (), end = mount_points_map.end (); iter != end; ++iter)
    listener->OnMount (iter->second.get ());
}

void MountPointsMap::UnregisterListener (IListener* listener)
{
  listeners.remove (listener);
}

/*
   ��������� ���������� ����� ����� ������������
*/

MountPointsMap::Pointer MountPointsMap::GetGlobalMap ()
{
  struct MountPointsMapWrapper
  {
    MountPointsMapWrapper () : instance (new MountPointsMap) {}

    ~MountPointsMapWrapper ()
    {
      instance->UnmountAll (); //???????????
    }

    MountPointsMap::Pointer instance;
  };

  typedef Singleton<MountPointsMapWrapper> MountPointsMapSingleton;

  return MountPointsMapSingleton::Instance ().instance;
}
