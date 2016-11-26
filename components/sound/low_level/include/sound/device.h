#ifndef SOUND_SYSTEM_LOW_LEVEL_HEADER
#define SOUND_SYSTEM_LOW_LEVEL_HEADER

#include <exception>

#include <xtl/functional_fwd>

#include <math/vector.h>

#include <sound/listener.h>

namespace sound
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим работы seek
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SeekMode
{
  SeekMode_Clamp,   //обрезание по длине проигрывания
  SeekMode_Repeat   //остаток от деления на длину проигрывания
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник звука
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Source
{
  math::vec3f position;            //позиция
  math::vec3f direction;           //направление
  math::vec3f velocity;            //скорость
  float       gain;                //громкость
  float       minimum_gain;        //минимальная громкость
  float       maximum_gain;        //максимальная громкость
  float       inner_angle;         //угол внутреннего конуса излучения
  float       outer_angle;         //угол внешнего конуса излучения
  float       outer_gain;          //громкость внутри внешнего конуса
  float       reference_distance;  //расстояние, с которого громкость равна volume
  float       maximum_distance;    //расстояние незатухания громкости
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Характеристики устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Capabilities
{
  unsigned short channels_count; //количество поддерживаемых каналов
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Дескриптор сэмпла звука
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SampleDesc
{
  unsigned int   samples_count;   //количество сэмплов
  unsigned int   frequency;       //количество сэмплов в секунду (частота)
  unsigned short channels;        //количество каналов
  unsigned char  bits_per_sample; //количество битов на сэмпл
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///Сэмпл с которым работает устройство
//////////////////////////////////////////////////////////////////////////////////////////////////
class ISample
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени сэмпла
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора сэмпла
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (SampleDesc& desc) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ISample () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневая система воспроизведения звука
///////////////////////////////////////////////////////////////////////////////////////////////////
struct IDevice
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Name () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации об устройстве
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetCapabilities (Capabilities&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество микшируемых каналов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned short ChannelsCount () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание сэмпла для проигрывания
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<unsigned int (unsigned int first_sample, unsigned int samples_count, void* data)> SampleReadFunction;

    virtual ISample* CreateSample (const char* name) = 0;
    virtual ISample* CreateSample (const SampleDesc& desc, const SampleReadFunction& fn) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего проигрываемого звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void     SetSample (unsigned short channel, ISample* sample) = 0;
    virtual ISample* GetSample (unsigned short channel) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка цикличности проигрывания канала
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsLooped (unsigned short channel) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров источника
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetSource (unsigned short channel, const Source&) = 0;
    virtual void GetSource (unsigned short channel, Source&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление проигрыванием
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void  Play      (unsigned short channel, bool looping = false) = 0;
    virtual void  Pause     (unsigned short channel) = 0;
    virtual void  Stop      (unsigned short channel) = 0;
    virtual void  Seek      (unsigned short channel, float time_in_seconds, SeekMode seek_mode) = 0;
    virtual float Tell      (unsigned short channel) = 0;
    virtual bool  IsPlaying (unsigned short channel) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка уровня громкости для устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void  SetVolume (float gain) = 0;
    virtual float GetVolume () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Блокировка проигрывания звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetMute (bool state) = 0;
    virtual bool IsMuted () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров слушателя
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetListener (const Listener&) = 0;
    virtual void GetListener (Listener&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetParamsNames  () = 0; //возвращение строку имён параметров устройства, разделитель - пробел
    virtual bool        IsIntegerParam  (const char* name) = 0;
    virtual bool        IsStringParam   (const char* name) = 0;
    virtual void        SetIntegerParam (const char* name, int value) = 0;
    virtual int         GetIntegerParam (const char* name) = 0;
    virtual void        SetStringParam  (const char* name, const char* value) = 0;
    virtual const char* GetStringParam  (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление подсчётом ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IDevice () {}
};

}

}

#endif
