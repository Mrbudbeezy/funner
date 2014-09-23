///������� ���������� ����� �����
class ShadowMapRenderer: public BasicRenderer
{
  public:
///����������� / ����������
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///��������� ������� �����
    manager::Texture& ShadowMap () { return depth_map; }

///��������� ����� ��������� ��������
    const char* LocalTextureName () { return local_texture_name.c_str (); }

///���������� �����
    void UpdateShadowMap (RenderingContext& parent_context, Light& light);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  color_map;                //����� �����
    manager::Texture  depth_map;                //����� �����
    RenderTargetArray color_render_targets;     //���� ����������
    RenderTargetArray depth_render_targets;     //���� ����������
    TraverseResult    traverse_result;          //��������� ������
    stl::string       color_render_target_name; //��� ���� ����������
    stl::string       depth_render_target_name; //��� ���� ����������
    stl::string       local_texture_name;       //��� ��������� ��������
};


