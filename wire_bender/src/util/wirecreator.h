#ifndef WIRECREATOR_H
#define WIRECREATOR_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QMatrix4x4>
#include <Qmatrix3x3>
#include <QQuaternion>
#include <QPair>
#include <QVector3D>

#include "wire.h"
class WireCreator: public QObject
{
    Q_OBJECT
public:
    WireCreator(QObject * parent = 0);
    void readData(QVector<QVector3D> vert, QVector<QPair<int,int>> ed);
    QVector<QVector3D> vertices;
    QVector< QPair<int,int> > edges;
    QVector<int> deadEnds;
    QVector<QVector<int>> possiblePathes;
    QVector<int> forks;
    bool calcAllPathes(QVector<int> currPath, QVector<QPair<int, int> > usedEdges, QPair<int, int> nextEdge);
    void createLine();
    void calcPathes();
    bool containsAllP(QVector<int> path);
    bool containsAllE(QVector<int> path);
    void cEndsNForks();
    QVector<int> traceBackDE(int point);
    bool containsEdge(int a, int b);
    wire m_wire;
    QVector<int> rawPath;
    QVector<QVector3D> cPath;
    QVector<QVector3D> applyMatrix(QVector<QVector3D> &path, QMatrix4x4 matrix);
    float calcMinimumAngle(QVector3D point, QPair<QVector3D, QVector3D> box);

    QMatrix4x4 calcRotationMatrix(QVector3D axis, float angle);
    bool checkPath(QVector<int> path);

    QVector<QVector3D> pathToPoints(QVector<int> path);

    QVector<QVector<int>> allPathes;
public slots:
    void collision(QVector3D point, QPair<QVector3D, QVector3D> box);
signals:
    collisionHandled();

};

#endif // WIRECREATOR_H
