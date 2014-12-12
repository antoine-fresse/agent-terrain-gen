#include "heightmap.h"

#include <stdexcept>
#include <iostream>

#include <cmath>
#include <ctime>
#include <QFile>

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "gamewidget.h"

#include <omp.h>


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
    , m_isMaterialDirty{false}
{
    m_vertices = new GLfloat[m_nbPoints * m_nbPoints];
    m_materials = new GLint[m_nbPoints * m_nbPoints];
    m_normals = new GLfloat[3 * m_nbPoints * m_nbPoints];
    m_indices = new GLuint[3 * 2 * (m_nbPoints - 1) * (m_nbPoints - 1)];
    m_maxHeight = 100.0;
    // Remplissage initiale de la heightmap
    int nbVertices = 0;
    for (int z = 0; z < m_nbPoints; ++z) {
        for (int x = 0; x < m_nbPoints; ++x) {
            m_vertices[nbVertices++] = 0.0f;
        }
    }
    // Remplissage des matériaux
    for (int i = 0; i < m_nbPoints * m_nbPoints; ++i) {
        m_materials[i] = HeightMap::Grass;
    }
    // Remplissage des normales
    #pragma omp parallel
    {
        #pragma omp for
        for (int y = 0; y < m_nbPoints; ++y) {
            for (int x = 0; x < m_nbPoints; ++x) {
                /*float sx = get(x < m_nbPoints - 1 ? x + 1 : x, y) - get(x > 0 ? x - 1 : x, y);
                if ((x == 0) || (x == m_nbPoints - 1)) {
                    sx *= 2.0f;
                }
                float sy = get(x, y < m_nbPoints - 1 ? y + 1 : y) - get(x, y > 0 ? y - 1 : y);
                if ((y == 0) || (y == m_nbPoints - 1)) {
                    sy *= 2.0f;
                }
                QVector3D normal(-sx, 2.0f * m_scale, sy);
                normal.normalize();

                m_normals[3 * (y * m_nbPoints + x) + 0] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 1] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 2] = 1.0f;
                */
                updateNormal(x,y);
            }
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
    m_isDirty = true;
    m_isMaterialDirty = true;
}

HeightMap::~HeightMap()
{
    delete m_program;
    delete m_vao;

    delete[] m_vertices;
    delete[] m_materials;
    delete[] m_normals;
    delete[] m_indices;

    delete m_waterTexture;
    delete m_sandTexture;
    delete m_grassTexture;
    delete m_rockTexture;
    delete m_snowTexture;
}

void HeightMap::initialize(GameWidget* gl)
{
    m_waterTexture = new QOpenGLTexture(QImage(":/textures/water.jpg"));
    m_waterTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_sandTexture = new QOpenGLTexture(QImage(":/textures/sand.png"));
    m_sandTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_grassTexture = new QOpenGLTexture(QImage(":/textures/grass.jpg"));
    m_grassTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_rockTexture = new QOpenGLTexture(QImage(":/textures/rock.png"));
    m_rockTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_snowTexture = new QOpenGLTexture(QImage(":/textures/snow.jpg"));
    m_snowTexture->setWrapMode(QOpenGLTexture::Repeat);

    m_program = new QOpenGLShaderProgram(gl);
    //m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    //m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");


    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_normalAttr = m_program->attributeLocation("normalAttr");
    m_materialAttr = m_program->attributeLocation("materialAttr");

    m_viewMatrixUniform = m_program->uniformLocation("view_matrix");
    m_modelMatrixUniform = m_program->uniformLocation("model_matrix");
    m_projMatrixUniform = m_program->uniformLocation("proj_matrix");


    m_program->bind();
    m_program->setUniformValue("nbVertex", getSize());
    m_program->setUniformValue("pointPerTexture", 4.0f);
    m_program->setUniformValue("maxHeight", m_maxHeight); // TODO
    m_program->setUniformValue("waterSampler", 0);
    m_program->setUniformValue("sandSampler", 1);
    m_program->setUniformValue("grassSampler", 2);
    m_program->setUniformValue("rockSampler", 3);
    m_program->setUniformValue("snowSampler", 4);
    m_program->release();

    gl->glGenBuffers(1, &m_vertexBuffer);
    gl->glGenBuffers(1, &m_materialBuffer);
    gl->glGenBuffers(1, &m_normalBuffer);
    gl->glGenBuffers(1, &m_indexBuffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    gl->glBufferData(GL_ARRAY_BUFFER, m_nbPoints * m_nbPoints * sizeof(GLfloat), m_vertices, GL_DYNAMIC_DRAW);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_materialBuffer);
    gl->glBufferData(GL_ARRAY_BUFFER, m_nbPoints * m_nbPoints * sizeof(GLint), m_materials, GL_DYNAMIC_DRAW);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    gl->glBufferData(GL_ARRAY_BUFFER, 3 * m_nbPoints * m_nbPoints * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
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

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_materialBuffer);
    gl->glEnableVertexAttribArray(m_materialAttr);
    gl->glVertexAttribIPointer(m_materialAttr, 1, GL_INT, 0, 0);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    gl->glEnableVertexAttribArray(m_normalAttr);
    gl->glVertexAttribPointer(m_normalAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    m_vao->release();
}

void HeightMap::render(GameWidget* gl, const QMatrix4x4& view, const QMatrix4x4& proj)
{
    if (m_isDirty) {
        update(gl, m_isMaterialDirty);
    }
    m_program->bind();
    //m_program->setUniformValue(m_matrixUniform, viewProj * getTranform());

    m_program->setUniformValue(m_viewMatrixUniform, view);
    m_program->setUniformValue(m_modelMatrixUniform, getTransform());
    m_program->setUniformValue(m_projMatrixUniform, proj);

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

void HeightMap::update(GameWidget* gl, bool updateMaterial)
{
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    void* buffer = gl->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(buffer, m_vertices, m_nbPoints * m_nbPoints * sizeof(GLfloat));
    gl->glUnmapBuffer(GL_ARRAY_BUFFER);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    float* normalBuffer = (float*)gl->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(normalBuffer, m_normals, 3 * m_nbPoints * m_nbPoints * sizeof(GLfloat));
    gl->glUnmapBuffer(GL_ARRAY_BUFFER);

    if (updateMaterial) {
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_materialBuffer);
        GLint* materialBuffer = (GLint*)gl->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(materialBuffer, m_materials, m_nbPoints * m_nbPoints * sizeof(GLint));
        gl->glUnmapBuffer(GL_ARRAY_BUFFER);

    }
    m_isDirty = false;
    m_isMaterialDirty = false;
}

void HeightMap::destroy(GameWidget* gl) {

    gl->glDeleteBuffers(1, &m_vertexBuffer);
    gl->glDeleteBuffers(1, &m_materialBuffer);
    gl->glDeleteBuffers(1, &m_normalBuffer);
    gl->glDeleteBuffers(1, &m_indexBuffer);
}

void HeightMap::set(int x, int z, float height)
{
    if ((x >= m_nbPoints) || (z >= m_nbPoints)) {
        throw std::runtime_error("HeightMap::set : Erreur de coordonées");
    }

    if(height > m_maxHeight)
        m_maxHeight = height;

    m_vertices[(z * m_nbPoints + x)] = height;

    if(m_computeNormals){
    // On recalcule les normales
        #pragma omp parallel
        {
            //#pragma omp for
            for (int dx = -1; dx < 2; ++dx) {
                if (((x + dx) < 0) || ((x + dx) >= m_nbPoints)) {
                    continue;
                }
                for (int dz = -1; dz < 2; ++dz) {
                    if (((z + dz) < 0) || ((z + dz) >= m_nbPoints)) {
                        continue;
                    }
                    updateNormal(x, z);
                }
            }
        }
    }

    m_isDirty = true;
}

void HeightMap::setMaterial(int x, int z, Material mat)
{
    if ((x >= m_nbPoints) || (z >= m_nbPoints) || x<0 || z<0) {
        throw std::runtime_error("HeightMap::setMaterial : Erreur de coordonées");
    }
    m_materials[(z * m_nbPoints + x)] = mat;

    m_isMaterialDirty = true;
}

HeightMap::Material HeightMap::getMaterial(int x, int z) {
    if ((x >= m_nbPoints) || (z >= m_nbPoints) || x<0 || z<0) {
        throw std::runtime_error("HeightMap::getMaterial : Erreur de coordonées");
    }
    if (m_materials[(z * m_nbPoints + x)] == HeightMap::Water) {
        return HeightMap::Water;
    } else {
        float height = get(x, z);
        if (height < 1.0) {
            return HeightMap::Water;
        } else if (height > 100) {
            return HeightMap::Snow;
        } else {
            return HeightMap::Grass;
        }
    }
}

float HeightMap::get(int x, int z)
{
    if ((x >= m_nbPoints) || (z >= m_nbPoints) || (x < 0) || (z < 0)) {
        throw std::runtime_error("HeightMap::get : Erreur de coordonées");
    }
    return m_vertices[(z * m_nbPoints + x)];
}

void HeightMap::computeNormals(){
    // Remplissage des normales
    #pragma omp parallel
    {
        #pragma omp for
        for (int y = 0; y < m_nbPoints; ++y) {
            for (int x = 0; x < m_nbPoints; ++x) {
                /*float sx = get(x < m_nbPoints - 1 ? x + 1 : x, y) - get(x > 0 ? x - 1 : x, y);
                if ((x == 0) || (x == m_nbPoints - 1)) {
                    sx *= 2.0f;
                }
                float sy = get(x, y < m_nbPoints - 1 ? y + 1 : y) - get(x, y > 0 ? y - 1 : y);
                if ((y == 0) || (y == m_nbPoints - 1)) {
                    sy *= 2.0f;
                }
                QVector3D normal(-sx, 2.0f * m_scale, sy);
                normal.normalize();

                m_normals[3 * (y * m_nbPoints + x) + 0] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 1] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 2] = 1.0f;
                */
                updateNormal(x,y);
            }
        }
    }

    m_isDirty = true;
}

void HeightMap::setComputeNormals(bool b){
    m_computeNormals = b;
}

float HeightMap::getHeight(QVector3D pos)
{
    QVector3D newPos = getTransform().inverted(nullptr) * pos;

    pos.setX(std::min(std::max(newPos.x(), 0.0f), (float)m_nbPoints - 1.0f));
    pos.setZ(std::min(std::max(newPos.z(), 0.0f), (float)m_nbPoints - 1.0f));

    int z = pos.z();
    int x = pos.x();

    return m_vertices[(z * m_nbPoints + x)];
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

QMatrix4x4 HeightMap::getTransform()
{
    QMatrix4x4 rotationMat;
    rotationMat.rotate(m_rotation, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 translationMat;
    translationMat.translate(-(float)getSize() / 2.0, 0.0, -(float)getSize() / 2.0);

    QMatrix4x4 scaleMatrix;
    scaleMatrix.scale(m_scale, 1.0, m_scale);
    return rotationMat * translationMat * scaleMatrix;
}

void HeightMap::reset()
{
    memset(m_vertices, 0, m_nbPoints * m_nbPoints * sizeof(GLfloat));
    m_maxHeight = 100.0;
    #pragma omp parallel
    {
        #pragma omp for
        for (int y = 0; y < m_nbPoints; ++y) {
            for (int x = 0; x < m_nbPoints; ++x) {
                /*float sx = get(x < m_nbPoints - 1 ? x + 1 : x, y) - get(x > 0 ? x - 1 : x, y);
                if ((x == 0) || (x == m_nbPoints - 1)) {
                    sx *= 2.0f;
                }
                float sy = get(x, y < m_nbPoints - 1 ? y + 1 : y) - get(x, y > 0 ? y - 1 : y);
                if ((y == 0) || (y == m_nbPoints - 1)) {
                    sy *= 2.0f;
                }
                QVector3D normal(-sx, 2.0f * m_scale, sy);
                normal.normalize();
                */
                m_normals[3 * (y * m_nbPoints + x) + 0] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 1] = 1.0f;
                m_normals[3 * (y * m_nbPoints + x) + 2] = 1.0f;
                updateNormal(x,y);

            }
        }
        #pragma omp for
        for (int i = 0; i < m_nbPoints * m_nbPoints; ++i) {
            m_materials[i] = HeightMap::Grass;
        }
    }

    m_isDirty = true;
    m_isMaterialDirty = true;
}

void HeightMap::updateNormal(int x, int y)
{
    /*
    float sx = get(x < m_nbPoints - 1 ? x + 1 : x, y) - get(x > 0 ? x - 1 : x, y);
    if ((x == 0) || (x == m_nbPoints - 1)) {
        sx *= 2.0f;
    }
    float sy = get(x, y < m_nbPoints - 1 ? y + 1 : y) - get(x, y > 0 ? y - 1 : y);
    if ((y == 0) || (y == m_nbPoints - 1)) {
        sy *= 2.0f;
    }
    QVector3D normal(-sx, 2.0f * m_scale, sy);
    normal.normalize();

    m_normals[3 * (y * m_nbPoints + x) + 0] = normal.x();
    m_normals[3 * (y * m_nbPoints + x) + 1] = normal.y();
    m_normals[3 * (y * m_nbPoints + x) + 2] = normal.z();

    */


    // Normals of point (x, m_vertices[(y * m_nbPoints + x)], z)

    m_normals[3 * (y * m_nbPoints + x) + 0] = 0.0;
    m_normals[3 * (y * m_nbPoints + x) + 1] = 0.0;
    m_normals[3 * (y * m_nbPoints + x) + 2] = 0.0;

    #define TRI_CNT 6
    int tris[TRI_CNT][3][2] = {
        { {0,0},{-1,0},{-1,-1}  }
        ,{ {0,0},{-1,-1},{0,-1}  }
        ,{ {0,0},{0,-1},{1,0}  }
        ,{ {0,0},{1,0},{1,1}  }
        ,{ {0,0},{1,1},{0,1}  }
        ,{ {0,0},{0,1},{-1,0}  }
    };
    QVector3D sum;
    for(int i=0 ; i<TRI_CNT ;i++){
        int x_ = x+tris[i][0][0];
        int y_ = y+tris[i][0][1];
        if(x_ < 0 || x_ >= m_nbPoints || y_ < 0 || y_ >= m_nbPoints)
            continue;
        QVector3D va(  x_,get(x_,y_), y_);

        x_ = x+tris[i][1][0];
        y_ = y+tris[i][1][1];
        if(x_ < 0 || x_ >= m_nbPoints || y_ < 0 || y_ >= m_nbPoints)
            continue;
        QVector3D vb(  x_,get(x_,y_), y_);

        x_ = x+tris[i][2][0];
        y_ = y+tris[i][2][1];
        if(x_ < 0 || x_ >= m_nbPoints || y_ < 0 || y_ >= m_nbPoints)
            continue;
        QVector3D vc(  x_,get(x_,y_), y_);

        QVector3D v1 = vb - va;
        QVector3D v2 = vc - va;

        QVector3D n = QVector3D::crossProduct(v2, v1);
        n.normalize();

        sum += n;


    }
    sum.normalize();

    m_normals[3 * (y * m_nbPoints + x) + 0] += sum.x();
    m_normals[3 * (y * m_nbPoints + x) + 1] += sum.y();
    m_normals[3 * (y * m_nbPoints + x) + 2] += sum.z();
}

void HeightMap::save(QFile& file)
{
    short* data = new short[m_nbPoints * m_nbPoints];
    for (int i = 0; i < (m_nbPoints * m_nbPoints); ++i) {
        data[i] = (short)m_vertices[i];
    }
    std::cout << "size " << m_nbPoints * m_nbPoints * sizeof(short) << std::endl;

    int toWrite = m_nbPoints * m_nbPoints * sizeof(short);
    int offset = 0;
    while (toWrite > 0) {
        int writed = file.write(((const char*)data) + offset, toWrite);
        if (writed == -1) {
            std::cout << "Error" << std::endl;
        }
        toWrite -= writed;
        offset += writed;
    }

    //file.resize(m_nbPoints * m_nbPoints * sizeof(short));
}


std::pair<int, int> HeightMap::getRandomInlandPosition()
{
    int tries = 1000;
    while (tries>0) {
        int x = rand()%m_nbPoints;
        int y = rand()%m_nbPoints;
        if (getMaterial(x, y) != HeightMap::Water) {
            return std::make_pair<int, int>(std::move(x), std::move(y));
        }
        tries--;
    }
    return std::make_pair<int, int>(m_nbPoints/2, m_nbPoints/2);
}

std::pair<int, int> HeightMap::getRandomInSeaPosition()
{
    int tries = 1000;
    while (tries>0) {
        int x = rand()%m_nbPoints;
        int y = rand()%m_nbPoints;
        if (getMaterial(x, y) == HeightMap::Water) {
            return std::make_pair<int, int>(std::move(x), std::move(y));
        }
        tries--;
    }
    return std::make_pair<int, int>(0, 0);
}

std::pair<int, int> HeightMap::getRandomMountainPosition()
{
    /*int tries = 1000;
    while (tries > 0) {
        int x = rand() % m_nbPoints;
        int y = rand() % m_nbPoints;
        if (getMaterial(x, y) == HeightMap::Snow) {
            return std::make_pair<int, int>(std::move(x), std::move(y));
        }
        tries--;
    }*/
    int index = rand() % (m_nbPoints * m_nbPoints);
    for (int i = index; i < m_nbPoints * m_nbPoints; ++i) {
        if (getMaterial(i % m_nbPoints, i / m_nbPoints) == HeightMap::Snow) {
            return std::make_pair<int, int>(i % m_nbPoints, i / m_nbPoints);
        }
    }
    for (int i = index; i >= 0; --i) {
        if (getMaterial(i % m_nbPoints, i / m_nbPoints) == HeightMap::Snow) {
            return std::make_pair<int, int>(i % m_nbPoints, i / m_nbPoints);
        }
    }
    return std::make_pair<int, int>(0, 0);
}

std::pair<int, int> HeightMap::getRandomPosition()
{
    return std::make_pair<int, int>(rand()%m_nbPoints, rand()%m_nbPoints);
}

void HeightMap::smoothAll()
{
    float *mapCpy = new float[m_nbPoints*m_nbPoints];

    memcpy(mapCpy, m_vertices, m_nbPoints*m_nbPoints*sizeof(float));

    int neighbors = 2;
    for (int y = 0; y < m_nbPoints; y++) {
        for (int x = 0; x < m_nbPoints; x++) {
            int count = 0;
            float height = 0.0;
            for (int i = -neighbors; i <= neighbors; ++i) {
                for (int j = -neighbors; j <= neighbors; ++j) {
                    int newX = x + i;
                    int newY = y + j;
                    if ((newX >= 0) && (newY >= 0) && (newX < m_nbPoints) && (newY < m_nbPoints)) {
                        height += mapCpy[(newY * m_nbPoints + newX)];
                        count++;
                    }
                }
            }
            height += 2 * mapCpy[(y * m_nbPoints + x)];
            height = height / (float)(count + 2);
            if (std::abs(get(x, y) - height) > 15) {
                set(x, y, height);
            }
        }
    }

    delete[] mapCpy;

}
