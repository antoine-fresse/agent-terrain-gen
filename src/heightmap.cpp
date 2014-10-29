#include "heightmap.h"

#include <stdexcept>
#include <iostream>

#include <cmath>
#include <ctime>

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "gamewidget.h"

static const char* vertexShaderSource =
    "#version 330\n"
    "in highp float posAttr;\n"
    "out float ex_height;\n"
    "out vec2 ex_textCoord;\n"
    "uniform highp int nbVertex;\n"
    "uniform highp float pointPerTexture;\n"
    "uniform highp float maxHeight;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   ex_height = posAttr / maxHeight;\n"
    "   ex_textCoord = vec2(mod(gl_VertexID, nbVertex), floor(gl_VertexID / nbVertex)) / pointPerTexture;\n"
    "   gl_Position = matrix * vec4(mod(gl_VertexID, nbVertex), posAttr, floor(gl_VertexID / nbVertex), 1.0);\n"
    "}\n";

static const char* fragmentShaderSource =
    "#version 330\n"
    "uniform sampler2D waterSampler;\n"
    "uniform sampler2D sandSampler;\n"
    "uniform sampler2D grassSampler;\n"
    "uniform sampler2D rockSampler;\n"
    "uniform sampler2D snowSampler;\n"
    "float sandLimit = 0.05;\n"
    "float grassLimit = 0.4;\n"
    "float rockLimit = 0.7;\n"
    "float snowLimit = 1.0;\n"
    "in vec2 ex_textCoord;\n"
    "in float ex_height;\n"
    "out vec4 out_color;\n"
    "void main() {\n"
    "    if (ex_height < sandLimit) {\n"
    "        out_color = texture2D(waterSampler, ex_textCoord.xy);\n"
    "    } else {\n"
    "        vec4 sand = texture2D(sandSampler, ex_textCoord.xy);\n"
    "        vec4 grass = texture2D(grassSampler, ex_textCoord.xy);\n"
    "        vec4 rock = texture2D(rockSampler, ex_textCoord.xy);\n"
    "        vec4 snow = texture2D(snowSampler, ex_textCoord.xy);\n"
    "        vec4 weights;"
    "        weights.x = clamp(1.0f - abs(ex_height - sandLimit) / 0.2f, 0.0, 1.0);\n"
    "        weights.y = clamp(1.0f - abs(ex_height - grassLimit) / 0.2f, 0.0, 1.0);\n"
    "        weights.z = clamp(1.0f - abs(ex_height - rockLimit) / 0.2f, 0.0, 1.0);\n"
    "        weights.w = clamp(1.0f - abs(ex_height - snowLimit) / 0.2f, 0.0, 1.0);\n"
    "        weights = weights / (weights.x + weights.y + weights.z + weights.w);\n"
    "        out_color = sand * weights.x + grass * weights.y + rock * weights.z + snow * weights.w;\n"
    "    }"
    "}\n";

HeightMap::HeightMap(int size)
    : m_program(nullptr)
    , m_vao{nullptr}
    , m_waterTexture{nullptr}
    , m_sandTexture{nullptr}
    , m_grassTexture{nullptr}
    , m_rockTexture{nullptr}
    , m_snowTexture{nullptr}
    , m_nbPoints(size)
    , m_rotation{0.0f}
    , m_scale{10.0f}
    , m_isDirty{false}
{
    m_vertices = new GLfloat[m_nbPoints * m_nbPoints];
    m_indices = new GLuint[3 * 2 * (m_nbPoints - 1) * (m_nbPoints - 1)];

    // Remplissage initiale de la heightmap
    int nbVertices = 0;
    for (int z = 0; z < m_nbPoints; ++z) {
        for (int x = 0; x < m_nbPoints; ++x) {
            m_vertices[nbVertices++] = 0;
        }
    }

    // Remplissage du tableau d'indices
    int nbIndice = 0;
    for (int z = 0; z < (m_nbPoints - 1); ++z) {
        for (int x = 0; x < (m_nbPoints - 1); ++x) {
            // Premier triangle
            m_indices[nbIndice++] = z * m_nbPoints + x;
            m_indices[nbIndice++] = (z + 1) * m_nbPoints + x;
            m_indices[nbIndice++] = z * m_nbPoints + (x + 1);

            // Deuxième triangle
            m_indices[nbIndice++] = (z + 1) * m_nbPoints + x;
            m_indices[nbIndice++] = (z + 1) * m_nbPoints + (x + 1);
            m_indices[nbIndice++] = z * m_nbPoints + (x + 1);
        }
    }
}

HeightMap::~HeightMap()
{
    delete m_program;
    delete m_vao;

    delete[] m_vertices;
    delete[] m_indices;

    delete m_waterTexture;
    delete m_sandTexture;
    delete m_grassTexture;
    delete m_rockTexture;
    delete m_snowTexture;
}

void HeightMap::initialize(GameWidget* gl)
{
    m_waterTexture = new QOpenGLTexture(QImage(":/textures/water.png"));
    m_waterTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_sandTexture = new QOpenGLTexture(QImage(":/textures/sand.png"));
    m_sandTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_grassTexture = new QOpenGLTexture(QImage(":/textures/grass.png"));
    m_grassTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_rockTexture = new QOpenGLTexture(QImage(":/textures/rock.png"));
    m_rockTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_snowTexture = new QOpenGLTexture(QImage(":/textures/snow.png"));
    m_snowTexture->setWrapMode(QOpenGLTexture::Repeat);

    m_program = new QOpenGLShaderProgram(gl);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
    m_program->bind();
    m_program->setUniformValue("nbVertex", getSize());
    m_program->setUniformValue("pointPerTexture", 3.0f);
    m_program->setUniformValue("maxHeight", (float)50); // TODO
    m_program->setUniformValue("waterSampler", 0);
    m_program->setUniformValue("sandSampler", 1);
    m_program->setUniformValue("grassSampler", 2);
    m_program->setUniformValue("rockSampler", 3);
    m_program->setUniformValue("snowSampler", 4);
    m_program->release();

    gl->glGenBuffers(1, &m_vertexBuffer);
    gl->glGenBuffers(1, &m_indexBuffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    gl->glBufferData(GL_ARRAY_BUFFER, m_nbPoints * m_nbPoints * sizeof(GLfloat), m_vertices, GL_DYNAMIC_DRAW);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * (m_nbPoints - 1) * (m_nbPoints - 1) * sizeof(GLuint), m_indices, GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_vao = new QOpenGLVertexArrayObject(gl);
    m_vao->create();
    m_vao->bind();
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    gl->glEnableVertexAttribArray(m_posAttr);
    gl->glVertexAttribPointer(m_posAttr, 1, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    m_vao->release();
}

void HeightMap::render(GameWidget* gl, const QMatrix4x4& viewProj)
{
    if (m_isDirty) {
        update(gl);
    }
    m_program->bind();
    m_program->setUniformValue(m_matrixUniform, viewProj * getTranform());

    m_vao->bind();

    m_waterTexture->bind(0);
    m_sandTexture->bind(1);
    m_grassTexture->bind(2);
    m_rockTexture->bind(3);
    m_snowTexture->bind(4);

    gl->glDrawElements(GL_TRIANGLES, (m_nbPoints - 1)*(m_nbPoints - 1)*6, GL_UNSIGNED_INT, 0);

    m_waterTexture->release();
    m_sandTexture->release();
    m_grassTexture->release();
    m_rockTexture->release();
    m_snowTexture->release();
    m_vao->release();
    m_program->release();

}

void HeightMap::update(GameWidget* gl)
{
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    void* buffer = gl->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    memcpy(buffer, m_vertices, m_nbPoints * m_nbPoints * sizeof(GLfloat));
    gl->glUnmapBuffer(GL_ARRAY_BUFFER);
}

void HeightMap::destroy(GameWidget* gl) {

    gl->glDeleteBuffers(1, &m_vertexBuffer);
    gl->glDeleteBuffers(1, &m_indexBuffer);
}

void HeightMap::set(int x, int z, float height)
{
    if ((x >= m_nbPoints) || (z >= m_nbPoints)) {
        throw std::runtime_error("HeightMap::set : Erreur de coordonées");
    }
    m_vertices[(z * m_nbPoints + x)] = height;
    m_isDirty = true;
}

float HeightMap::get(int x, int z)
{
    if ((x >= m_nbPoints) || (z >= m_nbPoints) || (x < 0) || (z < 0)) {
        throw std::runtime_error("HeightMap::get : Erreur de coordonées");
    }
    return m_vertices[(z * m_nbPoints + x)];
}

float HeightMap::getHeight(QVector3D pos)
{
    QVector3D newPos = getTranform().inverted(nullptr) * pos;

    pos.setX(std::min(std::max(newPos.x(), 0.0f), (float)m_nbPoints - 1.0f));
    pos.setZ(std::min(std::max(newPos.z(), 0.0f), (float)m_nbPoints - 1.0f));

    int z = pos.z();
    int x = pos.x();

    return m_vertices[(z * m_nbPoints + x)] * m_scale;
}

int HeightMap::getSize() const
{
    return m_nbPoints;
}

float HeightMap::getRotation() const
{
    return m_rotation;
}

void HeightMap::setRotation(float rotation)
{
    m_rotation = rotation;
}

QMatrix4x4 HeightMap::getTranform()
{
    QMatrix4x4 rotationMat;
    rotationMat.rotate(m_rotation, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 translationMat;
    translationMat.translate(-(float)getSize() / 2.0, 0.0, -(float)getSize() / 2.0);

    QMatrix4x4 scaleMatrix;
    scaleMatrix.scale(m_scale);
    return rotationMat * translationMat * scaleMatrix;
}

void HeightMap::reset()
{
    memset(m_vertices, 0, m_nbPoints * m_nbPoints * sizeof(GLfloat));
    m_isDirty = true;
}
