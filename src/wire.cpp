
#include <QDebug>
#include <QtMath>
#include <QObject>
#include <QVector>
#include <QPair>
#include <QMatrix4x4>

class wire: public QObject
{
    Q_OBJECT
public:
    wire(QObject * parent = 0);
    QString print_Instructions();
    void read_Input(QVector<QVector3D> &vert , QVector<int> &edg);
    void calculate_Instructions();
    float calc_head_angle(QVector3D inPoint);
    float calc_bend_angle(QVector3D inPoint);
    QVector<QString> instructions;
    QVector<QVector3D> vertices;
    QVector<int> edges;
    QMatrix4x4 rotation;
    QMatrix4x4 translation;
    QMatrix4x4 bending;
    float head_angle;
};


wire::wire(QObject *parent):
    QObject(parent),
    head_angle(0.0f)
{
    rotation.setToIdentity();
    translation.setToIdentity();
    bending.setToIdentity();


}

QString wire::print_Instructions()
{
    QString full;
    for(auto input : instructions){
        full+=input+"\r";
    }
    return full;

}

void wire::read_Input(QVector<QVector3D>& vert, QVector<int>& edg)
{
    vertices=vert;
    edges=edg;
}

void wire::calculate_Instructions()
{
    QVector3D last;
    QVector3D diff;
    float h_angle, b_angle;

    for(auto edge: edges){
        diff=vertices[edge]-last;
        float distance=vertices[edge].distanceToPoint(last);
        qDebug()<<"from"<<last<<"to"<<vertices[edge];
        if (distance==0)continue;
        h_angle=calc_head_angle(diff);
        if( head_angle!=h_angle){
            instructions.push_back("rotate "+QString::number(h_angle-head_angle));
            head_angle=h_angle;
        }
        b_angle=calc_bend_angle(diff);
        if(b_angle!=0){
            instructions.push_back("bend "+QString::number(b_angle));
        }
        instructions.push_back("extrude "+QString::number(distance*50));
        last=vertices[edge];
    }
}

float wire::calc_head_angle(QVector3D inPoint)
{
   auto temp= inPoint.normalized();
   float t_angle=qAcos(temp.z());
   if(temp.y()<0)t_angle*=-1;
   qDebug()<<inPoint;
   qDebug()<<qRadiansToDegrees(t_angle);
   return qRadiansToDegrees(t_angle);
}

float wire::calc_bend_angle(QVector3D inPoint)
{
    auto temp=inPoint.normalized();
    float t_angle=qAcos(temp.x());
    return qRadiansToDegrees(t_angle);
}
