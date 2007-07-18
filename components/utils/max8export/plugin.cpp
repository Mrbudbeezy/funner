#include "export.h"

#define DllApi extern "C" __declspec (dllexport)

class WXFExport;

Interface*    iface        = NULL;
ExpInterface* exp_iface    = NULL;
TimeValue     static_frame = 0;
INode*        root         = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
  static HINSTANCE  hInstance = hinstDLL;
  static int        controlsInit = FALSE;

  if (!controlsInit) 
  {
    controlsInit = TRUE;
    InitCustomControls (hInstance);
    InitCommonControls ();
  }

  switch(fdwReason) 
  {
    case DLL_PROCESS_ATTACH:   break;
    case DLL_THREAD_ATTACH:    break;
    case DLL_THREAD_DETACH:    break;
    case DLL_PROCESS_DETACH:   break;
  }

  return TRUE;
}

DllApi const TCHAR * LibDescription() 
{ 
  return _T ("Mega file exporter");
} 

DllApi int LibNumberClasses() 
{ 
  return 1; 
}

DllApi ULONG LibVersion () 
{ 
  return VERSION_3DSMAX; 
}

// Let the plug-in register itself for deferred loading
DllApi ULONG CanAutoDefer()
{
  return 1;
}

DllApi TCHAR* ClassDesc::GetRsrcString (INT_PTR)
{
  return _T(""); //GetString
}

///////////////////////////////////////

///��ᯮ����
class WXFExport: public SceneExport
{
  public:
///���ଠ�� �� �ᯮ����
        int            ExtCount      ()     { return 1; }  //������⢮ �����ন������ ���७��
        const TCHAR*   Ext           (int)  { return _T("WXF"); }        //N-��� ���७��
        const TCHAR*   LongDesc      ()     { return _T ("UNT Wood Exchange 3D format (WXF)"); } //������ ���ᠭ�� ����� �ᯮ����
        const TCHAR*   ShortDesc     ()     { return _T ("WXF Export"); } //���⪮� ���ᠭ�� ����� �ᯮ����
        const TCHAR*   AuthorName    ()     { return _T ("DadyCool"); }             //��� ����
        const TCHAR*   CopyrightMessage()   { return _T ("Copyright 2003 by UNT Games Studio");}         //Copyright
        const TCHAR*   OtherMessage1 ()     { return _T(""); }  //??
        const TCHAR*   OtherMessage2 ()     { return _T(""); }     //??
        unsigned int   Version       ()     { return 100; }  //����� ���ᨨ 㬭������ �� 100

///�������� About
        virtual void   ShowAbout     (HWND hWnd) ;          

///����⢥��� �ᯮ��
        virtual int    DoExport  (const TCHAR*,ExpInterface*,Interface*,BOOL,DWORD);

        virtual BOOL   SupportsOptions (int,DWORD);
};

///�����窠 ��� �ᯮ���
class ExpClassDesc: public ClassDesc 
{
  public:
     int           IsPublic      ()       { return 1; }
     void*         Create        (BOOL)   { return new WXFExport; }
     const TCHAR*  ClassName     ()       { return _T ("3D Studio"); }
     SClass_ID     SuperClassID  ()       { return SCENE_EXPORT_CLASS_ID; }
     Class_ID      ClassID       ()       { return Class_ID(0x7c412514, 0x78b563f5); }
     const TCHAR*  Category      ()       { return _T ("Scene Export"); }  
};

DllApi ClassDesc* LibClassDesc (int i) 
{
  static ExpClassDesc class_desc;

  switch(i) 
  {
    case 0:  return &class_desc;
    default: return 0;          
  }
}

void  WXFExport::ShowAbout (HWND hWnd) 
{
}

int WXFExport::DoExport (const TCHAR* name,ExpInterface* eiPtr,Interface* iPtr,BOOL suppressPrompts,DWORD options)
{
  flag_export_selected = (options & SCENE_EXPORT_SELECTED) ? true : false;

  logOpen (name);

  iface        = iPtr;
  exp_iface    = eiPtr;
  static_frame = 0;
  root         = iPtr->GetRootNode ();

  Export ();   

  logClose (); 

  return 1;     
}

BOOL WXFExport::SupportsOptions (int ext,DWORD options)
{
  assert (ext == 0);       // We only support one extension
  return (options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

