#include "shared.h"

namespace
{

/*
    ������� �����
*/

private ref class MainForm: public Form
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
ToolForm::Pointer create_main_form (IApplicationServer* server)
{
  return ToolForm::Pointer (new ToolForm (server, gcnew MainForm), false);
}

}

}

}
