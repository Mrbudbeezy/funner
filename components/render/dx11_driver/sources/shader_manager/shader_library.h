///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderLibrary: public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderLibrary  (const DeviceManager& device_manager);
    ~ShaderLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderPtr CreateShader (const ShaderDesc& shader_desc, const LogFunction& error_log);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск лэйаута константного буфера по хэшу
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConstantBufferLayoutPtr FindConstantBufferLayout (size_t hash) const; 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление лэйаута константного буфера по хэшу
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConstantBufferLayoutPtr AddConstantBufferLayout    (const ConstantBufferLayoutPtr& layout);
    void                    RemoveConstantBufferLayout (size_t hash);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение синхронизатора
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderBuffersSynchronizer& GetSynchronizer (const ProgramParametersLayout& src_layout, const ConstantBufferLayout& dst_layout);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение целевого константного буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBufferPtr FindConstantBuffer (size_t hash) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация и удаление константного буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddConstantBuffer    (size_t hash, const TargetConstantBufferPtr& buffer);
    void RemoveConstantBuffer (size_t hash);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение программы, устанавливаемой в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    BindableProgram& GetBindableProgram (const Program& program, const ProgramParametersLayout* layout);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск входного лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    DxInputLayoutPtr FindInputLayout (size_t input_hash, size_t shader_hash) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация и удаление входного лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddInputLayout    (size_t input_hash, size_t shader_hash, const DxInputLayoutPtr& layout);
    void RemoveInputLayout (size_t input_hash, size_t shader_hash);

  private:
    typedef stl::pair<const ProgramParametersLayout*, const ConstantBufferLayout*> SyncLayoutPair;
    typedef stl::pair<const Program*, const ProgramParametersLayout*>              BindableProgramKey;

    void RemoveShaderByHash (size_t hash);
    void RemoveSynchronizer (const SyncLayoutPair&);
    void RemoveBindableProgram (const BindableProgramKey&);

  private:
    typedef stl::hash_map<size_t, Shader*>                              ShaderMap;
    typedef stl::hash_map<size_t, ConstantBufferLayout*>                BufferLayoutMap;
    typedef stl::hash_map<SyncLayoutPair, ShaderBuffersSynchronizerPtr> SyncLayoutMap;
    typedef stl::hash_map<size_t, TargetConstantBufferPtr>              BufferMap;
    typedef stl::hash_map<BindableProgramKey, BindableProgramPtr>       BindableProgramMap;
    typedef stl::pair<size_t, size_t>                                   InputLayoutKey;
    typedef stl::hash_map<InputLayoutKey, DxInputLayoutPtr>             InputLayoutMap;

  private:
    ShaderMap          shaders;            //шейдеры
    BufferLayoutMap    buffer_layouts;     //лэйауты константных буферов
    SyncLayoutMap      layout_syncs;       //синхронизаторы лэйаутов
    BufferMap          buffers;            //целевые константные буферы
    BindableProgramMap bindable_programs;  //программы, устанавливаемые в контекст
    InputLayoutMap     input_layouts;      //входные лэйауты
};

typedef xtl::com_ptr<ShaderLibrary> ShaderLibraryPtr;
