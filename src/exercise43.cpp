#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QScopedPointer>
#include <QApplication>
#include <QMainWindow>
#include <QtMath>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"
#include "util/unitcube.h"

class Exercise43 : public AbstractExercise
{
public:
    Exercise43();
	~Exercise43() override;

    const QString hints() const override;
    bool initialize() override;
    void render() override;

protected:
    QMatrix4x4 rotateClockwise(int frame);
    QMatrix4x4  m_cubeTransform;
    QScopedPointer<QOpenGLShaderProgram> m_program;
    UnitCube m_cube;
};

Exercise43::Exercise43()
{
    m_cubeTransform.scale(0.5f);
}

Exercise43::~Exercise43()
{
}

const QString Exercise43::hints() const
{
	return QString("Press [SPACE] to pause or resume animation.");
}

bool Exercise43::initialize()
{
    AbstractExercise::initialize();

    m_camera->setEye(QVector3D(0.0f, 3.0f, -6.0f));

    m_program.reset(createBasicShaderProgram("data/cube.vert", "data/cube.frag"));
    m_program->bind();

    m_cube.initialize(*m_program);

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	return true;
}

void Exercise43::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
    m_program->setUniformValue("viewprojection", camera()->viewProjection());

    m_program->setUniformValue("transform", m_cubeTransform);
    m_program->setUniformValue("normalMatrix", (camera()->view() * m_cubeTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));

    m_cube.draw(*this);

    m_program->setUniformValue("transform", rotateClockwise(m_frame) * m_cubeTransform);
    m_program->setUniformValue("normalMatrix", (camera()->view() * rotateClockwise(m_frame) * m_cubeTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1.0f, 1.0f, 0.0f, 1.0f));

    m_cube.draw(*this);
}

QMatrix4x4 Exercise43::rotateClockwise(int frame)
{
	//enum direction{ TOP, RIGHT, BOT, LEFT };
	//direction currDirection=TOP;
	// TODO: Calculate rotation and translation for given frame
	auto cFrame = frame % 720;
	auto cDirection = (cFrame / 180) * 90;
	auto cRotation = cFrame % 180;
	
	QMatrix4x4 matrix;
	matrix.rotate(cDirection, 0.0f, 0.0f, 1.0f);
	matrix.translate(-0.5f, 0.5f, 0.0f);
	matrix.rotate(cRotation, 0.0f, 0.0f, 1.0f);
	matrix.translate(0.5f, 0.5f, 0.0f);
	
	return matrix;
}

int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
	GLViewer viewer(new Exercise43);
	viewer.show();

    // Run application
    return app.exec();
}
