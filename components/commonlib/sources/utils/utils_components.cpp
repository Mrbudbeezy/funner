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
    ComponentManagerImpl () : components (stl::simple_allocator<ComponentNode> (&malloc, &free)) {}

      //����������
    ~ComponentManagerImpl ()
    {
      Unload ("*");
    }

      //����������� ����������
    void Register (IComponent* component)
    {
      if (!component)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
        if (iter->component == component)
          return;

      components.push_back (ComponentNode (component));
    }

      //������ ����������� ����������
    void Unregister (IComponent* component)
    {
      if (!component)
        return;

      for (ComponentList::iterator iter=components.begin (), end=components.end (); iter != end; ++iter)
      {
        ComponentNode& node = *iter;
        
        if (node.component == component && !node.is_locked)
        {
          if (node.is_loaded)
          {
            try
            {
              node.is_locked = true;
              
              node.component->Unload ();
            }
            catch (...)
            {
              //���������� ���� ����������
            }
          }

          components.erase (iter);

          return;
        }
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
          const char* name = node.component->Name ();

          if (!wcimatch (name, wc_component_mask))
            continue;

          node.is_locked = true;

          try
          {        
            node.component->Load ();

            LogPrintf (log_handler, "Component '%s' loaded successull", name);
            
            node.is_loaded = true;
          }
          catch (std::exception& e)
          {
            LogPrintf (log_handler, "Exception at load component '%s': %s", name, e.what ());
          }
          catch (...)
          {
            LogPrintf (log_handler, "Unknown exception at load component '%s'", name);
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
          const char* name = node.component->Name ();

          if (!wcimatch (name, wc_component_mask))
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
      bool        is_loaded; //����: �������� �� ���������
      bool        is_locked; //����: ��������� ������������ ��� ������ �����������

      ComponentNode (IComponent* in_component) : component (in_component), is_loaded (false), is_locked (false) {}

      const char* Name () { return component->Name (); }

      bool IsLoaded () { return is_loaded; }
    };
    
  private:
    typedef stl::list<ComponentNode, stl::simple_allocator<ComponentNode> > ComponentList;

  private:
    ComponentList components;
};

/*
===================================================================================================
    ComponentManager
===================================================================================================
*/

typedef Singleton<ComponentManagerImpl, SingletonStatic> ComponentManagerSingleton;

/*
    ����������� ����������
*/

void ComponentManager::Register (IComponent* component)
{
  ComponentManagerSingleton::Instance ().Register (component);
}

void ComponentManager::Unregister (IComponent* component)
{
  ComponentManagerSingleton::Instance ().Unregister (component);
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

/*
===================================================================================================
    AutoRegisteredComponent
===================================================================================================
*/

AutoRegisteredComponent::AutoRegisteredComponent ()
{
  try
  {
    ComponentManager::Register (this);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

AutoRegisteredComponent::~AutoRegisteredComponent ()
{
  try
  {
    ComponentManager::Unregister (this);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
===================================================================================================
    ComponentLoader
===================================================================================================
*/

ComponentLoader::ComponentLoader (const char* wc_mask)
{
  ComponentManager::Load (wc_mask);
}
