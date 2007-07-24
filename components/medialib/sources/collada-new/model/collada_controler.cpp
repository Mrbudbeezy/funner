#include <media/collada/controller.h>

using namespace medialib::collada;

/*
    ���������� ����������
*/

struct Controller::Impl
{
  ControllerLibrary&        library; //����������
  medialib::collada::Skin&  skin;    //����, ��������� � ������������
  medialib::collada::Morph& morph;   //����, ��������� � ������������
  medialib::collada::ControllerType type;

  Impl (collada::ControllerType in_type, collada::Morph& in_morph, collada::Skin&  in_skin, ControllerLibrary& in_library) : type (in_type), 
  skin (skin), morph (morph), library (in_library) {}
};

/*
    ����������� / ����������
*/

Controller::Controller (medialib::collada::ControllerType type, medialib::collada::Morph& morph, medialib::collada::Skin&  skin,
                        ControllerLibrary& library, const char* id)
  : Entity (library.Owner (), id),
    impl (new Impl (type, morph, skin, library))
    {}

Controller::~Controller ()
{
  delete impl;
}

/*
    ����������
*/

ControllerLibrary& Controller::Library () const
{
  return impl->library;
}

/*
    ��� �����������
*/
ControllerType Controller::Type () const
{
  return impl->type;
}

/*
    ���� / ����
*/

const medialib::collada::Morph& Controller::Morph () const
{
  return impl->morph;
}

medialib::collada::Morph& Controller::Morph ()
{
  return impl->morph;
}

const medialib::collada::Skin& Controller::Skin () const
{
  return impl->skin;
}

medialib::collada::Skin& Controller::Skin ()
{
  return impl->skin;
}
