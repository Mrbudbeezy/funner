///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Render: public xtl::noncopyable, public xtl::reference_counter
{
  friend class Scene;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Render  (const RenderManager&);
    ~Render ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScenePtr GetScene (scene_graph::Scene&);        
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static RenderPtr GetRender (const RenderManager& manager);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterScene   (scene_graph::Scene& src_scene, Scene& render_scene);
    void UnregisterScene (scene_graph::Scene& src_scene);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
