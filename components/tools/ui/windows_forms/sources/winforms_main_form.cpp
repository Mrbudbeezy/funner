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
ToolFormPtr create_main_form (IApplicationServer* server)
{
  return ToolFormPtr (new ToolForm (server, gcnew MainForm), false);
}

}

}

}
