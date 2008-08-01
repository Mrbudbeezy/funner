#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    ����������� / ����������
*/

Form::Form (tools::ui::windows_forms::WindowSystem& in_window_system)
  : window_system (in_window_system),
    menu_strip (0)
{
}

Form::~Form ()
{
}

/*
    ��������� ������ �����
*/

void Form::SetText (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("tools::ui::windows_forms::Form::SetText", "text");

  FormHandle ()->Text = gcnew String (text);
}

const char* Form::Text ()
{
  return get_string (FormHandle ()->Text, text);
}

/*
    ���������� ���������� �����
*/

void Form::Show ()
{
  FormHandle ()->Show ();
}

void Form::Hide ()
{
  FormHandle ()->Hide ();
}

void Form::SetVisible (bool state)
{
  FormHandle ()->Visible = state;
}

bool Form::IsVisible ()
{
  return FormHandle ()->Visible;
}

/*
    ��������� ����
*/

void Form::SetMenuStrip (tools::ui::windows_forms::MenuStrip* in_menu_strip)
{
  if (menu_strip)
    FormHandle ()->Controls->Remove (menu_strip->Handle ());

  menu_strip = in_menu_strip;

  if (menu_strip)
    FormHandle ()->Controls->Add (menu_strip->Handle ());
}

/*
    ���������� / �������� ������ ������
*/

void Form::Insert (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    throw xtl::make_null_argument_exception ("tools::ui::Form::Insert(tools::ui::windows_forms::ToolStrip*)", "tool_strip");
    
  FormHandle ()->Controls->Add (tool_strip->Handle ());
}

void Form::Remove (windows_forms::ToolStrip* tool_strip)
{
  if (!tool_strip)
    return;
    
  FormHandle ()->Controls->Add (tool_strip->Handle ());
}

/*
    �������������� ���������� ����
*/

const void* Form::WindowHandle ()
{
  return (const void*)FormHandle ()->Handle;
}

/*
    ����������� ������
*/

void Form::RegisterInvokers (script::Environment& environment, const char* library_name)
{
  using namespace script;

  InvokerRegistry& lib = environment.CreateLibrary (library_name);

    //����������� �������

  lib.Register ("set_Text", make_invoker (&Form::SetText));
  lib.Register ("get_Text", make_invoker (&Form::Text));
  lib.Register ("set_Visible", make_invoker (&Form::SetVisible));
  lib.Register ("get_Visible", make_invoker (&Form::IsVisible));
  lib.Register ("set_MenuStrip", make_invoker (&Form::SetMenuStrip));
  lib.Register ("get_MenuStrip", make_invoker (&Form::MenuStrip));  

    //����������� �������

  lib.Register ("Add", make_invoker (&Form::Insert));
  lib.Register ("Remove", make_invoker (&Form::Remove));

    //����������� ����� ������

  environment.RegisterType<Form> (library_name);
}
