#ifndef SCENE_GRAPH_LIGHT_HEADER
#define SCENE_GRAPH_LIGHT_HEADER

#include <mathlib.h>
#include <sg/entity.h>

namespace sg
{

const float DEFAULT_LIGHT_RANGE = 1e9;  //������ �������� ��������� ����� �� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightType
{
  LightType_Direct, //������������
  LightType_Spot,   //����������
  LightType_Point   //��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Light : public Entity
{
  public: 
    typedef Entity BaseClass;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Light* Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetType (LightType type);
    sg::LightType Type    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////          
    void               SetLightColor (const math::vec3f& color);
    const math::vec3f& LightColor    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetSpotAngle    (float angle);
    void  SetSpotExponent (float exponent);
    float SpotAngle       () const;
    float SpotExponent    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� (constant, linear, quadratic)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetAttenuation (const math::vec3f& multiplier);
    const math::vec3f& Attenuation    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetRange (float range);
    float Range    () const;    

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Light  ();
    ~Light ();    

  private:
    void AcceptCore (Visitor&);

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
