#ifndef RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER
#define RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER

#include <render/low_level/driver.h>

#include <shared/gl.h>
#include <shared/extensions.h>
#include <shared/context_caps.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Stage
{
  Stage_Common,
  Stage_Input,
  Stage_Shading,
  Stage_Output,
  Stage_TextureManager,
  Stage_QueryManager,
  Stage_RenderTargetManager,

  Stage_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������, ������������� � ���� ��������� ��������� ��� ������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t CONTEXT_CACHE_STAGE_ENTRIES_NUM = 64;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CacheEntry
{
  CacheEntry_SharedFirst = 0,

  CacheEntry_ActiveTextureSlot, //����� �������� ��������� ����� ��������������
  CacheEntry_EnabledTextures,   //�����: ����������� �� �������� � ������������ �����
  CacheEntry_ScissorEnable,     //������� �� ���� ���������
  CacheEntry_ColorWriteMask0,   //����� ������ � ����� ����� 0
  CacheEntry_ColorWriteMask1,   //����� ������ � ����� ����� 1
  CacheEntry_ColorWriteMask2,   //����� ������ � ����� ����� 2
  CacheEntry_ColorWriteMask3,   //����� ������ � ����� ����� 3
  CacheEntry_ColorWriteMask4,   //����� ������ � ����� ����� 4
  CacheEntry_ColorWriteMask5,   //����� ������ � ����� ����� 5
  CacheEntry_ColorWriteMask6,   //����� ������ � ����� ����� 6
  CacheEntry_ColorWriteMask7,   //����� ������ � ����� ����� 7
  CacheEntry_DepthWriteEnable,  //�������� �� ������ � ����� �������
  CacheEntry_StencilWriteMask,  //����� ������ � ����� ���������

  CacheEntry_SharedLast = CacheEntry_SharedFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_InputStagePrivateFirst = CacheEntry_SharedLast,
  CacheEntry_InputStagePrivateLast  = CacheEntry_InputStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_ShadingStagePrivateFirst = CacheEntry_InputStagePrivateLast,
  CacheEntry_ShadingStagePrivateLast  = CacheEntry_ShadingStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_OutputStagePrivateFirst = CacheEntry_ShadingStagePrivateLast,
  CacheEntry_OutputStagePrivateLast  = CacheEntry_OutputStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_TextureManagerPrivateFirst = CacheEntry_OutputStagePrivateLast,
  CacheEntry_TextureManagerPrivateLast  = CacheEntry_TextureManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_QueryManagerPrivateFirst = CacheEntry_TextureManagerPrivateLast,
  CacheEntry_QueryManagerPrivateLast  = CacheEntry_QueryManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_RenderTargetManagerPrivateFirst = CacheEntry_QueryManagerPrivateLast,
  CacheEntry_RenderTargetManagerPrivateLast  = CacheEntry_RenderTargetManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_Num = CacheEntry_RenderTargetManagerPrivateLast
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� OpenGL (�������� ����������� - ������� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextManager
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;    
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextManager  (ISwapChain* swap_chain, const char* init_string);
    ContextManager  (const ContextManager&);
    ~ContextManager ();

    ContextManager& operator = (const ContextManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* GetAdapter () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*);
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*, const SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetSwapChain (ISwapChain* swap_chain);
    ISwapChain* GetSwapChain () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeContextCurrent    () const;
    bool TryMakeContextCurrent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const size_t* GetContextCache      () const;
          size_t* GetContextCache      ();
          void    SetContextCacheValue (size_t entry_id, size_t value);
          size_t  GetContextCacheValue (size_t entry_id) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������ ��� ���������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetTempBuffer (size_t size) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ������� ���������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetExtensions () const;
    const char* GetVersion    () const;
    const char* GetVendor     () const;
    const char* GetRenderer   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSupported (const Extension& extension) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������������� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ContextCaps& GetCaps () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ��������� � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (ISwapChain* swap_chain) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (const ContextManager&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogPrintf  (const char* format, ...) const;
    void LogVPrintf (const char* format, va_list args) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetValidationState (bool state); //��������� / ���������� �������� ������
    bool GetValidationState () const;
    void CheckErrors        (const char* source) const;
    void RaiseError         (const char* source) const;
    void ClearErrors        () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� ������� ������ / ������� ������ ���������� / �������� ������������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void StageRebindNotify        (Stage);
    void ResetRebindNotifications ();
    bool NeedStageRebind          (Stage) const;

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
