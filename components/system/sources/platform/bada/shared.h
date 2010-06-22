#ifndef SYSTEMLIB_BADA_SHARED_HEADER
#define SYSTEMLIB_BADA_SHARED_HEADER

#include <climits>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/singleton.h>

#include <syslib/window.h>
#include <syslib/platform/bada.h>

#include <platform/platform.h>

#include <FBase.h>
#include <FApp.h>
#include <FUi.h>

namespace syslib
{

namespace bada
{

//��������� ���������� ����������
void raise (const char* source, result r);

//��������� ���������� �������� ����������
Osp::App::Application& get_application ();

}

}

//��������� ����� ���������� ��� ������ OspMain
extern "C" int get_osp_main_argc ();

//��������� ��������� �� ������ ���������� OspMain
extern "C" const char** get_osp_main_argv ();


#endif
