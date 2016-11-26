#ifndef RENDER_DX11_DRIVER_RENDER_TARGET_MANAGER_SHARED_HEADER
#define RENDER_DX11_DRIVER_RENDER_TARGET_MANAGER_SHARED_HEADER

#include <xtl/trackable_ptr.h>

#include <common/hash.h>

#include <render/low_level/utils.h>

#include <shared/error.h>
#include <shared/render_target_manager.h>
#include <shared/swap_chain.h>
#include <shared/texture.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип отображения
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ViewType
{
  ViewType_RenderTarget,
  ViewType_DepthStencil,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отображение
///////////////////////////////////////////////////////////////////////////////////////////////////
class View: virtual public IView, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    View  (const DeviceManager& device_manager, ITexture* texture, const ViewDesc& desc);
    ~View ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11View& GetHandle () { return *view; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип отображения
///////////////////////////////////////////////////////////////////////////////////////////////////
    ViewType GetType () { return view_type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение целевой текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* GetTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора / флагов биндинга текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (ViewDesc&);

  private:
    typedef xtl::com_ptr<ITexture> TexturePtr;

  private:
    TexturePtr texture;    //указатель на текстуру
    DxViewPtr  view;       //низкоуровневый дескриптор
    ViewType   view_type;  //тип отображения
    ViewDesc   desc;       //дескриптор отображения        
};

}

}

}

#endif
