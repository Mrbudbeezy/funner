#include "shared.h"

using namespace input;
using namespace common;

namespace
{

void translation_map_saver (const char* file_name, const TranslationMap& source_map)
{
  XmlWriter        writer (file_name);
  XmlWriter::Scope scope (writer, "TranslationTable");

  for (TranslationMap::Iterator iter=source_map.CreateIterator (); iter; ++iter)
  {    
    TranslationMap::Translator& translator = *iter;

    XmlWriter::Scope scope (writer, "Translation");

    writer.WriteAttribute ("Event", translator.InputEvent ());
    writer.WriteAttribute ("Replacement", translator.Replacement ());
    
    const char* tag = translator.Tag ();
    
    if (*tag)
      writer.WriteAttribute ("Tag", tag);
  }
}

/*
   ��������� ��࠭���� ���� �࠭��樨
*/

class KeymapSaverComponent
{
  public:
    //����㧪� ���������
    KeymapSaverComponent () 
    {
      TranslationMapManager::RegisterSaver ("keymap", &translation_map_saver);
    }
};

extern "C"
{

ComponentRegistrator<KeymapSaverComponent> KeymapSaver ("input.savers.keymap");

}

}
