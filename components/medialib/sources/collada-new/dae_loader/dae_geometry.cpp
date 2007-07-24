#include "shared.h"

/*
    ������ ������⥪� ������ਨ
*/

void DaeParser::ParseLibraryGeometries (Parser::Iterator iter)
{
  if (!test (iter, "geometry"))
  {
    parse_log.Warning (iter, "Incorrect 'library_geometries' node. Must be at least one 'geometry' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "geometry", bind (&DaeParser::ParseGeometry, this, _1));  
}

/*
    ������ ������ਨ
*/

void DaeParser::ParseGeometry (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  Parser::Iterator mesh_iter = iter->First ("mesh");
  
  if (!mesh_iter)
    return;
  
  if (mesh_iter->Next ())
    LogWarning (mesh_iter->Next (), "Second mesh will be ignored");    
    
  Mesh& mesh = model.Meshes ().Create (id);  

  ParseMesh (mesh_iter, mesh);
}
