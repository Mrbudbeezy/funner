#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    ���������
*/

const size_t COMMAND_ARRAY_RESERVE_SIZE = 16; //������������� ����� ������ � ������

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

    //����������� ������, ����������� ������� �������

  ExecuterPtr executer = impl;

    //������� ������

  release (impl);

  impl = 0;

  return executer;
}
