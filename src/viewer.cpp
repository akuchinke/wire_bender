#include <qmath.h>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QRectF>
#include <QSettings>
#include <QScopedPointer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QList>
#include <QVector3D>
#include <QTime>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QThread>

#include "util/wire.h"
#include "util/wirecreator.h"
#include "util/camera.h"
#include "util/polygonaldrawable.h"
#include "util/objio.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"

enum class TranslationMode
{
	ConstantTime,
	ConstantSpeed
};

class Viewer : public AbstractExercise
{

public:
    Viewer();
    ~Viewer() override;

    void render() override;
    const QString hints() const override;


    bool onKeyPressed(QKeyEvent* keyEvent) override;
    bool initialize() override;
public slots:
    void collisionHandled();
    void changedPath(QVector<QVector3D> path);

protected:
    bool ani_running;
    bool finished;
    QVector<QVector3D> wirepath;
    wire m_wire;
    WireCreator m_wirecreator;
    int m_animationLength;
    PolygonalDrawable * m_drawable;
    PolygonalDrawable * m_pathDrawable;
    QVector<QVector3D> not_processed;
    QVector<QVector3D> already_processed;
    QTime m_time;
    int m_elapsed;
    QVector<QVector3D> m_path;
    qreal m_pathLength;
    TranslationMode m_translationMode;

    QScopedPointer<QOpenGLShaderProgram> m_modelProgram;
    QScopedPointer<QOpenGLShaderProgram> m_pathProgram;

    QMatrix4x4 computeTransformationMatrix(int currentFrame, int maxFrame);
    void updatePath(QVector<QVector3D> points);
    void createBBDrawables();
    void createIntersectDrawables();
    QVector<PolygonalDrawable*> bbDrawables;

    QVector<PolygonalDrawable*> borderDrawables;
     PolygonalDrawable* intersectDrawables;


};

Viewer::Viewer()
	: m_animationLength(10000)
    , m_drawable(nullptr)
	, m_pathDrawable(nullptr)
	, m_elapsed(0)
	, m_translationMode(TranslationMode::ConstantTime)
	, m_pathLength(0.0)
    ,finished(false)
    ,ani_running(false)
{
    QPair<int,int> a=qMakePair(3,5);
    qSwap(a.first, a.second);
    qDebug()<<a;
    //setCursor(Qt::BlankCursor);
    connect(&m_wirecreator, &WireCreator::collisionHandled, this, Viewer::collisionHandled);
    connect(&m_wire, &wire::collision, &m_wirecreator, WireCreator::collision);
    m_path
            <<5*QVector3D(0.0f, 0.0f, 0.0f)
            <<5*QVector3D(1.0f, 0.0f, 0.0f)
            <<5*QVector3D(1.0f, 0.0f, 1.0f)
            <<5*QVector3D(0.0f, 0.0f, 1.0f)
            <<5*QVector3D(1.0f, -1.0f, 1.0f)

              ;
    wirepath=m_path;

    QVector<QVector3D> vert;
    vert=m_path;
;
    QVector<QPair<int,int>> edges;
    edges
            <<qMakePair(0,1)
            <<qMakePair(1,2)
            <<qMakePair(2,3)
            <<qMakePair(2,4)
            //<<qMakePair(4,1)
            //<<qMakePair(4,0)
              ;

    m_wirecreator.readData(vert,edges);
    //qDebug()<<"deadends:";
    //qDebug()<<m_wirecreator.deadEnds;
    m_wirecreator.calcPathes();
    qDebug()<<"allPathes"<<m_wirecreator.allPathes;
    //m_wirecreator.createLine();
    //qDebug()<<"tracing back deadend";
    //qDebug()<<m_wirecreator.traceBackDE(3);
    //m_wirecreator.checkPath();
    qDebug()<<m_wirecreator.cPath;
    m_wire.read_Input(m_wirecreator.cPath);
    m_wire.boundingBox.push_back(qMakePair(QVector3D(0.4f, -4.5f, 6.0f),QVector3D(7.0f, 0.1f, -6.0f)));
    //qDebug()<<m_wire.innerPoints;
    //QPushButton button =new QPushButton("test");

  /*  for (auto it = m_path.begin(); it+1 != m_path.end(); ++it)
    {
        m_pathLength += QVector3D(*(it+1) - *it).length();
    }*/
}

Viewer::~Viewer()
{
    delete m_drawable;
}

bool Viewer::initialize()
{
    AbstractExercise::initialize();
    qDebug()<<"start initialize";
    if (m_drawable)
        return true;

    m_modelProgram.reset(createBasicShaderProgram("data/model.vert", "data/model.frag"));
    m_modelProgram->bind();

    m_drawable = ObjIO::fromObjFile("data/sphere.obj");

    m_drawable->createVAO(m_modelProgram.data());

    m_pathProgram.reset(createBasicShaderProgram("data/path.vert", "data/path.frag"));
    m_pathProgram->bind();

    m_pathDrawable = new PolygonalDrawable();
    m_pathDrawable->setMode(GL_LINE_STRIP);
    m_pathDrawable->vertices() = m_path;
    m_pathDrawable->createVAO(m_pathProgram.data());
    createBBDrawables();
    QVector<QVector3D> dummy;
    //dummy<<QVector3D(1.0f, 1.0f, 1.0f);
    intersectDrawables= new PolygonalDrawable();
    intersectDrawables->setMode(GL_POINTS);
    intersectDrawables->vertices()=dummy;
    intersectDrawables->createVAO(m_pathProgram.data());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_time.start();
    qDebug()<<"finished inititalize";
    return true;
}

void Viewer::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    static QMatrix4x4 globalScale;
    if(globalScale.isIdentity())
        globalScale.scale(0.6f);

    static QMatrix4x4 modelScale;
    if(modelScale.isIdentity())
        modelScale.scale(0.2f);

    m_modelProgram->bind();
    m_modelProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 4.0f));
    m_modelProgram->setUniformValue("viewprojection", camera()->viewProjection());

    if (m_active)
    {
        m_elapsed = (m_elapsed + m_time.elapsed()) % m_animationLength;
    }

    m_time.start();


    if(!finished && ani_running){
        auto newPath=m_wire.nextPoint(0.2f, 1.0f);
        //qDebug()<<newPath;
        if(newPath==m_path){
            finished=true;
        }
        qDebug()<<"calculated new path";
        updatePath(newPath);
    }
    //QMatrix4x4 transform = computeTransformationMatrix(m_elapsed, m_animationLength);
    QMatrix4x4 transform;
    transform.translate(0.0f, 0.0f, 0.0f);
    m_modelProgram->bind();
    m_modelProgram->setUniformValue("transform", globalScale * transform * modelScale);
    m_modelProgram->setUniformValue("normalMatrix", (camera()->view() * globalScale * transform * modelScale).normalMatrix());
    m_modelProgram->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

    m_drawable->draw(*this);

    m_pathProgram->bind();
    m_pathProgram->setUniformValue("viewprojection", camera()->viewProjection());
    m_pathProgram->setUniformValue("transform", globalScale);
    m_pathProgram->setUniformValue("color", QVector4D(0.3f, 0.3f, 0.3f, 1.0f));

    m_pathDrawable->draw(*this);




    m_pathProgram->setUniformValue("color", QVector4D(0.3f, 0.3f, 0.3f, 0.3f));
    for(auto db: borderDrawables){
        db->draw(*this);
    }
    m_pathProgram->setUniformValue("color", QVector4D(1.0f, 0.3f, 0.3f, 0.3f));
    for(auto db: bbDrawables){
        db->draw(*this);
    }

      m_pathProgram->setUniformValue("color", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    intersectDrawables->draw(*this);
}

bool Viewer::onKeyPressed(QKeyEvent* keyEvent)
{
    bool changed = false;
    if(keyEvent->key()==Qt::Key_Right){
        //qDebug()<<"start nextPoint";
        //qDebug()<<m_wire.nextPoint3();
        m_wire.nextPoint3();
        auto newPath=m_wire.outerPoints;
        qDebug()<<newPath;
        updatePath(newPath);
    }
	if (keyEvent->key() == Qt::Key_T)
    {
        m_translationMode = m_translationMode == TranslationMode::ConstantTime ? TranslationMode::ConstantSpeed : TranslationMode::ConstantTime;
        changed = true;
    }
    if (keyEvent->key() == Qt::Key_R)
    {
       finished=false;
       ani_running=false;
       m_wire.read_Input(wirepath);
       m_wire.checkBoundingBoxPlanes();
       updatePath(wirepath);

    }
    if (keyEvent->key() == Qt::Key_E)
    {
       ani_running=!ani_running;

    }

    return changed || AbstractExercise::onKeyPressed(keyEvent);
}

void Viewer::updatePath(QVector<QVector3D> points){
    m_path.clear();
    m_path=points;
    qDebug()<<"test";
    m_pathDrawable->vertices()=m_path;
    m_pathDrawable->createVAO(m_pathProgram.data());
    intersectDrawables->vertices()=m_wire.intersPoints;
    intersectDrawables->createVAO(m_pathProgram.data());

}

void Viewer::createBBDrawables()
{
    QVector<QPair<QVector3D, QVector3D> > bbs=m_wire.boundingBox;
    for(auto bb: bbs){
        qDebug()<<bb;
        PolygonalDrawable *box = new PolygonalDrawable();
        PolygonalDrawable *border = new PolygonalDrawable();

        box->setMode(GL_TRIANGLES);
        border->setMode(GL_LINE_STRIP);

        QVector<QVector3D> p;
        QVector<QVector3D> p2;
        p
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.second)                                  //7
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.second)                                  //7
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5

               ;

        p2
                //front
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5

               //right
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.second)                                  //7
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6

               //back
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.second)                                  //7
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8
                <<QVector3D(bb.second)                                  //7

               //left 4 1 8 1 5 8
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8

                //top 5 6 8 6 7 8
                <<QVector3D(bb.first.x(),bb.second.y(),bb.first.z())    //5
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8
                <<QVector3D(bb.second.x(),bb.second.y(),bb.first.z())   //6
                <<QVector3D(bb.second)                                  //7
                <<QVector3D(bb.first.x(),bb.second.y(),bb.second.z())   //8

                //bot 4 3 1 3 2 1
                <<QVector3D(bb.first.x(),bb.first.y(),bb.second.z())    //4
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.first)                                   //1
                <<QVector3D(bb.second.x(),bb.first.y(),bb.second.z())   //3
                <<QVector3D(bb.second.x(),bb.first.y(),bb.first.z())    //2
                <<QVector3D(bb.first)                                   //1



        ;
        box->vertices() = p2;
        border->vertices()=p;
        qDebug()<<"try creating vertices";




        box->createVAO(m_pathProgram.data());
        border->createVAO(m_pathProgram.data());

        bbDrawables.push_back(box);
        borderDrawables.push_back(border);
    }
}

void Viewer::createIntersectDrawables()
{
    qDebug()<<"creating intersects";
    intersectDrawables->setMode(GL_POINTS);
    intersectDrawables->vertices()=m_wire.intersPoints;
    intersectDrawables->createVAO(m_pathProgram.data());
}

void Viewer::collisionHandled()
{

    m_wire.read_Input(m_wirecreator.cPath);
    qDebug()<<"got new input:"<< m_wirecreator.cPath;
}

void Viewer::changedPath(QVector<QVector3D> path)
{
    updatePath(path);
}

QMatrix4x4 Viewer::computeTransformationMatrix(int currentFrame, int maxFrame)
{
	// TODO: Calculate manipulation matrix here.

}


const QString Viewer::hints() const
{
    return "Press [SPACE] to pause/resume animation. Use key T to modify the type of rotation and translation interpolation.";
}

int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
    GLViewer viewer(new Viewer);
	viewer.show();

    // Run application
    return app.exec();
}
