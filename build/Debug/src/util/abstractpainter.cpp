#include "abstractpainter.h"

#include <cassert>
#include <QStringList>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

#include "fileassociatedshader.h"

#include "camera.h"

AbstractPainter::AbstractPainter()
    : m_camera(0)
{
}
 
AbstractPainter::~AbstractPainter()
{
}

bool AbstractPainter::initialize()
{
    initializeOpenGLFunctions();

    return true;
}

void AbstractPainter::setCamera(Camera *camera)
{
    if (m_camera == camera)
        return;

    m_camera = camera;
    update();
}

Camera *AbstractPainter::camera()
{
    return m_camera;
}

bool AbstractPainter::onKeyPressed(QKeyEvent * event)
{
    return false;
}

GLuint AbstractPainter::getOrCreateTexture(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull())
    {
        qDebug() << "Loading image from" << fileName << "failed.";
        return -1;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height()
        , 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    return texture;
}

QOpenGLShaderProgram* AbstractPainter::createBasicShaderProgram(const QString &vertexShaderFileName, const QString &fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();
    program->create();

    m_shaders << FileAssociatedShader::getOrCreate(QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(QOpenGLShader::Fragment, fragmentShaderFileName, *program);
    program->link();

    return program;
}
