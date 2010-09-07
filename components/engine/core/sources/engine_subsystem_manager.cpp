#include "shared.h"

using namespace engine;

namespace
{

/*
    ���������
*/

const char*  LOG_PREFIX                      = "engine"; //������� ������ ����������������
const size_t DEFAULT_SUBSYSTEMS_RESERVE_SIZE = 128;      //������������� ���������� ���������

}

/*
    ���������� ������
*/

struct SubsystemManager::Impl
{
  public:
///�����������
    Impl (const char* in_name)
      : name (in_name),
        log (common::format ("%s.%s", LOG_PREFIX, in_name).c_str ()),
        auto_name_index (0)
    {
      subsystems.reserve (DEFAULT_SUBSYSTEMS_RESERVE_SIZE);
    }

///����������
    ~Impl ()
    {
      RemoveAll ();
    }

///��������
   common::Log& Log () { return log; }

///��������� �����
    void SetName (const char* in_name)
    {
      if (!in_name)
        throw xtl::make_null_argument_exception ("engine::SubsystemManager::SetName", "name");

      log.Printf ("SubsystemManager change name to '%s'", in_name);

      name = in_name;

      common::Log (common::format ("%s.%s", LOG_PREFIX, in_name).c_str ()).Swap (log);
    }

///��������� �����
    const char* Name () { return name.c_str (); }

///���������� ���������� ���������
    size_t SubsystemsCount () const { return subsystems.size (); }

///��������� ����������
    ISubsystemControl& Subsystem (size_t index) const
    {
      if (index >= subsystems.size ())
        throw xtl::make_range_exception ("engine::SubsystemManager::Subsystem", "index", index, subsystems.size ());

      return *subsystems [index];
    }

///���������� ����������
    void Add (const char* name, ISubsystem* subsystem)
    {
      static const char* METHOD_NAME = "engine::SubsystemManager::AddSubsystem";

        //�������� ������������ ����������

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!subsystem)
        throw xtl::make_null_argument_exception (METHOD_NAME, "subsystem");

      for (Subsystems::iterator iter=subsystems.begin (), end=subsystems.end (); iter!=end; ++iter)
      {
        if ((*iter)->name == name)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Subsystem has already registered");
      }

      subsystems.push_back (SubsystemControlPtr (new SubsystemControl (name, subsystem), false));

      log.Printf ("Add subsystem '%s'", name);
    }

    void Add (ISubsystem* subsystem)
    {
      try
      {
        ++auto_name_index;

        Add (common::format ("Subsystem%02u", auto_name_index).c_str (), subsystem);
      }
      catch (...)
      {
        auto_name_index--;
        throw;
      }
    }

///�������� ����������
    void Remove (ISubsystem* subsystem)
    {
      if (!subsystem)
        return;

      for (Subsystems::iterator iter=subsystems.begin (); iter!=subsystems.end ();)
      {
        if ((*iter)->subsystem == subsystem)
        {
          log.Printf ("Remove subsystem '%s'", (*iter)->name.c_str ());

          subsystems.erase (iter);
        }
        else ++iter;
      }
    }

///�������� ��������� �� ����� �����
    void Remove (const char* subsystems_name_masks)
    {
      if (!subsystems_name_masks)
        return;
        
      common::StringArray masks = common::split (subsystems_name_masks);

      for (Subsystems::reverse_iterator iter=subsystems.rbegin (); iter!=subsystems.rend ();)
      {
        bool removed = false;
        
        for (size_t i=0; i<masks.Size (); i++)
        {
          if (common::wcmatch ((*iter)->name.c_str (), masks [i]))
          {
            log.Printf ("Remove subsystem '%s'", (*iter)->name.c_str ());

            Subsystems::reverse_iterator next = iter;

            ++next;

            subsystems.erase (iter.base () - 1);

            iter    = next;
            removed = true;
            
            break;
          }
        }

        if (!removed)
          ++iter;
      }
    }

///�������� ���� ���������
    void RemoveAll ()
    {
      while (!subsystems.empty ())
      {
        log.Printf ("Remove subsystem '%s'", subsystems.back ()->name.c_str ());

        subsystems.erase (subsystems.end () - 1);
      }
    }
    
///���������� �������
    void Execute (const char* command)
    {
      for (Subsystems::iterator iter=subsystems.begin (), end=subsystems.end (); iter!=end; ++iter)
      {
        (*iter)->subsystem->Execute (command);
      }
    }

  private:
    typedef xtl::com_ptr<ISubsystem> SubsystemPtr;

    struct SubsystemControl: public ISubsystemControl, public xtl::reference_counter
    {
      stl::string  name;
      SubsystemPtr subsystem;

      SubsystemControl (const char* in_name, ISubsystem* in_subsystem) :
        name (in_name), subsystem (in_subsystem) {}

      const char* Name () { return name.c_str (); }
    };

    typedef xtl::intrusive_ptr<SubsystemControl> SubsystemControlPtr;
    typedef stl::vector<SubsystemControlPtr>     Subsystems;

  private:
    stl::string name;
    Subsystems  subsystems;
    common::Log log;
    size_t      auto_name_index;
};

/*
    ����������� / ����������
*/

SubsystemManager::SubsystemManager ()
{
  static size_t default_name_id = 1;

  impl = new Impl (common::format ("SubsystemManager%u", default_name_id++).c_str ());
}

SubsystemManager::~SubsystemManager ()
{
}

/*
    ���
*/

void SubsystemManager::SetName (const char* name)
{
  impl->SetName (name);
}

const char* SubsystemManager::Name () const
{
  return impl->Name ();
}

/*
    ������ ������
*/

void SubsystemManager::Start (const common::ParseNode& configuration_root, const char* subsystems_name_masks)
{
  try
  {
    if (!subsystems_name_masks)
      throw xtl::make_null_argument_exception ("", "subsystems_name_mask");

    StartupManagerSingleton::Instance ()->Start (configuration_root, subsystems_name_masks, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("engine::SubsystemManager::Start(const ParseNode&, const char*)");
    throw;
  }
}

void SubsystemManager::Start (const char* configuration_file_name, const char* subsystems_name_masks)
{
  try
  {
    if (!configuration_file_name)
      throw xtl::make_null_argument_exception ("", "configuration_file_name");

    common::Parser parser (configuration_file_name);

    Start (parser.Root ().First ("Configuration"), subsystems_name_masks);

    parser.Log ().Print (xtl::bind (&common::Log::Print, &impl->Log (), _1));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("engine::SubsystemManager::Start(const char*, const char*)");
    throw;
  }
}

/*
   ���������� ���������
*/

void SubsystemManager::Restart (const common::ParseNode& configuration_root, const char* subsystems_name_masks)
{
  Remove (subsystems_name_masks);
  Start  (configuration_root, subsystems_name_masks);
}

void SubsystemManager::Restart (const char* configuration_file_name, const char* subsystems_name_masks)
{
  Remove (subsystems_name_masks);
  Start  (configuration_file_name, subsystems_name_masks);
}

/*
    ������� ���������
*/

size_t SubsystemManager::SubsystemsCount () const
{
  return impl->SubsystemsCount ();
}

ISubsystemControl& SubsystemManager::Subsystem (size_t index) const
{
  return impl->Subsystem (index);
}

/*
    ���������� / �������� ���������
*/

void SubsystemManager::Add (const char* name, ISubsystem* subsystem)
{
  impl->Add (name, subsystem);
}

void SubsystemManager::Add (ISubsystem* subsystem)
{
  impl->Add (subsystem);
}

void SubsystemManager::Remove (ISubsystem* subsystem)
{
  impl->Remove (subsystem);
}

void SubsystemManager::Remove (const char* subsystems_name_masks)
{
  impl->Remove (subsystems_name_masks);
}

void SubsystemManager::RemoveAll ()
{
  impl->RemoveAll ();
}

/*
    ������� ������� �����������
*/

void SubsystemManager::Execute (const char* command)
{
  try
  {
    if (!command)
      throw xtl::make_null_argument_exception ("", "command");
      
    impl->Execute (command);
  }
  catch (xtl::exception& e)
  {
    e.touch ("engine::SubsystemManager::Execute");
    throw;
  }
}
