#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   ����������
*/

void FacebookSessionImpl::LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadAchievements", "Achievments not supported for facebook session");
}

/*
   ������
*/

void FacebookSessionImpl::LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadAchievementPicture", "Achievments not supported for facebook session");
}

/*
   ����������
*/

void FacebookSessionImpl::SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::SendAchievement", "Achievments not supported for facebook session");
}
