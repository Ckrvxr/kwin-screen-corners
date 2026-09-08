// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <effect/effect.h>

#include <memory>

namespace KWin
{
class GLShader;
class GLVertexBuffer;
class Output;
class RenderTarget;
class RenderViewport;

class KWinScreenCornersEffect final : public Effect
{
    Q_OBJECT

public:
    KWinScreenCornersEffect();
    ~KWinScreenCornersEffect() override;

    static bool supported();

    void reconfigure(ReconfigureFlags flags) override;
    void paintScreen(const RenderTarget &renderTarget,
                     const RenderViewport &viewport,
                     int mask,
                     const QRegion &region,
                     Output *screen) override;
    bool blocksDirectScanout() const override;
    int requestedEffectChainPosition() const override;

private:
    void paintMask(const RenderTarget &renderTarget, const RenderViewport &viewport);

    std::unique_ptr<GLShader> m_shader;
    std::unique_ptr<GLVertexBuffer> m_vertexBuffer;
    int m_radius = 24;
    bool m_topLeft = true;
    bool m_topRight = true;
    bool m_bottomLeft = true;
    bool m_bottomRight = true;
    bool m_antialiasing = true;
};

} // namespace KWin
