#include "shared.h"

using namespace common;

namespace
{

/*
    ���������
*/

const char* PARSER_COMPONENTS_MASK = "common.parsers.*"; //����� ��� ����������� ��������

/*
    ���������� ��������� ��������
*/

class ParserManagerImpl
{
  public:
    typedef ParserManager::ParseHandler ParseHandler;
    typedef ParserManager::CheckHandler CheckHandler;

///����������� ����������������� �������
    void RegisterParser (const char* type, const ParseHandler& parser, const CheckHandler& checker)
    {
      static const char* METHOD_NAME = "common::ParserManagerImpl::RegisterParser";

      if (!type)
        throw xtl::make_null_argument_exception (METHOD_NAME, "type");
        
      DeclMap::iterator iter = parsers.find (type);
      
      if (iter != parsers.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "type", type, "Parser with this type has already registered");

      parsers.insert_pair (type, ParserDecl (parser, checker));
    }
    
///������ ����������� �������
    void UnregisterParser (const char* type)
    {
      if (!type)
        return;
        
      parsers.erase (type);
    }
    
///������ ����������� ���� ��������
    void UnregisterAllParsers ()
    {
      parsers.clear ();
    }

///�������
    void Parse (ParseTreeBuilder& builder, ParseLog& log, const char* type, const char* name, size_t buffer_size, char* buffer)
    {            
      static const char* METHOD_NAME = "common::ParserManagerImpl::Parse";
      
        //�������� ������������ ����������
        
      if (!type)
        throw xtl::make_null_argument_exception (METHOD_NAME, "type");

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!buffer && buffer_size)
        throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");
        
        //�������� �������� �� ���������
        
      LoadDefaultParsers ();
        
        //����� ������� �� ����
        
      ParserDecl* decl = 0;
        
      if (strcmp (type, "auto"))
      {
        DeclMap::iterator iter = parsers.find (type);
        
        if (iter == parsers.end ())
          throw xtl::make_argument_exception (METHOD_NAME, "type", type, "Parser not found");
          
        decl = &iter->second;
      }
      else
      {      
          //����� �� ����������
          
        size_t name_len = strlen (name);
        
        if (name_len)
        {          
            //��������� ���������� ����� ������
          
          const char* ext = name + name_len - 1;
          
          for (; ext != name && *ext != '.'; --ext);
          
          ext++;
          
            //����� �������
          
          DeclMap::iterator iter = parsers.find (ext);
          
          if (iter != parsers.end ())
            decl = &iter->second;
        }
        
          //����� � ������� ����������� �������
          
        if (!decl)
        {          
          for (DeclMap::iterator iter=parsers.begin (), end=parsers.end (); iter!=end; ++iter)
          {
            ParserDecl& check_decl = iter->second;
            
              //�������� ����������� �������
            
            try
            {
              if (check_decl.check_handler && check_decl.check_handler (buffer_size, buffer))
              {
                decl = &check_decl;
                break;
              }
            }
            catch (...)
            {

            }
          }
        }
        
        if (!decl)
          throw xtl::format_operation_exception (METHOD_NAME, "Buffer '%s' has unknown type. Parser not found", name);
      }

        //������

      builder.SetSource (name);

      decl->parse_handler (builder, log, name, buffer_size, buffer);      
    }
    
  private:
///�������� �������� �� ���������
    static void LoadDefaultParsers ()
    {
      static ComponentLoader loader (PARSER_COMPONENTS_MASK);
    }

  private:
    struct ParserDecl
    {
      ParseHandler parse_handler; //���������� �������
      CheckHandler check_handler; //������� �������� ����������� �������
      
      ParserDecl (const ParseHandler& in_parse_handler) : parse_handler (in_parse_handler) {}

      ParserDecl (const ParseHandler& in_parse_handler, const CheckHandler& in_check_handler) :
        parse_handler (in_parse_handler), check_handler (in_check_handler) {}
    };

    typedef stl::hash_map<stl::hash_key<const char*>, ParserDecl> DeclMap;

  private:
    DeclMap parsers;
};

typedef Singleton<ParserManagerImpl> ParserManagerSingleton;

}

/*
    ������ ��� ParserManagerImpl
*/

void ParserManager::RegisterParser (const char* type, const ParseHandler& parser)
{
  ParserManagerSingleton::Instance ()->RegisterParser (type, parser, CheckHandler ());
}

void ParserManager::RegisterParser (const char* type, const ParseHandler& parser, const CheckHandler& checker)
{
  ParserManagerSingleton::Instance ()->RegisterParser (type, parser, checker);
}

void ParserManager::UnregisterParser (const char* type)
{
  ParserManagerSingleton::Instance ()->UnregisterParser (type);
}

void ParserManager::UnregisterAllParsers ()
{
  ParserManagerSingleton::Instance ()->UnregisterAllParsers ();
}

namespace common
{

/*
    �������������� ������
*/

void parse (ParseTreeBuilder& builder, ParseLog& log, const char* type, const char* name, size_t buffer_size, char* buffer)
{
  ParserManagerSingleton::Instance ()->Parse (builder, log, type, name, buffer_size, buffer);
}

ParseNode parse (ParseLog& log, const char* type, const char* name, size_t buffer_size, char* buffer)
{
  ParseTreeBuilder builder;
  
  parse (builder, log, type, name, buffer_size, buffer);
  
  return builder.Build (log);
}

}
