///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialImpl: public Object, public CacheSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialImpl  (const DeviceManagerPtr&, const TextureManagerPtr&, const ShadingManagerPtr&);
    ~MaterialImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Id    ();
    void        SetId (const char* id);    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelStateBlockPtr StateBlock ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������������ ���������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramParametersLayoutPtr ParametersLayout ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                   TexturesCount ();
    TexturePtr               Texture       (size_t index); //����� ������� 0 � ������ ������������ ��������
    LowLevelTexturePtr       DeviceTexture (size_t index); //����� ������� 0 � ������ ������������ ��������
    const char*              TextureName   (size_t index);
    LowLevelSamplerStatePtr  Sampler       (size_t index);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t        TagsCount ();
    const size_t* Tags      ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �� � ��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasDynamicTextures ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const media::rfx::Material&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
