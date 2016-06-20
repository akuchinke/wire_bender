#include "abstractexercise.h"

AbstractExercise::AbstractExercise()
	: m_frame(0)
    , m_active(true)
{
}

AbstractExercise::~AbstractExercise()
{
}

void AbstractExercise::paint()
{
    

    if (isActive())
    {
        ++m_frame;
    }

    render();
}

void AbstractExercise::resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void AbstractExercise::update()
{

}

void AbstractExercise::update(const QList<QOpenGLShaderProgram *> &programs)
{

}

void AbstractExercise::setActive(const bool active)
{
    m_active = active;
}

const bool AbstractExercise::isActive() const
{
    return m_active;
}

void AbstractExercise::enable()
{
    setActive(true);
}

void AbstractExercise::disable()
{
    setActive(false);
}

bool AbstractExercise::onKeyPressed(QKeyEvent * keyEvent)
{
    bool changed = false;
	if (keyEvent->key() == Qt::Key_Space && !keyEvent->modifiers())
    {
        setActive(!m_active);

        keyEvent->accept();
        changed = true;
    }

    return changed || AbstractPainter::onKeyPressed(keyEvent);
}
