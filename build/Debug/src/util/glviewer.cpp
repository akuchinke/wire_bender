#include "glviewer.h"

#include "abstractexercise.h"
#include "camera.h"

GLViewer::GLViewer(AbstractExercise *painter)
    : QOpenGLWindow()
    , m_painter(painter)
    , m_painterInitialized(false)
{
    qDebug() << painter->hints();

	QSurfaceFormat format;
	format.setVersion(3, 2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapInterval(1);
    format.setDepthBufferSize(16); // Required to allow both 16 and 24 bit depth buffers (fix for OS X and Intel HD)
	setFormat(format);

	resize(800, 600);

	auto camera = new Camera();
	camera->setFovy(40.0);
	camera->setCenter(QVector3D(0.f, 0.0f, 0.0f));
	camera->setEye(QVector3D(0.f, 3.f, 6.f));
	camera->setUp(QVector3D(0.f, 1.0f, 0.0f));
	painter->setCamera(camera);
}

GLViewer::~GLViewer()
{
    delete m_painter;
}

void GLViewer::initializeGL()
{
    if (!m_painterInitialized)
    {
        m_painterInitialized = true;
        m_painter->initialize();

        if (!m_initialSize.isEmpty())
        {
            m_painter->resize(m_initialSize.width(), m_initialSize.height());
            m_painter->camera()->setViewport(m_initialSize);
        }
    }
}

void GLViewer::resizeGL(int w, int h)
{
    if (!m_painterInitialized)
    {
        m_initialSize = QSize(w, h);

        return;
    }

	m_painter->resize(w, h);
	m_painter->camera()->setViewport(QSize(w, h));
}

void GLViewer::paintGL()
{
    if (renderLoopRequired())
	{
		m_painter->update();
		m_painter->paint();

		update(); //request next frame
	}
}

void GLViewer::keyPressEvent(QKeyEvent * event)
{
	m_painter->onKeyPressed(event);

	restartRenderLoopIfNecessary();
}

void GLViewer::exposeEvent(QExposeEvent * ev)
{
	restartRenderLoopIfNecessary();
}

bool GLViewer::renderLoopRequired()
{
	return isExposed() && m_painter->isActive();
}

void GLViewer::restartRenderLoopIfNecessary()
{
	if (renderLoopRequired())
	{
		//start render loop again
		update();
	}
}
