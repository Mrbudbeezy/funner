#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSDate.h>
#import <Foundation/NSError.h>
#import <Foundation/NSException.h>
#import <Foundation/NSFileHandle.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSString.h>

#include <xtl/function.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/default/shared.h>

using namespace common;

/*
    �������� �������, ���������� ����� Cocoa
*/

namespace
{

class CocoaFileSystem: public StdioFileSystem
{
  public:
///�����������/����������
    CocoaFileSystem ()
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      file_manager = [[NSFileManager defaultManager] retain];

      [pool release];
    }

    ~CocoaFileSystem ()
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      [file_manager release];

      [pool release];
    }

    void FileResize (file_t file,filesize_t new_size)
    {
      static const char* METHOD_NAME = "common::CocoaFileSystem::FileResize";

      if (ftruncate (fileno ((FILE*)file),new_size) == -1)
      {
        switch (errno)
        {
          case EBADF:  throw xtl::make_argument_exception      (METHOD_NAME,"file"); break;
          case EINVAL: throw xtl::make_null_argument_exception (METHOD_NAME,"buffer"); break;
          case ENOSPC: throw xtl::format_operation_exception   (METHOD_NAME,"No enough space for resize file up to %u bytes",new_size); break;
          default:     throw xtl::format_operation_exception   (METHOD_NAME,"Unknown error"); break;
        }
      }
    }

///���������� ������������� ������
    void Remove (const char* file_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)];

      stl::string error_string ("Unknown error");

      NSError* error = nil;

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      if (![file_manager removeItemAtPath:path error:&error])
#else
      BOOL result;

      @try
      {
        result = [file_manager removeFileAtPath:path handler:nil];
      }
      @catch (NSException *e)
      {
        error_string = [[e reason] UTF8String];
      }
      @catch (...)
      {
      }

      if (!result)
#endif
      {
        if (error)
          error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Remove", "Can't remove file '%s': error '%s'", file_name, error_string.c_str ());
      }

      [pool release];
    }

    void Rename (const char* file_name, const char* new_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString *src_path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)],
               *dst_path = [file_manager stringWithFileSystemRepresentation:new_name length:xtl::xstrlen (new_name)];

      stl::string error_string ("Unknown error");

      NSError* error = nil;

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      if (![file_manager moveItemAtPath:src_path toPath:dst_path error:&error])
#else
      if (![file_manager movePath:src_path toPath:dst_path handler:nil])
#endif
      {
        if (error)
          error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Rename", "Can't rename file '%s' to '%s': error '%s'", file_name, new_name, error_string.c_str ());
      }

      [pool release];
    }

    void Mkdir (const char* dir_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:dir_name length:xtl::xstrlen (dir_name)];

      stl::string error_string ("Unknown error");

      NSError* error = nil;

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      if (![file_manager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error])
#else
      if (![file_manager createDirectoryAtPath:path attributes:nil])
#endif
      {
        if (error)
          error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Mkdir", "Can't create directory '%s': error '%s'", dir_name, error_string.c_str ());
      }

      [pool release];
    }

///��������� ���������� � �����
    bool IsFileExist (const char* file_name)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)];

      bool return_value = [file_manager fileExistsAtPath:path];

      [pool release];

      return return_value;
    }

    bool GetFileInfo (const char* file_name, common::FileInfo& info)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:file_name length:xtl::xstrlen (file_name)];

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      NSDictionary* file_attributes = [file_manager attributesOfItemAtPath:path error:nil];
#else
      NSDictionary* file_attributes = [file_manager fileAttributesAtPath:path traverseLink:NO];
#endif

      if (!file_attributes)
      {
        [pool release];

        return false;
      }

      filetime_t modify_time = [[file_attributes fileModificationDate] timeIntervalSince1970];

      info.size         = [file_attributes fileSize];
      info.time_modify  = modify_time;
      info.time_access  = modify_time;
      info.time_create  = [[file_attributes fileCreationDate] timeIntervalSince1970];
      info.is_dir       = [[file_attributes fileType] isEqualToString:NSFileTypeDirectory];

      [pool release];

      return true;
    }

///����� �����
    void Search (const char* wc_mask, const FileSearchHandler& handler)
    {
      stl::string dir_name = dir (wc_mask), mask = wc_mask + dir_name.size (), file_name;            

      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      NSString* path = [file_manager stringWithFileSystemRepresentation:dir_name.c_str () length:dir_name.size ()];

      if (![file_manager fileExistsAtPath:path])
      {
        [pool release];
        return;
      }

      if (dir_name == "./")
        dir_name = "";

#if defined IPHONE || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050
      NSError* error = nil;

      NSArray* subpathes = [file_manager contentsOfDirectoryAtPath:path error:&error];

      if (error)
      {
        stl::string error_string = [[error localizedDescription] UTF8String];

        [pool release];

        throw xtl::format_operation_exception ("common::CocoaFileSystem::Search", "Can't enumerate directory '%s' contents: error '%s'", dir_name.c_str (), error_string.c_str ());
      }
#else
      NSArray* subpathes = [file_manager directoryContentsAtPath:path];
#endif

      common::FileInfo info;

      for (size_t i = 0, count = [subpathes count]; i < count; i++)
      {
        NSString* ns_file_name = [subpathes objectAtIndex:i];

        file_name = [ns_file_name UTF8String];

        if (file_name.data () [0] != '.' && file_name.find ("/.") == stl::string::npos && wcmatch (file_name.c_str (), mask.c_str ()))
        {
          file_name = dir_name + file_name;

          GetFileInfo (file_name.c_str (), info);

          handler (file_name.c_str (), info);
        }
      }

      [pool release];
    }

///������� ������
    void AddRef  () {}
    void Release () {}

  private:
    CocoaFileSystem (const CocoaFileSystem&); //no impl
    CocoaFileSystem& operator = (const CocoaFileSystem&); //no impl

  private:
    NSFileManager* file_manager;
};

}

/*
    ��������� �������� �������
*/

ICustomFileSystem* CocoaPlatform::GetFileSystem ()
{
  return &*Singleton<CocoaFileSystem>::Instance ();
}
