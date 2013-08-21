/*
    OutputStream
*/

/*
    ����������� / ����������
*/

inline OutputStream::OutputStream ()
{
  Reset (buffer);
}

inline OutputStream::~OutputStream ()
{
}

/*
    �����
*/

inline void OutputStream::Reset (const CommandBuffer& in_buffer)
{
  buffer        = in_buffer;
  pos           = reinterpret_cast<char*> (buffer.Data ());
  command_start = pos;
  buffer_end    = pos + buffer.Size ();
}

/*
    �����
*/

inline void OutputStream::Swap (CommandBuffer& in_buffer)
{
  CommandBuffer old_buffer = buffer;

  size_t size = pos - reinterpret_cast<char*> (buffer.Data ());

  Reset (in_buffer);

  old_buffer.Resize (size);

  in_buffer = old_buffer;
}

/*
    ������������ ��������� � ����� �������
*/

inline void OutputStream::BeginCommand (command_id_t id)
{
  EnsureSpaceAvailable (sizeof (Command));

  command_start  = pos;
  pos           += sizeof (Command);

  Command* command = reinterpret_cast<Command*> (command_start);

  command->command_id = id;
}

inline void OutputStream::EndCommand ()
{
  Command* command = reinterpret_cast<Command*> (command_start);

  command->command_size = pos - command_start;
}

/*
    ���������� ������������ ����� � ������ ��� ������
*/

inline void OutputStream::EnsureSpaceAvailable (size_t size)
{
  if (size_t (buffer_end - pos) < size)
    Resize (buffer.Size () + size);
}

inline void OutputStream::Resize (size_t new_size)
{
  char* buffer_start = reinterpret_cast<char*> (buffer.Data ());

  size_t pos_offset           = pos - buffer_start,
         command_start_offset = command_start - buffer_start;

  buffer.Resize (new_size, true);

  buffer_start  = reinterpret_cast<char*> (buffer.Data ());
  pos           = buffer_start + pos_offset;
  command_start = buffer_start + command_start_offset;
  buffer_end    = buffer_start + buffer.Size ();
}

/*
    ������ ����� ������
*/

inline void OutputStream::WriteData (const void* data, size_t size)
{
  EnsureSpaceAvailable (size);
  WriteDataUnsafe      (data, size);
}

inline void OutputStream::WriteDataUnsafe (const void* data, size_t size)
{
  memcpy (pos, data, size);

  pos += size;
}

template <class T> inline void OutputStream::Write (const T& value)
{
  WriteData (&value, sizeof (T));
}

/*
    InputStream
*/

/*
    ����������� / ����������
*/

inline InputStream::InputStream ()
{
  Reset (buffer);
}

inline InputStream::~InputStream ()
{
}

/*
    �����
*/

inline void InputStream::Reset (const CommandBuffer& in_buffer)
{
  buffer     = in_buffer;
  pos        = reinterpret_cast<const char*> (buffer.Data ());
  buffer_end = pos + buffer.Size ();
}

/*
    ���������� ��������� ��� ���������� ������
*/

inline size_t InputStream::Available () const
{
  return buffer_end - pos;
}

/*
    ������ ����� ������
*/

inline void InputStream::ReadData (void* data, size_t size)
{
  if (size <= Available ())
  {
    ReadDataUnsafe (data, size);
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(void*,size_t)", "Can't read %u bytes from input stream with %u bytes available", size, Available ());
  }
}

inline void InputStream::ReadDataUnsafe (void* data, size_t size)
{
  memcpy (data, pos, size);
}

inline const void* InputStream::ReadData (size_t size)
{
  if (size <= Available ())
  {
    const void* result = pos;

    pos += size;

    return result;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(size_t)", "Can't read %u bytes from input stream with %u bytes available", size, Available ());
  }
}

template <class T> inline const T& InputStream::Read ()
{
  return *reinterpret_cast<const T*> (ReadData (sizeof (T)));
}

/*
    �������
*/

inline void InputStream::Skip (size_t size)
{
  if (size >= Available ())
  {
    pos += size;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::Skip", "Can't skip %u bytes from input stream with %u bytes available", size, Available ());
  }
}

/*
    ������������ ������� �����
*/

inline void write (OutputStream& s, int32 value)   { s.Write (value); }
inline void write (OutputStream& s, uint32 value)  { s.Write (value); }
inline void write (OutputStream& s, int16 value)   { s.Write (value); }
inline void write (OutputStream& s, uint16 value)  { s.Write (value); }
inline void write (OutputStream& s, int8 value)    { s.Write (value); }
inline void write (OutputStream& s, uint8 value)   { s.Write (value); }
inline void write (OutputStream& s, float32 value) { s.Write (value); }

inline const int32&   read (InputStream& s, xtl::type<int32>)   { return s.Read<int32> (); }
inline const uint32&  read (InputStream& s, xtl::type<uint32>)  { return s.Read<uint32> (); }
inline const int16&   read (InputStream& s, xtl::type<int16>)   { return s.Read<int16> (); }
inline const uint16&  read (InputStream& s, xtl::type<uint16>)  { return s.Read<uint16> (); }
inline const int8&    read (InputStream& s, xtl::type<int8>)    { return s.Read<int8> (); }
inline const uint8&   read (InputStream& s, xtl::type<uint8>)   { return s.Read<uint8> (); }
inline const float32& read (InputStream& s, xtl::type<float32>) { return s.Read<float32> (); }
inline const Command& read (InputStream& s, xtl::type<Command>) { return s.Read<Command> (); }

inline void write (OutputStream& s, const char* str)
{
  if (!str)
    str = "";

  int32 length = strlen (str);

  write (s, length);
  
  s.WriteData (str, length + 1);
}

inline const char* read (InputStream& s, xtl::type<const char*>)
{
  int32 length = s.Read<int32> ();

  return reinterpret_cast<const char*> (s.ReadData (length + 1));
}
