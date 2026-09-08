// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwin_screen_corners_config.h"

#include "kwin_screen_corners.h"

#include <KPluginFactory>
#include <KSharedConfig>

#include <QCheckBox>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QVBoxLayout>

K_PLUGIN_CLASS(KWin::KWinScreenCornersConfig)

namespace KWin
{

KWinScreenCornersConfig::KWinScreenCornersConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto *rootLayout = new QVBoxLayout(widget());
    auto *formLayout = new QFormLayout;

    m_radius = new QSpinBox(widget());
    m_radius->setObjectName(QStringLiteral("kcfg_Radius"));
    m_radius->setRange(1, 512);
    m_radius->setSingleStep(1);
    m_radius->setSuffix(QStringLiteral(" px"));
    m_radius->setToolTip(tr("Physical pixels, independent of display scaling."));
    formLayout->addRow(tr("Corner radius (physical pixels):"), m_radius);

    rootLayout->addLayout(formLayout);

    auto *cornersBox = new QGroupBox(tr("Rounded corners"), widget());
    auto *cornersLayout = new QVBoxLayout(cornersBox);

    m_topLeft = new QCheckBox(tr("Top left"), cornersBox);
    m_topLeft->setObjectName(QStringLiteral("kcfg_TopLeft"));
    m_topRight = new QCheckBox(tr("Top right"), cornersBox);
    m_topRight->setObjectName(QStringLiteral("kcfg_TopRight"));
    m_bottomLeft = new QCheckBox(tr("Bottom left"), cornersBox);
    m_bottomLeft->setObjectName(QStringLiteral("kcfg_BottomLeft"));
    m_bottomRight = new QCheckBox(tr("Bottom right"), cornersBox);
    m_bottomRight->setObjectName(QStringLiteral("kcfg_BottomRight"));

    cornersLayout->addWidget(m_topLeft);
    cornersLayout->addWidget(m_topRight);
    cornersLayout->addWidget(m_bottomLeft);
    cornersLayout->addWidget(m_bottomRight);
    rootLayout->addWidget(cornersBox);

    m_antialiasing = new QCheckBox(tr("Enable antialiasing"), widget());
    m_antialiasing->setObjectName(QStringLiteral("kcfg_Antialiasing"));
    rootLayout->addWidget(m_antialiasing);
    rootLayout->addStretch();

    KWinScreenCorners::KWinScreenCornersConfigData::instance(
        KSharedConfig::openConfig(QStringLiteral("kwinrc")));
    addConfig(KWinScreenCorners::KWinScreenCornersConfigData::self(), widget());
}

void KWinScreenCornersConfig::save()
{
    KCModule::save();

    QDBusInterface kwinEffects(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/Effects"),
        QStringLiteral("org.kde.kwin.Effects"),
        QDBusConnection::sessionBus());
    kwinEffects.call(QStringLiteral("reconfigureEffect"),
                     QStringLiteral("kwin-screen-corners"));
}

} // namespace KWin

#include "kwin_screen_corners_config.moc"
