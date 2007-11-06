#ifndef MEDIALIB_SOUND_DECLARATION_HEADER
#define MEDIALIB_SOUND_DECLARATION_HEADER

#include <xtl/functional_fwd>
#include <xtl/intrusive_ptr.h>
#include <stl/auto_ptr.h>
#include <media/clone.h>

namespace xtl
{

//forward declaration
template <class T> class iterator;

}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SoundParam
{
  SoundParam_Gain,               //��������
  SoundParam_MinimumGain,        //����������� ��������
  SoundParam_MaximumGain,        //������������ ��������
  SoundParam_InnerAngle,         //���� ����������� ������ ���������� (���� "��������������")
  SoundParam_OuterAngle,         //���� �������� ������ ���������� (���� "������")
  SoundParam_OuterGain,          //�������� �� ��������� �������� ������
  SoundParam_ReferenceDistance,  //����������, �� �������� ��������� ����� gain
  SoundParam_MaximumDistance,    //������������ ����������, ������ �������� ��������� �� �����������
  SoundParam_CullDistance,       //���������� ���������
  
  SoundParam_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundDeclaration
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundDeclaration  ();
    SoundDeclaration  (const SoundDeclaration&, CloneMode = CloneMode_Instance);
    ~SoundDeclaration ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundDeclaration& operator = (const SoundDeclaration&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetType (const char* type);
    const char* Type    () const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLooping (bool looping);
    bool Looping    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SamplesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Sample (size_t sample_index) const;    
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AddSample        (const char* sample_name);
    void   RemoveSample     (size_t sample_index);
    void   RemoveAllSamples ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetParam (SoundParam param, float value);
    float Param    (SoundParam param) const;
     
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SoundDeclaration&);

  private:
    struct Impl;
    xtl::intrusive_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SoundDeclaration&, SoundDeclaration&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundDeclarationLibrary
{
  public:
    typedef xtl::iterator<SoundDeclaration>       Iterator;
    typedef xtl::iterator<const SoundDeclaration> ConstIterator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundDeclarationLibrary  ();
    SoundDeclarationLibrary  (const char* file_name);
    SoundDeclarationLibrary  (const SoundDeclarationLibrary&);
    ~SoundDeclarationLibrary ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundDeclarationLibrary& operator = (const SoundDeclarationLibrary&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � ���������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
          SoundDeclaration* Find (const char* name);
    const SoundDeclaration* Find (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ();
    ConstIterator CreateIterator () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach    (const char* name, SoundDeclaration& decl, CloneMode mode = CloneMode_Instance);
    void Detach    (const char* name); //no throw
    void DetachAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);
    void Save (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SoundDeclarationLibrary&);
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SoundDeclarationLibrary&, SoundDeclarationLibrary&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ���������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
class SoundDeclarationLibraryManager
{
  public:
    typedef xtl::function<void (const char*,       SoundDeclarationLibrary&)> LoadHandler;
    typedef xtl::function<void (const char*, const SoundDeclarationLibrary&)> SaveHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������������� ��������� �������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterLoader   (const char* extension, const LoadHandler& loader);
    static void RegisterSaver    (const char* extension, const SaveHandler& saver);
    static void UnregisterLoader (const char* extension);
    static void UnregisterSaver  (const char* extension);
    static void UnregisterAll    ();
};

}

#endif
