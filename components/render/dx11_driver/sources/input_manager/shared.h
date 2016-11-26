#ifndef RENDER_DX11_DRIVER_INPUT_STAGE_SHARED_HEADER
#define RENDER_DX11_DRIVER_INPUT_STAGE_SHARED_HEADER

#include <stl/vector>

#include <common/hash.h>

#include <render/low_level/utils.h>

#include <shared/error.h>
#include <shared/input_layout.h>
#include <shared/input_manager.h>

namespace render
{

namespace low_level
{

namespace dx11
{

//получение семантики по имени (returns (VertexAttributeSemantic)-1 if fails)
VertexAttributeSemantic get_semantic_by_name (const char* name);

//получение имени семантики
const char* get_semantic_name (VertexAttributeSemantic semantic);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аппаратный буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputBuffer: virtual public IBuffer, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputBuffer  (const DeviceManager&, const BufferDesc&, const void*);
    ~InputBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (BufferDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение / запись из буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t offset, size_t size, const void* data, IDeviceContext* context);
    void GetData (size_t offset, size_t size, void* data, IDeviceContext* context);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11Buffer& GetHandle () { return *buffer; }

  private:
    BufferDesc  desc;    //дескриптор буфера
    DxBufferPtr buffer;  //буфер      
};

}

}

}

#endif
