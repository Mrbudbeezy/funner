///������� ���������� ����� �����
class ShadowMapBuilder: public BasicTechnique
{
  public:
///����������� / ����������
    ShadowMapBuilder  (RenderManager& manager, const common::ParseNode& node);
    ~ShadowMapBuilder ();

///��������� ������� �����
    manager::Texture ShadowMap () const;

  private:
    manager::Texture shadow_map; //����� �����
};


