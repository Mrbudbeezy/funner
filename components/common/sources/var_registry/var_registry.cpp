#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

/*
   ���������
*/

const char* REGISTRY_COMPONENTS_MASK = "common.var_registries.*";

}

/*
    �������� ���������� ������� ����������
*/

class VarRegistry::Impl : public trackable, public reference_counter, private MountPointsMap::IListener
{
  public:
///������������
    Impl () {}

    Impl (const MountPointsMap::Pointer& in_mount_points_map, const char* in_branch_name) :
      branch_name (in_branch_name), mount_points_map (in_mount_points_map)
    {
        //����������� ��������� �� ����������/�������� ����� ������������

      if (mount_points_map)
        mount_points_map->RegisterListener (this);
    }

///����������
    ~Impl ()
    {
        //���������� �� �������� ���� ����� �������

      for (MountPointsList::iterator iter=assigned_mount_points.begin (), end=assigned_mount_points.end (); iter!=end; ++iter)
        UnmountRegistryNotify ((*iter)->mount_point);

        //������ ����������� ��������� �� ����������/�������� ����� ������������

      if (mount_points_map)
        mount_points_map->UnregisterListener (this);
    }

///��������� ������
    xtl::any GetValue (const char* var_name)
    {
      static const char* METHOD_NAME = "common::VarRegistry::Impl::GetValue";

      if (!var_name)
        throw make_null_argument_exception (METHOD_NAME, "var_name");

      stl::string var_sub_name;

      MountPoint* mount_point = FindMountPoint (var_name, var_sub_name);

      if (!mount_point)
        throw make_argument_exception (METHOD_NAME, "var_name", var_name, "Registry not mounted");

      return mount_point->Registry ()->GetValue (var_sub_name.c_str ());
    }

///��������� ������
    void SetValue (const char* var_name, const xtl::any& value)
    {
      static const char* METHOD_NAME = "common::VarRegistry::Impl::SetValue";

      if (!var_name)
        throw make_null_argument_exception (METHOD_NAME, "var_name");

      stl::string var_sub_name;

      MountPoint *mount_point = FindMountPoint (var_name, var_sub_name);

      if (!mount_point)
        throw make_argument_exception (METHOD_NAME, "var_name", var_name, "Registry not mounted");

      return mount_point->Registry ()->SetValue (var_sub_name.c_str (), value);
    }

///�������� ������� ����������
    bool HasVariable (const char* var_name)
    {
      if (!var_name)
        return false;

      try
      {
        stl::string var_sub_name;

        MountPoint *mount_point = FindMountPoint (var_name, var_sub_name);

        return mount_point && mount_point->Registry ()->HasVariable (var_sub_name.c_str ());
      }
      catch (...)
      {
        return false;
      }
    }

///��������� ����� �����
    const char* BranchName () { return branch_name.c_str (); }

///����� ���� ����������
    void EnumerateVars (const char* var_name_mask, const EnumHandler& handler)
    {
      if (!var_name_mask)
        throw make_null_argument_exception ("common::VarRegistry::Impl::EnumerateVars", "var_name_mask");

      LoadDefaultComponents ();

      EnumerationWrapper (assigned_mount_points, handler, branch_name.c_str (), branch_name.size (), var_name_mask);
    }

///������� ���������� ��������� ���������
    VarRegistry::Iterator CreateIterator (const VarRegistry& registry)
    {
      if (!iteration_set)
        iteration_set = new BranchLevelVarSet (registry);

      return iteration_set->CreateIterator ();
    }

///�������� �� ����������/���������/�������� ����������
    xtl::connection RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler)
    {
      static const char* METHOD_NAME = "common::VarRegistry::Impl::RegisterEventHandler";

      if (!var_name_mask)
        throw make_null_argument_exception (METHOD_NAME, "var_name_mask");

      switch (event)
      {
        case VarRegistryEvent_OnCreateVar:
        case VarRegistryEvent_OnDeleteVar:
        case VarRegistryEvent_OnChangeVar:
          break;
        default:
          throw make_argument_exception (METHOD_NAME, "event", event);
      }

      return signals [event].connect (EventFilter (var_name_mask, handler));
    }

///�������� ������ �������
    static Impl* GetNullBranch ()
    {
      return common::Singleton<ImplWrapper>::Instance ()->instance.get (); //??????
    }

    static Impl* CreateBranch  (const char* branch_name)
    {
      return new Impl (MountPointsMap::GetGlobalMap (), branch_name);
    }

  private:
    void LoadDefaultComponents ()
    {
      static bool is_loaded = false;

      if (!is_loaded)
      {
        is_loaded = true;

        static ComponentLoader loader (REGISTRY_COMPONENTS_MASK);
      }
    }

///����� ����� ������������ �� �����
    MountPoint* FindMountPoint (const char* var_name, stl::string& var_sub_name)
    {
      if (!mount_points_map)
        return 0;

      LoadDefaultComponents ();

      return mount_points_map->FindMountPoint (branch_name.c_str (), var_name, var_sub_name);
    }

///���������� � ���������/��������/��������� ����������
    void Notify (const char* mount_point_name, const char* var_name, VarRegistryEvent event)
    {
      try
      {
          //��������������!!!!

        stl::string full_var_name = format ("%s.%s", mount_point_name, var_name);

        signals [event] (full_var_name.c_str () + branch_name.size () + 1);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

      //������������� ����� ������������
    struct AssignedMountPoint
    {
      AssignedMountPoint (MountPoint* in_mount_point, const xtl::connection& in_event_connection)
        : mount_point (in_mount_point), event_connection (in_event_connection) {}

      MountPoint           *mount_point;
      xtl::auto_connection event_connection;
    };

///���������� � ��������� ����� ������������
    void OnMount (MountPoint* new_mount_point)
    {
      size_t match_size = stl::min (new_mount_point->Name ().size (), branch_name.size ());

      if (!xtl::xstrncmp (branch_name.c_str (), new_mount_point->Name ().c_str (), match_size))
      {
          //���������� ����� ����� ������������

        assigned_mount_points.push_back (AssignedMountPointPtr (new AssignedMountPoint (new_mount_point,
          new_mount_point->RegisterEventHandler (xtl::bind (&VarRegistry::Impl::Notify, this, _1, _2, _3)))));

          //���������� � ���������� ����� �������

        MountRegistryNotify (new_mount_point);
      }
    }

///���������� �� �������� ����� ������������
    void OnUnmount (MountPoint* deleted_mount_point)
    {
      for (MountPointsList::iterator iter=assigned_mount_points.begin (), end=assigned_mount_points.end (); iter!=end; ++iter)
        if ((*iter)->mount_point == deleted_mount_point)
        {
            //���������� �� �������� ����� �������

          UnmountRegistryNotify (deleted_mount_point);

            //�������� ����� �������

          assigned_mount_points.erase (iter);

          return;
        }
    }

///���������� � ���������� ����� �������
    void MountRegistryNotify (MountPoint* mount_point)
    {
      mount_point->Registry ()->EnumerateVars (xtl::bind (&Impl::Notify, this, mount_point->Name ().c_str (), _1, VarRegistryEvent_OnCreateVar));
    }

///���������� �� �������� ����� �������
    void UnmountRegistryNotify (MountPoint* mount_point)
    {
      mount_point->Registry ()->EnumerateVars (xtl::bind (&Impl::Notify, this, mount_point->Name ().c_str (), _1, VarRegistryEvent_OnDeleteVar));
    }

  private:
    typedef xtl::signal<void (const char*)>     VarRegistrySignal;
    typedef xtl::shared_ptr<AssignedMountPoint> AssignedMountPointPtr;
    typedef stl::list<AssignedMountPointPtr>    MountPointsList;
    typedef stl::auto_ptr<BranchLevelVarSet>    IterationSetPtr;

    //�����-�������, ��������������� ��� �������� ����������
    struct EnumerationWrapper
    {
      public:
        EnumerationWrapper (MountPointsList& list, const EnumHandler& in_handler, const char* branch_name, size_t in_branch_name_size,
                            const char* in_var_name_mask)
          : iter (list.begin ()),
            last (list.end ()),
            handler (in_handler),
            var_name_mask (in_var_name_mask),
            branch_name_size (in_branch_name_size),
            var_name_offset (0)
        {
          EnumHandler wrapper = xtl::ref (*this);

          for (; iter != last; ++iter)
          {
            MountPoint& mount_point = *(*iter)->mount_point;

            if (mount_point.Name ().size () > branch_name_size)
            {
              var_name_offset = 0;

              const char* sub_name = mount_point.Name ().c_str () + branch_name_size;

              if (*sub_name && branch_name_size)
                sub_name++;

              current_prefix = sub_name;

              if (!current_prefix.empty ())
                current_prefix += '.';
            }
            else
            {
              current_prefix.clear ();

              var_name_offset = branch_name_size - mount_point.Name ().size ();

              if (mount_point.Name ().size ())
                current_var_name_mask = format ("%s.%s", branch_name + mount_point.Name ().size () + 1, in_var_name_mask);
              else
                current_var_name_mask = in_var_name_mask;
            }

            mount_point.Registry ()->EnumerateVars (wrapper);
          }
        }

        void operator () (const char* var_name)
        {
          if (var_name_offset)
          {
            if (strlen (var_name) < var_name_offset)
              return;

            if (wcmatch (var_name, current_var_name_mask.c_str ()))
              handler (var_name + var_name_offset);
          }
          else
          {
            stl::string relative_var_name = current_prefix + var_name;

            if (wcmatch (relative_var_name.c_str (), var_name_mask))
              handler (relative_var_name.c_str ());
          }
        }

      private:
        MountPointsList::iterator iter, last;
        EnumHandler               handler;
        const char*               var_name_mask;
        size_t                    branch_name_size;
        stl::string               current_prefix;
        stl::string               current_var_name_mask;
        size_t                    var_name_offset;
    };

    //�����-�������, ��������������� ��� ���������� ��������� ��� ��������������� �������� (OnCreateVar/OnDeleteVar/OnChangeVar)
    struct EventFilter
    {
      public:
        typedef VarRegistry::EventHandler EventHandler;

        EventFilter (const char* in_var_mask, const EventHandler& in_handler) : handler (in_handler), var_mask (in_var_mask) {}

        void operator () (const char* var_name) const
        {
          if (!wcmatch (var_name, var_mask.c_str ()))
            return;

          try
          {
            handler (var_name);
          }
          catch (...)
          {
            //���������� ���� ����������
          }
        }

      private:
        EventHandler handler;
        stl::string  var_mask;
    };

    //������� ������������ ��� �������� ���������� (GetNullBranch)
    struct ImplWrapper
    {
      ImplWrapper () : instance (new Impl, false) {}

      xtl::intrusive_ptr<Impl> instance;
    };

  private:
    stl::string             branch_name;
    MountPointsMap::Pointer mount_points_map;
    MountPointsList         assigned_mount_points;
    VarRegistrySignal       signals [VarRegistryEvent_Num];
    IterationSetPtr         iteration_set;
};

/*
===================================================================================================
    VarRegistry
===================================================================================================
*/

/*
   ������������ / �����������
*/

VarRegistry::VarRegistry ()
  : impl (VarRegistry::Impl::GetNullBranch ())
{
  addref (impl);
}

VarRegistry::VarRegistry (const char* branch_name)
{
  if (!branch_name)
    throw make_null_argument_exception ("common::VarRegistry::VarRegistry", "branch_name");

  impl = VarRegistry::Impl::CreateBranch (branch_name);
}

VarRegistry::VarRegistry (const VarRegistry& source)
  : impl (source.impl)
{
  addref (impl);
}

VarRegistry::~VarRegistry ()
{
  release (impl);
}

/*
   �����������
*/

VarRegistry& VarRegistry::operator = (const VarRegistry& source)
{
  VarRegistry (source).Swap (*this);

  return *this;
}

/*
   ���������/��������� ������
*/

xtl::any VarRegistry::GetValue (const char* var_name) const
{
  return impl->GetValue (var_name);
}

void VarRegistry::SetValue (const char* var_name, const xtl::any& value)
{
  impl->SetValue (var_name, value);
}

/*
   �������� ������� ����������
*/

bool VarRegistry::HasVariable (const char* var_name) const
{
  return impl->HasVariable (var_name);
}

/*
   ��������� �����
*/

const char* VarRegistry::BranchName () const
{
  return impl->BranchName ();
}

/*
   �������� ���������� �� ������
*/

bool VarRegistry::IsOpened () const
{
  return impl != VarRegistry::Impl::GetNullBranch ();
}

/*
   �����������/���������� �������
*/

void VarRegistry::Open (const char* branch_name)
{
  VarRegistry (branch_name).Swap (*this);
}

void VarRegistry::Close ()
{
  VarRegistry ().Swap (*this);
}

/*
   ����� ���� ���������� / ���������� �� �����
*/

void VarRegistry::EnumerateVars (const char* var_name_mask, const EnumHandler& handler) const
{
  impl->EnumerateVars (var_name_mask, handler);
}

void VarRegistry::EnumerateVars (const EnumHandler& handler) const
{
  EnumerateVars ("*", handler);
}

/*
   ������� ���������� ��������� ���������
*/

VarRegistry::Iterator VarRegistry::CreateIterator () const
{
  return impl->CreateIterator (*this);
}

/*
   �������� �� ����������/���������/�������� ����������
*/

xtl::connection VarRegistry::RegisterEventHandler (const char* var_name_mask, VarRegistryEvent event, const EventHandler& handler) const
{
  return impl->RegisterEventHandler (var_name_mask, event, handler);
}

/*
   �����
*/

void VarRegistry::Swap (VarRegistry& source)
{
  stl::swap (impl, source.impl);
}

namespace common
{

/*
   �����
*/

void swap (VarRegistry& source1, VarRegistry& source2)
{
  source1.Swap (source2);
}

}
