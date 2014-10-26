#include "camera.h"

#include <cmath>
#include <QKeyEvent>

static const float pi = 4 * std::atan(1);

float degToRad(float x) {
    return x * pi / 180.0f;
}

float radToDeg(float x) {
    return x * 180.0f / pi;
}

Camera::Camera() : m_speed{60.0f}, m_phi{degToRad(-33.0f)}, m_theta{degToRad(-10.0f)},
                    m_thetaMax{degToRad(75.0f)}, m_sensi{0.5f}, m_position{0.0f, 0.0f, -2.0f},
                    m_direction{Direction::NONE}, m_mousePressed{false}, m_isDirty{true} {
}

void Camera::update(int dt) {
    // On translate la caméra
    float mul = (dt / 1000.0f) * m_speed;
    QVector3D move = getDirection();
    move = move * mul;
    move = move + m_position;

    setPosition(move);
}

void Camera::setPosition(const QVector3D& v) {
    m_lastPosition = m_position;
    m_position = v;

    m_isDirty = true;
}

QVector3D Camera::getLastPosition() const {
    return m_lastPosition;
}

QVector3D Camera::getPosition() const {
    return m_position;
}

QVector3D Camera::getDirection() {
    QVector3D frontdir = frontDir();
    QVector3D rightdir = QVector3D::crossProduct(frontDir(), QVector3D{0.0f, 1.0f, 0.0f});

    QVector3D d;

    switch (m_direction) {
    case Direction::NONE:
        return {};
        break;
    case Direction::UP:
        return frontdir;
        break;
    case Direction::DOWN:
        return frontdir * -1;
        break;
    case Direction::LEFT:
        return rightdir * -1;
        break;
    case Direction::RIGHT:
        return rightdir;
        break;
    case Direction::UP_RIGHT:
        d = (frontdir + rightdir);
        d.normalize();
        return d;
        break;
    case Direction::UP_LEFT:
        d = (frontdir + rightdir * -1);
        d.normalize();
        return d;
        break;
    case Direction::DOWN_RIGHT:
        d = (frontdir * -1 + rightdir);
        d.normalize();
        return d;
        break;
    case Direction::DOWN_LEFT:
        d = (frontdir + rightdir) * -1;
        d.normalize();
        return d;
        break;
    }
    return {};
}

const QMatrix4x4& Camera::getViewMatrix() {
    // Si la caméra a été modifiée on re-calcule la matrice
    if (m_isDirty) {
        QVector3D front = frontDir();
        front.normalize();
        QVector3D to = m_position + front;

        m_viewMatrix.setToIdentity();
        m_viewMatrix.lookAt(m_position, to, QVector3D(0.0, 1.0, 0.0));
        m_isDirty = false;
    }
    return m_viewMatrix;
}

void Camera::keyPressEvent(QKeyEvent* event) {
    Direction mod = Direction::NONE;
    if (event->key() == Qt::Key_Z) {
        mod = Direction::UP;
    } else if (event->key() == Qt::Key_S) {
        mod = Direction::DOWN;
    } else if (event->key() == Qt::Key_Q) {
        mod = Direction::LEFT;
    } else if (event->key() == Qt::Key_D) {
        mod = Direction::RIGHT;
    } else {
        mod = Direction::NONE;
    }
    m_direction = (Direction)((int)m_direction | (int)mod);
}

void Camera::keyReleaseEvent(QKeyEvent* event) {
    Direction mod = Direction::NONE;
    if (event->key() == Qt::Key_Z) {
        mod = Direction::UP;
    } else if (event->key() == Qt::Key_S) {
        mod = Direction::DOWN;
    } else if (event->key() == Qt::Key_Q) {
        mod = Direction::LEFT;
    } else if (event->key() == Qt::Key_D) {
        mod = Direction::RIGHT;
    } else {
        mod = Direction::NONE;
    }

    m_direction = (Direction)((int)m_direction & (~(int)mod));
}

void Camera::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::RightButton) {
        m_mousePressed = true;
        m_oldMousPos = event->pos();
    }
}

void Camera::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::RightButton) {
        m_mousePressed = false;
    }
}

void Camera::mouseMoveEvent(QMouseEvent* event) {
    if (m_mousePressed) {
        float dx = event->x() - m_oldMousPos.x();
        float dy = event->y() - m_oldMousPos.y();
        m_oldMousPos = event->pos();
        m_phi -= degToRad(dx * m_sensi);
        m_theta -= degToRad(dy * m_sensi);

        /*
         * On évite à phi de devenir trop grand (ou trop petit)
         *   en enlevant (ou ajoutant) 1 tour à chaque tour
         */
        if (m_phi > degToRad(360.0f)) {
            m_phi -= degToRad(360.0f);
        } else if (m_phi < 0.0f) {
            m_phi += degToRad(360.0f);
        }
        // On évite que theta dépasse la limite
        if (m_theta > m_thetaMax) {
            m_theta = m_thetaMax;
        } else if (m_theta < -m_thetaMax) {
            m_theta = -m_thetaMax;
        }

        m_isDirty = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// FONCTIONS PRIVEES
////////////////////////////////////////////////////////////////////////////////

QVector3D Camera::frontDir() {
    QVector3D v{cos(m_theta) * cos(m_phi),
                sin(m_theta),
                -cos(m_theta) * sin(m_phi)};
    return v;
}
