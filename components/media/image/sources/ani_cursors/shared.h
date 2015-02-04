#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/file.h>
#include <common/property_map.h>
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
  char     tag [4]; //RIFF tag
  uint32_t length;  //length of file
};

///Animation cursor header
struct AniHeader
{
  uint32_t header_size;    // ������ AniHeader (36 bytes)
  uint32_t images_count;   // ���������� ����������� � �����
  uint32_t frames_count;   // ���������� ������ � �����
  uint32_t cx, cy;         // ���������������
  uint32_t bits_per_pixel; // ���������������, ������ ���� ����� ����
  uint32_t planes_count;   // ���������������, ������ ���� ����� ����
  uint32_t fps;            // ���������� ������ � �������
  uint32_t flags;          // �����
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

}

}
