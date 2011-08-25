#include <cstdio>
#include <exception>

#include <stl/string>

#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/video.h>
#include <media/image.h>

using namespace media;

const char* APPLICATION_NAME    = "theora_unpack";
const char* DEFAULT_RESULTS_DIR = "results";

const size_t HELP_STRING_PREFIX_LENGTH  = 30;

struct Params;

typedef void (*ProcessOption)(const char* arg, Params& params);

//������
struct rgba_t
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

//�����
struct Option
{
  ProcessOption process;       //�������-���������� �����
  const char*   name;          //��� �������
  char          short_name;    //�������� ���
  const char*   argument_name; //��� ���������
  const char*   tip;           //���������
};

//��������� �������
struct Params
{
  const Option* options;                  //������ �����
  size_t        options_count;            //���������� �����
  stl::string   theora_name;              //��� ��������� �����
  stl::string   output_frames_dir_name;   //��� �������� � ����������� �������
  stl::string   frames_format;            //������ ������
  stl::string   mask_color;               //���� �����
  stl::string   mask_color_range;         //������� ����� �����
  bool          print_help;               //����� �� �������� ��������� ������
};

//������ ������ � ������� �� ���������
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

//��������� ��������� �� ���������
void command_line_help (const char*, Params& params)
{
  printf ("%s [<OPTIONS>] <SOURCE> ...\n", APPLICATION_NAME);
  printf ("  OPTIONS:\n");

  for (size_t i = 0; i < params.options_count; i++)
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

//��������� ����� ���� ���������� ������
void command_line_result_frames_dir (const char* file_name, Params& params)
{
  params.output_frames_dir_name = file_name;
}

//��������� ������� ���������� ������
void command_line_result_frames_format (const char* value, Params& params)
{
  params.frames_format = value;
}

//��������� ����� �����
void command_line_mask_color (const char* value, Params& params)
{
  params.mask_color = value;
}

//��������� �������� ����� �����
void command_line_mask_color_range (const char* value, Params& params)
{
  params.mask_color_range = value;
}

//������ ��������� ������
void command_line_parse (int argc, const char* argv [], Params& params)
{
  static Option options [] = {
    {command_line_result_frames_dir,        "frames-dir",          'o', "dir",       "set output frames directory"},
    {command_line_result_frames_format,     "frames-format",       0,   "string",    "set output frames format string"},
    {command_line_mask_color,               "mask-color",          0,   "string",    "set mask color"},
    {command_line_mask_color_range,         "mask-range",          0,   "string",    "set mask color range"},
    {command_line_help,                     "help",                '?', 0,           "print help message"},
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
      if (!params.theora_name.empty ())
      {
        error ("source file must be one");
        return;
      }

      params.theora_name = arg;

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
      else
        arg = "";
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

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

//�������� ������������ �����
void validate (Params& params)
{
  if (params.theora_name.empty ())
  {
    printf ("%s [<OPTIONS>] <SOURCE>\n  use: %s --help\n", APPLICATION_NAME, APPLICATION_NAME);
    error ("no input file");
    return;
  }

  if (params.output_frames_dir_name.empty ())
    params.output_frames_dir_name = common::notdir (common::basename (params.theora_name));
}

bool colors_equal (const rgba_t& color1, const rgba_t& color2, const rgba_t& range)
{
  return ((int)color1.r >= (int)color2.r - (int)range.r) && ((int)color1.r <= (int)color2.r + (int)range.r) &&
         ((int)color1.g >= (int)color2.g - (int)range.g) && ((int)color1.g <= (int)color2.g + (int)range.g) &&
         ((int)color1.b >= (int)color2.b - (int)range.b) && ((int)color1.b <= (int)color2.b + (int)range.b);
}

int main (int argc, const char *argv[])
{
  common::LogFilter log_filter ("*", &log_handler);

  //�������������
  Params params;

  params.options          = 0;
  params.options_count    = 0;
  params.frames_format    = "frame%03u.png";
  params.mask_color       = "255;0;255";
  params.mask_color_range = "0;0;0";
  params.print_help       = false;

  //������ ��������� ������
  command_line_parse (argc, argv, params);

  // --help ������ �������� ��������� ������
  if (params.print_help)
    return 0;

  //�������� ������������ �����
  validate (params);

  if (!common::FileSystem::IsDir (params.output_frames_dir_name.c_str ()))
    common::FileSystem::Mkdir (params.output_frames_dir_name.c_str ());

  stl::string image_name_format = common::format ("%s/%s", params.output_frames_dir_name.c_str (), params.frames_format.c_str ());

  common::StringArray mask_color_components       = common::split (params.mask_color, ";"),
                      mask_color_range_components = common::split (params.mask_color_range, ";");

  if (mask_color_components.Size () != 3 || mask_color_components.Size () != 3)
  {
    printf ("invalid mask color format, must be '12;34;56'\n");
    return 0;
  }

  rgba_t mask_color, mask_color_range;

  mask_color.r       = atoi (mask_color_components [0]),
  mask_color.g       = atoi (mask_color_components [1]),
  mask_color.b       = atoi (mask_color_components [2]),
  mask_color_range.r = atoi (mask_color_range_components [0]),
  mask_color_range.g = atoi (mask_color_range_components [1]),
  mask_color_range.b = atoi (mask_color_range_components [2]);

  try
  {
    VideoStream stream (params.theora_name.c_str ());

    Image image (stream.Width (), stream.Height (), 1, PixelFormat_RGBA8);

    for (size_t i = 0, count = stream.FramesCount (); i < count; i++)
    {
      stream.Decode (i, image);

      rgba_t* pixel = (rgba_t*)image.Bitmap ();

      for (size_t j = 0, count = image.Width () * image.Height (); j < count; j++, pixel++)
        if (colors_equal (*pixel, mask_color, mask_color_range))
          memset (pixel, 0, sizeof (*pixel));

      stl::string image_name = common::format (image_name_format.c_str (), i);

      image.Save (image_name.c_str ());
    }
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
