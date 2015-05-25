namespace detail
{

/// ��������� ��������� ���������
template <class Deserializer> class IIncomingCommandsProcessor
{
  public:
    virtual ~IIncomingCommandsProcessor () {}

    virtual void ProcessFeedback (Deserializer& deserializer, common::Log& log) = 0;
};

}

/// ���������� �������� ������
template <class Serializer, class Deserializer> template <class Processor>
class Context<Serializer, Deserializer>::IncomingCommandsProcessorImpl: public detail::IIncomingCommandsProcessor<Deserializer>
{
  public:
    IncomingCommandsProcessorImpl (Processor* in_processor) : processor (in_processor) {}

    void ProcessFeedback (Deserializer& deserializer, common::Log& log)
    {
      typedef typename Deserializer::CommandId CommandId;

      while (deserializer.Available ())
      {
        const Command& command   = read (deserializer, xtl::type<Command> ());
        size_t         body_size = command.command_size - sizeof (Command);

        if (deserializer.Available () < body_size)
        {
          stl::string command_name = get_command_name ((CommandId)command.command_id);

          log.Printf ("Can't read command %s with size %u: only %u bytes is available", command_name.c_str (), command.command_size, body_size);

          return; //no more data
        }

        size_t finish_position = deserializer.Position () + body_size;

        try
        {
          if (!deserializer.Deserialize ((CommandId)command.command_id, *processor))
          {
            log.Printf ("Unknown command %u with size %u. Will be skipped", command.command_id, command.command_size);

            deserializer.Skip (body_size);
          }
        }
        catch (std::exception& e)
        {
          log.Printf ("Error at processing command %s: %s", get_command_name ((CommandId)command.command_id).c_str (), e.what ());

          deserializer.SetPosition (finish_position);
        }
        catch (...)
        {
          log.Printf ("Unknown exception at processing command %s", get_command_name ((CommandId)command.command_id).c_str ());

          deserializer.SetPosition (finish_position);
        }
      }
    }

  private:
    Processor* processor;
};

/*
    ����������� / ����������
*/

template <class Serializer, class Deserializer> template <class Processor>
inline Context<Serializer, Deserializer>::Context (Processor& processor)
  : incoming_commands_processor (new IncomingCommandsProcessorImpl<Processor> (&processor))
  , log ("render.scene.interchange.Context")
{
}

template <class Serializer, class Deserializer>
inline Context<Serializer, Deserializer>::~Context ()
{
  try
  {
    SetCounterparty (0);
  }
  catch (...)
  {
  }
}

/*
    ��������� ��������� ����������
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::SetCounterparty (IConnection* in_connection)
{
  if (OutputStream::InprocessOwnerId ())
  {
    InprocessExchangeStorage::DeallocateOwnerId (OutputStream::InprocessOwnerId ());

    OutputStream::SetInprocessOwnerId (0);
  }

  connection = in_connection;

  if (connection && connection->IsInprocessed ())
    OutputStream::SetInprocessOwnerId (InprocessExchangeStorage::AllocateOwnerId ());
}

template <class Serializer, class Deserializer>
inline IConnection* Context<Serializer, Deserializer>::Counterparty () const
{
  return connection.get ();
}

/*
    ������� ��������� ������ �� ����������
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::Flush ()
{
  try
  {
    if (this->IsEmpty ())
      return;

    if (!connection)
      throw xtl::format_operation_exception ("", "Can't send rendering commands. No counterparty");

    CommandBuffer buffer = command_buffer_pool.CreateBuffer ();

    Serializer::Swap (buffer);

    try
    {
      connection->ProcessCommands (buffer);
    }
    catch (...)
    {
      Serializer::Swap (buffer);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::Context::Flush");
    throw;
  }
}

/*
    ��������� �������� ������
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::ProcessCommands (const CommandBuffer& buffer)
{
  try
  {
    Deserializer::Reset (buffer);

    incoming_commands_processor->ProcessFeedback (*this, log);     
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::Context::ProcessIncomingCommands");
    throw;
  }
}
