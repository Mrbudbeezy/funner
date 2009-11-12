#include "shared.h"

using namespace script;

namespace
{

/*
    ���������
*/

const char* COMPONENTS_MASK = "script.binds.*"; //����� ��� ����������� ���������� ���������

/*
    ���������� ��������� ���������
*/

class LibraryManagerImpl
{
  public:
    typedef LibraryManager::BindHandler BindHandler;
  
///����������� ����������
    void RegisterLibrary (const char* name, const BindHandler& binder)
    {
      static const char* METHOD_NAME = "script::LibraryManager::RegisterLibrary";

        //�������� ������������ ����������

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
        
      for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
        if (iter->name == name)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Library has already registered");

        //���������� ����������

      libraries.push_back (Library (name, binder));  
    }

///������ ����������� ����������
    void UnregisterLibrary (const char* name)
    {
      if (!name)
        return;

      for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
        if (iter->name == name)
        {
          libraries.erase (iter);
          return;
        }
    }

///������ ����������� ���� ���������
    void UnregisterAllLibraries ()
    {
      libraries.clear ();
    }
    
///������� ���������
    void BindLibraries (Environment& environment, const char* library_mask)
    {
      try
      {
        if (!library_mask)
          throw xtl::make_null_argument_exception ("", "library_mask");
          
        static common::ComponentLoader loader (COMPONENTS_MASK);

        for (LibraryList::iterator iter=libraries.begin (), end=libraries.end (); iter!=end; ++iter)
          if (common::wcmatch (iter->name.c_str (), library_mask))
            iter->binder (environment);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("script::LibraryManager::BindLibraries");
        throw;
      }
    }

  private:
///�������� ����������
    struct Library
    {
      stl::string name;   //��� ����������
      BindHandler binder; //������� �������� ����������

      Library (const char* in_name, const BindHandler& in_binder) : name (in_name), binder (in_binder) {}
    };

    typedef stl::list<Library> LibraryList;

  private:
    LibraryList libraries; //����������
};

typedef common::Singleton<LibraryManagerImpl> LibraryManagerSingleton;

}

/*
    �������� ��� �������� � LibraryManager
*/

void LibraryManager::RegisterLibrary (const char* name, const BindHandler& binder)
{
  LibraryManagerSingleton::Instance ()->RegisterLibrary (name, binder);
}

void LibraryManager::UnregisterLibrary (const char* name)
{
  LibraryManagerSingleton::Instance ()->UnregisterLibrary (name);
}

void LibraryManager::UnregisterAllLibraries ()
{
  LibraryManagerSingleton::Instance ()->UnregisterAllLibraries ();
}

void LibraryManager::BindLibraries (Environment& environment, const char* library_mask)
{
  LibraryManagerSingleton::Instance ()->BindLibraries (environment, library_mask);
}
