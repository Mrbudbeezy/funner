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
    }

///����������
    ~MainForm ()
    {
    }
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
