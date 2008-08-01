#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

namespace
{

/*
    ���������
*/

const char* INTERPERTER_NAME                          = "lua";                  //��� ��������������
const char* FORM_LIBRARY_NAME                         = "Form";                 //��� ���������� ������ �����
const char* MAIN_FORM_LIBRARY_NAME                    = "MainForm";             //��� ���������� ������ ������� �����
const char* MENU_STRIP_ITEM_LIBRARY_NAME              = "MenuItem";             //��� ���������� ��������� ����
const char* MENU_STRIP_LIBRARY_NAME                   = "MenuStrip";            //��� ���������� ������� ����
const char* TOOL_STRIP_BUTTON_LIBRARY_NAME            = "ToolButton";           //��� ���������� ������
const char* TOOL_STRIP_LIBRARY_NAME                   = "ToolStrip";            //��� ���������� ������� ������
const char* MENU_STRIP_ITEM_COLLECTION_LIBRARY_NAME   = "MenuItemCollection";   //��� ���������� ��������� ��������� ����
const char* MENU_STRIP_COLLECTION_LIBRARY_NAME        = "MenuStripCollection";  //��� ���������� ��������� ������� ����
const char* TOOL_STRIP_BUTTON_COLLECTION_LIBRARY_NAME = "ToolButtonCollection"; //��� ���������� ��������� ������
const char* TOOL_STRIP_COLLECTION_LIBRARY_NAME        = "ToolStripCollection";  //��� ���������� ��������� ������� ����

/*
    ������� ��� �������� ��������� ��������� Windows Forms
*/

class MessageQueueWrapper
{
  public:
    static void RegisterWrapper   () { RegisterWrapper (true); }    
    static void UnregisterWrapper () { RegisterWrapper (false); }

  private:
    MessageQueueWrapper ()
      : on_do_events_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnDoEvents, &MessageQueueWrapper::DoEvents)) {}  
  
    static void DoEvents ()
    {
      System::Windows::Forms::Application::DoEvents ();
    }

    static void RegisterWrapper (bool state)
    {
      static stl::auto_ptr<MessageQueueWrapper> current_wrapper        = 0; //��������� �� ������� �������
      static size_t                             current_register_count = 0; //���������� �����������

      if (state)
      {
        if (!current_register_count++)
          current_wrapper = new MessageQueueWrapper;
      }
      else
      {
        if (!current_register_count)
          return;

        if (!--current_register_count)
          current_wrapper = 0;
      }
    }

  private:
    xtl::connection on_do_events_connection; //���������� � �������� ��������� ������� ����
};

}

/*
    ����������� / ����������
*/

WindowSystem::WindowSystem (IApplicationServer* server)
  : application_server (server),
    shell_environment (new script::Environment),
    shell (INTERPERTER_NAME, shell_environment)
{
  try
  {
      //����������� ������� ��������� ��������� Window Forms
      
    MessageQueueWrapper::RegisterWrapper ();
    
      //�������� ������� �����
      
    main_form = MainForm::Create (*this);

    main_form->Show ();        
    
      //����������� ������
      
    RegisterInvokers ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::WindowSystem");
    throw;
  }
}

WindowSystem::~WindowSystem ()
{
    //������ ����������� ������� ��������� ��������� Windows Forms

  MessageQueueWrapper::UnregisterWrapper ();
}

/*
    ������� ������
*/

void WindowSystem::AddRef ()
{
  addref (this);
}

void WindowSystem::Release ()
{
  release (this);
}

/*
    ���������� ������� �� ������� ������� �������
*/

void WindowSystem::Execute (const char* name, const void* buffer, size_t buffer_size)
{
  try
  {        
    shell.Execute (name, buffer, buffer_size, log_handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::WindowSystem::Execute(const char*, const void*, size_t)");
    throw;
  }
}

void WindowSystem::Execute (const char* command)
{
  try
  {
    shell.Execute (command, log_handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::WindowSystem::Execute(const char*)");
    throw;
  }  
}

/*
    ����������������
*/

void WindowSystem::SetLogHandler (const LogFunction& log)
{
  log_handler = log;
}

void WindowSystem::LogMessage (const char* message)
{
  try
  {
    if (!message)
      return;

    log_handler (message);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void WindowSystem::LogVPrintf (const char* format, va_list list)
{
  if (!format)
    return;
    
  try
  {
    log_handler (common::vformat (format, list).c_str ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void WindowSystem::LogPrintf (const char* format, ...)
{
  va_list list;

  va_start (list, format);

  LogVPrintf (format, list);
}

/*
    �������� ������ ������������
*/

void WindowSystem::LoadConfiguration (const char* file_name_mask)
{
  using namespace common;

  if (!file_name_mask)
    throw xtl::make_null_argument_exception ("tools::ui::WindowSystem::LoadConfiguration", "file_name_mask");

  for (FileListIterator i=FileSystem::Search (file_name_mask, FileSearch_Files | FileSearch_Sort); i; ++i)
    ConfigurationParser (i->name, *this);
}

/*
    ����������� ������
*/

void WindowSystem::RegisterInvokers ()
{
  try
  {
    using namespace script;    

      //����������� ������ ���������

    Form::RegisterInvokers                    (*shell_environment, FORM_LIBRARY_NAME);
    MainForm::RegisterInvokers                (*shell_environment, MAIN_FORM_LIBRARY_NAME, FORM_LIBRARY_NAME);
    MenuStripItem::RegisterInvokers           (*shell_environment, MENU_STRIP_ITEM_LIBRARY_NAME);
    MenuStrip::RegisterInvokers               (*shell_environment, MENU_STRIP_LIBRARY_NAME);
    MenuStripItemRegistry::RegisterInvokers   (*shell_environment, MENU_STRIP_ITEM_COLLECTION_LIBRARY_NAME);
    MenuStripRegistry::RegisterInvokers       (*shell_environment, MENU_STRIP_COLLECTION_LIBRARY_NAME);
    ToolStripButton::RegisterInvokers         (*shell_environment, TOOL_STRIP_BUTTON_LIBRARY_NAME);
    ToolStrip::RegisterInvokers               (*shell_environment, TOOL_STRIP_LIBRARY_NAME);
    ToolStripButtonRegistry::RegisterInvokers (*shell_environment, TOOL_STRIP_BUTTON_COLLECTION_LIBRARY_NAME);
    ToolStripRegistry::RegisterInvokers       (*shell_environment, TOOL_STRIP_COLLECTION_LIBRARY_NAME);

      //�������� ���������� ����� �����

    InvokerRegistry& lib = shell_environment->Library ("static.Application");

      //����������� �������

    lib.Register ("get_MainForm",    make_const (main_form));
    lib.Register ("get_MenuItems",   make_const (xtl::ref (menu_strip_items)));
    lib.Register ("get_MenuStrips",  make_const (xtl::ref (menu_strips)));
    lib.Register ("get_ToolButtons", make_const (xtl::ref (tool_strip_buttons)));
    lib.Register ("get_ToolStrips",  make_const (xtl::ref (tool_strips)));

      //����������� �������

    lib.Register ("LoadConfiguration", make_invoker<void (const char*)> (xtl::bind (&WindowSystem::LoadConfiguration, this, _1)));
    lib.Register ("Execute", make_invoker<void (const char*)> (xtl::bind (&IApplicationServer::ExecuteCommand, &*application_server, _1)));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::RegisterInvokers");
    throw;
  }
}
