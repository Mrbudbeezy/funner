#include <common/exception.h>
#include <common/log.h>

using namespace common;

/*
   �������� ���������� ������ LogFile
*/

struct LogFile::Impl
{
  public:
/*
   ���������/��������� �������� �����
*/
    void SetFile (const common::File& in_file)
    {
      file = in_file;
    }

    const common::File& File () const
    {
      return file;
    }

/*
   ������ ��������� � ����
*/
    void Print (const char* log_name, const char* message)
    {
      //!!!!!!!!!
    }

/*
   ���������/�������� �������� ������ ���������
*/
    size_t AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
    {
      return 0; //!!!!!!!!!
    }

    void SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
    {
      //!!!!!!!!!
    }

    void RemoveFilter (size_t filter_index)
    {
      //!!!!!!!!!
    }

    void RemoveAllFilters ()
    {
      //!!!!!!!!!
    }

/*
   ����� ��������� �������
*/
    void Flush ()
    {
      file.Flush ();
    }

  private:
    common::File file;
};

/*
   ������ ��������� � ����
*/

/*
   �����������/����������
*/

LogFile::LogFile ()
  : impl (new Impl)
{
}

LogFile::LogFile (const common::File& file)
  : impl (new Impl)
{
  SetFile (file);
}

LogFile::~LogFile ()
{
  delete impl;
}

/*
   ���������/��������� �������� �����
*/

void LogFile::SetFile (const common::File& file)
{
  impl->SetFile (file);
}

const common::File& LogFile::File () const
{
  return impl->File ();
}

/*
   ������ ��������� � ����
*/

void LogFile::Print (const char* log_name, const char* message)
{
  impl->Print (log_name, message);
}

/*
   ���������/�������� �������� ������ ���������
*/

size_t LogFile::AddFilter (const char* replace_mask, const char* replacement, size_t sort_order)
{
  return impl->AddFilter (replace_mask, replacement, sort_order);
}

void LogFile::SetFilter (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order)
{
  impl->SetFilter (filter_index, replace_mask, replacement, sort_order);
}

void LogFile::RemoveFilter (size_t filter_index)
{
  impl->RemoveFilter (filter_index);
}

void LogFile::RemoveAllFilters ()
{
  impl->RemoveAllFilters ();
}

/*
   ����� ��������� �������
*/

void LogFile::Flush ()
{
  impl->Flush ();
}
