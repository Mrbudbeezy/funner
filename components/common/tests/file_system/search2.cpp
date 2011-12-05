#include "test.h"

const char* SEARCH_MASK = "layout/*";
const char* SEARCH_PATH = "data/search-zip";

inline void PrintLog (const char* message)
{
  printf ("\t%s\n",message);
}

int main ()
{
  printf ("Results of search2_test:\n");

  try
  {
    printf ("Add search path '%s':\n",SEARCH_PATH);

    FileSystem::AddSearchPath (SEARCH_PATH, &PrintLog);
    FileSystem::AddSearchPath ("res", &PrintLog);    

    printf ("Search '%s':\n",SEARCH_MASK);

    for (FileListIterator i=FileSystem::Search (SEARCH_MASK,FileSearch_FilesAndDirs|FileSearch_SubDirs|FileSearch_Sort);i;++i)
      printf ("  found: %s\n",i->name);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
