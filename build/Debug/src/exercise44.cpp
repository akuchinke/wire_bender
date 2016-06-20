#include <QDebug>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <qmath.h>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QApplication>
#include <QMainWindow>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"
#include "util/icosahedron.h"
#include "util/unitcube.h"

class Exercise44 : public AbstractExercise
{
public:
    Exercise44();
	~Exercise44() override;

    const QString hints() const override;

    bool initialize() override;
    void render() override;

protected:
    void drawEnvironment();

    virtual QMatrix4x4 applyBallTransformation(const int frame);

    GLuint m_textureID;

    QScopedPointer<QOpenGLShaderProgram> m_cubeProgram;

    Icosahedron m_sphere;
    UnitCube m_cube;
    QMatrix4x4 m_sphereScale;
    QScopedPointer<QOpenGLShaderProgram> m_sphereProgram;
};

namespace 
{
    const QString TEXTURE_FILENAME = "data/luxo.png";
}

Exercise44::Exercise44()
: m_textureID(-1)
, m_sphere(3)
{
    m_sphereScale.scale(0.3f);
}

Exercise44::~Exercise44()
{
	if (m_textureID != -1)
    {
        glDeleteTextures(1, &m_textureID);
    }
}

void Exercise44::drawEnvironment()
{
    m_cubeProgram->bind();
    m_cubeProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
    m_cubeProgram->setUniformValue("viewprojection", camera()->viewProjection());

    // Draw big cubes

    m_cubeProgram->setUniformValue("color", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));

    QMatrix4x4 transform;
    transform.translate(-4.9f, -1.201f, 0.0f);
    transform.scale(4.f, 2.0f,  4.0f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);


    transform.setToIdentity();
    transform.translate(4.9f, -1.401f, 0.0f);
    transform.scale(4.f, 1.8f, 4.0f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);


    transform.setToIdentity();
    transform.translate(0.0f, -3.0501f, 0.0f);
    transform.scale(1.0f, 2.0f,  4.0f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);


    // Draw trail

    m_cubeProgram->setUniformValue("color", QVector4D(0.9f, 0.9f, 0.9f, 1.0f));

    transform.setToIdentity();
    transform.translate(-4.9f, 0.805f, 0.0f);
    transform.scale(4.f, 0.0f, 0.2f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);


    transform.setToIdentity();
    transform.translate(4.9f, 0.405f, 0.0f);
    transform.scale(4.f, 0.0f, 0.2f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);

    transform.setToIdentity();
    transform.translate(0.0f, -1.045f, 0.0f);
    transform.scale(1.0f, 0.0f,  0.2f);

    m_cubeProgram->setUniformValue("transform", transform);
    m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

    m_cube.draw(*this);
}

void Exercise44::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    drawEnvironment();

	glBindTexture(GL_TEXTURE_2D, m_textureID);
    glActiveTexture(GL_TEXTURE0);

    QMatrix4x4 transform = applyBallTransformation(m_frame) * m_sphereScale;

    m_sphereProgram->bind();
    m_sphereProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
    m_sphereProgram->setUniformValue("viewprojection", camera()->viewProjection());

    m_sphereProgram->setUniformValue("transform", transform);
    m_sphereProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());
    m_sphereProgram->setUniformValue("diffuse", 0);

    m_sphere.draw(*this);
}

QMatrix4x4 Exercise44::applyBallTransformation(const int frame)
{
	static const float fX = 0.01f;
	static const int numFramesPerAnimation = static_cast<int>(4.0f / fX);

	static const float r = 0.3f;
	static const float d = 0.3f * r;

    // TODO: Calculate manipulation matrix here.
    //       The sphere's environment is defined as follows:
    //              start at    x = -2.0 and y = 0.8
    //              left  cliff x = -0.9
    //              bottom at   y = -1.05;
    //              right cliff x = +0.9 and y = 0.4
    //              end at      x = +2.0

	return QMatrix4x4();
}

bool Exercise44::initialize()
{
    AbstractExercise::initialize();

    m_textureID = getOrCreateTexture(TEXTURE_FILENAME);


    m_cubeProgram.reset(createBasicShaderProgram("data/cube.vert", "data/cube.frag"));

    m_cube.initialize(*m_cubeProgram);

    m_sphereProgram.reset(createBasicShaderProgram("data/sphere.vert", "data/sphere.frag"));

    m_sphere.initialize(*m_sphereProgram);

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return true;
}

const QString Exercise44::hints() const
{
    return QString("Press [SPACE] to pause or resume animation.");
}

//[-------------------------------------------------------]
//[ Main function                                         ]
//[-------------------------------------------------------]
int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
	GLViewer viewer(new Exercise44);
	viewer.show();

    // Run application
    return app.exec();
}
