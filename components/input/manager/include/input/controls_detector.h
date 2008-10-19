#ifndef INPUT_SYSTEM_CONTROLS_DETECTOR_HEADER
#define INPUT_SYSTEM_CONTROLS_DETECTOR_HEADER

#include <input/events_source.h>

#include <xtl/iterator.h>

#include <common/serializer_manager.h>

namespace input
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ControlsDetector
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    class IFilter
    {
      public:
        virtual const char* Action      () = 0;
        virtual const char* EventMask   () = 0;
        virtual const char* Replacement () = 0;
        
      protected:
        virtual ~IFilter () {}
    };

    typedef xtl::iterator<IFilter> Iterator;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
   ControlsDetector  ();
   ControlsDetector  (const char* file_name);
   ControlsDetector  (const ControlsDetector&);
   ~ControlsDetector ();

   ControlsDetector& operator = (const ControlsDetector&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add    (const char* action, const char* input_event_mask, const char* replacement);
    void Remove (const char* action, const char* input_event_mask);
    void Remove (const Iterator&);
    void Remove (const char* action);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator CreateIterator () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ��� ���������� ���� � ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* action, const char* event, const char* replacement)> EventHandler;

    xtl::connection Detect (const EventsSource& source, const char* action, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ControlsDetector&);    

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (ControlsDetector&, ControlsDetector&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<void (const char* file_name, ControlsDetector& map), 
                                          void (const char* file_name, const ControlsDetector& map)> ControlsDetectorManager;

}

#endif
