///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene: public xtl::noncopyable, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene  (scene_graph::Scene& scene, const RenderPtr& render);
    ~Scene ();    

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
