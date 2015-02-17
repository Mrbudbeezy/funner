///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureImpl  (const DeviceManagerPtr& manager,
                  TextureDimension        dimension,
                  unsigned int            width,
                  unsigned int            height,
                  unsigned int            depth,
                  PixelFormat             format,
                  bool                    generate_mipmaps,
                  const char*             name = "");
    TextureImpl  (const DeviceManagerPtr&       manager,
                  const low_level::TextureDesc& desc,
                  const char*                   name = "");                  
    TextureImpl  (const DeviceManagerPtr&       manager,
                  TextureDimension              dimension,
                  const media::CompressedImage& image,
                  const char*                   name = "");
    ~TextureImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    ();
    void        SetName (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelTexturePtr DeviceTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureDimension Dimension ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PixelFormat  Format ();
    unsigned int Width  ();
    unsigned int Height ();
    unsigned int Depth  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr RenderTarget (unsigned int layer, unsigned int mip_level);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const media::Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture (unsigned int layer, unsigned int mip_level, media::Image& image);
    void Capture (unsigned int mip_level, media::Image& image);
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
