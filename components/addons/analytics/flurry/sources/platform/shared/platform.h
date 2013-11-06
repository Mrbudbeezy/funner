#ifndef ANALYTICS_FLURRY_PLATFORM_HEADER
#define ANALYTICS_FLURRY_PLATFORM_HEADER

#include <shared/default.h>
#include <shared/ios.h>

namespace analytics
{

namespace flurry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef IPHONE
  typedef IOsPlatform Platform;
#else
  typedef DefaultPlatform Platform;
#endif

}

}

#endif
