#ifndef RENDER_SCENE_INTERCHANGE_PROPERTY_SYNCHRONIZER_HEADER
#define RENDER_SCENE_INTERCHANGE_PROPERTY_SYNCHRONIZER_HEADER

#include <stl/auto_ptr.h>

#include <common/property_map.h>

#include <render/scene/interchange/streams.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPropertyMapWriterListener
{
  public:
    virtual void OnPropertyMapRemoved    (uint64 id) = 0;
    virtual void OnPropertyLayoutRemoved (uint64 id) = 0;

  protected:
    virtual ~IPropertyMapWriterListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapWriter: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapWriter  (IPropertyMapWriterListener* listener = 0);
    ~PropertyMapWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&, const common::PropertyMap&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������������� ���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapAutoWriter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapAutoWriter  (IPropertyMapWriterListener* listener = 0);
    ~PropertyMapAutoWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach (const common::PropertyMap&);
    void Detach (const common::PropertyMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� ������� (� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&, const common::PropertyMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapReader: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapReader  ();
    ~PropertyMapReader ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    common::PropertyMap GetProperties (uint64 id) const;
    bool                HasProperties (uint64 id) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Read (InputStream&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveProperties (uint64 id);
    void RemoveLayout     (uint64 id);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
