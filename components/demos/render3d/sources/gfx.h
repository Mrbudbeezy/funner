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
    static Pointer Create ();

///���������� ��������
    void AddShot (const scene_graph::Node& gun, const math::vec4f& color, float distance);

///��������� ����������� ��������
    const math::vec3f& ShotDirection (size_t sprite) const;

  protected:
///����������� / ����������
    ShotGfx ();
    ~ShotGfx ();

///�����, ���������� ��� ��������� ������� �������
    void AcceptCore (Visitor& visitor);

  private:
///���������� �������
    void UpdateEffect (float dt);

  private:
    struct ShotDesc : public xtl::reference_counter
    {
      math::vec3f position;
      math::vec3f direction;
      math::vec4f color;
      float       distance;
      size_t      born_time;

      void AddRef  () { addref  (this); }
      void Release () { release (this); }
    };

  private:
    typedef xtl::com_ptr <ShotDesc>   ShotDescPtr;
    typedef stl::vector <ShotDescPtr> ShotsArray;

  private:
    size_t               time;
    ShotsArray           shots;
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
