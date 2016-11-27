///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизатор буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBuffersSynchronizer: public xtl::reference_counter, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательные структуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct Chunk
    {
      size_t                          slot;       //индекс слота
      size_t                          src_offset; //смещение в исходном буфере
      size_t                          dst_offset; //смещение в результирующем буфере
      size_t                          size;       //размер блока
      ShaderParameterConvertOperation operation;  //операция преобразования

      Chunk () : slot (), src_offset (), dst_offset (), size (), operation (ShaderParameterConvertOperation_Copy) {}

      bool operator < (const Chunk&) const;
    };

    struct Slot
    {
      size_t first_chunk;          //индекс первого блока
      size_t min_src_buffer_size;  //минимальный размер буфера

      Slot () : first_chunk (), min_src_buffer_size () {}
    };

    typedef stl::vector<Chunk> ChunkArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderBuffersSynchronizer (const ProgramParametersLayout& src_layout, const ConstantBufferLayout& dst_layout);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение минимального размера буфера для указанного слота
/// (без проверок корректности аргументов)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetMinSourceBufferSize      (size_t slot_index) const { return slots [slot_index].min_src_buffer_size; }
    size_t GetMinDestinationBufferSize ()                  const { return min_dst_buffer_size; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение слотов и блоков
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t       GetChunksCount () const { return chunks.size (); }
    const Chunk* GetChunks      () const { return chunks.empty () ? (Chunk*)0 : &chunks [0]; }
    const Slot*  GetSlots       () const { return &slots [0]; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование
/// (без проверок корректности аргументов)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (size_t slot_index, const char* src_buffer, char* dst_buffer) const
    {
      const Slot&  slot  = slots [slot_index];
      const Chunk* chunk = &chunks [0] + slot.first_chunk;

      for (; chunk->slot == slot_index; chunk++)
      {
        const void* src = src_buffer + chunk->src_offset;
        void*       dst = dst_buffer + chunk->dst_offset;

        switch (chunk->operation)
        {
          case ShaderParameterConvertOperation_Copy:
            memcpy (dst, src, chunk->size);
            break;
          case ShaderParameterConvertOperation_TransposeFloat2x2:
            math::detail::matrix_transpose ()(*reinterpret_cast<const math::mat2f*> (src), *reinterpret_cast<math::mat2f*> (dst));
            break;
          case ShaderParameterConvertOperation_TransposeFloat3x3:
            math::detail::matrix_transpose ()(*reinterpret_cast<const math::mat3f*> (src), *reinterpret_cast<math::mat3f*> (dst));
            break;
          case ShaderParameterConvertOperation_TransposeFloat4x4:
            math::detail::matrix_transpose ()(*reinterpret_cast<const math::mat4f*> (src), *reinterpret_cast<math::mat4f*> (dst));
            break;
          default:
            break;
        }
      }
    }

  private:
    ChunkArray chunks;                                     //блоки синхронизации
    Slot       slots [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //соответствие слоты константного буфера
    size_t     min_dst_buffer_size;                        //минимальный размер результирующего буфера
};

typedef xtl::intrusive_ptr<ShaderBuffersSynchronizer> ShaderBuffersSynchronizerPtr;
