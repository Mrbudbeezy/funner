#include "shared.h"

using namespace media;

namespace
{

class CharsetManagerImpl
{
  public:
    //����������� ������� ��������
    void RegisterCharset (const char* name, const char* chars)
    {
      CharsetsMap::iterator iter = charsets.find (name);

      if (iter != charsets.end ())
        iter->second = chars;
      else
        charsets.insert_pair (name, chars);
    }

    void UnregisterCharset (const char* name)
    {
      charsets.erase (name);
    }

    void UnregisterAllCharsets ()
    {
      charsets.clear ();
    }

    ///����� ������ ��������
    const char* FindCharset (const char* name)
    {
      CharsetsMap::iterator iter = charsets.find (name);

      if (iter == charsets.end ())
        return 0;

      return iter->second.c_str ();
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, stl::string> CharsetsMap;

  private:
    CharsetsMap charsets;
};

typedef common::Singleton<CharsetManagerImpl> CharsetManagerSingleton;

}

/*
   �������� ������� ��������
*/

/*
   ����������� ������� ��������
*/

void CharsetManager::RegisterCharset (const char* name, const char* chars)
{
  static const char* METHOD_NAME = "media::CharsetManager::RegisterCharset";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!chars)
    throw xtl::make_null_argument_exception (METHOD_NAME, "chars");

  CharsetManagerSingleton::Instance ()->RegisterCharset (name, chars);
}

void CharsetManager::UnregisterCharset (const char* name)
{
  if (!name)
    return;

  CharsetManagerSingleton::Instance ()->UnregisterCharset (name);
}

void CharsetManager::UnregisterAllCharsets ()
{
  CharsetManagerSingleton::Instance ()->UnregisterAllCharsets ();
}

/*
   ����� ������ ��������
*/

const char* CharsetManager::FindCharset (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::CharsetManager::FindCharset", "name");

  return CharsetManagerSingleton::Instance ()->FindCharset (name);
}
