#include "wirecreator.h"
#include <QDebug>
#include <QtMath>
#include <QQuaternion>
#include <QMatrix4x4>
WireCreator::WireCreator(QObject *parent):
    QObject(parent)
{

}


void WireCreator::readData(QVector<QVector3D> vert, QVector<QPair<int, int> > ed)
{
    vertices=vert;
    edges=ed;
    cEndsNForks();
    qDebug()<<forks;
    qDebug()<<deadEnds;
}

bool WireCreator::calcAllPathes(QVector<int> currPath, QVector<QPair<int,int>> usedEdges, QPair<int,int> nextEdge)
{

    qDebug()<<"in creator currPath"<<currPath;
    /*if(containsAllE(currPath)){
        qDebug()<<"success: "<<currPath;
        allPathes.push_back(currPath);
        return true;
    }*/
    int nextPoint=-1;
    if(currPath.last()==nextEdge.first)nextPoint=nextEdge.second;
    if(currPath.last()==nextEdge.second)nextPoint=nextEdge.first;


    if(nextPoint!=-1){
        currPath.append(nextPoint);
        usedEdges.push_back(nextEdge);
    }
    if(containsAllE(currPath)){
        qDebug()<<currPath;
        allPathes.push_back(currPath);
        return true;
    }

    if(deadEnds.indexOf(nextPoint)!=-1){
        qDebug()<<"Deadend";
        currPath.append(traceBackDE(nextPoint));
    }
    for(auto edge: edges){
        if(usedEdges.indexOf(edge)==-1){
            if(edge.first==currPath.last() || edge.second==currPath.last()){
                calcAllPathes(currPath, usedEdges, edge);
            }
        }
    }
    qDebug()<<currPath;
   return false;

}

void WireCreator::createLine()
{
    QVector<int> usedPoints;
    int currPoint=deadEnds[0];
    //currPath.push_back(vertices[currPoint]);
    usedPoints.push_back(currPoint);
    //while(currPoint!=deadEnds.last()){
    while(deadEnds.indexOf(currPoint)==-1||usedPoints.length()==1){

        for(auto p: edges){
            if(p.first==currPoint){
                currPoint=p.second;
                break;
            }
        }
      //currPath.push_back(vertices[currPoint]);
      usedPoints.push_back(currPoint);
    }
    //usedPoints.pop_back();
    qDebug()<<containsAllE(usedPoints);
    //modifiedPath=currPath;
    //qDebug()<<currPath;
    rawPath=usedPoints;
}

void WireCreator::calcPathes()
{
    QVector<int> vec={0, 1, 2, 4, 2, 3};
    qDebug()<<vec;
    qDebug()<<"test"<<containsAllE(vec);


    auto nextPoint=deadEnds[0];
    auto nextEdge=edges[0];
    for(auto edge: edges){
        if(edge.first==nextPoint || edge.second==nextPoint)
        nextEdge=edge;
    }
    QVector<int> currPath;
    currPath<<nextPoint;
    QVector<QPair<int,int>> usedEdges;
    calcAllPathes(currPath, usedEdges, nextEdge);
    for(auto p: allPathes){
        qDebug()<<checkPath(p);
        if(allPathes.length())break;
    }

}

bool WireCreator::containsAllP(QVector<int> path)
{
    for(int p=0; p<vertices.length(); p++ ){
        if(path.indexOf(p)==-1)return false;
    }
    return true;
}

bool WireCreator::containsAllE(QVector<int> path)
{
    bool check=false;
    for(auto edge: edges){
        int iFirst= path.indexOf(edge.first);
        for(int i=0; i<path.length();i++){
            if(path[i]==edge.first){
                if(path[i-1]==edge.second || path[i+1]==edge.second)
                    check=true;
            }
        }
        if(check==false)return false;
        check=false;
    }
    return true;
}

void WireCreator::cEndsNForks()
{
    QVector<int> tempPoints;
    tempPoints.fill(0,vertices.length());
    for(auto pair : edges){
        tempPoints[pair.first]++;
        tempPoints[pair.second]++;

    }
    for(int i=0; i<tempPoints.length();i++){
        if(tempPoints[i]==1)deadEnds.push_back(i);
        if(tempPoints[i]>2)forks.push_back(i);
    }
}

QVector<int> WireCreator::traceBackDE(int point)
{
    QVector<int> path;
    int lastPoint=-1;
    int currPoint=point;
    int nextPoint=point;
    while(forks.indexOf(currPoint)==-1|| path.length()==0){
        currPoint=nextPoint;
        for(auto edge: edges){
            if(currPoint==edge.first&&edge.second!=lastPoint){
                lastPoint=currPoint;
                nextPoint=edge.second;
                break;
            }
            if(currPoint==edge.second&&edge.first!=lastPoint){
                lastPoint=currPoint;
                nextPoint=edge.first;
                break;
            }
        }
    path.push_back(currPoint);
    }
    path.pop_front();
    return path;
}

bool WireCreator::containsEdge(int a, int b)
{
    //for()
}

void WireCreator::collision(QVector3D point, QPair<QVector3D, QVector3D> box)
{
    auto collidedPoint= point;
    //qDebug()<<point<<"  "<<collidedPoint;
    collidedPoint.setX(0);


    auto axis = QVector3D::crossProduct(collidedPoint, QVector3D(0.0f, 1.0f, 0.0f));
    float angle= calcMinimumAngle(collidedPoint, box);
    angle=qRadiansToDegrees(angle);
    //qDebug()<<"angle: "<<angle;
    //qDebug()<<"axis: "<<axis;
    QQuaternion quat= QQuaternion::fromAxisAndAngle(axis, angle);
    QMatrix3x3 rot3= quat.toRotationMatrix();
    QMatrix4x4 result;
    for(int i=0;i<3;i++){
        for(int j=0; j<3; j++){
            if(i<3 && j<3)result(i,j)=rot3(i,j);
        }
    }
    //currPath=applyMatrix(currPath, result);
      //qDebug()<<"collision handled";
    emit collisionHandled();


}


QVector<QVector3D> WireCreator::applyMatrix(QVector<QVector3D> &path, QMatrix4x4 matrix)
{
    QVector<QVector3D> temppath;
    for(auto point: path){
        temppath.push_back(matrix*point);
    }
    return temppath;
}

float WireCreator::calcMinimumAngle(QVector3D point, QPair<QVector3D, QVector3D> box)
{
    float maxY=qMax(box.first.y(),box.second.y())+0.3f;
    if(point.length()<maxY) return 0;
    float angle=qAsin(maxY/point.length());
    return angle;

}

bool WireCreator::checkPath(QVector<int> path)
{
    auto modifiedPath=pathToPoints(path);
    auto basicPath=modifiedPath;

    m_wire.read_Input(modifiedPath);


    for(float rot=0.0f;rot<360.0f; rot+=5.0f ){

        while(!m_wire.nextPoint3()){
            //qDebug()<<"no collision for: "<<m_wire.outerPoints;
            if(m_wire.success){

               cPath=modifiedPath;
                return true;
        }
        }

        auto rotMat=QMatrix4x4();
        rotMat.setToIdentity();
        rotMat.rotate(rot,1.0f, 0.0f, 0.0f);
        //qDebug()<<"old path"<<modifiedPath;
        modifiedPath=applyMatrix(basicPath, rotMat);
        //qDebug()<<"new path"<<modifiedPath;
        m_wire.read_Input(modifiedPath);
    }
    //qDebug()<<"no rotation found";
    return false;

}

QVector<QVector3D> WireCreator::pathToPoints(QVector<int> path)
{
    QVector<QVector3D> res;
 for(auto i: path){
     res.push_back(vertices[i]);
 }
 return res;
}


QMatrix4x4 WireCreator::calcRotationMatrix(QVector3D axis, float angle)
{

    QQuaternion quat=QQuaternion::fromAxisAndAngle(axis, angle);
    QMatrix3x3 rot3= quat.toRotationMatrix();
    QMatrix4x4 result;
    for(int i=0;i<3;i++){
        for(int j=0; j<3; j++){
            if(i<3 && j<3)result(i,j)=rot3(i,j);
        }
    }
    return result;


}
