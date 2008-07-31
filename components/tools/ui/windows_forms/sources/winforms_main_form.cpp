#include "shared.h"

namespace
{

/*
    ������� �����
*/

private ref class MainForm: public System::Windows::Forms::Form
{
  public:
///�����������
    MainForm ()
    {
      IsMdiContainer = true;      
      dock_panel     = gcnew WeifenLuo::WinFormsUI::DockPanel;
      
      SuspendLayout ();

      dock_panel->ActiveAutoHideContent = nullptr;
      dock_panel->Dock                  = System::Windows::Forms::DockStyle::Fill;
           

      Controls->Add (dock_panel);
      
      System::Windows::Forms::Form^ form = gcnew Form;
      
//      form->Dock = System::Windows::Forms::DockStyle::Fill;      
//      form->MdiParent = dock_panel;
      
//      form->Show ();      
      form->Show (dock_panel);           
      
      ResumeLayout (false);
    }

///����������
    ~MainForm ()
    {
    }
    
  private:
    WeifenLuo::WinFormsUI::DockPanel^ dock_panel; //����������� ������
};

}

namespace tools
{

namespace ui
{

namespace windows_forms
{

///�������� ������� �����
Form::Pointer create_main_form (WindowSystem& window_system)
{
  return Form::Pointer (new Form (window_system, gcnew MainForm), false);
}

}

}

}
