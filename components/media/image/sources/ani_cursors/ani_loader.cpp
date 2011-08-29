#include "shared.h"

using namespace media;
using namespace media::ani_cursors;

namespace components
{

namespace ani_cursor_loader
{

/*
    ��������� ANI ��������
*/

class AniLoader
{
  public:
///�����������
    AniLoader (const char* file_name, Image& image)
      : file (file_name)
    {
      try
      {
        memset (&ani_header, 0, sizeof ani_header);
        
          //������ �����
        
        RiffHeader riff_header;
        
        Read (riff_header);
        
        if (memcmp ("RIFF", riff_header.tag, sizeof (riff_header.tag)))
          throw xtl::format_operation_exception ("", "No 'RIFF' chunk in file");
          
        if (file.Size () < riff_header.length)
          throw xtl::format_operation_exception ("", "RiffHeader require file length %u that less than file size %u", riff_header.length, file.Size ());

        char acon_tag [4];
        
        Read (acon_tag);
        
        if (memcmp ("ACON", acon_tag, sizeof acon_tag))
          throw xtl::format_operation_exception ("", "No 'ACON' chunk in file");
          
        while (!file.Eof ())
        {
          char tag [4];
          
          Read (tag);
          
          size_t chunk_size = 0;
          
          Read (chunk_size);
          
          size_t new_pos = file.Tell () + chunk_size; 
          
          if      (!memcmp ("anih", tag, sizeof tag)) ParseAniHeader ();
          else if (!memcmp ("LIST", tag, sizeof tag)) ParseFramesList ();
          else if (!memcmp ("seq ", tag, sizeof tag)) ParseSequence ();

          if (new_pos != (size_t)file.Seek (new_pos))
            throw xtl::format_operation_exception ("", "Can' skip chunk '%s'", tag);
        }
        
          //���������� �����������
          
        if (!images.size ())
          throw xtl::format_operation_exception ("", "Empty cursor file");
          
        if (frames.empty ())
          frames = images;          

        Image result (frames.size (), &frames [0]);

        result.Swap (image);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursors::AniLoader::AniLoader('%s')", file_name);
        throw;
      }
    }
    
  private:    
///������ ���������
    void ParseAniHeader ()
    {
      Read (ani_header);
      
      if (ani_header.header_size != sizeof (AniHeader))
        throw xtl::format_operation_exception ("media::ani_cursors::AniLoader::ReadAniHeader", "Ani header size %u less than expected size %u",
          ani_header.header_size, sizeof (AniHeader));
    }        
    
///������ ������ ������
    void ParseFramesList ()
    {
      try
      {
        images.reserve (ani_header.images_count);
        
        char tag [4];
        
        Read (tag);
        
        if (!memcmp ("INAM", tag, sizeof tag) || !memcmp ("IART", tag, sizeof tag))
        {
          size_t chunk_size = 0;
          
          Read (chunk_size);
          
          size_t new_pos = file.Tell () + chunk_size;

          if (new_pos != (size_t)file.Seek (new_pos))
            throw xtl::format_operation_exception ("", "Can' skip chunk '%s'", tag);
        }
        else if (!memcmp ("fram", tag, sizeof tag))
        {
          for (size_t i=0; i<ani_header.images_count; i++)    
            ParseFrame ();
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursor::AniLoader::ParseFramesList");
        throw;
      }
    }
    
///������ �����
    void ParseFrame ()
    {
      try
      {
        char tag [4];
        
        Read (tag);
        
        if (memcmp ("icon", tag, sizeof tag))
          throw xtl::format_operation_exception ("", "Can't read frame. No tag 'icon' found");
          
        size_t length = 0;
        
        Read (length);
        
        icon_buffer.resize (length, false);

        size_t read_size = file.Read (icon_buffer.data (), length);

        if (read_size != length)
          throw xtl::format_operation_exception ("", "Can't read %u bytes from input file", length);

        common::MemFile mem_file (icon_buffer.data (), length);        
        
        stl::string old_path = mem_file.Path (), new_path = common::format ("%s.ico", old_path.c_str ());

        common::FileSystem::Rename (old_path.c_str (), new_path.c_str ());

        images.push_back (media::Image (new_path.c_str ()));
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursors::AniLoader::ParseFrame");
        throw;
      }
    }
    
///������ ������������������ ������
    void ParseSequence ()
    {
      try
      {
        stl::vector<size_t> frame_indexes (ani_header.frames_count);
        
        size_t required_size = frame_indexes.size () * sizeof (size_t),
               read_size     = file.Read (&frame_indexes [0], required_size);
        
        if (read_size != required_size)
          throw xtl::format_operation_exception ("", "Can't read %u bytes from input file", required_size);
          
        frames.reserve (ani_header.frames_count);
        
        for (size_t i=0; i<frame_indexes.size (); i++)
        {
          size_t index = frame_indexes [i];
          
          if (index >= images.size ())
            throw xtl::format_operation_exception ("", "Attempt to access to image #%u in sequence with %u images", index, images.size ());

          frames.push_back (images [index]);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::AniCursorLoader::ParseSequence");
        throw;
      }
    }

///������ ������
    template <class T>
    void Read (T& data)
    {
      try
      {
        size_t read_size = file.Read (&data, sizeof (T));
      
        if (read_size != sizeof (T))
          throw xtl::format_operation_exception ("", "Can't read %u bytes from input file", sizeof (T));
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::ani_cursors::AniLoader::Read");
        throw;
      }
    }

  private:
    typedef stl::vector<Image> ImageList;
    typedef stl::vector<Image> FrameList;

  private:
    AniHeader                        ani_header;  //��������� �������������� �������
    common::InputFile                file;        //���� � ������� �������
    xtl::uninitialized_storage<char> icon_buffer; //����� ��� ������ ������ �����������
    ImageList                        images;      //������ �����������
    FrameList                        frames;      //������ ������
};

/*
    ���������
*/

class Component
{
  public:
    Component ()
    {
      ImageManager::RegisterLoader ("ani", &Component::LoadImage);
    }

  private:
    static void LoadImage (const char* file_name, Image& image)
    {
      AniLoader (file_name, image);
    }
};

extern "C" common::ComponentRegistrator<Component> AniCursorLoader ("media.image.loaders.ani_cursor");

}

}
