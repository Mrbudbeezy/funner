#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    �������� ���������� ������� �����
*/

namespace tools
{

namespace ui
{

namespace windows_forms
{

private ref class ChildFormImpl: public WeifenLuo::WinFormsUI::Docking::DockContent
{
  public:
///�����������
    ChildFormImpl (WindowSystem& in_window_system, ICustomChildWindow* in_child_window) :
      window_system (in_window_system), child_window (in_child_window)
    {
        //������������� ������� �����
      
      Text         = "Fucking dock!";
      ShowHint     = WeifenLuo::WinFormsUI::Docking::DockState::Document;
      HideOnClose  = true;

        //�������� �� ������� �����

      HandleCreated   += gcnew EventHandler (this, &ChildFormImpl::OnCreateHandle);
      HandleDestroyed += gcnew EventHandler (this, &ChildFormImpl::OnDestroyHandle);
      Resize          += gcnew EventHandler (this, &ChildFormImpl::OnResize);

        //��������� ���������� ����������

      UpdateParent ();
      UpdateRect ();

        //����������� ����

      child_window->Show (true);
    }    

  private:
///���������� ������������� ����
    void UpdateParent ()
    {
      try
      {
        try
        {
          child_window->SetParent ((const void*)Handle);
        }
        catch (System::Exception^ exception)
        {
          throw DotNetException ("tools::ui::windows_forms::ChildFormImpl::UpdateParent", exception);
        }
      }
      catch (std::exception& exception)
      {
        window_system.LogPrintf ("%s", exception.what ());
      }
      catch (...)
      {
        window_system.LogPrintf ("Unknown exception at tools::ui::windows_forms::ChildFormImpl::UpdateParent");
      }      
    }

///���������� ��������
    void UpdateRect ()
    {
      try
      {
        try
        {
          System::Drawing::Rectangle^ rect = ClientRectangle;

          child_window->SetPosition (rect->Left, rect->Top);
          child_window->SetSize     (rect->Width, rect->Height);
        }
        catch (System::Exception^ exception)
        {
          throw DotNetException ("tools::ui::windows_forms::ChildFormImpl::UpdateRect", exception);
        }
      }
      catch (std::exception& exception)
      { 
        window_system.LogPrintf ("%s", exception.what ());
      }
      catch (...)
      {
        window_system.LogPrintf ("Unknown exception at tools::ui::windows_forms::ChildFormImpl::UpdateRect");
      }
    }

///��������� ������� �������� ����������� �����
    void OnCreateHandle (System::Object^, EventArgs^)
    {
//      printf ("ChildFormImpl::OnCreateHandle (%p)\n", (const void*)Handle);
      child_window->SetParent ((const void*)Handle);      
    }

///��������� ������� ����������� ����������� �����
    void OnDestroyHandle (System::Object^, EventArgs^)
    {
//      printf ("ChildFormImpl::OnDestroyHandle (%p)\n", (const void*)Handle);      
      child_window->SetParent (0);
    }

///��������� ������� ��������� �������� �����
    void OnResize (Object^, EventArgs^)
    {
      UpdateRect ();
    }        

  private:
    WindowSystem&       window_system;
    ICustomChildWindow* child_window;
};

}

}

}

/*
    ����������� / ����������
*/

ChildForm::ChildForm (tools::ui::windows_forms::WindowSystem& window_system, const char* init_string)
  : Form (window_system)
{
  static const char* METHOD_NAME = "tools::ui::ChildForm::ChildForm";

  try
  {
      //�������� ����������������� ��������� ����

    IApplicationServer& application_server = window_system.ApplicationServer ();

    child_window = ChildWindowPtr (application_server.CreateChildWindow (init_string), false);

      //�������� ����������

    form = gcnew ChildFormImpl (window_system, &*child_window);
    
      //�����������
      
    window_system.MainForm ().Insert (form);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException (METHOD_NAME, exception);
  }
}

ChildForm::~ChildForm ()
{
}

/*
    �������� �����
*/

ChildForm::Pointer ChildForm::Create (tools::ui::windows_forms::WindowSystem& window_system, const char* init_string)
{
  return Pointer (new ChildForm (window_system, init_string), false);
}

/*
    ��������� ref-��������� �����
*/

System::Windows::Forms::Form^ ChildForm::FormHandle ()
{
  return form;
}

/*
    ����������� ������
*/

void ChildForm::RegisterInvokers (script::Environment& environment, const char* library_name, const char* base_library_name)
{
  using namespace script;  

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //������������ �� Form

  lib.Register (environment.Library (base_library_name));  

    //����������� ����� ������

  environment.RegisterType<ChildForm> (library_name);
}
