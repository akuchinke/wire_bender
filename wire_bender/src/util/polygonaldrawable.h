#pragma once

#include <QMap>
#include <QVector>

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class OpenGLFunctions;
class QOpenGLShaderProgram;


class PolygonalDrawable
{
public:
    PolygonalDrawable();
    virtual ~PolygonalDrawable();

    virtual void draw(OpenGLFunctions & gl);

    const GLenum mode() const;
    void setMode(const GLenum mode);

	const QVector<QVector3D> & vertices() const;
	QVector<QVector3D> & vertices();
	const QVector<QVector3D> & normals() const;
	QVector<QVector3D> & normals();
	const QVector<QVector2D> & texcs() const;
	QVector<QVector2D> & texcs();
	const QVector<quint32> & indices() const;
	QVector<quint32> & indices();

    void retrieveNormals();

    void createVAO(QOpenGLShaderProgram * program);

protected:
	QVector<QVector3D> m_vertices;
	QVector<QVector3D> m_normals;
	QVector<QVector2D> m_texcs;
	QVector<quint32> m_indices;

    GLenum  m_mode;

    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_texcoordBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vao;
};
