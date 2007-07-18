#pragma once

#include <windows.h>
#include <max.h>
#include <decomp.h>
#include <stdmat.h>
#include <iparamb2.h>
#include <iskin.h>

#include <stdarg.h>

#include "log.h"

extern Interface*    iface;      //����䥩� ����㯠
extern ExpInterface* exp_iface;   //����䥩� ����㯠 ��� �ᯮ��
extern TimeValue     static_frame;  //����� ����  
extern INode*        root;          //��७�

extern Interval      anim_range;
extern float         anim_fps;        

const float ANIM_FRAMES_STEP = 100;

///䫠�� �ᯮ��

extern bool flag_export_materials;
extern bool flag_export_selected;
extern bool flag_export_nodes;
extern bool flag_export_lights;
extern bool flag_export_cameras;
extern bool flag_export_shapes;
extern bool flag_export_geometry;
extern bool flag_export_helpers;

///ᮡ�⢥��� �ᯮ��

void Preprocess     (INode*);

void Export         ();
void Export         (INode*,INode*);
void Export         (Mtl* mtl,Mtl* parent_mtl);
void ExportMesh     (INode*,TimeValue=0);
void ExportAnimKeys (INode*);
void ExportHelper   (INode*);
void ExportLight    (INode*);
void ExportCamera   (INode*);

//����䨪����
ISkin* FindSkin (INode*);

///�ࠢ������ �ண��ᮬ
void logProgressNext ();

//���� �� �������
bool TestAnim (Control*);

//��� ���ਠ���
void ResetMaterials ();

//⨯ helper'�
const char* GetHelperType (Object&);
