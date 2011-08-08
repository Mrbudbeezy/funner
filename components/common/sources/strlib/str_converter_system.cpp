#include <stl/hash_map>
#include <stl/string>

#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>

#include <common/component.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/strconv.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

using namespace stl;
using namespace common;

namespace
{

/*
    ���������
*/

const char* DEFAULT_CONVERTERS_MASK = "common.string.converters.*";

/*
    ���������� ������ ����������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Utf-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class UtfConverter: public IStringConverter
{
  public:
    void AddRef  () {}
    void Release () {}

    void Convert (const void*& source, size_t& source_size, void*& destination, size_t& destination_size)
    {
      convert_encoding (source_encoding, source, source_size, destination_encoding, destination, destination_size);
    }
    
    static IStringConverter* Create (Encoding source_encoding, Encoding destination_encoding)
    {
      static bool         init = false;
      static UtfConverter converters [Encoding_Num][Encoding_Num];

      if (!init)
      {
        for (int source=Encoding_ASCII7; source<Encoding_Num; source++)
          for (int destination=Encoding_ASCII7; destination<Encoding_Num; destination++)
          {
            UtfConverter& conv = converters [source][destination];
            
            conv.source_encoding      = (Encoding)source;
            conv.destination_encoding = (Encoding)destination;
          }

        init = true;
      }

      return &converters [source_encoding][destination_encoding];
    }
    
  private:
    UtfConverter () {}

  private:
    Encoding source_encoding, destination_encoding;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringConverterSystemImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StringConverterSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterConverter       (const char* source_encoding, const char* destination_encoding, const StringConverterSystem::ConverterFn&);
    void UnregisterConverter     (const char* source_encoding, const char* destination_encoding);
    void UnregisterAllConverters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsConverterRegistered (const char* source_encoding, const char* destination_encoding);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IStringConverter* CreateConverter (const char* source_encoding, const char* destination_encoding);  

  private:
    struct ConverterDesc
    {
      stl::string                        source_encoding;
      stl::string                        destination_encoding;
      StringConverterSystem::ConverterFn converter;
      
      ConverterDesc (const char* in_source_encoding, const char* in_destination_encoding, const StringConverterSystem::ConverterFn& in_converter)
        : source_encoding (in_source_encoding)
        , destination_encoding (in_destination_encoding)
        , converter (in_converter)
      {
      }
    };
    
    typedef stl::list<ConverterDesc> StringConverterList;
    
    StringConverterList::iterator FindConverter (const char* source_encoding, const char* destination_encoding);

  private:
    StringConverterList converters;
};

typedef common::Singleton<StringConverterSystemImpl> StringConverterSystemImplSingleton;

/*
===================================================================================================
    StringConverterSystemImpl
===================================================================================================
*/

/*
    ����������� / ����������
*/

namespace
{

const char* get_encoding_name (Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_ASCII7:  return "ASCII7";
    case Encoding_UTF8:    return "UTF8";
    case Encoding_UTF16LE: return "UTF16LE";
    case Encoding_UTF16BE: return "UTF16BE";
    default:               return "Unknown";
  }
}

}

StringConverterSystemImpl::StringConverterSystemImpl()
{ 
  for (int source=Encoding_ASCII7; source<Encoding_Num; source++)
    for (int destination=Encoding_ASCII7; destination<Encoding_Num; destination++)
      RegisterConverter (get_encoding_name ((Encoding)source), get_encoding_name((Encoding)destination),
                         xtl::bind (&UtfConverter::Create, (Encoding)source, (Encoding)destination));
}

/*
    ����� ����������
*/

StringConverterSystemImpl::StringConverterList::iterator StringConverterSystemImpl::FindConverter (const char* source_encoding, const char* destination_encoding)
{
  static ComponentLoader loader (DEFAULT_CONVERTERS_MASK);

  if (!source_encoding || !destination_encoding)
    return converters.end ();
    
  for (StringConverterList::iterator iter=converters.begin (), end=converters.end (); iter!=end; ++iter)
    if (wcimatch (source_encoding, iter->source_encoding.c_str ()) && wcimatch (destination_encoding, iter->destination_encoding.c_str ()))
      return iter;
      
  return converters.end ();
}

/*
    ����������� �����������
*/

void StringConverterSystemImpl::RegisterConverter
 (const char*                               source_encoding,
  const char*                               destination_encoding,
  const StringConverterSystem::ConverterFn& converter)
{
  static const char* METHOD_NAME = "common::StringConverterSystemImpl::RegisterConverter";

  try
  {
    if (!source_encoding)
      throw xtl::make_null_argument_exception ("", "source_encoding");

    if (!destination_encoding)
      throw xtl::make_null_argument_exception ("", "destination_encoding");      
      
    converters.push_back (ConverterDesc (source_encoding, destination_encoding, converter));
  }
  catch (xtl::exception& exception)
  {
    exception.touch(METHOD_NAME);
    throw;
  }
}

void StringConverterSystemImpl::UnregisterConverter
 (const char* source_encoding,
  const char* destination_encoding)
{
  if (!source_encoding)
    return;

  if (!destination_encoding)
    return;
    
  for (StringConverterList::iterator iter=converters.begin (), end=converters.end (); iter!=end; ++iter)
    if (source_encoding == iter->source_encoding && destination_encoding == iter->destination_encoding)
    {
      converters.erase (iter);
      return;
    }
}

void StringConverterSystemImpl::UnregisterAllConverters ()
{
  converters.clear ();
}

bool StringConverterSystemImpl::IsConverterRegistered (const char* source_encoding, const char* destination_encoding)
{
  if (!source_encoding)
    return false;

  if (!destination_encoding)
    return false;

  StringConverterList::iterator iter = FindConverter (source_encoding, destination_encoding);

  return iter != converters.end ();
}

IStringConverter* StringConverterSystemImpl::CreateConverter (const char* source_encoding, const char* destination_encoding)
{
  static const char* METHOD_NAME = "common::StringConverterSystemImpl::CreateConverter";

  try
  {
    if (!source_encoding)
      throw xtl::make_null_argument_exception ("", "source_encoding");

    if (!destination_encoding)
      throw xtl::make_null_argument_exception ("", "destination_encoding");

    StringConverterList::iterator iter = FindConverter (source_encoding, destination_encoding);
    
    if (iter == converters.end ())
      throw xtl::format_operation_exception ("", "A converter functor for \'%s-to-%s\' is not registered", source_encoding, destination_encoding);
   
    return iter->converter (source_encoding, destination_encoding);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
===================================================================================================
    StringConverterSystem
===================================================================================================
*/

void StringConverterSystem::RegisterConverter
 (const char*        source_encoding,
  const char*        destination_encoding,
  const ConverterFn& converter)
{
  StringConverterSystemImplSingleton::Instance ()->RegisterConverter (source_encoding, destination_encoding, converter);
}

void StringConverterSystem::UnregisterConverter (const char* source_encoding, const char* destination_encoding)
{
  StringConverterSystemImplSingleton::Instance()->UnregisterConverter(source_encoding, destination_encoding);
}

void StringConverterSystem::UnregisterAllConverters ()
{
  StringConverterSystemImplSingleton::Instance ()->UnregisterAllConverters ();
}

bool StringConverterSystem::IsConverterRegistered (const char* source_encoding, const char* destination_encoding)
{
  return StringConverterSystemImplSingleton::Instance ()->IsConverterRegistered (source_encoding, destination_encoding);
}

/*
===================================================================================================
    ��������� �����
===================================================================================================
*/

StringConverter::StringConverter (const char* source_encoding, const char* destination_encoding)
{
  converter = StringConverterSystemImplSingleton::Instance ()->CreateConverter (source_encoding, destination_encoding); 
}

StringConverter::StringConverter (const StringConverter& conv)
  : converter (conv.converter)
{
  converter->AddRef ();
}

StringConverter::~StringConverter ()
{
  try
  {
    converter->Release ();
  }
  catch (...)
  {
  }
}

StringConverter& StringConverter::operator = (const StringConverter& conv)
{
  StringConverter (conv).Swap (*this);

  return *this;
}
  
void StringConverter::Convert
 (const void*& source_buffer_ptr,
  size_t&      source_buffer_size,
  void*&       destination_buffer_ptr,
  size_t&      destination_buffer_size) const
{
  try
  {
    converter->Convert (source_buffer_ptr, source_buffer_size, destination_buffer_ptr, destination_buffer_size);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::StringConverter::Convert");
    throw;
  }
}

/*
    �����
*/

void StringConverter::Swap (StringConverter& conv)
{
  IStringConverter* tmp = converter;
  converter             = conv.converter;
  conv.converter        = tmp;
}

namespace common
{

void swap (StringConverter& conv1, StringConverter& conv2)
{
  conv1.Swap (conv2);
}

} //namespace common
