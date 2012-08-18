#include <cstdio>
#include <cstdlib>
#include <exception>

#include <stl/list>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/file.h>
#include <common/string.h>
#include <common/strlib.h>

#include <debug/map_file.h>

using namespace common;
using namespace debug;

/*
    ���������
*/

const size_t MAX_LINE_SIZE             = 2048;
const size_t HELP_STRING_PREFIX_LENGTH = 30;

/*
    �������
*/

void error (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);

  printf  ("error: ");
  vprintf (format, args);
  printf  ("\n");
  fflush  (stdout);

  exit (1);
}

//���������� map-file
struct MapFileDesc
{  
  MapFile     map_file;           //map-����
  stl::string map_file_base_name; //������� ��� map-�����
  filetime_t  modification_time;  //����� ����������� �����
  
  MapFileDesc (const char* file_name) : map_file (file_name, "gcc"), map_file_base_name (basename (notdir (file_name))), modification_time (FileSystem::GetFileTime (file_name)) {}
};

typedef stl::list<MapFileDesc> MapFileList;

//���������� ��������
struct SegmentDesc
{
  size_t        start_address;  //��������� ����� ��������
  size_t        finish_address; //�������� ����� ��������
  MapFileDesc*  map_file;       //map-����
};

typedef stl::list<SegmentDesc> SegmentList;

//�����
struct Option
{
  common::CommandLine::SwitchHandler handler;       //���������� �����
  const char*                        name;          //��� �������
  char                               short_name;    //�������� ���
  const char*                        argument_name; //��� ���������
  const char*                        tip;           //���������
};

//��������� �������
struct Params
{
  const Option* options;             //������ �����
  size_t        options_count;       //���������� �����
  MapFileList   map_files;           //������ map-������
  SegmentList   segments;            //������ ���������
  bool          silent;              //������� �� ����� ��� ������ ���������
  bool          print_help;          //����� �� �������� ��������� ������  
};

//��������� ��������� �� ���������
void command_line_help (const char*, Params& params)
{
  printf ("android-crash-resolver [<OPTIONS>] <SOURCE> ...\n");
  printf ("  OPTIONS:\n");  
  
  for (size_t i=0; i<params.options_count; i++)
  {
    const Option& option = params.options [i];
    
    stl::string prefix = common::format (option.argument_name ? "    --%s=<%s>" : "    --%s", option.name, option.argument_name);

    if (option.short_name)
      prefix += common::format (option.argument_name ? ", -%c <%s>" : ", -%c", option.short_name, option.argument_name);

    printf ("%s ", prefix.c_str ());

    if (prefix.size () < HELP_STRING_PREFIX_LENGTH)
      for (size_t i=0, count=HELP_STRING_PREFIX_LENGTH-prefix.size (); i<count; i++)
        printf (" ");

    printf ("%s\n", option.tip);   
  }
  
  params.print_help = true;
}

//��������� ��������� ������ ��������� ����������
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//������ ������������������ �����
bool read_hex (const char* string, size_t& value)
{
  if (!string || !*string)
    return false;    
    
  unsigned long tmp = strtoul (string, (char**)&string, 16);

  if (!*string || *string == ' ')
  {  
    value = tmp;
    return true;
  }  

  return false;
}

//������ ������ � �����
bool read_line (FILE* file, stl::string& buffer)
{
  if (!fgets (&buffer [0], buffer.capacity (), file))
  {
    buffer.clear ();
    return false;
  }

  buffer.fast_resize (strlen (buffer.c_str ()));
  
  if (!buffer.empty () && buffer [buffer.size ()-1] == '\n')
    buffer.pop_back ();

  return true;
}

//�������� ��������� ����
void reload_map_files (Params& params)
{
  for (MapFileList::iterator i=params.map_files.begin (); i!=params.map_files.end (); ++i)
  {        
    MapFileDesc& desc = *i;
    
    filetime_t modification_time = FileSystem::GetFileTime (desc.map_file.Name ());
    
    if (modification_time != desc.modification_time)
    {
      try
      {
        try
        {
          desc.map_file          = MapFile (desc.map_file.Name (), "gcc");
          desc.modification_time = modification_time;
        }
        catch (...)
        {
          MapFileList::iterator next = i;
          
          ++next;
          
          params.map_files.erase (i);
          
          i = --next;          
          
          throw;
        }
      }
      catch (std::exception& e)
      {
        fprintf (stderr, "%s\n    at reload_map_files\n", e.what ());
      }
      catch (...)
      {
        fprintf (stderr, "unknown exception\n    at reload_map_files\n");
      }
    }
  }
}

//�������� ������������ �����
void validate (Params& params)
{ 
}

//������ ���������
void parse_segment (Params& params, const char* line)
{
  StringArray tokens = split (line, " -");
  
  if (tokens.Size () < 4)
    return;        
    
  size_t start_address = 0, finish_address = 0;

  if (!read_hex (tokens [1], start_address) || !read_hex (tokens [2], finish_address))
    return;

  const char* module_name = tokens [tokens.Size ()-1];
  
  stl::string module_base_name = basename (notdir (module_name));

  for (MapFileList::iterator i=params.map_files.begin (), end=params.map_files.end (); i!=end; ++i)
  {
    MapFileDesc& desc = *i;

    if (i->map_file_base_name != module_base_name)
      continue;

    SegmentDesc segment_desc;
    
    segment_desc.start_address  = start_address;
    segment_desc.finish_address = finish_address;
    segment_desc.map_file       = &*i;
    
    params.segments.push_back (segment_desc);
    
    return;
  }
}

//���������� ������
void resolve_address (Params& params, size_t address, const char* module_name)
{
  stl::string module_base_name = basename (notdir (module_name));  
  
  for (SegmentList::iterator i=params.segments.begin (), end=params.segments.end (); i!= end; ++i)
  {
    SegmentDesc& desc = *i;
    
    if (desc.map_file->map_file_base_name != module_base_name)
      continue;

    if (address < desc.start_address || address >= desc.finish_address)
    {
      Symbol* symbol = desc.map_file->map_file.FindSymbol (address);    
      
      if (!symbol)
        continue;
        
      printf (" [%s]", symbol->Name ());      
    }
    else
    {
      size_t related_address = address - desc.start_address;        
      
      Symbol* symbol = desc.map_file->map_file.FindSymbol (related_address);    
      
      if (!symbol)
        continue;
        
      printf (" [%s]", symbol->Name ());                
    }
    
    return;    
  }
}

//��������� ������ ����
void process_log_line (Params& params, const char* line_start)
{
  const char* s = line_start;
  
  for (;s && *s;)
  {
    switch (*s)
    {
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      {
        if (s != line_start && s [-1] != ' ' && s [-1] != '\t')
        {
          s++;
          continue;
        }
        
        size_t address = 0;

        const char* token = s;
        
        s = strchr (s, ' ');        

        if (!read_hex (token, address))
          continue;        

        if (!s)
          return;

        for (;*s && (*s == ' ' || *s == '\t'); s++);

        if (!*s)
          return;

        if (*s != '/')
          continue;

        if (const char* next_space = strchr (s, ' ')) //only last path token at line will be processed
        {
          s = next_space;
          continue;
        }
        
        resolve_address (params, address, s);

        return;
      }
      default:
        s++;
        break;
    }
  }
}

//��������� ���� Android
void process_android_log (Params& params)
{    
  stl::string line;
  
  line.reserve (MAX_LINE_SIZE);
  
  while (!feof (stdin))
  {
    try
    {
      if (!read_line (stdin, line))
        continue;

      printf ("%s", line.c_str ()); fflush (stdout);

      const char* line_start = strchr (line.c_str (), ':');
      
      if (!line_start)
      {
        printf ("\n");
        fflush (stdout);
        continue;
      }

      line_start++;

      if (!*line_start)
      {
        printf ("\n");
        fflush (stdout);        
        continue;
      }

      if (!strcmp (line_start, "Crash detected"))
      {
        reload_map_files (params);
        
        params.segments.clear ();
      }
      else if (const char* s = strstr (line_start, "segment "))
      {
        parse_segment (params, s);
      }
      else
      {
        process_log_line (params, line_start);
      }
      
      printf ("\n");
      fflush (stdout);      
    }
    catch (std::exception& e)
    {
      printf ("\n");
      fflush (stdout);      
      
      fprintf (stderr, "%s\n    at process_android_log\n", e.what ());
    }
    catch (...)
    {
      printf ("\n");
      fflush (stdout);      
      
      fprintf (stderr, "unknown exception\n    at process_android_log\n");            
    }
  }
}

//����� �����
int main (int argc, const char* argv [])
{
  try
  {
      //�������������
      
    Params params;

    static Option options [] = {
      {xtl::bind (&command_line_help,    _1, xtl::ref (params)), "help",             '?',          0, "print help message"},
      {xtl::bind (&command_line_silent,  _1, xtl::ref (params)), "silent",           's',          0, "quiet mode"},
    };

    params.options                   = options;
    params.options_count             = sizeof (options) / sizeof (*options);
    params.print_help                = false;
    params.silent                    = false;


    common::CommandLine command_line;

    for (size_t i = 0; i < params.options_count; i++)
      command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

      //������ ��������� ������

    command_line.Process (argc, argv);

      // --help ������ �������� ��������� ������

    if (params.print_help)
      return 0;

    if (command_line.ParamsCount () < 1)
    {
      printf ("%s [<OPTIONS>] <SOURCE>\n  use: %s --help\n", argv [0], argv [0]);
      error ("no input file");
      return 1;
    }

      //�������� ����
      
    for (size_t i=0; i<command_line.ParamsCount (); i++)
    {
      const char* file_name = command_line.Param (i);
      
      MapFileDesc desc (file_name);
      
      params.map_files.push_back (desc);
    }

      //�������� ������������ �����

    validate (params);
      
      //������ �������

    process_android_log (params);

    return 0;
  }
  catch (std::exception& e)
  {
    fprintf (stderr, "%s\n", e.what ());
    return 1;
  }

  return 0;
}
