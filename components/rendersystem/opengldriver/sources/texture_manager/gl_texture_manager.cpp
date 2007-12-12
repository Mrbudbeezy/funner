#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �������� ���������� SoundDeclarationLibrary
*/

struct TextureManager::Impl: public ContextObject
{
    //�����������
  Impl (const ContextManager& context_manager) : ContextObject (context_manager) {}
};


/*
   ����������� / ����������
*/

TextureManager::TextureManager (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

TextureManager::~TextureManager ()
{
}
    
/*
   �������� �������� � ��������
*/

ITexture* TextureManager::CreateTexture (const TextureDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateTexture");
  return 0;
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::CreateSamplerState");
  return 0;
}

/*
   ��������� ������� �������� � ��������
*/

void TextureManager::SetTexture (size_t sampler_slot, ITexture* texture)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetTexture");
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetSampler");
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetTexture");
  return 0;
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetSampler");
  return 0;
}
