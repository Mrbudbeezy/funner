#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
===================================================================================================
    ToolMenuItem
===================================================================================================
*/

/*
    ����������� / ����������
*/

ToolMenuItem::ToolMenuItem (IApplicationServer& in_server)
  : item (gcnew ToolStripMenuItem),
    server (in_server)
{
  item->Click += make_event_handler (this, &ToolMenuItem::OnClick);
}

ToolMenuItem::~ToolMenuItem ()
{
}

/*
    ���������� ������ ������ ����
*/

void ToolMenuItem::OnClick (System::Object^, System::EventArgs^)
{
  if (!on_click_command.empty ())
    server.ExecuteCommand (on_click_command.c_str ());
}

/*
    ��������� ������
*/

void ToolMenuItem::SetText (const stl::string& text)
{
  item->Text = gcnew String (text.c_str ());
}

stl::string ToolMenuItem::Text ()
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::ToolMenuItem::Text");
}

/*
    ���������
*/

void ToolMenuItem::SetTip (const stl::string& text)
{
  item->ToolTipText = gcnew String (text.c_str ());
}

stl::string ToolMenuItem::Tip ()
{
  throw xtl::make_not_implemented_exception ("tools::ui::windows_forms::ToolMenuItem::Tip");
}

/*
    ��������� �������, ����������� ��� ������ ������ ����
*/

void ToolMenuItem::SetOnClickCommand (const stl::string& command)
{
  on_click_command = command;
}

/*
    ���������� ��������
*/

void ToolMenuItem::Insert (const ToolMenuItem::Pointer& sub_item)
{
  item->DropDownItems->Add (sub_item->Handle ());
}

/*
    ����������� ������
*/

void ToolMenuItem::RegisterInvokers (script::Environment&)
{
}

/*
===================================================================================================
    ToolMenuStrip
===================================================================================================
*/

/*
    ����������� / ����������
*/

ToolMenuStrip::ToolMenuStrip ()
  : menu (gcnew MenuStrip)
{
}

ToolMenuStrip::~ToolMenuStrip ()
{
}

/*
    ���������� ������� ����
*/

void ToolMenuStrip::Insert (const ToolMenuItem::Pointer& item)
{
  menu->Items->Add (item->Handle ());  
}

/*
    ����������� ������
*/

void ToolMenuStrip::RegisterInvokers (script::Environment&)
{
}
