#ifndef SOUND_LOW_LEVEL_TESTS_SHARED_HEADER
#define SOUND_LOW_LEVEL_TESTS_SHARED_HEADER

#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/common_exceptions.h>

#include <sound/driver.h>
#include <sound/device.h>

using namespace sound::low_level;

//тестовый драйвер
class TestDriver: virtual public IDriver, public xtl::reference_counter
{
  public:
    TestDriver () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "TestDriver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных устройств
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount ()
    {
      throw xtl::make_not_implemented_exception ("TestDriver::GetDevicesCount");
      return 0;
    }
    const char* GetDeviceName (size_t index)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::GetDeviceName");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создаение устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateDevice");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }
    void Release () { release (this); }
};

#endif
