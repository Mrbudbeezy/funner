#include "shared.h"

using namespace common;

StdFile::StdFile (const char* file_name,filemode_t mode_flags)
  : File (FileSystemSingleton::Instance ().OpenFile (file_name,mode_flags))
  { }

StdFile::StdFile (const char* file_name,filemode_t mode_flags,size_t buffer_size)
  : File (FileSystemSingleton::Instance ().OpenFile (file_name,mode_flags,buffer_size))
  { }

InputFile::InputFile (const char* file_name)
  : StdFile (file_name,FileMode_ReadOnly)
  { }
  
InputFile::InputFile (const char* file_name,size_t buffer_size)
  : StdFile (file_name,FileMode_ReadOnly,buffer_size)
  { }

OutputFile::OutputFile (const char* file_name)
  : StdFile (file_name,FileMode_WriteOnly)
  { }

OutputFile::OutputFile (const char* file_name,size_t buffer_size)
  : StdFile (file_name,FileMode_WriteOnly,buffer_size)
  { }

AppendFile::AppendFile (const char* file_name)
  : StdFile (file_name,FileMode_ReadWrite|FileMode_Create)
{ 
  Seek (Size ());
}

AppendFile::AppendFile (const char* file_name,size_t buffer_size)
  : StdFile (file_name,FileMode_ReadWrite|FileMode_Create,buffer_size)
{
  Seek (Size ());
}

MemFile::MemFile (void* buf,size_t size,filemode_t mode)
  : File (FileImplPtr (new MemFileImpl (buf,size,mode), false))
  { }
  
CustomFile::CustomFile (ICustomFileSystemPtr file_system,const char* name,filemode_t mode)
 : File (FileImplPtr (new CustomFileImpl (file_system,name,mode), false))
 { }

CustomFile::CustomFile (ICustomFileSystemPtr file_system,ICustomFileSystem::file_t handle,filemode_t mode,bool auto_close)
 : File (FileImplPtr (new CustomFileImpl (file_system,handle,mode,auto_close), false))
 { }

CryptoFile::CryptoFile (const File& source_file, const char* read_crypto_method, const char* write_crypto_method, const void* key, size_t key_bits)
  : File (FileImplPtr (new CryptoFileImpl (source_file.GetImpl (), FileSystemSingleton::Instance ().GetDefaultFileBufferSize (), read_crypto_method, write_crypto_method, key, key_bits), false))
 { }

CryptoFile::CryptoFile (const File& source_file, const char* read_crypto_method, const void* key, size_t key_bits)
  : File (FileImplPtr (new CryptoFileImpl (source_file.GetImpl (), FileSystemSingleton::Instance ().GetDefaultFileBufferSize (), read_crypto_method, key, key_bits), false))
 { }
