#pragma once

#include <QOpenGLWindow>

class AbstractExercise;

class GLViewer : public QOpenGLWindow
{
public:
    explicit GLViewer(AbstractExercise *painter);
    virtual ~GLViewer();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent * event) override;
    void exposeEvent(QExposeEvent * ev) override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;

    bool renderLoopRequired();
    void restartRenderLoopIfNecessary();

    AbstractExercise *m_painter;

    // needed for platforms where resizeGL is called before initializeGL
    bool m_painterInitialized;
    QSize m_initialSize;
};
