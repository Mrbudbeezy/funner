///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputLayoutManager: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputLayoutManager  (const LowLevelDevicePtr&, const SettingsPtr&);
    ~InputLayoutManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelInputLayoutPtr CreateInputLayout (size_t hash, const render::low_level::InputLayoutDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::geometry::VertexFormat Clone (const media::geometry::VertexFormat& format) const;

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
