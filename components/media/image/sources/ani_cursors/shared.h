#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/file.h>
#include <common/strlib.h>

#include <media/image.h>

namespace media
{

namespace ani_cursors
{

#ifdef _MSC_VER
  #pragma pack (push, 1)
#else
  #pragma pack (1)
#endif

///Riff data header 
struct RiffHeader
{
  char   tag [4]; //RIFF tag
  size_t length;  //length of file
};

///Animation cursor header
struct AniHeader
{
  size_t header_size;    // ������ AniHeader (36 bytes)
  size_t images_count;   // ���������� ����������� � �����
  size_t frames_count;   // ���������� ������ � �����
  size_t cx, cy;         // ���������������
  size_t bits_per_pixel; // ���������������, ������ ���� ����� ����
  size_t planes_count;   // ���������������, ������ ���� ����� ����
  size_t fps;            // ���������� ������ � �������
  size_t flags;          // �����
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

}

}
