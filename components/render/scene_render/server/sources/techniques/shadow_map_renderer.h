///������� ���������� ����� �����
class ShadowMapRenderer: public BasicRenderer
{
  public:
///����������� / ����������
    ShadowMapRenderer  (server::RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapRenderer ();

///��������� ������� �����
    manager::Texture& ShadowMap () { return shadow_map; }

///��������� ����� ��������� ��������
    const char* LocalTextureName () { return local_texture_name.c_str (); }

///���������� �����
    void UpdateShadowMap (RenderingContext& parent_context, Light& light);

  private:
    typedef stl::vector<manager::RenderTarget> RenderTargetArray;

  private:
    manager::Texture  shadow_map;         //����� �����
    RenderTargetArray render_targets;     //���� ����������
    TraverseResult    traverse_result;    //��������� ������
    stl::string       render_target_name; //��� ���� ����������
    stl::string       local_texture_name; //��� ��������� ��������
};


