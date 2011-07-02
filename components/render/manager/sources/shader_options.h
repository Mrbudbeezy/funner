///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShaderOptions
{
  stl::string options;      //������ �����
  size_t      options_hash; //��� ������ �����
  
  ShaderOptions () : options_hash (0xffffffff) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderOptionsLayout: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderOptionsLayout  ();
    ~ShaderOptionsLayout ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CachedShaderBuildersCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      Size   ();
    const char* Item   (size_t index);
    void        Add    (const char* name);
    void        Remove (const char* name);
    void        Clear  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Hash ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 GetShaderOptions        (const common::PropertyMap& defines, ShaderOptions& out_options);
    const ShaderOptions& GetDefaultShaderOptions ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������� �� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable ();
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderOptionsCache: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderOptionsCache  ();
    ~ShaderOptionsCache ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ������������ ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CachedLayoutsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ShaderOptions& GetShaderOptions (ShaderOptionsLayout& layout);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

