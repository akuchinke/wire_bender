#include "wire.h"
#include <QDebug>
#include <QtMath>
#include <QQuaternion>


enum states{done=0, rotate=1, translate=2};

wire::wire(QObject *parent):
    QObject(parent),
    head_angle(0.0f),
    success(false)
{
    rotation.setToIdentity();
    translation.setToIdentity();
    bending.setToIdentity();
    outerPoints.clear();
    //boundingBox.push_back(qMakePair(QVector3D(0.4f, -4.5f, 6.0f),QVector3D(7.0f, 0.1f, -6.0f)));
    /*boundingBox.push_back(qMakePair(QVector3D(0.0f, 1.0f, 0.0f),QVector3D(1.0f, 2.0f, 1.0f)));
    boundingBox.push_back(qMakePair(QVector3D(0.0f, 1.0f, 0.0f),QVector3D(2.0f, 3.0f, 2.0f)));
    boundingBox.push_back(qMakePair(QVector3D(0.0f, 1.0f, 0.0f),QVector3D(3.0f, 4.0f, 3.0f)));
    boundingBox.push_back(qMakePair(QVector3D(0.0f, 1.0f, 0.0f),QVector3D(4.0f, 5.0f, 4.0f)));*/




}

QString wire::print_Instructions()
{
    QString full;
    for(auto input : instructions){
        full+=input+"\r";
    }
    return full;

}

void wire::read_Input(QVector<QVector3D> &vert)
{
    outerPoints.clear();
    innerPoints.clear();
    allPoints.clear();

    for(auto point : vert){

        allPoints.push_back(point-vert[0]);

        innerPoints.push_back(point-vert[0]);
    }

}

void wire::read_Input(QVector<QVector3D>& vert, QVector<int>& edg)
{
    outerPoints.clear();
    allPoints=vert;
    innerPoints=vert;
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
        //qDebug()<<"from"<<last<<"to"<<vertices[edge];
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

QVector<QVector3D> wire::nextPoint(float mAngle, float mdistance)
{
        //qDebug()<<"Bondung box violation:"<<checkBoundingBox();
    bool cDistance=false;
    bool cAngle=false;
    QVector3D nPoint=innerPoints[0];
    float distance = nPoint.distanceToPoint(QVector3D());
    if(mdistance && mdistance<distance){
        distance=mdistance;
        cDistance=true;
    }
    if(mAngle && mdistance<distance){
        distance=mdistance;
        cDistance=true;
    }
    if(outerPoints.last()==QVector3D()&&outerPoints.length()>1){
        //qDebug()<<"popping last point";
        outerPoints.pop_back();
    }
    if((qAbs(nPoint.z())>0.01f || qAbs(nPoint.y())>0.01f)){
        //qDebug()<<"Rotating";
        //if(outerPoints.last()==QVector3D()&&outerPoints.length()>1)outerPoints.pop_back();
        //qDebug()<<"calculating new matrix";
        float bAngle=calc_bend_angle(nPoint);
        //qDebug()<<"angle:" <<bAngle;
        if(mAngle && mAngle<bAngle){
            bAngle=mAngle;
            cAngle=true;
        }
        //if(nPoint.x()>0)bAngle=180-bAngle;
        //qDebug()<<"actual angle"<<bAngle;
        auto matrix=calcRotationMatrix(nPoint,bAngle);
        //qDebug()<<matrix;
        //float distance = nPoint.distanceToPoint(QVector3D());
        innerPoints=applyMatrix(innerPoints, matrix);
        outerPoints=applyMatrix(outerPoints, matrix);
        allPoints=applyMatrix(allPoints, matrix);
        //outerPoints.push_back(QVector3D());
    }else{
        //qDebug()<<"Translating";
        QMatrix4x4 translate;
        translate.setToIdentity();
        translate.translate(-distance, 0.0f, 0.0f);
       // qDebug()<<"translating by"<<distance;
        //if(outerPoints.last()==QVector3D()&&outerPoints.length()>1)outerPoints.pop_back();
        innerPoints=applyMatrix(innerPoints, translate);
        outerPoints=applyMatrix(outerPoints, translate);
        allPoints=applyMatrix(allPoints, translate);
    }

    outerPoints.push_back(QVector3D());
    //qDebug()<<"pushed last point";

   // qDebug()<<"first inner point"<<innerPoints[0];
    if(outerPoints.last()!=QVector3D())outerPoints.push_back(QVector3D());
    //outerPoints.push_back(innerPoints[0]);
    if(qAbs(innerPoints.front().x())<0.01f &&qAbs(innerPoints.front().y())<0.01f&&qAbs(innerPoints.front().z())<0.01f&&innerPoints.length()>0){

        //qDebug()<<"popping"<<innerPoints.front();
        innerPoints.pop_front();
        outerPoints.push_back(QVector3D());
    }
    //qDebug()<<"outer points"<<outerPoints;
    //qDebug()<<"innerPoints"<<innerPoints;

    return outerPoints;
}



bool wire::nextPoint3()
{

    int state=checkState();
    //if()

    if(state==done){
        auto bbCheck=checkBoundingBoxPlanes();
        if(!bbCheck){
                success=true;
                return bbCheck;
    }
    }
    //if(outerPoints.length()==0)outerPoints.push_back(QVector3D());
    if(state==rotate)return rotationState();
    if(state==translate)return translationState();
}







QVector<QVector3D> wire::applyMatrix(QVector<QVector3D> &path, QMatrix4x4 matrix)
{
    QVector<QVector3D> temppath;
    for(auto point: path){
        temppath.push_back(matrix*point);
    }
    return temppath;
}

QMatrix4x4 wire::calcRotationMatrix(QVector3D point, float angle)
{
    QVector3D cross=QVector3D::crossProduct(point, QVector3D(1.0f, 0.0f,0.0f));
    //qDebug()<<"rotation axis: "<<cross;
    QQuaternion quat=QQuaternion::fromAxisAndAngle(cross, angle);
    QMatrix3x3 rot3= quat.toRotationMatrix();
    QMatrix4x4 result;
    for(int i=0;i<3;i++){
        for(int j=0; j<3; j++){
            if(i<3 && j<3)result(i,j)=rot3(i,j);
        }
    }
    return result;

}

float wire::calc_head_angle(QVector3D inPoint)
{
   auto temp= inPoint.normalized();
   float t_angle=qAcos(temp.z());
   if(temp.y()<0)t_angle*=-1;
   //qDebug()<<inPoint;
   //qDebug()<<qRadiansToDegrees(t_angle);
   return qRadiansToDegrees(t_angle);
}

float wire::calc_bend_angle(QVector3D inPoint)
{
    auto temp=inPoint.normalized();
    float t_angle=qAcos(temp.x());
    return qRadiansToDegrees(t_angle);
}

void wire::addBoundingBox(QVector3D a, QVector3D b)
{
 boundingBox.push_back(qMakePair(a,b));
}

void wire::addBoundingBox(QVector<QPair<QVector3D, QVector3D>> bbs)
{
    boundingBox.append(bbs);
}

void wire::createBBDrawable()
{
    for(auto bb: boundingBox){
        PolygonalDrawable *box= new PolygonalDrawable();
    }
}

int wire::checkState()
{
    if(innerPoints.length()==0){
        return done;
    }
    auto nextPoint=innerPoints.first();
    if(qAbs(nextPoint.y())>0.01f||qAbs(nextPoint.z())>0.01f){
        return rotate;
    }
    else{
        return translate;
    }
}

bool wire::translationState()
{
    auto nextPoint=innerPoints.first();
   /* if(qFuzzyCompare(nextPoint),QVector3D){
        innerPoints.pop_front();
    }*/
    float distance=nextPoint.x();
    qDebug()<<distance;
    qDebug()<<nextPoint;
    QMatrix4x4 transMat;
    transMat.translate(-distance,0.0f, 0.0f);
    innerPoints=applyMatrix(innerPoints, transMat);
    outerPoints=applyMatrix(outerPoints, transMat);
    outerPoints.push_back(QVector3D());
    innerPoints.pop_front();

    //checkBoundingBoxPlanes();
    return checkBoundingBoxPlanes();
}

bool wire::rotationState()
{
    auto nextPoint=innerPoints.first();
    float bAngle=calc_bend_angle(nextPoint);
    auto rotMat=calcRotationMatrix(nextPoint,bAngle);
    innerPoints=applyMatrix(innerPoints, rotMat);
    outerPoints=applyMatrix(outerPoints, rotMat);
    //checkBoundingBoxPlanes();
    return checkBoundingBoxPlanes();

}



bool wire::checkBoundingBoxPlanes()
{
    intersPoints.clear();
    bool check=false;
    for(int i=0; i<outerPoints.length()-1; i++ ){
        auto aVec=outerPoints[i];
        auto bVec=outerPoints[i+1];
        QVector3D resultPoint;
        for(auto bb: boundingBox){
            auto xAPlane=QVector3D(bb.first.x(),0.0f, 0.0f);
            auto xBPlane=QVector3D(bb.second.x(),0.0f, 0.0f);
            auto yAPlane=QVector3D(0.0f, bb.first.y(), 0.0f);
            auto yBPlane=QVector3D(0.0f, bb.second.y(), 0.0f);
            auto zAPlane=QVector3D(0.0f, 0.0f, bb.first.z());
            auto zBPlane=QVector3D(0.0f, 0.0f, bb.second.z());


            if(planeIntersect(aVec, bVec, xAPlane, resultPoint)){

                    if(resultPoint.y() <qMax(bb.first.y(),bb.second.y()) && resultPoint.y()>qMin(bb.first.y(),bb.second.y())){
                        if(resultPoint.z()< qMax(bb.first.z(),bb.second.z()) && resultPoint.z()>qMin(bb.first.z(),bb.second.z())){
                            check=true;
                            intersPoints.push_back(resultPoint);
                        }
                    }


            }


            if(planeIntersect(aVec, bVec, xBPlane, resultPoint)){

                    if(resultPoint.y() <qMax(bb.first.y(),bb.second.y()) && resultPoint.y()>qMin(bb.first.y(),bb.second.y())){
                        if(resultPoint.z()< qMax(bb.first.z(),bb.second.z()) && resultPoint.z()>qMin(bb.first.z(),bb.second.z())){
                            check=true;
                            intersPoints.push_back(resultPoint);
                        }
                    }

            }


            if(planeIntersect(aVec, bVec, yAPlane, resultPoint)){
                if(resultPoint.x()<qMax(bb.first.x(),bb.second.x()) && resultPoint.x()>qMin(bb.first.x(),bb.second.x())){

                        if(resultPoint.z()< qMax(bb.first.z(),bb.second.z()) && resultPoint.z()>qMin(bb.first.z(),bb.second.z())){
                            check=true;
                            intersPoints.push_back(resultPoint);
                        }

                }

            }
            if(planeIntersect(aVec, bVec, yBPlane, resultPoint)){
                if(resultPoint.x()<qMax(bb.first.x(),bb.second.x()) && resultPoint.x()>qMin(bb.first.x(),bb.second.x())){

                        if(resultPoint.z()< qMax(bb.first.z(),bb.second.z()) && resultPoint.z()>qMin(bb.first.z(),bb.second.z())){
                            check=true;
                            intersPoints.push_back(resultPoint);
                        }

                }
            }
            if(planeIntersect(aVec, bVec, zAPlane, resultPoint)){
                if(resultPoint.x()<qMax(bb.first.x(),bb.second.x()) && resultPoint.x()>qMin(bb.first.x(),bb.second.x())){
                    if(resultPoint.y() <qMax(bb.first.y(),bb.second.y()) && resultPoint.y()>qMin(bb.first.y(),bb.second.y())){

                            check=true;
                            intersPoints.push_back(resultPoint);

                    }
                }
            }
            if(planeIntersect(aVec, bVec, zBPlane, resultPoint)){
                if(resultPoint.x()<qMax(bb.first.x(),bb.second.x()) && resultPoint.x()>qMin(bb.first.x(),bb.second.x())){
                    if(resultPoint.y() <qMax(bb.first.y(),bb.second.y()) && resultPoint.y()>qMin(bb.first.y(),bb.second.y())){

                            check=true;
                            intersPoints.push_back(resultPoint);

                    }
                }
            }
        }
    }

    return check;
}

bool wire::planeIntersect(QVector3D aVec, QVector3D bVec, QVector3D plane, QVector3D &resultPoint)
{

   QVector3D resPoint;
   QVector3D diff=bVec-aVec;
   // plane in z-Y
   if(plane.x()!=0){

       if(qMin(aVec.x(),bVec.x())<plane.x()&&qMax(aVec.x(),bVec.x())>plane.x()){
            float dist=plane.x()-aVec.x();
            float div=diff.x()/dist;
            resultPoint=(diff/div)+aVec;
            return true;
       }
       else return false;
   }

   //plane in z-x
   if(plane.y()!=0){

       if(qMin(aVec.y(),bVec.y())<plane.y()&&qMax(aVec.y(),bVec.y())>plane.y()){
            float dist=plane.y()-aVec.y();
            float div=diff.y()/dist;
            resultPoint=(diff/div)+aVec;
            return true;
       }
       else return false;
   }

   //plane in y-x
   if(plane.z()!=0){

       if(qMin(aVec.z(),bVec.z())<plane.z()&&qMax(aVec.z(),bVec.z())>plane.z()){
            float dist=plane.z()-aVec.z();
            float div=diff.z()/dist;
            resultPoint=(diff/div)+aVec;
            return true;
       }
       else return false;
   }
}


