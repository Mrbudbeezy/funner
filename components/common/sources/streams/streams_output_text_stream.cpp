#include "shared.h"

using namespace common;

const size_t FORMAT_BUFFER_SIZE = 16; //������ ������ ��������������

/*
    �������� ���������� ���������� ������ ������
*/

struct OutputTextStream::Impl
{
  OutputStreamBuffer buffer; //�����

  Impl (size_t buffer_size, const WriteFunction& in_writer = &OutputStreamBuffer::DefaultWriter) : buffer (in_writer, buffer_size) {}
};

/*
    ����������� / ����������
*/

OutputTextStream::OutputTextStream (size_t buffer_size)
  : impl (new Impl (buffer_size))
  {}

OutputTextStream::OutputTextStream (const WriteFunction& writer, size_t buffer_size)
  : impl (new Impl (buffer_size, writer))
  {}

OutputTextStream::~OutputTextStream ()
{
}

/*
    ����� ������
*/

const common::OutputStreamBuffer& OutputTextStream::Buffer () const
{
  return impl->buffer;
}

common::OutputStreamBuffer& OutputTextStream::Buffer ()
{
  return impl->buffer;
}

/*
    ����� �����
*/

void OutputTextStream::Write (const char* string, size_t size)
{
  if (!string && size)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const char*,size_t)", "string");

  size_t write_size = impl->buffer.Write (string, size);

  if (write_size != size)
    throw xtl::format_exception<StreamWriteException> ("common::OutputTextStream::Write", "Could not write block with size %u to the buffer (write_size=%u)",
                                 size, write_size);
}

void OutputTextStream::Write (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const char*)", "string");

  Write (string, strlen (string));
}

void OutputTextStream::Write (const wchar_t*, size_t)
{
  throw xtl::make_not_implemented_exception ("common::OutputTextStream::Write(const wchar_t*,size_t)");
}

void OutputTextStream::Write (const wchar_t* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("common::OutputTextStream::Write(const wchar_t*)", "string");

  return Write (string, wcslen (string));
}

namespace common
{

/*
    ����� �����
*/

void write (OutputTextStream& stream, const char* string)
{
  stream.Write (string);
}

void write (OutputTextStream& stream, const wchar_t* string)
{
  stream.Write (string);
}

/*
    ����� ��������
*/

void write (OutputTextStream& stream, char symbol)
{
  stream.Write (&symbol, 1);
}

void write (OutputTextStream& stream, wchar_t symbol)
{
  stream.Write (&symbol, 1);
}

void write (OutputTextStream& stream, size_t count, char symbol)
{
  if (!count)
    return;

  char buf [128];
  const size_t BUF_SIZE = sizeof (buf);

  memset (buf, symbol, BUF_SIZE < count ? BUF_SIZE : count);

  for (size_t i=0, passes_count=count/BUF_SIZE; i<passes_count; i++)
    stream.Write (buf, BUF_SIZE);

  stream.Write (buf, count % BUF_SIZE);
}

void write (OutputTextStream& stream, size_t count, wchar_t symbol)
{
  if (!count)
    return;

  const size_t BUF_SIZE = 128;
  wchar_t buf [BUF_SIZE];

  for (size_t i=0, fill_size=count<BUF_SIZE?count:BUF_SIZE; i<fill_size; i++)
    buf [i] = symbol;

  for (size_t i=0, passes_count=count/BUF_SIZE; i<passes_count; i++)
    stream.Write (buf, BUF_SIZE);

  stream.Write (buf, count % BUF_SIZE);
}

/*
    ����� ����� �����
      format:
        ""      - ����� � ��������������� �� ���������
        "hex"   - ����� � ����������������� �������
        "#"     - ����� � ������������ �� ��������� (' ') � ������� �� ������ 1
        "000"   - ����� � ������������ 0 � ������� 3
        "+##"   - ����� � ������������ �� ���������, ������� 3 � ��������� �����
        "hex:#" - ����� � ������������ �� ���������, ������� 1 � ����������������� �������
*/

namespace
{

//���������� ������� � printf-�����
void get_int_printf_format (const char*& format, bool sign, char buffer [FORMAT_BUFFER_SIZE])
{
  char*       pos         = buffer;
  const char* base_format = sign ? "lld" : "llu";

  *pos++ = '%';

  if (!strncmp (format, "hex:", 4))
  {
    base_format  = "llx";
    format      += 4;
  }
  else if (!strncmp (format, "oct:", 4))
  {
    base_format  = "llo";
    format      += 4;
  }
  else if (!strncmp (format, "dec:", 4))
  {
    base_format  = "llu";
    format      += 4;
  }

  switch (*format)
  {
    case '+':
    case '-':
      *pos++ = '+';
      format++;
      break;
  }

  xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%u%s",
                             *format == '0' ? "0" : "", strlen (format), base_format);
}

//������ ������ �����
void write_int (OutputTextStream& stream, unsigned long long value, const char* format, bool sign)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");

  char format_buffer [FORMAT_BUFFER_SIZE], value_buffer [16];

  get_int_printf_format (format, sign, format_buffer);

  xtl::xsnprintf (value_buffer, sizeof (value_buffer), format_buffer, value);

  stream.Write (value_buffer);
}

}

void write (OutputTextStream& stream, int value, const char* format)
{
  write_int (stream, static_cast<unsigned long long> (value), format, true);
}

void write (OutputTextStream& stream, long value, const char* format)
{
  write_int (stream, static_cast<unsigned long long> (value), format, true);
}

void write (OutputTextStream& stream, long long value, const char* format)
{
  write_int (stream, static_cast<unsigned long long> (value), format, true);
}

void write (OutputTextStream& stream, unsigned int value, const char* format)
{
  write_int (stream, static_cast<unsigned long long> (value), format, false);
}

void write (OutputTextStream& stream, unsigned long value, const char* format)
{
  write_int (stream, static_cast<unsigned long long> (value), format, false);
}

void write (OutputTextStream& stream, unsigned long long value, const char* format)
{
  write_int (stream, value, format, false);
}

/*
  ����� ����� � ��������� ������
    format (���������� ����� ������, �������� ������� ���������):
      ���������� ����� - ������ ������� ����� � ��������������� �� ���������
      ".##" - ������ �� ������ 2-� (1.2=1.2, 1.21=1.21)
      ".00" - ������ 2 (1.2=1.20, 1.211=1.21)
*/

namespace
{

//���������� ������� � printf-�����
void get_float_printf_format (const char*& format, char buffer [FORMAT_BUFFER_SIZE], unsigned int int_size)
{
  char* pos = buffer;
  
  bool has_sign = false;

  *pos++ = '%';

  switch (*format)
  {
    case '+':
    case '-':
      *pos++ = '+';
      format++;
      has_sign = true;
      break;
  }

  const char *dot = strchr (format, '.');

  if (dot)
  {
    const char*  frac_format = dot + 1;
    unsigned int frac_size   = (unsigned int)strlen (frac_format),
                 width       = (unsigned int)(dot - format) + frac_size + 1;
                
    if (*frac_format != '0')
    {
      if (has_sign)
      {
        width++;
        frac_size++;
      }

      width     += int_size;
      frac_size += int_size;
    }                
    
    if (dot - format)
    {
      xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%u.%u%s",
        *format == '0' ? "0" : "", width, frac_size, *frac_format == '0' ? "f" : "g");
    }
    else
    {
      xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s.%u%s", *format == '0' ? "0" : "", frac_size,
        *frac_format == '0' ? "f" : "g");
    }
  }
  else
  {
    size_t width = strlen (format);
    
    xtl::xsnprintf (pos, FORMAT_BUFFER_SIZE - (pos - buffer), "%s%ug", *format == '0' ? "0" : "", width);
  }
}

}

void write (OutputTextStream& stream, double value, const char* format)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");

  char format_buffer [FORMAT_BUFFER_SIZE], value_buffer [16];
  
  get_int_printf_format (format, true, format_buffer);

  xtl::xsnprintf (value_buffer, sizeof (value_buffer), format_buffer, (long long)value);

  const char* pos = value_buffer;
  
  while (*pos == ' ') pos++;

  get_float_printf_format (format, format_buffer, (unsigned int)strlen (pos));

  xtl::xsnprintf (value_buffer, sizeof (value_buffer), format_buffer, value);

  stream.Write (value_buffer);
}

void write (OutputTextStream& stream, float value, const char* format)
{
  write (stream, static_cast<double> (value), format);
}

void write (OutputTextStream& stream, long double value, const char* format)
{
  write (stream, static_cast<double> (value), format);
}

/*
    ����� ���������� ��������
      format:
        "alpha" - ����� � ��������� ���� (true, false)
        ""      - ����� � ��������� ���� (0, 1)
*/

void write (OutputTextStream& stream, bool value, const char* format)
{
  if (!format)
    throw xtl::make_null_argument_exception ("common::write", "format");

  if (!strcmp (format, "alpha")) stream.Write (value ? "true" : "false");
  else if (!*format)             stream.Write (value ? "1" : "0");
  else
  {
    throw xtl::make_argument_exception ("common::write", "format", format);
  }
}

}
