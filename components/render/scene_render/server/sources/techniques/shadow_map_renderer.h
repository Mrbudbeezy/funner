///������� ���������� ����� �����
class ShadowMapRenderer: public BasicRenderer
{
  public:
///����������� / ����������
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///��������� ������� �����
    manager::Texture& ShadowMap () { return shadow_map; }

///���������� �����
    void UpdateShadowMap (RenderingContext& parent_context, Light& light, ITraverseResultCache& traverse_result_cache);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  shadow_map;         //����� �����
    RenderTargetArray render_targets;     //���� ����������
    stl::string       render_target_name; //��� ���� ����������
};


