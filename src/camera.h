#ifndef CAMERA_H
#define CAMERA_H

#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>

/**
 * Enumeration permettant de savoir dans quelle direction avancer.
 */
enum Direction {
    UP = 1,
    DOWN = 2,
    RIGHT = 4,
    LEFT = 8,
    UP_RIGHT = 5,
    UP_LEFT = 9,
    DOWN_RIGHT = 6,
    DOWN_LEFT = 10,
    NONE = 0

};

/**
 * @brief Classe représentant une caméra pouvant naviguer dans un espace en 3D.
 */
class Camera {
public:
    Camera();

    /**
     * @brief Met à jour la caméra.
     * @param dt La durée écoulée depuis la dernière mise à jour.
     */
    void update(int dt);
    /**
     * @brief Modifie la position de la caméra.
     * @param v La nouvelle position de la caméra.
     */
    void setPosition(const QVector3D& v);
    /**
     * Renvoie la position de la camera.
     */
    QVector3D getPosition() const;
    /**
     * Renvoie la position précédante de la camera.
     */
    QVector3D getLastPosition() const;
    /**
     * Renvoie la direction de DEPLACEMENT de la caméra.
     * NOTE : La direction de déplacement peut être différente de la direction
     *         de VUE de la caméra.
     * @return Un vecteur normalisé.
     */
    QVector3D getDirection();
    /**
     * @brief Renvoie la matrice de vue de la caméra.
     */
    const QMatrix4x4& getViewMatrix();

    /*
     * Fonctions gérant les événements.
     */
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);

private:
    /*
     * Renvoie la direction dans laquelle la direction regarde.
     */
    QVector3D frontDir();

    float m_speed; // Vitesse de déplacement de la caméra (Unités / seconde).
    float m_phi;   // Orientation de la vue horizontale par rapport a x+ (en radian).
    float m_theta; // Orientation de la vue verticale par rapport a y+  (en radian).
    float m_thetaMax; // Angle theta maximum par rapport à (OZ) (en radian).
    float m_sensi; // Sensibilité de la souris (Degrés / pixel).

    QVector3D m_position; // La position de la caméra
    QVector3D m_lastPosition;
    Direction m_direction; // La direction de déplacement de la caméra

    bool m_mousePressed; // Le clique droit est-il appuyé ?
    QPoint m_oldMousPos; // La dernière position de la souris

    bool m_isDirty; // La matrice doit elle être recalculée ?
    QMatrix4x4 m_viewMatrix; // La matrice de vue
};
#endif // CAMERA_H
