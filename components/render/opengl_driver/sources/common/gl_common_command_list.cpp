#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    ���������
*/

const size_t COMMAND_ARRAY_RESERVE_SIZE = 16; //������������� ����� ������ � ������

/*
    �����, ���������� ���������� ������
*/

/*struct DisplayListCaller: public xtl::reference_counter, public ICommandListExecuter
{
  int display_list; //����� ����������� ������

///�����������
  DisplayListCaller (int in_display_list) : display_list (in_display_list) {}

///����������
  ~DisplayListCaller ()
  {
    glDeleteLists (display_list, 1);
  }

///���������� ������
  void ExecuteCommands ()
  {
    glCallList (display_list);
  }

///������� ������
  void AddRef () { addref (this); }
  void Release () { release (this); }
};*/

}

/*
    �������� ���������� ����������� ������ ������
*/

typedef stl::vector<detail::ICommand*> CommandArray;

struct CommandListBuilder::Impl: public xtl::reference_counter, public ICommandListExecuter
{
  CommandArray commands; //�������

///�����������
  Impl ()
  {
    commands.reserve (COMMAND_ARRAY_RESERVE_SIZE);
  }

///����������
  ~Impl ()
  {
    DeleteCommands ();
  }
  
///�������� ������
  void DeleteCommands ()
  {
    for (CommandArray::iterator iter=commands.begin (), end=commands.end (); iter!=end; ++iter)
      delete *iter;

    commands.clear ();
  }

///���������� ������� ������
  void ExecuteCommands ()
  {
    try
    {
      for (CommandArray::iterator iter=commands.begin (), end=commands.end (); iter!=end; ++iter)
      {
        (*iter)->Execute ();
      }
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

///������� ������
  void AddRef () { addref (this); }
  void Release () { release (this); }
};

/*
    ����������� / ����������
*/

CommandListBuilder::CommandListBuilder ()
  : impl (0)
{
}

CommandListBuilder::~CommandListBuilder ()
{
  if (impl)
    release (impl);
}

/*
    ���������� �������
*/

void CommandListBuilder::AddCore (detail::ICommand* command)
{
  if (!impl)
    impl = new Impl;

  impl->commands.push_back (command);
}

/*
    ���������� ������ ������
*/

CommandListBuilder::ExecuterPtr CommandListBuilder::Finish ()
{
  if (!impl)
    impl = new Impl;

    //�������� ����������� ���������� ����������� ������ ���������� OpenGL

/*  int display_list = glGenLists (1);
  
  if (display_list)
  {
      //������ ������ ���������� OpenGL
      
    glNewList (display_list, GL_COMPILE);

    impl->Impl::ExecuteCommands ();

    glEndList ();    

      //�������� ������, ����������� ���������� ������

    try
    {
      ExecuterPtr executer (new DisplayListCaller (display_list), false);

        //������� ������� ������

      impl->commands.clear ();

      return executer;
    }
    catch (...)
    {
      glDeleteLists (display_list, 1);
      throw;
    }
  }*/

    //����������� ������, ����������� ������� �������

  ExecuterPtr executer = impl;

    //������� ������

  release (impl);

  impl = 0;

  return executer;
}
