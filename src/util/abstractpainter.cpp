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
    const float move_distance = 0.1f;
    //if (event->modifiers())return;
        if (event->key() == Qt::Key_W){
            auto view_direction = (m_camera->center() - m_camera->eye()).normalized();
            m_camera->setEye(m_camera->eye() + move_distance* view_direction);
            m_camera->setCenter(m_camera->center() + move_distance* view_direction);
        }
        if (event->key() == Qt::Key_S){
            auto view_direction = (m_camera->center() - m_camera->eye()).normalized();
            m_camera->setEye(m_camera->eye() - move_distance* view_direction);
            m_camera->setCenter(m_camera->center() - move_distance* view_direction);
        }
        if (event->key() == Qt::Key_A){
            auto view_direction = m_camera->center() - m_camera->eye();
            auto local_x_Axis = QVector3D::crossProduct(m_camera->up(), view_direction);
            m_camera->setEye(m_camera->eye() + (move_distance* local_x_Axis.normalized()));
            m_camera->setCenter(m_camera->center() + (move_distance* local_x_Axis.normalized()));
        }

        if (event->key() == Qt::Key_D){
            auto view_direction = m_camera->center() - m_camera->eye();
            auto local_x_Axis = QVector3D::crossProduct(m_camera->up(), view_direction);
            m_camera->setEye(m_camera->eye() - (move_distance* local_x_Axis.normalized()));
            m_camera->setCenter(m_camera->center() - (move_distance* local_x_Axis.normalized()));
        }
        if (event->key() == Qt::Key_Plus){
            m_camera->setEye(m_camera->eye() + move_distance*(m_camera->up().normalized()));
            m_camera->setCenter(m_camera->center() + move_distance*(m_camera->up().normalized()));
        }
        if (event->key() == Qt::Key_Minus){
            m_camera->setEye(m_camera->eye() - move_distance*(m_camera->up().normalized()));
            m_camera->setCenter(m_camera->center() - move_distance*(m_camera->up().normalized()));
        }
        return true;
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


void AbstractPainter::mouseMoveEvent(QMouseEvent* event) {
    const float angle = 0.2f;
    static int last_x = 0;
    static int last_y = 0;
    int dist_x = 0;
    int dist_y = 0;
    if (event->buttons()&Qt::LeftButton){
        dist_x = event->pos().x() - last_x;
        dist_y = event->pos().y() - last_y;
        if (!(event->modifiers())){


            auto view_direction = m_camera->center() - m_camera->eye();

            //rotate up/down
            auto rotation_axis = QVector3D::crossProduct(m_camera->up(), view_direction);
            auto quat_x = QQuaternion::fromAxisAndAngle(rotation_axis, dist_y*angle);
            auto newDirection = quat_x.rotatedVector(view_direction);
            auto newUp = quat_x.rotatedVector(m_camera->up());

            //rotate up and view direction around y axis to keep image upright
            auto quat_y = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), -dist_x*angle);
            newDirection = quat_y.rotatedVector(newDirection);

            newUp = quat_y.rotatedVector(newUp);
            m_camera->setCenter(m_camera->eye() + newDirection);
            m_camera->setUp(newUp);
        }
        // little bonus: hold shift to rotate camera around the look-at point (center)
        if (event->modifiers() == Qt::ShiftModifier){
            auto relative_eye = m_camera->eye() - m_camera->center();
            auto rotation_axis = QVector3D::crossProduct(relative_eye, m_camera->up());
            auto quat_x = QQuaternion::fromAxisAndAngle(rotation_axis, dist_y*angle);
            auto new_eye = quat_x.rotatedVector(relative_eye);
            auto new_up = quat_x.rotatedVector(m_camera->up());

            auto quat_y = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), -dist_x*angle);
            new_eye = quat_y.rotatedVector(new_eye);
            new_up = quat_y.rotatedVector(new_up);
            m_camera->setEye(new_eye + m_camera->center());
            m_camera->setUp(new_up);


        }
    }
    last_x = event->pos().x();
    last_y = event->pos().y();





}
