#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    ����������� / ����������
*/

AsyncPredicate::AsyncPredicate  (const ContextManager& manager)
  : ContextObject (manager)
{
    //��������� �������� ���������

  MakeContextCurrent ();

    //�������� �������

  if (glGenQueries) glGenQueries    (1, &query);
  else              glGenQueriesARB (1, &query);

    //�������� ������

  CheckErrors ("render::low_level::opengl::AsyncPredicate::AsyncPredicate");
}

AsyncPredicate::~AsyncPredicate ()
{
  try
  {
      //��������� �������� ���������

    MakeContextCurrent ();

      //�������� �������

    if (glDeleteQueries) glDeleteQueries    (1, &query);
    else                 glDeleteQueriesARB (1, &query);

      //�������� ������

    CheckErrors ("");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::AsyncPredicate::~AsyncPredicate");

    LogPrintf ("%s", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    �������� ������ �������
*/

void AsyncPredicate::Begin ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::Begin";

    //�������� ����������� �������� �������

  const size_t current_is_in_ranges = GetContextCacheValue (CacheEntry_IsInQueryRanges);

  if (current_is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "Begin already called without end call");

    //��������� �������� ���������

  MakeContextCurrent ();

    //�������� �������

  if (glBeginQuery) glBeginQuery    (GL_SAMPLES_PASSED, query);
  else              glBeginQueryARB (GL_SAMPLES_PASSED, query);

    //�������� ������

  CheckErrors (METHOD_NAME);

    //��������� ���-����������

  SetContextCacheValue (CacheEntry_IsInQueryRanges, 1);
}

void AsyncPredicate::End ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::End";

    //�������� ����������� �������� �������

  const size_t current_is_in_ranges = GetContextCacheValue (CacheEntry_IsInQueryRanges);

  if (!current_is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "There was not begin call");

    //��������� �������� ���������

  MakeContextCurrent ();

    //������� �������

  if (glEndQuery) glEndQuery    (GL_SAMPLES_PASSED);
  else            glEndQueryARB (GL_SAMPLES_PASSED);

    //�������� ������

  CheckErrors (METHOD_NAME);

    //��������� ���-����������

  SetContextCacheValue (CacheEntry_IsInQueryRanges, 0);
}

/*
    ��������� ���������� ���������
      (�������� ����� �������� � ��������� ���������� ���� �� ���������� ���������)
*/

bool AsyncPredicate::GetResult ()
{
    //��������� �������� ���������

  MakeContextCurrent ();

    //��������� �������� ���������� �������

  size_t count = 0;

  if (glGetQueryObjectuiv) glGetQueryObjectuiv    (query, GL_QUERY_RESULT, &count);
  else                     glGetQueryObjectuivARB (query, GL_QUERY_RESULT, &count);

    //�������� ������

  CheckErrors ("render::low_level::opengl::AsyncPredicate::GetResult");

  return count != 0;
}

/*
    ��������� ����������� ���������� ���������
*/

bool AsyncPredicate::IsResultAvailable ()
{
    //��������� �������� ���������

  MakeContextCurrent ();

    //�������� ���������� ����������

  GLint available = 0;

  if (glGetQueryObjectiv) glGetQueryObjectiv    (query, GL_QUERY_RESULT_AVAILABLE, &available);
  else                    glGetQueryObjectivARB (query, GL_QUERY_RESULT_AVAILABLE, &available);

    //�������� ������

  CheckErrors ("render::low_level::opengl::AsyncPredicate::IsResultAvailable");

  return available != 0;
}

#endif
