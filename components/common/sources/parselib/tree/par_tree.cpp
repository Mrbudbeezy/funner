#include "shared.h"

using namespace common;

/*
    ����������� / ����������
*/

ParseTree::ParseTree (ParseTreeBuffer& in_buffer, const ParseLog* in_log)
{
  if (in_log)
    log = new ParseLog (*in_log);

  buffer.swap (in_buffer);  
}

ParseTree::~ParseTree ()
{
}

/*
    �������� �������
*/

ParseLog& ParseTree::Log ()
{
  if (!log)
    log = new ParseLog;
    
  return *log;  
}
