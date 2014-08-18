///������� ������� ���������� ��������
class BasicTechnique: public Technique
{
  public:
///����������� / ����������
    BasicTechnique  (RenderManager& in_manager, const common::ParseNode& node);
    ~BasicTechnique ();

  protected:
///������ ��� ���������� ������� � ������ �����
    struct PrivateData
    {
      BasicRendererPtr renderer;

      PrivateData (BasicTechnique& technique) : renderer (new BasicRenderer (technique.manager, technique.effect_name.c_str ()), false) {}
    };

///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder);

///���������� �������
    void UpdatePropertiesCore ();

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap&);

  private:
    RenderManager manager;     //�������� ����������
    stl::string   effect_name; //��� �������
};
