#include "shared.h"

namespace
{

/*
    ������� �����
*/

private ref class TestForm: public WeifenLuo::WinFormsUI::DockContent
{
  public:
    TestForm ()
    {
      InitializeComponent ();
    }

    ~TestForm ()
    {
      if (components)
      {
        delete components;
      }
    }
    
  private:
    System::ComponentModel::Container^ components;

    void InitializeComponent ()
    {
      components = gcnew System::ComponentModel::Container;

      Text = "Fucking dock!";
      ShowHint = WeifenLuo::WinFormsUI::DockState::DockRight/*AutoHide*/;
      HideOnClose = true;

      ResumeLayout (false);
    }    
};

private ref class MainForm: public System::Windows::Forms::Form
{
  public:
///�����������
    MainForm ()
    {
      components = gcnew System::ComponentModel::Container;
      dock_panel = gcnew WeifenLuo::WinFormsUI::DockPanel;
      form       = gcnew TestForm;

      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;

      form->Show (dock_panel);

      IsMdiContainer = true;
      MinimumSize = System::Drawing::Size (640, 480);
      StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
      Controls->Add (dock_panel);

      ResumeLayout (false);
      PerformLayout ();
    }

///����������
    ~MainForm ()
    {
      if (components)
      {
        delete components;
      }
    }

    void ShowDock ()
    {
      printf ("MainForm::ShowDock\n");
      form->Show (dock_panel);
    }    

  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //����������� ������
    System::ComponentModel::Container^ components;
    TestForm^ form;
};

}

namespace tools
{

namespace ui
{

namespace windows_forms
{

//��������
MainForm^ get_main_form (Form& form)
{
  return (MainForm^)((System::Windows::Forms::Form^)form.Handle ());
}

void show_dock (Form& form)
{
  get_main_form (form)->ShowDock ();
}

void register_main_form_invokers (script::Environment& env, Form& form)
{
  using namespace script;

  InvokerRegistry& lib = env.Library ("global");
  
  lib.Register ("dbgShowDock", make_invoker<void ()> (xtl::bind (&show_dock, xtl::ref (form))));
}

///�������� ������� �����
Form::Pointer create_main_form (WindowSystem& window_system)
{  
  Form::Pointer form (new Form (window_system, gcnew MainForm), false);
  
    //��������
  register_main_form_invokers (window_system.ShellEnvironment (), *form);
  
  return form;
}

}

}

}
