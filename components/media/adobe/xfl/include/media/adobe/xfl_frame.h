#ifndef MEDIALIB_ADOBE_XFL_FRAME_HEADER
#define MEDIALIB_ADOBE_XFL_FRAME_HEADER

#include <cstddef>

#include <media/adobe/xfl_animation_core.h>
#include <media/adobe/xfl_collection.h>
#include <media/adobe/xfl_frame_element.h>

namespace media
{

namespace adobe
{

namespace xfl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кадр анимации
///////////////////////////////////////////////////////////////////////////////////////////////////
class Frame
{
  public:
    typedef INamedCollection<FrameElement> FrameElementList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Frame  ();
    Frame  (const Frame&);
    ~Frame ();

    Frame& operator = (const Frame&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тайминги
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FirstFrame    () const;              //номер кадра (в глобальном времени), с которого начинается отображение данного кадра
    size_t Duration      () const;              //длительность отображения данного кадра в количестве кадров (в глобальном времени)
    void   SetFirstFrame (size_t first_frame);
    void   SetDuration   (size_t duration);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кадры анимации слоя
///////////////////////////////////////////////////////////////////////////////////////////////////
          FrameElementList& Elements ();
    const FrameElementList& Elements () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Анимация кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
    const AnimationCore& Animation    () const;
          AnimationCore& Animation    ();
    void                 SetAnimation (const AnimationCore& animation);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Frame&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Frame&, Frame&);

}

}

}

#endif
