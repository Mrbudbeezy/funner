#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::mid_level;

namespace
{

///��������� ��������
class EffectLoader
{
  public:
///�����������  
    EffectLoader (const char* file_name)
      : parser (file_name, "wxf")
    {
      try
      {
        ParseRoot (parser.Root ());
      }
      catch (...)
      {
        const common::ParseLog& parse_log = parser.Log ();

        try
        {  
          for (size_t i=0; i<parse_log.MessagesCount (); ++i)
            log.Print (parse_log.Message (i));
        }
        catch (...)
        {
          //���������� ���� ����������
        }
        
        throw;
      }
    }
    
  private:
///������ �����
    void ParseRoot (Parser::Iterator iter)
    {
      for_each_child (*iter, "blend",        xtl::bind (&EffectLoader::ParseBlendState, this, _1));
      for_each_child (*iter, "depthstencil", xtl::bind (&EffectLoader::ParseDepthStencilState, this, _1));
      for_each_child (*iter, "rasterizer",   xtl::bind (&EffectLoader::ParseRasterizerState, this, _1));
      for_each_child (*iter, "sampler",      xtl::bind (&EffectLoader::ParseSamplerState, this, _1));
      for_each_child (*iter, "program",      xtl::bind (&EffectLoader::ParseProgram, this, _1));
    }
    
///������ ��������� ������ ���������� ������
    void ParseBlendState (Parser::Iterator iter)
    {
    }
    
///������ ��������� ������ ���������
    void ParseDepthStencilState (Parser::Iterator iter)
    {
    }
    
///������ ��������� ������ ������������
    void ParseRasterizerState (Parser::Iterator iter)
    {
    }
    
///������ ��������� ��������
    void ParseSamplerState (Parser::Iterator iter)
    {
    }
    
///������ ���������
    void ParseProgram (Parser::Iterator iter)
    {
    }    
  
  private:
    common::Parser         parser;  //������ ����� ��������
    EffectLoaderLibrary    library; //���������� �������� ��������
    render::mid_level::Log log;     //�������� �������
};

}

namespace render
{

namespace mid_level
{

void parse_effect_library (const char* file_name)
{
  EffectLoader loader (file_name);
}

}

}
