#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/vector>

#include <common/strlib.h>

#include <media/image.h>
#include <media/atlas_builder.h>

/*
    ���������
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;
const char*  DEFAULT_ATLAS_FILE_NAME    = "images.xatlas";
const char*  DEFAULT_LAYOUT_FILE_SUFFIX = ".xatlas";

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

typedef void (*ProcessOption)(const char* arg, Params& params);

//�����
struct Option
{
  ProcessOption process;       //�������-���������� �����
  const char*   name;          //��� �������
  char          short_name;    //�������� ���
  const char*   argument_name; //��� ���������
  const char*   tip;           //���������    
};

typedef stl::vector<const char*> StringArray;

//��������� �������
struct Params
{
  const Option* options;          //������ �����  
  size_t        options_count;    //���������� �����
  StringArray   sources;          //��� �������� �����������
  stl::string   atlas_file_name;  //��� ��������������� �����������
  stl::string   layout_file_name; //��� ����� ��������
  bool          silent;           //����������� ����� ���������  
  bool          print_help;       //����� �� �������� ��������� ������  
  bool          need_layout;      //����� ������������ ���� ��������
  bool          need_pot_rescale; //����� �� �������������� ����������� � �������� ������� ������� ������
  bool          invert_x;         //�������������� ���������� X ������
  bool          invert_y;         //�������������� ���������� Y ������
  bool          swap_axises;      //����� ���� �������
};

//��������� ��������� �� ���������
void command_line_help (const char*, Params& params)
{
  printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n");
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

//��������� ����� ��������������� ����� ������
void command_line_result_atlas (const char* file_name, Params& params)
{
  params.atlas_file_name = file_name;  
}

//��������� ����� ��������������� ����� ��������
void command_line_result_layout (const char* file_name, Params& params)
{
  params.layout_file_name = file_name;
}

//��������� ����� ��������� ����� ��������
void command_line_no_layout (const char* file_name, Params& params)
{
  params.need_layout = false;
}

//��������� ��������� ��������������� � ������� ������
void command_line_pot (const char*, Params& params)
{
  params.need_pot_rescale = true;  
}

//��������� ��������� ������ ��������� ����������
void command_line_silent (const char*, Params& params)
{
  params.silent = true;
}

//��������� ��������� �������������� ������ �� ��� X
void command_line_invert_x (const char*, Params& params)
{
  params.invert_x = true;
}

//��������� ��������� �������������� ������ �� ��� Y
void command_line_invert_y (const char*, Params& params)
{
  params.invert_y = true;
}

//��������� ��������� ������ ���� �������
void command_line_swap_axises (const char*, Params& params)
{
  params.swap_axises = true;
}

//������ ��������� ������
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_help,          "help",       '?',      0, "print help message"},
    {command_line_silent,        "silent",     's',      0, "quiet mode"},    
    {command_line_result_atlas,  "atlas",      'o', "file", "set output atlas file"},    
    {command_line_result_layout, "layout",     'l', "file", "set output layout file"},
    {command_line_no_layout,     "no-layout",   0,       0, "don't generatoe layout file"},    
    {command_line_pot,           "pot",         0,       0, "resize atlas texture to nearest greater power of two sizes"},
    {command_line_invert_x,      "invert-x",    0,       0, "invert X coordinate in layout of tiles"},
    {command_line_invert_y,      "invert-y",    0,       0, "invert Y coordinate in layout of tiles"},
    {command_line_swap_axises,   "swap-axises", 0,       0, "swap axises at layout tiles"},
  };
  
  static const size_t options_count = sizeof (options) / sizeof (*options);
  
  params.options       = options;
  params.options_count = options_count;

    //������ ��������� ������

  for (int i=1; i<argc; i++)
  {
    const char* arg = argv [i];    
    
    static const char*  LONG_OPTION_NAME_PREFIX        = "--";
    static const size_t LONG_OPTION_NAME_PREFIX_LENGTH = strlen (LONG_OPTION_NAME_PREFIX);

    bool long_option  = !strncmp (LONG_OPTION_NAME_PREFIX, arg, LONG_OPTION_NAME_PREFIX_LENGTH);
    bool short_option = !long_option && *arg == '-' && arg [1];    
    
    if (!long_option && !short_option)
    {
      params.sources.push_back (arg);
      
      continue;
    }

    stl::string   option_name;
    stl::string   option_argument;      
    const Option* option = 0;
    
      //������ ������� �����
    
    if (long_option)
    {
      arg += LONG_OPTION_NAME_PREFIX_LENGTH;
      
      const char* option_argument = 0;      
      const char* end_option_name = strchr (arg, '=');        
      
      if (end_option_name)
      {
        option_name.assign (arg, end_option_name);

        arg = end_option_name + 1;
      }
      else
      {
        option_name  = arg;          
        arg         += strlen (arg);
      }
      
      for (size_t j=0; j<options_count; j++)
      {
        const Option& test_option        = options [j];
        size_t        test_option_length = strlen (test_option.name);

        if (!strncmp (test_option.name, option_name.c_str (), test_option_length))
        {
          option = &test_option;
          break;
        }
      }
      
      if (!option)
      {
        error ("wrong option '--%s'", option_name.c_str ());
        
        return;
      }
      
      option_name = LONG_OPTION_NAME_PREFIX + option_name;
    }

      //������ �������� �����

    if (short_option)
    {
      arg++;
      
      if (arg [1])
      {
        error ("wrong option '-%c'", *arg);
        
        return;
      }        

      for (size_t j=0; j<options_count; j++)
      {
        if (*arg == options [j].short_name)
        {
          option = &options [j];
          
          break;
        }
      }
      
      if (!option)
      {
        error ("wrong option '-%c'", *arg);
        
        return;
      }

      option_name = common::format ("-%c", *arg);      
      
      if (option->argument_name)
      {
        i++;

        if (i >= argc)
        {
          error ("option '-%c' require argument", *arg);
          
          return;
        }                
        
        arg = argv [i];
      }            
    }
            
      //��������� ���������                      
      
    for (;*arg; arg++)
    {
      switch (*arg)
      {
        case '\'':
        case '"':
        {
          const char* end = strchr (arg + 1, *arg);
          
          if (end)
          {
            option_argument.append (arg + 1, end);
            
            arg = end;
          }
          else
          {
            error ("unquoted string at parse option '%s'", option_name.c_str ());
            
            return;
          }

          break;
        }
        default:
          option_argument.push_back (*arg);
          break;
      }
    }    
    
    if (option->argument_name && option_argument.empty ())
    {
      error ("option '%s' require argument", option_name.c_str ());
      
      return;
    }
    
    if (!option->argument_name && !option_argument.empty ())
    {
      error ("option '%s' has not arguments", option_name.c_str ());
      
      return;
    }
    
    if (option->process)
      option->process (option_argument.c_str (), params);
  }
}

//�������� ������������ �����
void validate (Params& params)
{
  if (params.atlas_file_name.empty ())
  {
    error ("no result atlas file");
    return;
  }
  
  if (params.layout_file_name.empty ())
  {
    params.layout_file_name = common::basename (params.atlas_file_name) + DEFAULT_LAYOUT_FILE_SUFFIX;
  }  

  if (params.sources.empty ())
  {
    printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n  use: atlas-builder --help\n");
    error ("no input files");
    return;
  }  
}

//���������� ������
void build (Params& params)
{
  try
  {
      //���������������� ����������� ������      
      
    media::AtlasBuilder builder;
    media::Image        atlas_image;
    media::Atlas        atlas;

    builder.SetAtlasImageName (params.atlas_file_name.c_str ());    
    
    for (size_t i=0, count=params.sources.size (); i<count; i++)
    {
      const char* source_name = params.sources [i];
      
      if (!params.silent)
        printf ("Load image '%s'", source_name);

      try
      {
        builder.Insert (source_name);
      }
      catch (...)
      {
        if (!params.silent)
          printf (" - Fail!\n");

        throw;
      }

      if (!params.silent)
        printf ("\n");
    }
    
      //���������� ������
      
    if (!params.silent)
      printf ("Build atlas");

    size_t pack_flags = 0;

    if (params.need_pot_rescale) pack_flags |= media::AtlasPackFlag_PowerOfTwoEdges;
    if (params.invert_x)         pack_flags |= media::AtlasPackFlag_InvertTilesX;
    if (params.invert_y)         pack_flags |= media::AtlasPackFlag_InvertTilesY;
    if (params.swap_axises)      pack_flags |= media::AtlasPackFlag_SwapAxises;

    try
    {
      builder.Build (atlas, atlas_image, pack_flags);
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }
    
    if (!params.silent)
      printf (" %ux%u\n", atlas_image.Width (), atlas_image.Height ());
    
      //���������� ������
      
    if (!params.silent)
    {
      if (params.need_layout) printf ("Save atlas '%s' and layout '%s'", params.atlas_file_name.c_str (), params.layout_file_name.c_str ());
      else                    printf ("Save atlas '%s'", params.atlas_file_name.c_str ());
    }

    try
    {
      atlas_image.Save (params.atlas_file_name.c_str ());
      
      if (params.need_layout)
        atlas.Save (params.layout_file_name.c_str ());
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }

    if (!params.silent)
      printf ("\nBuild successfull!\n");
  }
  catch (std::exception& e)
  {
    error ("%s", e.what ());
  }
}

int main (int argc, const char* argv [])
{  
    //�������������

  Params params;
  
  params.options          = 0;
  params.options_count    = 0;
  params.atlas_file_name  = DEFAULT_ATLAS_FILE_NAME;
  params.print_help       = false;
  params.silent           = false;
  params.need_layout      = true;
  params.need_pot_rescale = false;
  params.invert_x         = false;
  params.invert_y         = false;
  params.swap_axises      = false;

    //������ ��������� ������

  command_line_parse (argc, argv, params);
  
    // --help ������ �������� ��������� ������
  
  if (params.print_help)
    return 0;
    
    //�������� ������������ �����

  validate (params);
  
    //���������� ������

  build (params);

  return 0;
}
