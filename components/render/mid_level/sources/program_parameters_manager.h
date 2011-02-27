///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ProgramParametersSlot
{
  ProgramParametersSlot_Frame,
  ProgramParametersSlot_Environment,
  ProgramParametersSlot_Transformations,
  ProgramParametersSlot_Entity,
  ProgramParametersSlot_Material,
  
  ProgramParametersSlot_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ProgramParametersManager: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramParametersManager  (const LowLevelDevicePtr&);
    ~ProgramParametersManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Convert (const common::PropertyMap*          in_parameters [ProgramParametersSlot_Num],
                  LowLevelProgramParametersLayoutPtr& out_layout,
                  LowLevelBufferPtr                   out_constant_buffers [ProgramParametersSlot_Num]);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
