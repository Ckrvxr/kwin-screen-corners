// SPDX-License-Identifier: AGPL-3.0-or-later

// Run with the effect source path as argv[1]. Tests the actual embedded shaders.
#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QFile>
#include <QRegularExpression>
#include <QMatrix4x4>
#include <QImage>
#include <QDebug>
int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    if (argc != 2 && argc != 5) return 2;
    const float scale = argc == 5 ? QByteArray(argv[2]).toFloat() : 1.f;
    const float ox = argc == 5 ? QByteArray(argv[3]).toFloat() : 0.f;
    const float oy = argc == 5 ? QByteArray(argv[4]).toFloat() : 0.f;
    const int pixels = qRound(128 * scale);
    QFile file(QString::fromLocal8Bit(argv[1]));
    if (!file.open(QIODevice::ReadOnly)) return 2;
    const QString source = QString::fromUtf8(file.readAll());
    auto extract = [&](QString name) {
        return QRegularExpression(name + QStringLiteral(" = R\"\\((.*?)\\)\";"), QRegularExpression::DotMatchesEverythingOption).match(source).captured(1).toUtf8();
    };
    QSurfaceFormat format; format.setVersion(3, 1);
    QOpenGLContext context; context.setFormat(format);
    if (!context.create()) return 3;
    QOffscreenSurface surface; surface.setFormat(context.format()); surface.create();
    if (!context.makeCurrent(&surface)) return 3;
    QOpenGLShaderProgram shader;
    if (!shader.addShaderFromSourceCode(QOpenGLShader::Vertex, extract(QStringLiteral("vertexShader"))) ||
        !shader.addShaderFromSourceCode(QOpenGLShader::Fragment, extract(QStringLiteral("fragmentShader")))) return 4;
    // Match KWin 6.3.6 ShaderManager's attribute binding.
    shader.bindAttributeLocation("vertex", 0); shader.bindAttributeLocation("texCoord", 1);
    if (!shader.link() || !shader.bind()) return 4;
    qInfo() << "position attribute:" << shader.attributeLocation("position") << "vertex attribute:" << shader.attributeLocation("vertex");
    auto *gl = context.functions();
    QOpenGLFramebufferObject fbo(pixels, pixels); if (!fbo.bind()) return 5;
    gl->glViewport(0, 0, pixels, pixels); gl->glClearColor(1, 1, 1, 1); gl->glClear(GL_COLOR_BUFFER_BIT);
    shader.bind();
    // Reproduce KWin's global device-pixel projection, then the effect's fix.
    QMatrix4x4 matrix;
    matrix.ortho(ox * scale, (ox + 128) * scale, (oy + 128) * scale, oy * scale, -1.f, 1.f);
    if (!qEnvironmentVariableIsSet("KWIN_SCREEN_CORNERS_TEST_OLD_PROJECTION")) matrix.scale(scale, scale);
    shader.setUniformValue("modelViewProjectionMatrix", matrix);
    shader.setUniformValue("origin", QVector2D(ox, oy));
    shader.setUniformValue("screenSize", QVector2D(128, 128));
    shader.setUniformValue("radius", 24.f / scale);
    shader.setUniformValue("antialiasWidth", 1.f / scale);
    shader.setUniformValue("antialiasing", 1.f);
    shader.setUniformValue("cornerMask", QVector4D(1, 1, 1, 1));
    QOpenGLVertexArrayObject vao; if (!vao.create()) return 5; vao.bind();
    const float vertices[] = {ox,oy,ox+128,oy,ox,oy+128,ox+128,oy+128};
    GLuint buffer; gl->glGenBuffers(1, &buffer); gl->glBindBuffer(GL_ARRAY_BUFFER, buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0); gl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    gl->glEnable(GL_BLEND); gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    qInfo() << "draw error" << gl->glGetError() << context.format();
    const auto image = fbo.toImage();
    bool ok = image.pixelColor(pixels/2,pixels/2).red() == 255;
    for (const QPoint p : {QPoint(0,0), QPoint(pixels-1,0), QPoint(0,pixels-1), QPoint(pixels-1,pixels-1)}) ok &= image.pixelColor(p).red() == 0;
    // Midpoints along the physical edges must not contain displaced corners.
    for (const QPoint p : {QPoint(pixels/2,0), QPoint(0,pixels/2), QPoint(pixels-1,pixels/2), QPoint(pixels/2,pixels-1)}) ok &= image.pixelColor(p).red() == 255;
    bool antialias = false;
    // Fixed device-pixel samples distinguish a 24px radius from a scaled one.
    ok &= image.pixelColor(6,6).red() < 32;
    ok &= image.pixelColor(10,10).red() > 223;
    for (int y=0; y<24; ++y) for (int x=0; x<24; ++x) { int r=image.pixelColor(x,y).red(); antialias |= r>0 && r<255; }
    ok &= antialias;
    gl->glDeleteBuffers(1, &buffer);
    qInfo() << (ok ? "PASS: black corners, white center, antialiased edge" : "FAIL");
    return ok ? 0 : 6;
}
