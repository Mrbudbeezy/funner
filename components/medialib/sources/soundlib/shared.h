#ifndef SOUNDLIB_SHARED_HEADER
#define SOUNDLIB_SHARED_HEADER

#include <vorbis/vorbisfile.h>

#include <common/exception.h>
#include <common/singleton.h>
#include <common/file.h>
#include <common/xml_writer.h>

#include <stl/vector>
#include <stl/string>
#include <stl/hash_set> //??������!!!
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <media/sound.h>
#include <media/sound_declaration.h>

#include <shared/clone.h>
#include <shared/resource_library.h>
#include <shared/resource_manager.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSampleImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    stl::string               str_name;  //SoundSample name
    SoundSampleInfo           info;      //���������� � �����
    stl::auto_ptr<SoundCodec> codec;     //�����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSampleSystemImpl
{
  public:
    typedef media::SoundSampleSystem::CodecLoadFunc CodecLoadFunc;
    typedef media::SoundSampleSystem::DebugLogFunc  DebugLogFunc;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundSampleSystemImpl ();
    ~SoundSampleSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� ������ / �������� ���� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterSoundSample   (media::SoundSample&);
    void UnregisterSoundSample (media::SoundSample&);
    void CloseAllSoundSamples  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � ����� ���������������� ������� ���� �����-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDebugLog (const DebugLogFunc&);
    void DebugLog    (const char* debug_message);   

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ��������� ���������������� ������� �������� � ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool           RegisterLoadFunc   (const char* extension, const CodecLoadFunc& load_codec);
    void           UnregisterLoadFunc (const char* extension);
    void           UnregisterAllFuncs ();
    media::SoundSampleSystem::CodecLoadFunc* GetLoadFunc(const char* extension);

  private:
    typedef stl::hash_set<media::SoundSample*>        OpenSoundSamplesSet;
    typedef stl::hash_map<stl::string, CodecLoadFunc> LoadCodecs;

  private:    
    DebugLogFunc        log_function;         //��������������� ������� �����-����
    OpenSoundSamplesSet open_sound_samples;   //������ �������� ������
    LoadCodecs          load_codecs;          //������ ���������������� ������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<SoundSampleSystemImpl> SoundSystemSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundDeclarationLibraryManagerImpl: public ResourceManager<SoundDeclarationLibraryManager::LoadHandler, SoundDeclarationLibraryManager::SaveHandler>
{
  public:
    SoundDeclarationLibraryManagerImpl ();
};

typedef common::Singleton<SoundDeclarationLibraryManagerImpl> SoundDeclarationLibraryManagerSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� SoundDeclarationLibrary
///////////////////////////////////////////////////////////////////////////////////////////////////
void snddecl_save_library (const char* file_name, const SoundDeclarationLibrary& library);
void snddecl_load_library (const char* file_name, SoundDeclarationLibrary& library);

}

#endif
