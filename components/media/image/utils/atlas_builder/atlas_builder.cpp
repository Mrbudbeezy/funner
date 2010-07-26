#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>

#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/list>
#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/strlib.h>

#include <media/image.h>
#include <media/atlas_builder.h>

/*
    ���������
*/

const size_t HELP_STRING_PREFIX_LENGTH  = 30;
const char*  DEFAULT_ATLAS_FILE_NAME    = "images.png";
const char*  DEFAULT_LAYOUT_FILE_NAME   = "layout.xatlas";

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
  const Option* options;                  //������ �����
  size_t        options_count;            //���������� �����
  StringArray   sources;                  //��� �������� �����������
  stl::string   atlas_file_format;        //��� ��������������� �����������
  stl::string   layout_atlas_file_format; //��� ��������������� ����������� � ������
  stl::string   layout_file_name;         //��� ����� ��������
  stl::string   isolated_images_wildcard; //����� ��� ������������� ����������� �� �������� � ����� �����
  size_t        max_image_size;           //������������ ������ ������ �����������
  size_t        margin;            //������ ����� ���������� � ������  
  bool          silent;                   //����������� ����� ���������
  bool          print_help;               //����� �� �������� ��������� ������
  bool          need_layout;              //����� ������������ ���� ��������
  bool          need_pot_rescale;         //����� �� �������������� ����������� � �������� ������� ������� ������
  bool          invert_x;                 //�������������� ���������� X ������
  bool          invert_y;                 //�������������� ���������� Y ������
  bool          swap_axises;              //����� ���� �������
  bool          square_axises;          //���������� ��������
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

//��������� ������� ����� ��������������� ����� ������
void command_line_result_atlas (const char* file_name, Params& params)
{
  params.atlas_file_format = file_name;
}

//��������� ������� ����� ��������������� ����� ������ ��� ��������� ����� ��������
void command_line_result_layout_atlas (const char* file_name, Params& params)
{
  params.layout_atlas_file_format = file_name;
}

//��������� ����� ��������������� ����� ��������
void command_line_result_layout (const char* file_name, Params& params)
{
  params.layout_file_name = file_name;
}

//��������� ������������� ������� ����� ��������
void command_line_max_image_size (const char* size, Params& params)
{
  params.max_image_size = atoi (size);
}

//��������� ������� ����� ����������
void command_line_margin (const char* size, Params& params)
{
  params.margin = atoi (size);
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

//��������� ��������� ���������� �������
void command_line_square_axises (const char*, Params& params)
{
  params.square_axises = true;
}

//��������� ����� ������������� �����������
void command_line_isolated_images (const char* string, Params& params)
{
  params.isolated_images_wildcard = string;
}

//������ ��������� ������
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_help,                "help",            '?',        0, "print help message"},
    {command_line_silent,              "silent",          's',        0, "quiet mode"},
    {command_line_result_atlas,        "atlas",           'o',   "file", "set output atlas file format"},
    {command_line_result_layout_atlas, "layout-atlas",    0,     "file", "set output atlas file format that used in layout"},
    {command_line_result_layout,       "layout",          'l',   "file", "set output layout file"},
    {command_line_max_image_size,      "max-image-size",  0,     "size", "set maximum atlas image side size"},
    {command_line_margin,              "margin",          0,     "size", "set margin beetween images in atlas"},    
    {command_line_no_layout,           "no-layout",       0,          0, "don't generate layout file"},
    {command_line_pot,                 "pot",             0,          0, "resize atlas texture to nearest greater power of two sizes"},
    {command_line_invert_x,            "invert-x",        0,          0, "invert X coordinate in layout of tiles"},
    {command_line_invert_y,            "invert-y",        0,          0, "invert Y coordinate in layout of tiles"},
    {command_line_swap_axises,         "swap-axises",     0,          0, "swap axises at layout tiles"},
    {command_line_isolated_images,     "isolated-images", 0, "wildcard", "set wildcard for standalone images which will be isolate from general atlases"},
    {command_line_square_axises,       "square",          0,          0, "square axises"},    
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
  if (params.atlas_file_format.empty ())
  {
    error ("no result atlas file");
    return;
  }
  
  if (params.layout_file_name.empty ())
    params.layout_file_name = DEFAULT_LAYOUT_FILE_NAME;

  if (params.sources.empty ())
  {
    printf ("atlas-builder [<OPTIONS>] <SOURCE> ...\n  use: atlas-builder --help\n");
    error ("no input files");
    return;
  }  
  
  if (params.layout_atlas_file_format.empty ())
  {
    params.layout_atlas_file_format = params.atlas_file_format;
  }
}

//���������� ������
void build (Params& params)
{
  try
  {
      //�������� ��������
    stl::hash_map<stl::hash_key<const char*>, media::Image> images;

    for (size_t i = 0, count = params.sources.size (); i < count; i++)
      images.insert_pair (params.sources [i], media::Image (params.sources [i]));

      //���������������� ����������� ������

    media::AtlasBuilder builder;
    media::Image        atlas_image;
    media::Atlas        atlas, result_atlas;
    size_t              pack_flags  = 0,
                        atlas_index = 0;
    StringArray         sources_to_process = params.sources;

      //��������� ������������� �����������
      
    common::StringArray isolated_images_wildcard_list = common::split (params.isolated_images_wildcard.c_str ());
      
    stl::hash_set<stl::hash_key<const char*> > is_isolated_image;      

    for (size_t i=0, count=sources_to_process.size (); i<count; i++)
    {
      const char* source       = sources_to_process [i];      
      bool        need_isolate = false;

      for (size_t i=0, count=isolated_images_wildcard_list.Size (); i<count; i++)
      {
        const char* mask = isolated_images_wildcard_list [i];        

        if (common::wcmatch (source, mask))
        {
          need_isolate = true;
          break;
        }
      }
      
      if (!need_isolate)
        continue;        

      is_isolated_image.insert (source);
    }
    
      //���������� �������
      
    if (params.need_pot_rescale) pack_flags |= media::AtlasPackFlag_PowerOfTwoEdges;
    if (params.invert_x)         pack_flags |= media::AtlasPackFlag_InvertTilesX;
    if (params.invert_y)         pack_flags |= media::AtlasPackFlag_InvertTilesY;
    if (params.swap_axises)      pack_flags |= media::AtlasPackFlag_SwapAxises;      
    if (params.square_axises)    pack_flags |= media::AtlasPackFlag_SquareAxises;
      
    while (!sources_to_process.empty ())
    {
      if (!params.silent)
        printf ("Build atlas ");

      builder.Reset ();

      typedef stl::pair<const char*, size_t> SourceDescPair;
      typedef stl::list<SourceDescPair>      ProcessedSourcesList;

      ProcessedSourcesList processed_sources;

      for (size_t i = 0, count = sources_to_process.size (); i < count; i++)
      {
        const char* source      = sources_to_process [i];
        bool        is_isolated = is_isolated_image.find (source) != is_isolated_image.end ();
        
        if (is_isolated && !processed_sources.empty ()) //������������� ����������� ������ ������� � ����� ����
          continue;

          //������� �������� ����������� � ������� �����

        try
        {
          builder.Insert (images [source], media::AtlasBuilderInsertMode_Reference);
        }
        catch (...)
        {
          if (!params.silent)
            printf ("- Fail!\n");

          throw;
        }

        size_t result_image_width = 0, result_image_height = 0;

        try
        {
          builder.GetBuildResults (result_image_width, result_image_height, params.margin, pack_flags);
        }
        catch (...)
        {
          if (!params.silent)
            printf ("- Fail!\n");

          throw;
        }        

        if (result_image_width > params.max_image_size || result_image_height > params.max_image_size)
        {         
          builder.Reset ();

          for (ProcessedSourcesList::iterator iter = processed_sources.begin (), end = processed_sources.end (); iter != end; ++iter)
            builder.Insert (images [iter->first], media::AtlasBuilderInsertMode_Reference);
        }
        else
        {
          processed_sources.push_back (SourceDescPair (sources_to_process [i], i));
          
          if (is_isolated)
            break; //���� ������������� ����������� ����� ���� ��������� - ����� ��� � ����� ����
        }
      }

      if (processed_sources.empty ())
        throw xtl::format_operation_exception ("build", "Can't build atlas, image '%s' size greater than maximum atlas image size %u",
          params.sources [0], params.max_image_size);

        //���������� ������

      stl::string atlas_file_name, layout_atlas_file_name;

      if (atlas_index || processed_sources.size () != params.sources.size ())
      {
        stl::string atlas_file_format, layout_atlas_file_format;
        
        if (params.atlas_file_format.find ("%") == stl::string::npos)
          atlas_file_format = common::format ("%s%%u%s", common::basename (params.atlas_file_format).c_str (), common::suffix (params.atlas_file_format).c_str ());          
        else
          atlas_file_format = params.atlas_file_format;

        atlas_file_name = common::format (atlas_file_format.c_str (), atlas_index);
        
        if (params.layout_atlas_file_format.find ("%") == stl::string::npos)
          layout_atlas_file_format = common::format ("%s%%u%s", common::basename (params.layout_atlas_file_format).c_str (), common::suffix (params.layout_atlas_file_format).c_str ());                  
        else
          layout_atlas_file_format = params.layout_atlas_file_format;
          
        layout_atlas_file_name = common::format (layout_atlas_file_format.c_str (), atlas_index);
      }
      else
      {
        atlas_file_name        = common::format (params.atlas_file_format.c_str (), atlas_index);
        layout_atlas_file_name = common::format (params.layout_atlas_file_format.c_str (), atlas_index);
      }

      builder.SetAtlasImageName (layout_atlas_file_name.c_str ());

      try
      {
        builder.Build (atlas, atlas_image, params.margin, pack_flags);

        for (size_t i = 0, count = atlas.TilesCount (); i < count; i++)
          result_atlas.Insert (atlas.Tile (i));
          
        result_atlas.SetImageSize (layout_atlas_file_name.c_str (), atlas.ImageSize (layout_atlas_file_name.c_str ()));
      }
      catch (...)
      {
        if (!params.silent)
          printf ("- Fail!\n");

        throw;
      }

      if (!params.silent)
        printf ("%ux%u\n", atlas_image.Width (), atlas_image.Height ());

      if (!params.silent)
        printf ("Save atlas '%s'\n", atlas_file_name.c_str ());

      try
      {
        atlas_image.Save (atlas_file_name.c_str ());
      }
      catch (...)
      {
        if (!params.silent)
          printf (" - Fail!\n");

        throw;
      }

      for (ProcessedSourcesList::reverse_iterator iter = processed_sources.rbegin (), end = processed_sources.rend (); iter != end; ++iter)
        sources_to_process.erase (sources_to_process.begin () + iter->second);

      atlas_index++;
    }

    try
    {
      if (params.need_layout)
      {
        if (!params.silent)
          printf ("Save layout '%s'\n", params.layout_file_name.c_str ());

        result_atlas.Save (params.layout_file_name.c_str ());
      }
    }
    catch (...)
    {
      if (!params.silent)
        printf (" - Fail!\n");

      throw;
    }

    if (!params.silent)
      printf ("Build successfull!\n");
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
  
  params.options           = 0;
  params.options_count     = 0;
  params.atlas_file_format = DEFAULT_ATLAS_FILE_NAME;
  params.max_image_size    = -1;
  params.margin            = 0;
  params.print_help        = false;
  params.silent            = false;
  params.need_layout       = true;
  params.need_pot_rescale  = false;
  params.invert_x          = false;
  params.invert_y          = false;
  params.swap_axises       = false;
  params.square_axises     = false;

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
