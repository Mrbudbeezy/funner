#ifndef RENDER_MANAGER_MATERIAL_HEADER
#define RENDER_MANAGER_MATERIAL_HEADER

#include <render/texture.h>

namespace media
{

namespace rfx
{

//forward declaration
class Material;
class MaterialLibrary;
class EffectLibrary;
class ShaderLibrary;

}

}

namespace render
{

//implementation forwards
class MaterialImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material  (const Material&);
    ~Material ();
    
    Material& operator = (const Material&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const media::rfx::Material&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Material&);

  private:
    Material (MaterialImpl*);
  
  private:
    MaterialImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Material&, Material&);

}

#endif
