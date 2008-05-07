#include <common/component.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <xtl/function.h>
#include <xtl/iterator.h>

#include <stl/list>

using namespace common;

/*
    ���������� ��������� �����������
*/

class ComponentManagerImpl
{
  public:
    typedef ComponentManager::LogHandler LogHandler;
    typedef ComponentManager::Iterator   Iterator;
  
      //�����������
    ComponentManagerImpl () : components (MallocAllocator (&malloc, &free)) {}

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

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
        if (iter->component == component || iter->name_hash == name_hash)
          return;

      components.push_back (ComponentNode (name, component));
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
        
        if (wcimatch (node.name.c_str (), wc_component_mask) && !node.is_locked) components.erase (iter++);
        else                                                                     ++iter;
      }
    }

      //�������� �����������
    void Load (const char* wc_component_mask, const LogHandler& log_handler)
    {
      if (!wc_component_mask)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
      {
        ComponentNode& node = *iter;

        if (node.is_loaded || node.is_locked)
          continue;
          
        try
        {
          if (!wcimatch (node.name.c_str (), wc_component_mask))
            continue;

          node.is_locked = true;

          try
          {        
            node.component->Load ();

            LogPrintf (log_handler, "Component '%s' loaded successull", node.name.c_str ());
            
            node.is_loaded = true;
          }
          catch (std::exception& e)
          {
            LogPrintf (log_handler, "Exception at load component '%s': %s", node.name.c_str (), e.what ());
          }
          catch (...)
          {
            LogPrintf (log_handler, "Unknown exception at load component '%s'", node.name.c_str ());
          }
        }
        catch (...)
        {
          //���������� ���� ����������
        }

        node.is_locked = false;  
      }
    }
    
      //�������� ����������
    void Unload (const char* wc_component_mask)
    {
      if (!wc_component_mask)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
      {
        ComponentNode& node = *iter;

        if (!node.is_loaded || node.is_locked)
          continue;

        try
        {
          if (!wcimatch (node.name.c_str (), wc_component_mask))
            continue;

          node.is_locked = true;

          node.component->Unload ();          
        }
        catch (...)
        {
          //���������� ���� ����������
        }

        node.is_loaded = false;
        node.is_locked = false;  
      }
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
      IComponent* component; //��������� �� ���������
      stl::string name;      //��� ����������
      size_t      name_hash; //��� ����� ����������
      bool        is_loaded; //����: �������� �� ���������
      bool        is_locked; //����: ��������� ������������ ��� ������ �����������

      ComponentNode (const char* in_name, IComponent* in_component) : component (in_component), name (in_name, MallocAllocator (&malloc, &free)),
        is_loaded (false), is_locked (false)
      {
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

      const char* Name () { return name.c_str (); }

      bool IsLoaded () { return is_loaded; }
    };
    
  private:
    typedef stl::simple_allocator<char>               MallocAllocator;
    typedef stl::list<ComponentNode, MallocAllocator> ComponentList;

  private:
    ComponentList components;
};

/*
    ComponentManager
*/

typedef Singleton<ComponentManagerImpl, SingletonStatic> ComponentManagerSingleton;

/*
    ����������� ����������
*/

void ComponentManager::Register (const char* name, IComponent* component)
{
  ComponentManagerSingleton::Instance ().Register (name, component);
}

void ComponentManager::Unregister (IComponent* component)
{
  ComponentManagerSingleton::Instance ().Unregister (component);
}

void ComponentManager::Unregister (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ().Unregister (wc_component_mask);
}

/*
    �������� �����������
*/

void ComponentManager::Load (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ().Load (wc_component_mask, LogHandler ());
}

void ComponentManager::Load (const char* wc_component_mask, const LogHandler& log_handler)
{
  ComponentManagerSingleton::Instance ().Load (wc_component_mask, log_handler);
}

void ComponentManager::Unload (const char* wc_component_mask)
{
  ComponentManagerSingleton::Instance ().Unload (wc_component_mask);
}

/*
    �������� ���������
*/

ComponentManager::Iterator ComponentManager::CreateIterator ()
{
  return ComponentManagerSingleton::Instance ().CreateIterator ();
}
