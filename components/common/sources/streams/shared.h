#ifndef STREAMS_SHARED_HEADER
#define STREAMS_SHARED_HEADER

#include <string.h>

#include <stl/stack>
#include <stl/string>

#include <xtl/function.h>
#include <xtl/string.h>
#include <xtl/common_exceptions.h>

#include <common/streams.h>
#include <common/wxf_writer.h>
#include <common/xml_writer.h>
#include <common/file.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class StreamBuffer
{
  public:
    StreamBuffer  ();
    ~StreamBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка пользовательского буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUserBuffer (void* buffer, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер буфера / изменение размера буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return visible_size; }
    void   Resize  (size_t size, size_t initialized_size=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Data () const { return buffer; }
    
  private:
    StreamBuffer (const StreamBuffer&); //no impl
    StreamBuffer& operator = (const StreamBuffer&); //no impl
    
    void SetBuffer (void* buffer, size_t size, bool is_user_buffer);

  private:
    bool   is_user_buffer; //является ли буфер пользовательским
    void*  buffer;         //указатель на начало буфера
    size_t buffer_size;    //размер буфера
    size_t visible_size;   //видимый размер
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функтор сброса буферов потока в файл
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FileWriter
{
  FileWriter (const char* file_name) : file (file_name) {}

  size_t operator () (const void* buffer, size_t size)
  {
    size_t result = file.Write (buffer, size);

    file.Flush ();

    return result;
  }

  OutputFile file;
};

}

#endif
