#include "shared.h"

using namespace script;  

namespace
{

/*
    ���������
*/

const char* INTERPRETERS_COMPONENTS_MASK = "script.interpreters.*"; //����� ��� ����������� ���������������

/*
    ���������� ��������� ���������������
*/

class InterpreterManagerImpl
{
  public:
    typedef InterpreterManager::InterpreterCreater InterpreterCreater;
    typedef Shell::EnvironmentPtr                  EnvironmentPtr;
    
      //����������� ��������������
    void Register (const char* name, const InterpreterCreater& creater)
    {
      static const char* METHOD_NAME = "script::InterpreterManagerImpl::Register";
      
      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!creater)
        throw xtl::make_null_argument_exception (METHOD_NAME, "creater");
        
      InterpreterCreaterMap::iterator iter = creaters.find (name);
      
      if (iter == creaters.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Interpreter with this name has already registered");
        
      creaters.insert_pair (name, creater);
    }
    
      //�������� ��������������
    void Unregister (const char* name)
    {
      if (!name)
        return;
      
      creaters.erase (name);
    }
    
      //������� ����� ���������������
    void UnregisterAll ()
    {
      creaters.clear ();
    }
    
      //��������� ��������������
    IInterpreter* CreateInterpreter (const char* name, const EnvironmentPtr& environment)
    {
      static const char* METHOD_NAME = "script::InterpreterManagerImpl::CreateInterpreter";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
        
      if (!environment)
        throw xtl::make_null_argument_exception (METHOD_NAME, "environment");

      InterpreterCreaterMap::iterator iter = creaters.find (name);
      
      if (iter == creaters.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Interpreter not found");

      return iter->second (environment);
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, InterpreterCreater> InterpreterCreaterMap;

  private:
    InterpreterCreaterMap creaters; //����� ��������� �������� ��������� ���������������
};

typedef common::Singleton<InterpreterManagerImpl> InterpreterManagerSingleton;

}

/*
===================================================================================================
    InterpreterManager
===================================================================================================
*/

/*
    ����������� ��������� ��������
*/

void InterpreterManager::RegisterInterpreter (const char* name, const InterpreterCreater& creater)
{
  InterpreterManagerSingleton::Instance ().Register (name, creater);
}

void InterpreterManager::UnregisterInterpreter (const char* name)
{
  InterpreterManagerSingleton::Instance ().Unregister (name);
}

void InterpreterManager::UnregisterAllInterpreters ()
{
  InterpreterManagerSingleton::Instance ().UnregisterAll ();
}

/*
===================================================================================================
    Shell
===================================================================================================
*/

/*
    ������������ / ���������� / ������������
*/

Shell::Shell ()
  : interpreter (0)
{
}

Shell::Shell (const char* interpreter_name, const EnvironmentPtr& environment)
{     
  try
  {
    static common::ComponentLoader default_interpreters_loader (INTERPRETERS_COMPONENTS_MASK);
    
    interpreter = InterpeterPtr (InterpreterManagerSingleton::Instance ().CreateInterpreter (interpreter_name, environment), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Shell::Shell");
    throw;
  }
}

Shell::Shell (const InterpeterPtr& in_interpreter)
  : interpreter (in_interpreter)
{
}

Shell::Shell (const Shell& shell)
  : interpreter (shell.interpreter)
{
}

Shell::~Shell ()
{
}

Shell& Shell::operator = (const Shell& shell)
{
  Shell (shell).Swap (*this);
  
  return *this;
}

/*
    ��������� ����� �������������� / ��������� ��������������
*/

const char* Shell::InterpeterName () const
{
  return interpreter ? interpreter->Name () : "(no interpreter binded)";
}

Shell::InterpeterPtr Shell::Interpeter () const
{
  return interpreter;
}

/*
    ��������� ��������������
*/

void Shell::SetInterpreter (const char* interpreter_name, const EnvironmentPtr& environment)
{
  Shell (interpreter_name, environment).Swap (*this);  
}

void Shell::SetInterpreter (const InterpeterPtr& in_interpreter)
{
  Shell (in_interpreter).Swap (*this);
}

/*
    �������� ������� ������� � ��������� �������������
*/

bool Shell::HasFunction (const char* name) const
{
  if (!interpreter || !name)
    return false;

  return interpreter->HasFunction (name);
}

/*
    ���������� ������
*/

namespace
{

void dummy_log (const char*)
{
}

}

void Shell::ExecuteImpl (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
  static const char* METHOD_NAME = "script::Shell::ExecuteImpl";

  if (!interpreter)
    throw xtl::format_operation_exception (METHOD_NAME, "No interpreter binded");
    
  if (!buffer_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer_name");
    
  if (!buffer && !buffer_size)
    return;
    
  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");
    
  interpreter->DoCommands (buffer_name, buffer, buffer_size, log);
}

void Shell::Execute (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
  ExecuteImpl (buffer_name, buffer, buffer_size, log);
}

void Shell::Execute (const char* buffer_name, const void* buffer, size_t buffer_size)
{
  ExecuteImpl (buffer_name, buffer, buffer_size, &dummy_log);
}

void Shell::Execute (const char* command, const LogFunction& log)
{
  if (!command)
    throw xtl::make_null_argument_exception ("script::Shell::Execute", "command");
    
  ExecuteImpl (command, command, strlen (command), log);
}

void Shell::Execute (const char* command)
{
  Execute (command, &dummy_log);  
}

/*
    ���������� ������, ������������� � ����� / ������
*/

void Shell::ExecuteFile (const char* file_name, const LogFunction& log)
{
  try
  {
    stl::string buffer;
  
    common::FileSystem::LoadTextFile (file_name, buffer);
  
    ExecuteImpl (file_name, buffer.c_str (), buffer.size (), log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Shell::ExecuteFile");
    throw;
  }
}

void Shell::ExecuteFile (const char* file_name)
{
  ExecuteFile (file_name, &dummy_log);
}

void Shell::ExecuteFileList (const char* file_mask, const LogFunction& log)
{
  try
  {
    using namespace common;
    
    for (FileListIterator i = FileSystem::Search (file_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
      ExecuteFile (i->name, log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Shell::ExecuteFileList");
    throw;
  }
}

void Shell::ExecuteFileList (const char* file_mask)
{
  ExecuteFileList (file_mask, &dummy_log);
}

/*
    �����
*/

void Shell::Swap (Shell& shell)
{
  interpreter.swap (shell.interpreter);
}

namespace script
{

void swap (Shell& shell1, Shell& shell2)
{
  shell1.Swap (shell2);
}

}
