class TargetConstantBuffer;

typedef xtl::com_ptr<TargetConstantBuffer> TargetConstantBufferPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TargetConstantBufferPrototype: public xtl::reference_counter, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBufferPrototype  (ShaderLibrary& library, const ProgramParametersLayout& src_layout, const ProgramBufferLayout& dst_layout);
    ~TargetConstantBufferPrototype ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBufferPtr CreateBuffer (const DeviceManager& device_manager, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char        GetSourceBuffersCount   () const { return indices.size (); }
    const unsigned char* GetSourceBuffersIndices () const { return indices.empty () ? (unsigned char*)0 : &indices [0]; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ������� ������ ��� ���������� �����
/// (��� �������� ������������ ����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetMinSourceBufferSize      (size_t index) const { return synchronizer.GetMinSourceBufferSize (indices [index]); }
    size_t GetMinDestinationBufferSize ()             const { return synchronizer.GetMinDestinationBufferSize (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
/// (��� �������� ������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (size_t index, const char* src_buffer, char* dst_buffer) const
    {
      synchronizer.CopyTo (indices [index], src_buffer, dst_buffer);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11Buffer& buffer, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const;

  private:
    typedef stl::vector<unsigned char> IndexArray;
    typedef ProgramBufferLayout::Slot  Slot;

  private:
    ShaderBuffersSynchronizer& synchronizer; //�������������
    const ProgramBufferLayout& dst_layout;   //������� ������
    IndexArray                 indices;      //�������
    size_t                     slots_count;  //���������� ������
    const Slot*                slots;        //�����
};

typedef xtl::intrusive_ptr<TargetConstantBufferPrototype> TargetConstantBufferPrototypePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class TargetConstantBuffer: public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBuffer  (const DeviceManager&, TargetConstantBufferPrototype& prototype, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]);
    ~TargetConstantBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11DeviceContext&, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]);

  private:
    struct SourceBufferDesc
    {
      SourceConstantBuffer* buffer; //�������� �����
      size_t                hash;   //��� ���������� ������

      SourceBufferDesc (SourceConstantBuffer* in_buffer)
        : buffer (in_buffer)
        , hash ()
      {
      }
    };

    typedef stl::vector<SourceBufferDesc> BufferDescArray;

  private:
    TargetConstantBufferPrototype& prototype;   //��������
    DxBufferPtr                    dst_buffer;  //������� �����  
    BufferDescArray                src_buffers; //�������� ������
};
