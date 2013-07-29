#ifndef RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER
#define RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER

#include <common/log.h>

#include <render/scene/interchange/connection.h>
#include <render/scene/interchange/streams.h>

namespace render
{

namespace scene
{

namespace interchange
{

namespace detail
{

//implementation forwards
template <class Deserializer> class IIncomingCommandsProcessor;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Serializer, class Deserializer>
class Context: public Serializer, private Deserializer, private IConnectionListener
{
  public:
    typedef xtl::com_ptr<IConnection> ConnectionPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Processor> Context (const ConnectionPtr&, Processor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������ �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessIncomingCommands ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnIncomingCommands (const CommandBuffer&);

  private:
    typedef detail::IIncomingCommandsProcessor<Deserializer> IncomingCommandsProcessor;
    typedef stl::auto_ptr<IncomingCommandsProcessor>         IncomingCommandsProcessorPtr;

  private:
    ConnectionPtr                connection;
    CommandBufferPool            command_buffer_pool;
    IncomingCommandsProcessorPtr incoming_commands_processor;
    common::Log                  log;
};

#include <render/scene/interchange/context.inl>

}

}

}

#endif
