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
const char* CHILD_FORM_LIBRARY_NAME                   = "ChildForm";            //��� ���������� ������ �������� ����
const char* DOCK_STATE_LIBRARY_NAME                   = "static.DockState";     //��� ���������� ������������ ���������� ��������� ����
const char* MENU_STRIP_ITEM_LIBRARY_NAME              = "MenuItem";             //��� ���������� ������ ��������� ����
const char* MENU_STRIP_LIBRARY_NAME                   = "MenuStrip";            //��� ���������� ������ ������� ����
const char* TOOL_STRIP_BUTTON_LIBRARY_NAME            = "ToolButton";           //��� ���������� ������ ������
const char* TOOL_STRIP_LIBRARY_NAME                   = "ToolStrip";            //��� ���������� ������ ������� ������
const char* MENU_STRIP_ITEM_COLLECTION_LIBRARY_NAME   = "MenuItemCollection";   //��� ���������� ������ ��������� ��������� ����
const char* MENU_STRIP_COLLECTION_LIBRARY_NAME        = "MenuStripCollection";  //��� ���������� ������ ��������� ������� ����
const char* TOOL_STRIP_BUTTON_COLLECTION_LIBRARY_NAME = "ToolButtonCollection"; //��� ���������� ������ ��������� ������
const char* TOOL_STRIP_COLLECTION_LIBRARY_NAME        = "ToolStripCollection";  //��� ���������� ������ ��������� ������� ����

/*
    ������� ��� ������� ���������� Windows Forms
*/

class ApplicationModelWrapper
{
  public:
    static void RegisterWrapper   () { RegisterWrapper (true); }    
    static void UnregisterWrapper () { RegisterWrapper (false); }

  private:
    ApplicationModelWrapper ()
      : on_do_events_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnDoEvents, &ApplicationModelWrapper::DoEvents))
    {
        //��������������� COM ��� ������ � .NET ����������� ������ (� ������� ���� ��� ����� ��������� � SystemLib!)

      CoUninitialize ();
      CoInitializeEx (0, COINIT_APARTMENTTHREADED);
    }  

    static void DoEvents ()
    {
        //��������� ������� Windows Forms
      
      System::Windows::Forms::Application::DoEvents ();

        //��������� ��������� ������� ��� ��������� � System::Windows::Forms::Application::DoEvents      

      syslib::Application::CancelSystemEventsProcess ();
    }

    static void RegisterWrapper (bool state)
    {
      static stl::auto_ptr<ApplicationModelWrapper> current_wrapper        = 0; //��������� �� ������� �������
      static size_t                                 current_register_count = 0; //���������� �����������

      if (state)
      {        
        if (!current_register_count++)
          current_wrapper = new ApplicationModelWrapper;          
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
    shell (INTERPERTER_NAME, shell_environment),
    next_child_form_uid (0)
{
  try
  {
      //����������� ������� ��������� ��������� Window Forms
      
    ApplicationModelWrapper::RegisterWrapper ();
    
      //�������� ������� �����
      
    main_form = tools::ui::windows_forms::MainForm::Create (*this);

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

  ApplicationModelWrapper::UnregisterWrapper ();
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

[STAThreadAttribute]
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

[STAThreadAttribute]
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
    �������� �������� �����
*/

ChildForm::Pointer WindowSystem::CreateChildForm (const char* id, const char* init_string, FormDockState dock_state)
{
  try
  {
    if (!id)
      throw xtl::make_null_argument_exception ("", "id");

    ChildForm::Pointer form = ChildForm::Create (*this, init_string, dock_state);

    child_forms.Register (id, form);

    return form;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::CreateChildForm");
    throw;
  }
}

ChildForm::Pointer WindowSystem::CreateChildForm (const char* init_string, FormDockState dock_state)
{
  try
  {
    if (++next_child_form_uid == 0)
      throw xtl::format_operation_exception ("tools::ui::WindowSystem::CreateChildForm", "No free UID's");

    return CreateChildForm (common::format ("Form%u", next_child_form_uid).c_str (), init_string, dock_state);
  }
  catch (...)
  {
    --next_child_form_uid;    
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

    Form::RegisterInvokers                               (*shell_environment, FORM_LIBRARY_NAME);
    tools::ui::windows_forms::MainForm::RegisterInvokers (*shell_environment, FORM_LIBRARY_NAME, MAIN_FORM_LIBRARY_NAME);
    ChildForm::RegisterInvokers                          (*shell_environment, FORM_LIBRARY_NAME, CHILD_FORM_LIBRARY_NAME, DOCK_STATE_LIBRARY_NAME);
    MenuStripItem::RegisterInvokers                      (*shell_environment, MENU_STRIP_ITEM_LIBRARY_NAME);
    MenuStrip::RegisterInvokers                          (*shell_environment, MENU_STRIP_LIBRARY_NAME);
    MenuStripItemRegistry::RegisterInvokers              (*shell_environment, MENU_STRIP_ITEM_COLLECTION_LIBRARY_NAME);
    MenuStripRegistry::RegisterInvokers                  (*shell_environment, MENU_STRIP_COLLECTION_LIBRARY_NAME);
    ToolStripButton::RegisterInvokers                    (*shell_environment, TOOL_STRIP_BUTTON_LIBRARY_NAME);
    ToolStrip::RegisterInvokers                          (*shell_environment, TOOL_STRIP_LIBRARY_NAME);
    ToolStripButtonRegistry::RegisterInvokers            (*shell_environment, TOOL_STRIP_BUTTON_COLLECTION_LIBRARY_NAME);
    ToolStripRegistry::RegisterInvokers                  (*shell_environment, TOOL_STRIP_COLLECTION_LIBRARY_NAME);

      //�������� ���������� ����� �����

    InvokerRegistry& lib = shell_environment->Library ("static.Application");

      //����������� �������

    lib.Register ("get_MainForm",    make_const (main_form));
    lib.Register ("get_MenuItems",   make_const (xtl::ref (menu_strip_items)));
    lib.Register ("get_MenuStrips",  make_const (xtl::ref (menu_strips)));
    lib.Register ("get_ToolButtons", make_const (xtl::ref (tool_strip_buttons)));
    lib.Register ("get_ToolStrips",  make_const (xtl::ref (tool_strips)));
    lib.Register ("get_Forms",       make_const (xtl::ref (child_forms)));

      //����������� �������

    lib.Register ("LoadConfiguration", make_invoker<void (const char*)> (xtl::bind (&WindowSystem::LoadConfiguration, this, _1)));
    lib.Register ("Execute", make_invoker<void (const char*)> (xtl::bind (&IApplicationServer::ExecuteCommand, &*application_server, _1)));
    lib.Register ("CreateForm", make_invoker (
      make_invoker<void (const char*, const char*, FormDockState)> (xtl::bind (xtl::implicit_cast<ChildForm::Pointer (WindowSystem::*)(const char*, const char*, FormDockState)> (&WindowSystem::CreateChildForm), this, _1, _2, _3)),
      make_invoker<void (const char*, FormDockState)> (xtl::bind (xtl::implicit_cast<ChildForm::Pointer (WindowSystem::*)(const char*, FormDockState)> (&WindowSystem::CreateChildForm), this, _1, _2)),
      make_invoker<void (const char*, const char*)> (xtl::bind (xtl::implicit_cast<ChildForm::Pointer (WindowSystem::*)(const char*, const char*, FormDockState)> (&WindowSystem::CreateChildForm), this, _1, _2, FormDockState_Default)),
      make_invoker<void (const char*)> (xtl::bind (xtl::implicit_cast<ChildForm::Pointer (WindowSystem::*)(const char*, FormDockState)> (&WindowSystem::CreateChildForm), this, _1, FormDockState_Default))
    ));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::windows_forms::WindowSystem::RegisterInvokers");
    throw;
  }
}
