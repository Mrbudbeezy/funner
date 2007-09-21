#include <sound/manager.h>
#include <sound/device.h>
#include <stl/hash_set>
#include <xtl/intrusive_ptr.h>

using namespace sound;

/*
    �������� ���������� SoundManager
*/

typedef stl::hash_set<Emitter*>               EmitterSet;
typedef xtl::com_ptr<low_level::ISoundDevice> DevicePtr;

struct SoundManager::Impl
{
  syslib::Window&             window;          //����
  DevicePtr                   device;          //���������� ���������������
  sound::WindowMinimizeAction minimize_action; //��������� ��� ������������ ����
  float                       volume;          //���������� ���������
  bool                        is_muted;        //���� ���������� ������������ �����
  sound::Listener             listener;        //��������� ���������
  EmitterSet                  emitters;        //���������� �����
};
