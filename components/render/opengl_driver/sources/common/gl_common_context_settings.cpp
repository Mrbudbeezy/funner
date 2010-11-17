#include "shared.h"

using namespace render::low_level::opengl;

/*
    �������� ���������� ����������� ��������
*/

struct ContextSettings::Impl
{
  ExtensionSet required_extensions;                       //����������, ������������� � ������ ������������
  ExtensionSet enabled_extensions;                        //����������, ����������� � ������������� � ������ �������������
  Version      min_version;                               //����������� ��������� ������ OpenGL
  Version      max_version;                               //������������ ����������� ������ OpenGL
  bool         need_check_errors;                         //����� �� ��������� ������
  size_t       int_settings [ContextSettingsInteger_Num]; //������������� ��������� ���������
  bool         is_ffp_allowed;                          //�������������� �� ffp
  
///�����������
  Impl (const char* init_string)
    : need_check_errors (true), is_ffp_allowed (true)
  {
      //�������� ������������ ����������
    
    if (!init_string)
      throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextSettings::ContextSettings::Impl", "init_string");
      
      //������������� ����������
      
    enabled_extensions.Set (true);

    memset (int_settings, 0, sizeof (int_settings));

      //������ ������ �������������
    
    common::parse_init_string (init_string, xtl::bind (&Impl::SetProperty, this, _1, _2));    
  }
  
///��������� ��������
  void SetProperty (const char* name, const char* value)
  {
    if (!xtl::xstricmp (name, "require"))
    {
      required_extensions.SetGroup (value, true);
      return;
    }

    if (!xtl::xstricmp (name, "disable"))
    {     
      enabled_extensions.SetGroup (value, false);
      return;
    }

    if (!xtl::xstricmp (name, "ffp"))
    {
      if (!xtl::xstrcmp (value, "0"))
        is_ffp_allowed = false;

      return;
    }

    if (!xtl::xstricmp (name, "enable"))
    {
      enabled_extensions.SetGroup (value, true);
      return;
    }

    if (!xtl::xstrnicmp (name, "gl_", 3))
    {
      if (!xtl::xstrcmp (value, "0"))
      {
        enabled_extensions.Set (name, false);
        return;
      }
      
      if (!xtl::xstrcmp (value, "1"))
      {
        enabled_extensions.Set (name, true);
        return;
      }
      
      if (!xtl::xstrcmp (value, "!"))
      {
        required_extensions.Set (name, true);
        return;
      }
      
      if (!xtl::xstrcmp (value, "1!"))
      {
        enabled_extensions.Set (name, true);
        required_extensions.Set (name, true);

        return;
      }
    }
    
    if (!xtl::xstricmp (name, "min_version"))
    {
      min_version = Version (value);
      return;
    }
    
    if (!xtl::xstricmp (name, "max_version"))
    {
      max_version = Version (value);
      return;
    }
    
    if (!xtl::xstricmp (name, "max_texture_size"))
    {
      int_settings [ContextSettingsInteger_MaxTextureSize] = atoi (value);
      return;
    }
    
    if (!xtl::xstricmp (name, "max_anisotropy"))
    {
      int_settings [ContextSettingsInteger_MaxAnisotropy] = atoi (value);
      return;
    }
    
    if (!xtl::xstricmp (name, "texture_units_count"))
    {
      int_settings [ContextSettingsInteger_TextureUnitsCount] = atoi (value);
      return;
    }    

    if (!xtl::xstricmp (name, "check_gl_errors"))
    {
      need_check_errors = atoi (value) != 0;
      return;
    }
  }    
};

/*
    ����������� / ����������
*/

ContextSettings::ContextSettings (const char* init_string)
  : impl (new Impl (init_string))
{
}

ContextSettings::~ContextSettings ()
{
}

/*
    ��������� ����������
*/

//������������ ����������
const ExtensionSet& ContextSettings::RequiredExtensions () const
{
  return impl->required_extensions;
}

//����������� ����������
const ExtensionSet& ContextSettings::EnabledExtensions () const
{
  return impl->enabled_extensions;
}

/*
    ����� �� ��������� ������
*/

bool ContextSettings::IsNeedCheckErrors () const
{
  return impl->need_check_errors;
}

/*
    ������ OpenGL
*/

//����������� ��������� ������ OpenGL
const Version& ContextSettings::MinVersion () const
{
  return impl->min_version;
}

//������������ ������ (��� ������� ����������� ����������������)
const Version& ContextSettings::MaxVersion () const
{
  return impl->max_version;
}

/*
    ��������� ������������� �������� ���������
*/

size_t ContextSettings::GetInteger (ContextSettingsInteger tag) const
{
  if (tag < 0 || tag >= ContextSettingsInteger_Num)
    throw xtl::make_argument_exception ("render::low_level::opengl::ContextSettings::GetInteger", "tag", tag);

  return impl->int_settings [tag];
}

/*
   �������������� �� ffp
*/

bool ContextSettings::IsFfpAllowed () const
{
  return impl->is_ffp_allowed;
}
