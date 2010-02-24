#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_GFX_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_GFX_HEADER

namespace scene_graph
{

///������ ��������
class ShotGfx : public SpriteList
{
  public:
    typedef xtl::com_ptr <ShotGfx>       Pointer;
    typedef xtl::com_ptr <const ShotGfx> ConstPointer;

///�������� �������
    static Pointer Create (const math::vec4f& color, float distance);

    void               SetShotDirection (const math::vec3f& in_shot_direction) { shot_direction = in_shot_direction; }
    const math::vec3f& ShotDirection    () const { return shot_direction; }

  protected:
///����������� / ����������
    ShotGfx (const math::vec4f& in_color, float distance);
    ~ShotGfx ();

///�����, ���������� ��� ��������� ������� �������
    void AcceptCore (Visitor& visitor);

  private:
///���������� �������
    void UpdateEffect (float dt);

  private:
    math::vec4f          color;
    math::vec3f          shot_direction;
    float                life_time;
    xtl::auto_connection update_connection;
};

///������� ������ ���������� � ������
class BillboardSpriteList : public SpriteList
{
  public:
    typedef xtl::com_ptr <BillboardSpriteList>       Pointer;
    typedef xtl::com_ptr <const BillboardSpriteList> ConstPointer;

///��������
    static Pointer Create ();

  protected:
///����������� / ����������
    BillboardSpriteList ();
    ~BillboardSpriteList ();

///�����, ���������� ��� ��������� ������� �������
    void AcceptCore (Visitor& visitor);
};

}

#endif
