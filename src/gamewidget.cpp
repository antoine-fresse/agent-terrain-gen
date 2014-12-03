#include "gamewidget.h"

#include <iostream>
#include <QKeyEvent>
#include <sstream>
#include <iomanip>

GameWidget::GameWidget(int frequency) : m_heightmap(512), m_rotationSpeed{10.0f}, m_rotate{false},
                                        m_isProjMatrixDirty{true}, m_ownCamera{true},
                                        m_isInitialized{false}, m_frequency{frequency}, m_lastDelta{0}
{
    m_camera = new Camera();
    m_timer.setInterval(1000 / frequency);
    m_timer.connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    m_timer.start();
    m_deltaTimer.start();

    memset(m_lastDeltas, 0, 10 * sizeof(int));
    m_currentDeltaIndex = 0;
}

GameWidget::~GameWidget()
{
    m_heightmap.destroy(this);
    if (m_ownCamera) {
        delete m_camera;
    }
}

void GameWidget::initializeGL()
{
    initializeOpenGLFunctions();
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.0f, 0.0f, 0.75f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    m_heightmap.initialize(this);

    m_isInitialized = true;
}

void GameWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_lastDelta = m_deltaTimer.elapsed();
    m_deltaTimer.restart();
    m_lastDeltas[m_currentDeltaIndex++] = m_lastDelta;
    m_currentDeltaIndex = m_currentDeltaIndex % 10;

    m_camera->update(m_lastDelta);

    m_projMatrix = getProjectionMatrix();
    if (m_rotate) {
        m_heightmap.setRotation(m_heightmap.getRotation() + m_rotationSpeed * getDeltaTime() / 1000.0);
    }

    QVector3D pos = getCamera()->getPosition();
    float minHeight = m_heightmap.getHeight(pos);
    if ((minHeight + 5.0) > pos.y()) {
        pos.setY(minHeight + 5.0);
    }
    getCamera()->setPosition(pos);

    m_heightmap.render(this, getCamera()->getViewMatrix(), getProjectionMatrix());
}

void GameWidget::setFrequency(int frequency)
{
    frequency = std::max(frequency, 1);
    m_timer.setInterval(1000 / frequency);
    m_frequency = frequency;
}

int GameWidget::getFrequency()
{
    return m_frequency;
}

int GameWidget::getDeltaTime()
{
    return m_lastDelta;
}

HeightMap* GameWidget::getHeightMap()
{
    return &m_heightmap;
}

Camera* GameWidget::getCamera() const
{
    return m_camera;
}

void GameWidget::setCamera(Camera* camera)
{
    if ((camera == nullptr) && (!m_ownCamera)) {
        m_camera = new Camera();
        m_ownCamera = true;
    } else {
        if (m_ownCamera) {
            delete m_camera;
        }
        m_camera = camera;
        m_ownCamera = false;
    }
}

const QMatrix4x4 &GameWidget::getProjectionMatrix()
{
    if (m_isProjMatrixDirty) {
        m_projMatrix.setToIdentity();
        m_projMatrix.perspective(60.0f, (float)size().width() / (float)size().height(), 1.0f, 10000.0f);
        m_isProjMatrixDirty = false;
    }
    return m_projMatrix;
}

void GameWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        exit(0);
    } else if (event->key() == Qt::Key_P) {
        setFrequency(getFrequency() * 2);
    } else if (event->key() == Qt::Key_M) {
        setFrequency(std::max(getFrequency() / 2, 1));
    } else if (event->key() == Qt::Key_C) {
        m_rotate = !m_rotate;
    } else if (event->key() == Qt::Key_Plus) {
        m_rotationSpeed += 10.0;
    } else if (event->key() == Qt::Key_Minus) {
        m_rotationSpeed -= 10.0;
    }
    m_camera->keyPressEvent(event);
}

void GameWidget::keyReleaseEvent(QKeyEvent* event)
{
    m_camera->keyReleaseEvent(event);
}

void GameWidget::mousePressEvent(QMouseEvent* event)
{
    m_camera->mousePressEvent(event);
    setFocus(Qt::MouseFocusReason);
}

void GameWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_camera->mouseReleaseEvent(event);
}

void GameWidget::mouseMoveEvent(QMouseEvent * event)
{
    m_camera->mouseMoveEvent(event);
}

void GameWidget::resizeGL(int w, int h)
{
    if (m_isInitialized) {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, w * retinaScale, h * retinaScale);
    }
    m_isProjMatrixDirty = true;
}

float GameWidget::getFPS() const
{
    float fps = 0;
    for (int i = 0; i < 10; ++i) {
        fps += m_lastDeltas[i];
    }
    return 1000.0 / (fps / 10.0);
}
