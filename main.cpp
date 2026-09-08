// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwin_screen_corners_effect.h"

namespace KWin
{

KWIN_EFFECT_FACTORY_SUPPORTED(KWinScreenCornersEffect,
                              "kwin-screen-corners.json",
                              return KWinScreenCornersEffect::supported();)

} // namespace KWin

#include "main.moc"
