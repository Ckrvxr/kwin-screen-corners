// SPDX-License-Identifier: AGPL-3.0-or-later

#include "kwin_screen_corners_effect.h"

#include "effect/effecthandler.h"
#include "opengl/glshader.h"
#include "opengl/glshadermanager.h"
#include "opengl/glvertexbuffer.h"
#include "core/renderviewport.h"
#include "kwin_screen_corners.h"

#include <QRectF>
#include <QVector2D>

#include <algorithm>
#include <array>

#include <epoxy/gl.h>

namespace KWin
{

namespace
{

constexpr auto vertexShader = R"(#version 140
in vec2 position;
out vec2 localPosition;

uniform mat4 modelViewProjectionMatrix;
uniform vec2 origin;

void main()
{
    localPosition = position - origin;
    gl_Position = modelViewProjectionMatrix * vec4(position, 0.0, 1.0);
}
)";

constexpr auto fragmentShader = R"(#version 140
in vec2 localPosition;
out vec4 fragColor;

uniform vec2 screenSize;
uniform float radius;
uniform float antialiasWidth;
uniform float antialiasing;
uniform vec4 cornerMask;

float cornerAlpha(vec2 point, vec2 center, float enabled)
{
    if (enabled < 0.5) {
        return 0.0;
    }

    float distanceToArc = length(point - center) - radius;
    if (antialiasing > 0.5) {
        return smoothstep(-antialiasWidth, antialiasWidth, distanceToArc);
    }
    return step(0.0, distanceToArc);
}

void main()
{
    vec2 p = localPosition;
    vec2 size = screenSize;
    float alpha = 0.0;

    if (p.x < radius && p.y < radius) {
        alpha = cornerAlpha(p, vec2(radius, radius), cornerMask.x);
    } else if (p.x > size.x - radius && p.y < radius) {
        alpha = cornerAlpha(p, vec2(size.x - radius, radius), cornerMask.y);
    } else if (p.x < radius && p.y > size.y - radius) {
        alpha = cornerAlpha(p, vec2(radius, size.y - radius), cornerMask.z);
    } else if (p.x > size.x - radius && p.y > size.y - radius) {
        alpha = cornerAlpha(p, vec2(size.x - radius, size.y - radius), cornerMask.w);
    }

    fragColor = vec4(0.0, 0.0, 0.0, alpha);
}
)";

} // namespace

KWinScreenCornersEffect::KWinScreenCornersEffect()
{
    KWinScreenCorners::KWinScreenCornersConfigData::instance(effects->config());
    reconfigure(ReconfigureAll);

    if (effects->isOpenGLCompositing()) {
        m_shader = ShaderManager::instance()->loadShaderFromCode(
            QByteArray(vertexShader), QByteArray(fragmentShader));
        if (m_shader && m_shader->isValid()) {
            m_vertexBuffer = std::make_unique<GLVertexBuffer>(GLVertexBuffer::Stream);
        } else {
            m_shader.reset();
        }
    }
}

KWinScreenCornersEffect::~KWinScreenCornersEffect() = default;

bool KWinScreenCornersEffect::supported()
{
    return effects && effects->isOpenGLCompositing();
}

void KWinScreenCornersEffect::reconfigure(ReconfigureFlags)
{
    auto *config = KWinScreenCorners::KWinScreenCornersConfigData::self();
    config->read();

    m_radius = std::clamp(config->radius(), 1, 4096);
    m_topLeft = config->topLeft();
    m_topRight = config->topRight();
    m_bottomLeft = config->bottomLeft();
    m_bottomRight = config->bottomRight();
    m_antialiasing = config->antialiasing();

    if (effects) {
        effects->addRepaintFull();
    }
}

void KWinScreenCornersEffect::paintScreen(const RenderTarget &renderTarget,
                                      const RenderViewport &viewport,
                                      int mask,
                                      const QRegion &region,
                                      Output *screen)
{
    effects->paintScreen(renderTarget, viewport, mask, region, screen);
    paintMask(renderTarget, viewport);
}

bool KWinScreenCornersEffect::blocksDirectScanout() const
{
    return true;
}

int KWinScreenCornersEffect::requestedEffectChainPosition() const
{
    return 100;
}

void KWinScreenCornersEffect::paintMask(const RenderTarget &, const RenderViewport &viewport)
{
    if (!m_shader || !m_vertexBuffer) {
        return;
    }

    const QRectF rect = viewport.renderRect();
    if (!rect.isValid() || rect.width() <= 0.0 || rect.height() <= 0.0) {
        return;
    }

    const float radius = std::min<float>({
        // The configured radius is in physical pixels, independent of DPI.
        static_cast<float>(m_radius / viewport.scale()),
        static_cast<float>(rect.width() / 2.0),
        static_cast<float>(rect.height() / 2.0),
    });
    if (radius <= 0.0f) {
        return;
    }

    const std::array<GLVertex2D, 4> vertices = {{
        {QVector2D(rect.left(), rect.top()), QVector2D()},
        {QVector2D(rect.right(), rect.top()), QVector2D()},
        {QVector2D(rect.left(), rect.bottom()), QVector2D()},
        {QVector2D(rect.right(), rect.bottom()), QVector2D()},
    }};
    m_vertexBuffer->setVertices(vertices);

    ShaderBinder binder(m_shader.get());
    // KWin's projection is in device pixels; vertices and shader distances
    // are in global logical coordinates. Scale before applying the projection.
    // Keep the viewport's output transform and global origin intact.
    QMatrix4x4 projection = viewport.projectionMatrix();
    projection.scale(viewport.scale(), viewport.scale());
    m_shader->setUniform(GLShader::Mat4Uniform::ModelViewProjectionMatrix,
                         projection);
    m_shader->setUniform("origin", QVector2D(rect.left(), rect.top()));
    m_shader->setUniform("screenSize", QVector2D(rect.width(), rect.height()));
    m_shader->setUniform("radius", radius);
    m_shader->setUniform("antialiasWidth",
                         m_antialiasing ? static_cast<float>(1.0 / viewport.scale()) : 0.0001f);
    m_shader->setUniform("antialiasing", m_antialiasing ? 1.0f : 0.0f);
    m_shader->setUniform("cornerMask", QVector4D(
                                          m_topLeft ? 1.0f : 0.0f,
                                          m_topRight ? 1.0f : 0.0f,
                                          m_bottomLeft ? 1.0f : 0.0f,
                                          m_bottomRight ? 1.0f : 0.0f));

    const GLboolean wasBlending = glIsEnabled(GL_BLEND);
    GLint sourceRgb = GL_ONE;
    GLint destinationRgb = GL_ZERO;
    GLint sourceAlpha = GL_ONE;
    GLint destinationAlpha = GL_ZERO;
    GLint equationRgb = GL_FUNC_ADD;
    GLint equationAlpha = GL_FUNC_ADD;
    glGetIntegerv(GL_BLEND_SRC_RGB, &sourceRgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &destinationRgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &sourceAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &destinationAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &equationRgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &equationAlpha);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    m_vertexBuffer->render(GL_TRIANGLE_STRIP);

    glBlendFuncSeparate(sourceRgb, destinationRgb, sourceAlpha, destinationAlpha);
    glBlendEquationSeparate(equationRgb, equationAlpha);
    if (!wasBlending) {
        glDisable(GL_BLEND);
    }
}

} // namespace KWin
