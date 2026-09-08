// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <kcmodule.h>

class QCheckBox;
class QSpinBox;

namespace KWin
{

class KWinScreenCornersConfig : public KCModule
{
    Q_OBJECT

public:
    explicit KWinScreenCornersConfig(QObject *parent, const KPluginMetaData &data);

public Q_SLOTS:
    void save() override;

private:
    QSpinBox *m_radius = nullptr;
    QCheckBox *m_topLeft = nullptr;
    QCheckBox *m_topRight = nullptr;
    QCheckBox *m_bottomLeft = nullptr;
    QCheckBox *m_bottomRight = nullptr;
    QCheckBox *m_antialiasing = nullptr;
};

} // namespace KWin
