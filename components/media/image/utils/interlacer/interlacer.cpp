#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/command_line.h>
#include <common/strlib.h>

#include <media/image.h>

using namespace common;
using namespace media;

/*
    ���������
*/

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

/*
    ��������� ��������� ������
*/

struct Params;

//�����
struct Option
{
  CommandLine::SwitchHandler handler;       //���������� �����
  const char*                name;          //��� �������
  char                       short_name;    //�������� ���
  const char*                argument_name; //��� ���������
  const char*                tip;           //���������
};

//��������� �������
struct Params
{
  const Option* options;                  //������ �����
  size_t        options_count;            //���������� �����
  StringArray   sources;                  //��� �������� �����������
  stl::string   output_file_format;       //����� ���� �������� ������
  bool          silent;                   //����������� ����� ���������
  bool          print_help;               //����� �� �������� ��������� ������
};

//��������� ��������� �� ���������
void command_line_help (const char*, Params& params)
{
  printf ("interlacer [<OPTIONS>] <SOURCE> ...\n");
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

//��������� ��������� ����� ���� �������� ������
void command_line_output_file_format (const char* value, Params& params)
{
  params.output_file_format = value;
}

//�������� ������������ �����
void validate (Params& params)
{
  if (params.sources.IsEmpty ())
  {
    printf ("interlacer [<OPTIONS>] <SOURCE> ...\n  use: atlas-builder --help\n");
    error ("no input files");
    return;
  }
}

//���������� ������
void build (Params& params)
{
  try
  {
    Image dst_image;
    
    for (size_t i = 0, count = params.sources.Size (); i < count; i++)
    {
      if (!params.silent)
      {        
        printf ("...processing %s\n", params.sources [i]);      
        fflush (stdout);
      }
      
      Image src_image (params.sources [i]);   

      if (i > 0)  //�������� �� ����������� ��� ������� ����� � ������������������
      {
        if (src_image.Format () != dst_image.Format ())
          error ("Image format mismatch on image '%s'", params.sources [i]);
          
        if (src_image.Width () != dst_image.Width () || src_image.Height () != dst_image.Height ())
          error ("Image size mismatch on image '%s'", params.sources [i]);
          
        if (src_image.Depth () != 1)
          error ("Image depth not equal to 1 on image '%s'", params.sources [i]);
          
        size_t width     = src_image.Width (),
               height    = src_image.Height (),
               copy_size = width * get_bytes_per_pixel (src_image.Format ()),
               offset    = (i % 2) * copy_size; 
          
        for (size_t y=i%2; y<height; y+=2, offset += copy_size * 2)
          memcpy ((char*)dst_image.Bitmap () + offset, (char*)src_image.Bitmap () + offset, copy_size);
      }
      else dst_image = src_image;
      
        //����������� ����� ��������������� �����������
        
      stl::string result_name = params.output_file_format.empty () ? params.sources [i] : format (params.output_file_format.c_str (), i);
      
      dst_image.Save (result_name.c_str ());      
    }
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
}

int main (int argc, const char* argv [])
{  
  Params params;

  static Option options [] = {
    {xtl::bind (&command_line_help,                _1, xtl::ref (params)), "help",   '?', 0,          "print help message"},
    {xtl::bind (&command_line_silent,              _1, xtl::ref (params)), "silent", 's', 0,          "quiet mode"},
    {xtl::bind (&command_line_output_file_format,  _1, xtl::ref (params)), "output", 'o', "wildcard", "output file name format"},    
  };

  static const size_t options_count = sizeof (options) / sizeof (*options);

    //�������������

  params.options           = options;
  params.options_count     = options_count;
  params.print_help        = false;
  params.silent            = false;

  CommandLine command_line;

  for (size_t i = 0; i < params.options_count; i++)
    command_line.SetSwitchHandler (options [i].name, options [i].short_name, options [i].argument_name, options [i].handler);

    //������ ��������� ������

  command_line.Process (argc, argv);
  
    // --help ������ �������� ��������� ������
  
  if (params.print_help)
    return 0;

  for (size_t i = 0, count = command_line.ParamsCount (); i < count; i++)
    params.sources.Add (command_line.Param (i));

    //�������� ������������ �����

  validate (params);
  
    //����������

  build (params);

  return 0;
}
