struct BindableProgramContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������, ��������������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BindableProgram: public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BindableProgram  (ShaderLibrary& library, const Program& program, const ProgramParametersLayout* layout);
    ~BindableProgram ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Program&                 GetProgram                 () const { return program; }
    const ProgramParametersLayout* GetProgramParametersLayout () const { return parameters_layout; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11DeviceContext&          context,
               ShaderLibrary&                library,
               const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT],
               const InputLayout&            input_layout,
               BindableProgramContext&       program_context);

  private:
    typedef stl::vector<TargetConstantBufferPrototypePtr> BufferPrototypeArray;
    
  private:
    const Program&                 program;           //�������� ���������
    const ProgramParametersLayout* parameters_layout; //������ ���������� ��������� (����� ���� �������)
    BufferPrototypeArray           buffer_prototypes; //��������� �������
};

typedef xtl::intrusive_ptr<BindableProgram> BindableProgramPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BindableProgramContext
{
  bool             has_dirty_buffers;                                  //������������ ������� ������
  bool             program_binded;                                     //��������� ����������� � ��������
  bool             dirty_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //"�������" ������
  DxInputLayoutPtr input_layout;                                       //������� ������  

/// �����������
  BindableProgramContext ()
  {
    Reset ();
  }

/// ����� ���������
  void Reset ()
  {
    program_binded    = false;
    has_dirty_buffers = true;

    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      dirty_buffers [i] = true;

    input_layout = DxInputLayoutPtr ();
  }
};
