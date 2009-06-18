#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/string.h>

#include <stl/auto_ptr.h>
#include <stl/list>

using namespace common;

namespace
{

/*
    ���������
*/

const char*  COMPONENT_MANAGER_DEFAULT_LOG = "common.ComponentManager"; //��� ������ ����������������
const size_t COMPONENT_NAME_BUFFER_SIZE    = 128;                       //����� ��� �������� ����� ����������

/*
    ���������� ��������� �����������
*/

class ComponentManagerImpl
{
  public:
    typedef ComponentManager::LogHandler LogHandler;
    typedef ComponentManager::Iterator   Iterator;
  
      //�����������
    ComponentManagerImpl () : components (MallocAllocator (&malloc, &free)), load_indent (0) {}

      //����������
    ~ComponentManagerImpl ()
    {
      Unload ("*");
    }

      //����������� ����������
    void Register (const char* name, IComponent* component)
    {
      if (!component || !name)
        return;

      size_t name_hash = strihash (name);
      
      ComponentList::iterator iter, end;

      for (iter=components.begin (), end=components.end (); iter != end; ++iter)
        if (iter->component == component || iter->name_hash == name_hash)
          return;

      for (iter=components.begin (), end=components.end (); iter != end && strcmp (name, iter->name) > 0; ++iter);

      components.insert (iter, ComponentNode (name, component));
    }

      //������ ����������� ����������
    void Unregister (IComponent* component)
    {
      if (!component)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end;)
      {
        ComponentNode& node = *iter;

        if (node.component == component && !node.is_locked) components.erase (iter++);
        else                                                ++iter;
      }
    }
    
      //������ ����������� ���������� �� ����� �����
    void Unregister (const char* wc_component_mask)
    {
      if (!wc_component_mask)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end;)
      {
        ComponentNode& node = *iter;
        
        if (wcimatch (node.name, wc_component_mask) && !node.is_locked) components.erase (iter++);
        else                                                            ++iter;
      }
    }

      //�������� �����������
    void Load (const char* wc_component_mask, const LogHandler& log_handler)
    {
      if (!wc_component_mask)
        return;
        
      Log log (COMPONENT_MANAGER_DEFAULT_LOG);      
      
      if (load_indent)
      {
        log.Printf ("...load components '%s'", wc_component_mask);
      }
      else
      {
        log.Printf ("Load components '%s'", wc_component_mask);
      }

      load_indent++;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
      {
        ComponentNode& node = *iter;

        if (node.is_loaded || node.is_locked)
          continue;
          
        try
        {
          if (!wcimatch (node.name, wc_component_mask))
            continue;

          node.is_locked = true;

          try
          {
            log.Printf ("...load component '%s'", node.name);

            node.component->Load ();

            LogPrintf (log_handler, "Component '%s' loaded successull", node.name);
            
            node.is_loaded = true;
          }
          catch (std::exception& e)
          {
            log.Printf (".....%s", e.what ());
            LogPrintf (log_handler, "Exception at load component '%s': %s", node.name, e.what ());
          }
          catch (...)
          {
            log.Print (".....unknown exception");
            LogPrintf (log_handler, "Unknown exception at load component '%s'", node.name);
          }
        }
        catch (...)
        {
          //���������� ���� ����������
        }

        node.is_locked = false;  
      }

      load_indent--;

      if (!load_indent)
        log.Printf ("...components loaded successull");
    }
    
      //�������� ����������
    void Unload (const char* wc_component_mask)
    {
      if (!wc_component_mask)
        return;
        
      Log log (COMPONENT_MANAGER_DEFAULT_LOG);

      if (load_indent)
      {
        log.Printf ("...unload components '%s'", wc_component_mask);        
      }
      else
      {
        log.Printf ("Unload components '%s'", wc_component_mask);        
      }
      
      load_indent++;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
      {
        ComponentNode& node = *iter;

        if (!node.is_loaded || node.is_locked)
          continue;

        try
        {
          if (!wcimatch (node.name, wc_component_mask))
            continue;

          node.is_locked = true;

          log.Printf ("...unload component '%s'", node.name);

          node.component->Unload ();          
        }
        catch (...)
        {
          //���������� ���� ����������
        }

        node.is_loaded = false;
        node.is_locked = false;  
      }
      
      load_indent--;
      
      if (!load_indent)      
        log.Printf ("...components unloaded successull");
    }
    
      //�������� ���������
    Iterator CreateIterator ()
    {
      return Iterator (components.begin (), components.begin (), components.end ());
    }

  private:
    void LogPrintf (const LogHandler& log_handler, const char* format_string, ...)
    {
      if (!log_handler)
        return;

      va_list list;

      va_start (list, format_string);

      try
      {
        log_handler (vformat (format_string, list).c_str ());
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

  private:
    struct ComponentNode: public IComponentState
    {
      IComponent* component;                         //��������� �� ���������
      char        name [COMPONENT_NAME_BUFFER_SIZE]; //��� ����������
      size_t      name_hash;                         //��� ����� ����������
      bool        is_loaded;                         //����: �������� �� ���������
      bool        is_locked;                         //����: ��������� ������������ ��� ������ �����������

      ComponentNode (const char* in_name, IComponent* in_component) :
        component (in_component),
        is_loaded (false),
        is_locked (false)
      {
        xtl::xstrncpy (name, in_name, COMPONENT_NAME_BUFFER_SIZE - 1);

        name [COMPONENT_NAME_BUFFER_SIZE - 1] = '\0';

        name_hash = strhash (in_name);
      }

      ~ComponentNode ()
      {
        if (!is_loaded)
          return;

        try
        {
          is_locked = true;

          component->Unload ();
        }
        catch (...)
        {
          //���������� ���� ����������
        }
      }

      const char* Name () { return name; }

      bool IsLoaded () { return is_loaded; }
    };

    typedef stl::simple_allocator<char>               MallocAllocator;
    typedef stl::list<ComponentNode, MallocAllocator> ComponentList;

  private:
    ComponentList components;
    size_t        load_indent;
};

/*
    ComponentManager
*/

typedef Singleton<ComponentManagerImpl, SingletonStatic> ComponentManagerSingleton;

}

/*
    ����������� ����������
*/

void ComponentManager::Register (const char* name, IComponent* component)
{
  ComponentManagerSingleton::Instance ()->Register (name, component);
}

void ComponentManager::Unregister (IComponent* component)
{
  ComponentManagerSingleton::Instance ()->Unregister (component);
}

void ComponentManager::Unregister (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ()->Unregister (wc_component_mask);
}

/*
    �������� �����������
*/

void ComponentManager::Load (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ()->Load (wc_component_mask, LogHandler ());
}

void ComponentManager::Load (const char* wc_component_mask, const LogHandler& log_handler)
{
  ComponentManagerSingleton::Instance ()->Load (wc_component_mask, log_handler);
}

void ComponentManager::Unload (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ()->Unload (wc_component_mask);
}

/*
    �������� ���������
*/

ComponentManager::Iterator ComponentManager::CreateIterator ()
{
  return ComponentManagerSingleton::Instance ()->CreateIterator ();
}
