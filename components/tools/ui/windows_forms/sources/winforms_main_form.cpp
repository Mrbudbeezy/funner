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

private ref class MainFormImpl: public System::Windows::Forms::Form
{
  public:
///�����������
    MainFormImpl ()
    {
      dock_panel = gcnew WeifenLuo::WinFormsUI::Docking::DockPanel;
      
      IsMdiContainer = true;

      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;

      Controls->Add (dock_panel);      

      ResumeLayout ();
    }
    
///����������� ������
    WeifenLuo::WinFormsUI::Docking::DockPanel^ DockPanel () { return dock_panel; }

  private:
    WeifenLuo::WinFormsUI::Docking::DockPanel^ dock_panel; //����������� ������
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
  try
  {
    form = gcnew MainFormImpl;
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MainForm::MainForm", exception);
  }
}

MainForm::~MainForm ()
{
  try
  {
    form = 0;
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    �������� �����
*/

MainForm::Pointer MainForm::Create (tools::ui::windows_forms::WindowSystem& window_system)
{
  return Pointer (new MainForm (window_system), false);
}

/*
    ���������� �������� �����
*/

void MainForm::Insert (WeifenLuo::WinFormsUI::Docking::DockContent^ sub_form)
{
  try
  {
    sub_form->Show (form->DockPanel ());
  }
  catch (System::Exception^ exception)
  {
    throw DotNetException ("tools::ui::windows_forms::MainForm::Insert", exception);
  }  
}

/*
    ��������� ref-��������� �����
*/

System::Windows::Forms::Form^ MainForm::FormHandle ()
{
  return form;
}

/*
    ����������� ������
*/

void MainForm::RegisterInvokers (script::Environment& environment, const char* base_library_name, const char* library_name)
{
  using namespace script;  

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //������������ �� Form

  lib.Register (environment.Library (base_library_name));  

    //����������� ����� ������

  environment.RegisterType<MainForm> (library_name);
}
