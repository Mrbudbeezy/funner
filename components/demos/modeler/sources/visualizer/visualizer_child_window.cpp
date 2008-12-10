#include "shared.h"

const char* CONFIGURATION_FILE_NAME = "media/conf/form_config.xml";

//�������� ���������������� �������� ����

///�����������

MyChildWindow::MyChildWindow ()
  : window (syslib::WindowStyle_PopUp, 1024, 768)
{
//  if (AttachmentRegistry::Find<syslib::Window> ("MainWindow"))
//    throw xtl::format_operation_exception ("MyChildWindow::MyChildWindow", "Window with attachment name 'MainWindow' already registered");

  engine::AttachmentRegistry::Register ("MainWindow", xtl::ref (window));

  manager.Start (CONFIGURATION_FILE_NAME);
}

///��������� ��������� ����

void MyChildWindow::SetPosition (size_t x, size_t y)
{
  try
  {
    window.SetPosition (x, y);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("MyChildWindow::SetPosition");
    throw;
  }
}

///��������� �������� ����

void MyChildWindow::SetSize (size_t width, size_t height)
{
  try
  {
    window.SetSize (width, height);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("MyChildWindow::SetSize");
    throw;
  }
}

///��������� ������������� ����

void MyChildWindow::SetParent (const void* parent_window_handle)
{
  try
  {
    window.SetParentHandle (parent_window_handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("MyChildWindow::SetParent");
    throw;
  }
}

///���������� ���������� ����

void MyChildWindow::Show (bool state)
{
  try
  {
    window.SetVisible (state);

    if (state)
      window.Invalidate ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("MyChildWindow::Show");
    throw;
  }
}
