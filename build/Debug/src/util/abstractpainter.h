#pragma once

#include <QList>
#include <QSize>
#include <QKeyEvent>

#include "openglfunctions.h"

class QStringList;
class QOpenGLShaderProgram;
class QOpenGLShader;

class Camera;
class AbstractSceneObject;

class AbstractPainter : public QObject
    , public OpenGLFunctions
{
public:
    enum PaintMode
    {
		NORMAL,
		WIREFRAME
    };

public:
    AbstractPainter();
    virtual ~AbstractPainter();

    void setCamera(Camera *camera);
    Camera *camera();

    /** this should setup the OpenGL state and allocate all required static 
        resources (context is 3.2 core for this one).
    */
    virtual bool initialize();

    /** returns whether m_context is 0 or not
    */
    bool isValid() const;

    virtual void paint() = 0;
    virtual void resize(int width, int height) = 0;

    virtual void update() = 0;
    virtual void update(const QList<QOpenGLShaderProgram *> &programs) = 0;

    virtual bool onKeyPressed(QKeyEvent * event);
protected:
    Camera *m_camera;

    QList<QOpenGLShader*> m_shaders;

    GLuint getOrCreateTexture(const QString & fileName);
    QOpenGLShaderProgram* createBasicShaderProgram(const QString &vertexShaderFileName, const QString &fragmentShaderFileName);
};
