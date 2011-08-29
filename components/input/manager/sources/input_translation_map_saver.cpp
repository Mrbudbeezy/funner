#include "shared.h"

using namespace input;
using namespace common;

namespace components
{

namespace xkeymap_saver
{

void translation_map_saver (const char* file_name, const TranslationMap& source_map)
{
  XmlWriter        writer (file_name);
  XmlWriter::Scope scope (writer, "TranslationTable");

  for (TranslationMap::Iterator iter=source_map.CreateIterator (); iter; ++iter)
  {    
    TranslationMap::ITranslator& translator = *iter;

    XmlWriter::Scope scope (writer, "Translation");

    writer.WriteAttribute ("Event", translator.InputEvent ());
    writer.WriteAttribute ("Replacement", translator.Replacement ());
    
    const char* tag = translator.Tag ();
    
    if (*tag)
      writer.WriteAttribute ("Tag", tag);
  }
}

/*
   ��������� ���������� ���� ����������
*/

class KeymapSaverComponent
{
  public:
    //�������� ����������
    KeymapSaverComponent () 
    {
      TranslationMapManager::RegisterSaver ("xkeymap", &translation_map_saver);
    }
};

extern "C" ComponentRegistrator<KeymapSaverComponent> XKeymapSaver ("input.savers.xkeymap");

}

}
