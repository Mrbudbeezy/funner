#include "shared.h"

using namespace render::scene::interchange;

/*
    ���������
*/

const size_t DESIRED_FPS       = 60;
const size_t SPIN_WAIT_TIME_MS = 1000 / DESIRED_FPS;

/*
    �������� ���������� ������� ������
*/

typedef syslib::SharedQueue<CommandQueueItem> Queue;

struct CommandQueue::Impl
{
  Queue queue; //������� �������

/// �����������
  Impl (size_t max_queue_size)
    : queue (max_queue_size, SPIN_WAIT_TIME_MS)
  {
    if (!max_queue_size)
      throw xtl::make_null_argument_exception ("", "max_queue_size");
  }
};

/*
    ����������� / ����������
*/

CommandQueue::CommandQueue (size_t max_queue_size)
  : impl (new Impl (max_queue_size))
{
}

CommandQueue::~CommandQueue ()
{
}

/*
    ������ ������� / �������� �� �������
*/

size_t CommandQueue::Size () const
{
  return impl->queue.Size ();
}

bool CommandQueue::IsEmpty () const
{
  return impl->queue.IsEmpty ();
}

/*
    ��������� �������� � ������� / ���������� �� �������
*/

void CommandQueue::Push (const CommandQueueItem& command)
{
  impl->queue.Push (command);
}

bool CommandQueue::Pop (CommandQueueItem& command, size_t timeout)
{
  return impl->queue.TryPop (command, timeout);
}
