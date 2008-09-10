#include "shared.h"

namespace
{

//��������� ������ � ���������� �� ������
stl::string get_error_message (DWORD error_code)
{
  void* buffer = 0;

  size_t buffer_len = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     0, error_code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, 0);                 

  if (!buffer)
  {
    return common::format ("Win32 error %u", error_code);
  }
  else
  {    
      //��������� ����������� \n � ��������      

    char* iter = (char*)buffer;    
    
    iter [buffer_len] = '\0';    

    iter += buffer_len;

    if (iter == buffer)
      return "";      
      
    for (bool loop=true; loop;)
      switch (*--iter)
      {
        case '\n':
        case '\r':
        case ' ':
        case '\t':
          break;
        default:
        {
          iter [1] = '\0';
          loop     = false;
          break;
        }
      }

    stl::string message = common::format ("Win32 error %u. %s", error_code, buffer);        

    LocalFree (buffer);

    return message;
  }
}

}

namespace render
{

namespace low_level
{

namespace opengl
{

namespace windows
{

//��������� ����������
void raise_error (const char* source)
{
  DWORD error_code = GetLastError ();

  if (error_code)
    throw xtl::format_operation_exception (source, "%s", get_error_message (error_code).c_str ());

  throw xtl::format_operation_exception (source, "Operation failed");
}

///�������� ��������� ����������
stl::string get_wgl_extensions_string (const WglExtensionEntries& wgl_extension_entries, HDC device_context)
{
  stl::string extensions;
    
  if (wgl_extension_entries.GetExtensionsStringARB)
    extensions += wgl_extension_entries.GetExtensionsStringARB (device_context);
    
  if (wgl_extension_entries.GetExtensionsStringEXT)
    extensions += wgl_extension_entries.GetExtensionsStringEXT ();

  return extensions;
}

bool has_extension (const char* extensions_string, const char* extension)
{
  return extensions_string && extension && strstr (extensions_string, extension);
}

//��������������� ������� Dll
void redirect_dll_call (HMODULE module, const char* import_module_name, void* src_thunk, void* dst_thunk)
{
  try
  {
    if (!module)
      throw xtl::make_null_argument_exception ("", "module");

    if (!import_module_name)
      throw xtl::make_null_argument_exception ("", "import_module_name");

    if (!src_thunk)
      throw xtl::make_null_argument_exception ("", "src_thunk");
      
    if (!dst_thunk)
      throw xtl::make_null_argument_exception ("", "dst_thunk");      
    
      //��������� �������� ������ �������� �������� � ������� PE-����������

    char*             image_base = reinterpret_cast<char*> (module);
    PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER> (image_base);
    PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS> (image_base + dos_header->e_lfanew);
    
    size_t import_offset = nt_headers->OptionalHeader.DataDirectory [IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    if (!import_offset)
      throw xtl::format_operation_exception ("", "Null import table offset");

      //��������� ������ ������� �������

    PIMAGE_IMPORT_DESCRIPTOR import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR> (image_base + import_offset);

    for (;import_descriptor->Name; ++import_descriptor) //������� ������������� �������
    {
      const char* module_name = reinterpret_cast<const char*> (image_base + import_descriptor->Name);      

      if (xtl::xstricmp (module_name, import_module_name))
        continue;

        //������� ����� �����

      void**            thunk      = reinterpret_cast<void**> (image_base + import_descriptor->FirstThunk);
      char*             hint       = import_descriptor->OriginalFirstThunk ? image_base + import_descriptor->OriginalFirstThunk : (char*)thunk;
      PIMAGE_THUNK_DATA thunk_data = PIMAGE_THUNK_DATA (hint);        

      for (;thunk_data->u1.AddressOfData; thunk_data++, thunk++)
      {
        if (*thunk == src_thunk)
        {
          DWORD old_protect = 0;

          if (!VirtualProtect (thunk, sizeof thunk, PAGE_EXECUTE_READWRITE, &old_protect))
            raise_error ("VirtualProtect");

          *thunk = dst_thunk;

          if (!VirtualProtect (thunk, sizeof thunk, old_protect, &old_protect))
            raise_error ("VirtualProtect");

          return;
        }
      }
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::redirect_dll_call");
    throw;
  }
}

}

}

}

}
