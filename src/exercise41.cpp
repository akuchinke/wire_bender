#include <QApplication>

#include <cfloat>

#include "util/abstractexercise.h"
#include "util/camera.h"
#include "util/glviewer.h"
#include "util/objio.h"
#include "util/polygonaldrawable.h"
#include "util/unitcube.h"
#include <iostream>

class Exercise41 : public AbstractExercise
{
public:
	Exercise41();
	~Exercise41() override;

	const QString hints() const override;

	bool initialize() override;
	void render() override;

protected:
	UnitCube m_cube;
	PolygonalDrawable * m_drawable;

	QScopedPointer<QOpenGLShaderProgram> m_cubeProgram;
	QScopedPointer<QOpenGLShaderProgram> m_modelProgram;

	QMatrix4x4 m_modelTransform;

	static QMatrix4x4 calculateModelTransform(const PolygonalDrawable * const drawable);
};

QMatrix4x4 Exercise41::calculateModelTransform(const PolygonalDrawable * const drawable)
{
	// TODO: Calculate manipulation matrix here

	auto vertices = drawable->vertices();
	auto min = QVector3D(vertices[0][0], vertices[0][1], vertices[0][2]);
	auto max = QVector3D(vertices[0][0], vertices[0][1], vertices[0][2]);

	for (auto i : vertices) {
		if (i[0] < min[0]) min[0] = i[0];
		if (i[1] < min[1]) min[1] = i[1];
		if (i[2] < min[2]) min[2] = i[2];

		if (i[0] > max[0]) max[0] = i[0];
		if (i[1] > max[1]) max[1] = i[1];
		if (i[2] > max[2]) max[2] = i[2];
	}

	float diff_x = max[0] - min[0];
	float diff_y = max[1] - min[1];
	float diff_z = max[2] - min[2];

	float diff_max = diff_x;
	if (diff_y > diff_max) diff_max = diff_y;
	if (diff_z > diff_max) diff_max = diff_z;

	float f = static_cast<float>(1) / diff_max;

	auto translation = QMatrix4x4(1, 0, 0, -(max[0]-(diff_x/2)),
		0, 1, 0, -(max[1]-diff_y/2),
		0, 0, 1, -(max[2]-diff_z/2),
		0, 0, 0, 1);

	auto skalierung = QMatrix4x4(f, 0, 0, 0,
		0, f, 0, 0,
		0, 0, f, 0,
		0, 0, 0, 1);

	auto out = skalierung * translation;
	return out;
}

Exercise41::Exercise41()
	: m_drawable(nullptr)
{

}

Exercise41::~Exercise41()
{
	delete m_drawable;
}

const QString Exercise41::hints() const
{
	return "";
}

bool Exercise41::initialize()
{
	AbstractExercise::initialize();

	m_modelProgram.reset(createBasicShaderProgram("data/model.vert", "data/model.frag"));
	m_modelProgram->bind();

	m_drawable = ObjIO::fromObjFile("data/suzanne2.obj");
	m_drawable->createVAO(m_modelProgram.data());

	m_cubeProgram.reset(createBasicShaderProgram("data/cube.vert", "data/cube.frag"));

	m_cube.initialize(*m_cubeProgram);
	camera()->setEye(QVector3D(1.0f, 1.0f, 4.0f));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	m_modelTransform = calculateModelTransform(m_drawable);

	return true;
}

void Exercise41::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//draw model
	m_modelProgram->bind();
	m_modelProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 4.0f));
	m_modelProgram->setUniformValue("viewprojection", camera()->viewProjection());

	m_modelProgram->bind();
	m_modelProgram->setUniformValue("transform", m_modelTransform);
	m_modelProgram->setUniformValue("normalMatrix", (camera()->view() * m_modelTransform).normalMatrix());
	m_modelProgram->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

	m_drawable->draw(*this);

	//draw cube
	m_cubeProgram->bind();
	m_cubeProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
	m_cubeProgram->setUniformValue("viewprojection", camera()->viewProjection());
	m_cubeProgram->setUniformValue("color", QVector4D(0.8f, 0.2f, 0.2f, 0.2f));

	QMatrix4x4 transform;
	transform.scale(0.5f, 0.5f, 0.5f);

	m_cubeProgram->setUniformValue("transform", transform);
	m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

	m_cube.draw(*this);
}

int main(int argc, char *argv[])
{
	// Create application
	QApplication app(argc, argv);

	// Create main window
	GLViewer viewer(new Exercise41);
	viewer.show();

	// Run application
	return app.exec();
}