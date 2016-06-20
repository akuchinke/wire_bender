#ifndef WIRE_H
#define WIRE_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QMatrix4x4>
#include <Qmatrix3x3>
#include <QQuaternion>
#include <QPair>
#include <QVector3D>

#include "polygonaldrawable.h";
class wire: public QObject
{
    Q_OBJECT
public:
    wire(QObject * parent = 0);
    QString print_Instructions();
    void read_Input(QVector<QVector3D> &vert);
    void read_Input(QVector<QVector3D> &vert , QVector<int> &edg);
    void calculate_Instructions();
    QVector<QVector3D> nextPoint(float mAngle=0.0f, float mdistance=0.0f);
    bool nextPoint2();
    bool nextPoint3();


    QVector<QVector3D> applyMatrix(QVector<QVector3D> & path, QMatrix4x4 matrix);


    QMatrix4x4 calcRotationMatrix(QVector3D point, float angle);
    float calc_head_angle(QVector3D inPoint);
    float calc_head_angle_from_cross(QVector3D inPoint);
    float calc_bend_angle(QVector3D inPoint);

    void addBoundingBox(QVector3D a, QVector3D b);
    void addBoundingBox(QVector<QPair<QVector3D,QVector3D>> bbs);
    void createBBDrawable();

    int checkState();
    bool translationState();
    bool rotationState();


    bool checkBoundingBoxPlanes();
    bool planeIntersect(QVector3D aVec, QVector3D bVec, QVector3D plane, QVector3D &resultPoint);


    QVector<QPair<QVector3D,QVector3D>> boundingBox;
    QVector<PolygonalDrawable> drawBB;
    QVector<QVector3D> outerPoints;
    QVector<QVector3D> innerPoints;

    QVector<QVector3D> allPoints;
    QVector<QVector3D> intersPoints;

    QVector<QString> instructions;
    QVector<QVector3D> vertices;
    QVector<int> edges;
    QQuaternion quatRotation;
    QMatrix4x4 rotation;
    QMatrix4x4 translation;
    QMatrix4x4 bending;
    float head_angle;
    bool success;

signals:
    collision(QVector3D, QPair<QVector3D, QVector3D>);
};

#endif // WIRE_H
