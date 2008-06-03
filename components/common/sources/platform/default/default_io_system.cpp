#include "shared.h"

using namespace common;

/*
    Stdio files entries
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StdioFile
{
  const char* name;     //��� � StdioIOSystem
  const char* dir_name; //��� ��������
  FILE*       stream;   //�����
  filemode_t  mode;     //����� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
static StdioFile stdio_files [] = {
  {"stdout","stdout/",stdout,FILE_MODE_WRITE},
  {"stderr","stderr/",stderr,FILE_MODE_WRITE},
  {"stdin","stdin/",stdin,FILE_MODE_READ}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������ ����������� ����������
//////////////////////////////////////////////,/////////////////////////////////////////////////////
const size_t STDIO_FILES_NUM = sizeof (stdio_files)/sizeof (StdioFile);

/*
    StdioIOSystem
*/

namespace
{

//�������� ������������� ����� � ������
bool is_file_exist (const char* file_name,StdioFile& file)
{
  return !stricmp (file.name,file_name) || !strnicmp (file.dir_name,file_name,strlen (file.dir_name));
}

}
  

StdioIOSystem::file_t StdioIOSystem::FileOpen (const char* file_name,filemode_t mode,size_t)
{
  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (is_file_exist (file_name,*stdio_file))
    {
      if (stdio_file->mode & ~mode)
      {
        raise<FileLoadException> ("StdioIOSystem::FileOpen","Wrong <file_mode>='%s' at open '%s' stream",
                                  strfilemode (mode).c_str (),file_name);
      }

      return (file_t)stdio_file->stream;
    }
  
  raise<FileNotFoundException> ("StdioIOSystem::FileOpen","File '%s' not found",file_name);
    
  return (file_t)NULL;
}

bool StdioIOSystem::IsFileExist (const char* file_name)
{
  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (is_file_exist (file_name,*stdio_file))
      return true;

  return false;
}

void StdioIOSystem::Search (const char* wc_mask,const FileSearchHandler& handler)
{
  FileInfo info;
  
  memset (&info,0,sizeof (info));

  StdioFile* stdio_file = stdio_files;
  
  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (!(wcimatch (wc_mask,stdio_file->name)))
      handler (stdio_file->name,info);  
}

bool StdioIOSystem::GetFileInfo (const char* file_name,FileInfo& info)
{
  StdioFile* stdio_file = stdio_files;

  for (size_t i=0;i<STDIO_FILES_NUM;i++,stdio_file++)
    if (is_file_exist (file_name,*stdio_file))
    {
      memset (&info,0,sizeof (info));
      return true;
    }

  return false;
}

void StdioIOSystem::Remove (const char* file_name)
{
  raise_not_supported ("StdioIOSystem::Remove","File '%s' can not be removed",file_name);
}

void StdioIOSystem::Rename (const char* file_name,const char*)
{
  raise_not_supported ("StdioIOSystem::Rename","File '%s' can not be renamed",file_name);
}

void StdioIOSystem::Mkdir (const char* dir_name)
{
  raise_not_supported ("StdioIOSystem::Mkdir","Unable to make dir '%s'. This file system is static",dir_name);
}
