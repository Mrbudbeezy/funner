#include "shared.h"

#include <common/time.h>

using namespace script;

namespace
{

const char* COMMON_TIMER_LIBRARY = "Common.Timer";

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Timer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Timer ()
    {
      Reset ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� / �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Pause ()
    {
      if (paused)
        return;
      
      offset += common::milliseconds () - start_time;
      paused  = true;
    }
    
    void Resume ()
    {
      if (!paused)
        return;

      paused     = false;
      start_time = common::milliseconds ();
    }
    
    bool IsPaused () const { return paused; }
    
    void SetPaused (bool state)
    {
      if (state) Pause ();
      else       Resume ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset ()
    {
      start_time = common::milliseconds ();
      paused     = false;
      offset     = 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ���������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SecondsEllapsed ()
    {
      return paused ? offset / 1000.0f : (common::milliseconds () - start_time + offset) / 1000.f;
    }

  private:
    bool   paused;
    size_t start_time;
    size_t offset;
};

}

Timer create_timer ()
{
  return Timer ();
}

namespace engine
{

void bind_common_timer (script::Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_TIMER_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_timer));

    //����������� ��������

  lib.Register ("Reset",               make_invoker (&Timer::Reset));
  lib.Register ("Pause",               make_invoker (&Timer::Pause));
  lib.Register ("Resume",              make_invoker (&Timer::Resume));  
  lib.Register ("get_Paused",          make_invoker (&Timer::IsPaused));
  lib.Register ("set_Paused",          make_invoker (&Timer::SetPaused));
  lib.Register ("get_SecondsEllapsed", make_invoker (&Timer::SecondsEllapsed));

    //����������� ����� ������

  environment.RegisterType<Timer> (COMMON_TIMER_LIBRARY);
}

}
