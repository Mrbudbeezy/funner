#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>

#include <media/image.h>
#include <media/video.h>

using namespace media;

namespace
{

/*
    ���������
*/

const char* VIDEO_DECODERS_COMPONENTS_MASK = "media.video.decoders.*"; //����� ��� ����������� �������� ���������� �����

/*
    ��������������� ���������
*/

typedef xtl::uninitialized_storage<char> TempBuffer;
typedef stl::auto_ptr<IVideoDecoder>     DecoderPtr;

/*
    ������� �� ���������
*/

class DefaultDecoder: public IVideoDecoder
{
  public:
    float  GetFramesPerSecond  () { return 1.f; }
    size_t GetWidth            () { return 0; }
    size_t GetHeight           () { return 0; }
    size_t GetFramesCount      () { return 0; }
    float  GetPixelAspectRatio () { return 1.0f; }
    
    void Decode (size_t frame, Pixel* data_buffer)
    {
      throw xtl::format_operation_exception ("media::DefaultDecoder::Decode", "Can't decode video from default decoder");
    }
    
    static DecoderPtr Create ()
    {
      return DecoderPtr (new DefaultDecoder);
    }
};

}

/*
    �������� ���������� ����� ������
*/

struct VideoStream::Impl
{
  DecoderPtr   decoder;            //������� �����
  stl::string  file_name;          //��� ����� (��� ����������� ����������� �����)
  stl::string  name;               //��� ����� ������
  VideoQuality quality;            //�������� ������������� �����
  size_t       frames_count;       //���������� ������
  size_t       frames_per_second;  //���������� ������ � �������
  size_t       width;              //������ �����
  size_t       height;             //������ �����
  float        pixel_aspect_ratio; //����������� ������ �����
  TempBuffer   temp_decode_buffer; //��������� ����� �������������
  
  Impl (const char* in_file_name, VideoQuality in_quality, DecoderPtr& in_decoder)
    : decoder (in_decoder)
    , file_name (in_file_name)
    , name (in_file_name)
    , quality (in_quality)
  {
    try
    {
      frames_count       = decoder->GetFramesCount ();
      frames_per_second  = decoder->GetFramesPerSecond ();
      width              = decoder->GetWidth ();
      height             = decoder->GetHeight ();
      pixel_aspect_ratio = decoder->GetPixelAspectRatio ();
      
      if (!frames_per_second)
        throw xtl::format_operation_exception ("", "Frames per second is null for '%s'", in_file_name);
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::VideoStream::Impl::Impl");
      throw;
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

VideoStream::VideoStream ()
{
  try
  {
    DecoderPtr decoder (DefaultDecoder::Create ());
    
    impl = new Impl ("", VideoQuality_High, decoder);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::VideoStream()");
    throw;
  }
}

VideoStream::VideoStream (const char* file_name, VideoQuality quality)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    switch (quality)
    {
      case VideoQuality_Low:
      case VideoQuality_Middle:
      case VideoQuality_High:
        break;
      default:
        throw xtl::make_argument_exception ("", "quality");
    }
    
    static common::ComponentLoader loader (VIDEO_DECODERS_COMPONENTS_MASK);

    DecoderPtr decoder (VideoStreamManager::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, quality));
    
    impl = new Impl (file_name, quality, decoder);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::VideoStream(const char*,VideoQuality)");
    throw;
  }
}

VideoStream::VideoStream (const VideoStream& stream)
{
  try
  {
    if (stream.impl->file_name.empty ())
    {
      VideoStream ().Swap (*this);
    }
    else
    {    
      Load (stream.impl->file_name.c_str (), stream.impl->quality);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::VideoStream(VideoStream&)");
    throw;
  }
}

VideoStream::~VideoStream ()
{
}

VideoStream& VideoStream::operator = (const VideoStream& stream)
{
  VideoStream (stream).Swap (*this);
  return *this;
}

/*
    ��� ������
*/

const char* VideoStream::Name () const
{
  return impl->name.c_str ();
}

void VideoStream::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::VideoStream::Rename", "new_name");
    
  impl->name = new_name;
}

/*
    ��������� ���������� �����
*/

VideoQuality VideoStream::Quality () const
{
  return impl->quality;
}

size_t VideoStream::FramesCount () const
{
  return impl->frames_count;
}

float VideoStream::FramesPerSecond  () const
{
  return impl->frames_per_second;
}

size_t VideoStream::Width () const
{
  return impl->width;
}

size_t VideoStream::Height () const
{
  return impl->height;
}

float VideoStream::PixelAspectRatio () const
{
  return impl->pixel_aspect_ratio;
}

float VideoStream::Duration () const
{
  return impl->frames_count / (float)impl->frames_per_second;
}

size_t VideoStream::FrameSize () const
{
  try
  {
    return Width () * Height () * sizeof (Pixel);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::FrameSize");
    throw;
  }  
}

/*
    ��������� ��������������� �����
*/

void VideoStream::Decode (size_t frame_number, Image& image, size_t image_offset_x, size_t image_offset_y, size_t image_offset_z)
{
  try
  {
    if (!image_offset_x && !image_offset_y && image.Width () == impl->width && image.Height () == impl->height && image.Format () == PixelFormat_RGBA8)
    {
        //������� ������ - ����������� � ����� ����������� ��������
        
      Decode (frame_number, reinterpret_cast<Pixel*> (image.Bitmap (image_offset_z)));

      return;
    }
    
      //����� ������
     
    if (impl->temp_decode_buffer.size () == 0)
    {
      impl->temp_decode_buffer.resize (FrameSize ());
    }
    
    Decode (frame_number, reinterpret_cast<Pixel*> (impl->temp_decode_buffer.data ()));
    
    image.PutImage (image_offset_x, image_offset_y, image_offset_z, impl->width, impl->height, 1, PixelFormat_RGBA8,
      impl->temp_decode_buffer.data ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::Decode(size_t,Image&,size_t,size_t)");
    throw;
  }
}

void VideoStream::Decode (size_t frame_number, Pixel* frame_buffer)
{
  try
  {
    if (frame_number >= impl->frames_count)
      throw xtl::make_range_exception ("", "frame_number", frame_number, impl->frames_count);
      
    if (!frame_buffer)
      throw xtl::make_null_argument_exception ("", "frame_buffer");
      
    impl->decoder->Decode (frame_number, frame_buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::Decode(size_t,Pixel*)");
    throw;
  }
}

/*
    �������� �����
*/

void VideoStream::Load (const char* file_name, VideoQuality quality)
{
  try
  {
    VideoStream (file_name, quality).Swap (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::VideoStream::Load");
    throw;
  }
}

/*
    �����
*/

void VideoStream::Swap (VideoStream& stream)
{
  impl.swap (stream.impl);
}

namespace media
{

void swap (VideoStream& stream1, VideoStream& stream2)
{
  stream1.Swap (stream2);
}

}
