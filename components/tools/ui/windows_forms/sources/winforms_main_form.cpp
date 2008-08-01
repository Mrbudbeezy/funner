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

private ref class TestForm: public WeifenLuo::WinFormsUI::DockContent
{
  public:
    TestForm ()
    {
      InitializeComponent ();
    }
    
  private:
    void InitializeComponent ()
    {
      Text        = "Fucking dock!";
      ShowHint    = WeifenLuo::WinFormsUI::DockState::DockRight/*AutoHide*/;
      HideOnClose = true;
    }    
};

private ref class MainFormImpl: public System::Windows::Forms::Form
{
  public:
///�����������
    MainFormImpl ()
    {
      InitializeComponent ();
      IsMdiContainer = true;      
    }

///����������
    ~MainFormImpl ()
    {
    }
    
    void InitializeComponent ()
    {
      dock_panel = gcnew WeifenLuo::WinFormsUI::DockPanel;
      
      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;

      Controls->Add (dock_panel);
      
      form = gcnew TestForm;      

      form->Show (dock_panel);

      ResumeLayout ();
    }

  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //����������� ������
    TestForm^                         form;
};

}

}

}

/*
    ����������� / ����������
*/

MainForm::MainForm (tools::ui::windows_forms::WindowSystem& window_system)
  : Form (window_system)
{
  form = gcnew MainFormImpl;
}

MainForm::~MainForm ()
{
}

/*
    �������� �����
*/

MainForm::Pointer MainForm::Create (tools::ui::windows_forms::WindowSystem& window_system)
{
  return Pointer (new MainForm (window_system), false);
}

/*
    ��������� ref-��������� �����
*/

System::Windows::Forms::Form^ MainForm::Handle ()
{
  return form;
}

/*
    ����������� ������
*/

void MainForm::RegisterInvokers (script::Environment& environment, const char* library_name, const char* base_library_name)
{
  using namespace script;  

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //������������ �� Form

  lib.Register (environment.Library (base_library_name));  

    //����������� ����� ������

  environment.RegisterType<MainForm> (library_name);
}
