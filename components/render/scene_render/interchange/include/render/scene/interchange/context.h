#ifndef RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER
#define RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER

#include <stl/auto_ptr.h>

#include <common/log.h>

#include <render/scene/interchange/command_buffer.h>
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
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Processor> Context  (Processor&);
                               ~Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetConnection (const ConnectionPtr&);
    const ConnectionPtr& Connection    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������ �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessIncomingCommands ();

  private:
    Context (const Context&); //no impl
    Context& operator = (const Context&); //no impl

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

#include <render/scene/interchange/detail/context.inl>

}

}

}

#endif
