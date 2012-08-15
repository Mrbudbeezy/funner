#include <clocale>
#include <cstdio>

#include <stl/string>

#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/utf_converter.h>

#include <media/font_converter.h>
#include <media/image.h>

using namespace media;
using namespace common;

const char* FONT_FILE_NAME         = "data/a_damn_mess.ttf";
const char* RESULT_FONT_FILE_NAME  = "/io/stdout/a_damn_mess.xfont";
const char* RESULT_IMAGE_FILE_NAME = "data/a_damn_mess.tga";

const size_t FIRST_GLYPH_CODE = 32;
const size_t LAST_GLYPH_CODE  = 127;
const size_t FONT_SIZE        = LAST_GLYPH_CODE - FIRST_GLYPH_CODE + 1;

void dump_hash (const char* file_name)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

void dump_hash (media::Image& image)
{
  printf ("Image '%s' hash: {", image.Name ());

  unsigned char hash[16];

  md5 (image.Bitmap (), image.Width () * image.Height () * get_bytes_per_pixel (image.Format ()), hash);

  for (size_t i=0;i<15;i++)
    printf ("%02x,", hash [i]);

  printf ("%02x}\n", hash [15]);
}

void log_handler (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
}

int main ()
{
  printf ("Results of font_converter_test:\n");

  common::LogFilter log_filter ("media.font.font_converter", log_handler);

  try
  {
    Font     font;
    Image    image (1, 1, 1, PixelFormat_RGBA8);
    FontDesc font_desc;

    memset (&font_desc, 0, sizeof (font_desc));

    xtl::uninitialized_storage<size_t> utf32_buffer (FONT_SIZE);

    for (size_t i = FIRST_GLYPH_CODE; i <= LAST_GLYPH_CODE; i++)
      utf32_buffer.data () [i - FIRST_GLYPH_CODE] = i;

    font_desc.char_codes       = utf32_buffer.data ();
    font_desc.char_codes_count = utf32_buffer.size ();
    font_desc.file_name        = FONT_FILE_NAME;
    font_desc.glyph_size       = 64;
    font_desc.first_glyph_code = FIRST_GLYPH_CODE;
    font_desc.glyph_interval   = 5;
    font_desc.fast_convert     = false;

    convert (font_desc, font, image);

    image.Rename (RESULT_IMAGE_FILE_NAME);

    font.Rename       (RESULT_FONT_FILE_NAME);
    font.SetImageName (RESULT_IMAGE_FILE_NAME);

    setlocale (LC_ALL, "C");

    font.Save  (RESULT_FONT_FILE_NAME);
    image.Save (RESULT_IMAGE_FILE_NAME);

    dump_hash (image);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
