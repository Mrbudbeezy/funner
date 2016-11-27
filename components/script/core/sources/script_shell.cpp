#include "shared.h"

using namespace script;  

namespace
{

/*
    Константы
*/

const char* INTERPRETERS_COMPONENTS_MASK = "script.interpreters.*"; //маска имён компонентов интерпретаторов

/*
    Реализация менеджера интерпретаторов
*/

class InterpreterManagerImpl
{
  public:
    typedef InterpreterManager::InterpreterCreater InterpreterCreater;
    
      //регистрация интерпретатора
    void Register (const char* name, const InterpreterCreater& creater)
    {
      static const char* METHOD_NAME = "script::InterpreterManagerImpl::Register";
      
      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!creater)
        throw xtl::make_null_argument_exception (METHOD_NAME, "creater");

      InterpreterCreaterMap::iterator iter = creaters.find (name);
      
      if (iter != creaters.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Interpreter with this name has already registered");
        
      stl::pair<InterpreterCreaterMap::iterator, bool> insert_result = creaters.insert_pair (name, creater);
    }
    
      //удаление интерпретатора
    void Unregister (const char* name)
    {
      if (!name)
        return;
      
      creaters.erase (name);
    }
    
      //очистка карты интерпретаторов
    void UnregisterAll ()
    {
      creaters.clear ();
    }
    
      //получение интерпретатора
    IInterpreter* CreateInterpreter (const char* name, const Environment& environment)
    {
      static const char* METHOD_NAME = "script::InterpreterManagerImpl::CreateInterpreter";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
        
      InterpreterCreaterMap::iterator iter = creaters.find (name);
      
      if (iter == creaters.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Interpreter not found");

      return iter->second (environment);
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, InterpreterCreater> InterpreterCreaterMap;

  private:
    InterpreterCreaterMap creaters; //карта функторов создания командных интерпретаторов
};

typedef common::Singleton<InterpreterManagerImpl> InterpreterManagerSingleton;

}

/*
===================================================================================================
    InterpreterManager
===================================================================================================
*/

/*
    Регистрация функторов создания
*/

void InterpreterManager::RegisterInterpreter (const char* name, const InterpreterCreater& creater)
{
  InterpreterManagerSingleton::Instance ()->Register (name, creater);
}

void InterpreterManager::UnregisterInterpreter (const char* name)
{
  InterpreterManagerSingleton::Instance ()->Unregister (name);
}

void InterpreterManager::UnregisterAllInterpreters ()
{
  InterpreterManagerSingleton::Instance ()->UnregisterAll ();
}

/*
===================================================================================================
    Shell
===================================================================================================
*/

/*
    Конструкторы / деструктор / присваивание
*/

Shell::Shell ()
  : interpreter (0)
{
}

Shell::Shell (const char* interpreter_name, const Environment& environment)
{     
  try
  {
    static common::ComponentLoader default_interpreters_loader (INTERPRETERS_COMPONENTS_MASK);
    
    interpreter = InterpeterPtr (InterpreterManagerSingleton::Instance ()->CreateInterpreter (interpreter_name, environment), false);
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
    Получение имени интерптератора / получение интерпретатора
*/

const char* Shell::InterpreterName () const
{
  return interpreter ? interpreter->Name () : "(no interpreter binded)";
}

Shell::InterpeterPtr Shell::Interpreter () const
{
  return interpreter;
}

/*
    Изменение интерпретатора
*/

void Shell::SetInterpreter (const char* interpreter_name, const Environment& environment)
{
  Shell (interpreter_name, environment).Swap (*this);  
}

void Shell::SetInterpreter (const InterpeterPtr& in_interpreter)
{
  Shell (in_interpreter).Swap (*this);
}

/*
    Проверка наличия функции в контексте интерпретации
*/

bool Shell::HasFunction (const char* name) const
{
  if (!interpreter || !name)
    return false;

  return interpreter->HasFunction (name);
}

/*
    Выполнение команд
*/

void Shell::ExecuteImpl (const char* buffer_name, const void* buffer, size_t buffer_size)
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
    
  interpreter->DoCommands (buffer_name, buffer, buffer_size);
}

void Shell::Execute (const char* buffer_name, const void* buffer, size_t buffer_size)
{
  ExecuteImpl (buffer_name, buffer, buffer_size);
}

void Shell::Execute (const char* command)
{
  if (!command)
    throw xtl::make_null_argument_exception ("script::Shell::Execute", "command");
    
  ExecuteImpl (command, command, strlen (command));
}

/*
    Выполнение команд, расположенных в файле / файлах
*/

void Shell::ExecuteFile (const char* file_name)
{
  try
  {
    stl::string buffer;

    common::FileSystem::LoadTextFile (file_name, buffer);

    ExecuteImpl (file_name, buffer.c_str (), buffer.size ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Shell::ExecuteFile");
    throw;
  }
}

void Shell::ExecuteFileList (const char* file_mask)
{
  try
  {
    using namespace common;
    
    for (FileListIterator i = FileSystem::Search (file_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
      ExecuteFile (i->name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Shell::ExecuteFileList");
    throw;
  }
}

/*
    Обмен
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
