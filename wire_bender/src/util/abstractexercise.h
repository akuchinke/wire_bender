#pragma once

#include "abstractpainter.h"

#include <QKeyEvent>
#include <QMatrix4x4>

class AbstractExercise : public AbstractPainter
{
public:
    AbstractExercise();
    virtual ~AbstractExercise();

    virtual const QString hints() const = 0;

    virtual void render() = 0;

    void paint() override;

    void resize(int width, int height) override;

    void update() override;
    void update(const QList<QOpenGLShaderProgram *> &programs) override;

    bool onKeyPressed(QKeyEvent * event) override;

    void setActive(const bool active);
    const bool isActive() const;

    virtual void enable();
    virtual void disable();

protected:
    int m_frame;
    bool m_active;
};
