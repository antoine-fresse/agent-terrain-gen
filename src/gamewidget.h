#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "heightmap.h"
#include "camera.h"

#include <QTimer>
#include <QTime>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QOpenGLFunctions_3_3_Core>

class GameWidget : public QGLWidget, public QOpenGLFunctions_3_3_Core
{
public:
    /**
     * @brief Constructeur de la classe GameWidget.
     * @param frequency La fréquence de rendu.
     */
    GameWidget(int frequency);
    ~GameWidget();

    /**
     * @brief Initialise la fenêtre.
     */
    void initializeGL();
    /**
     * @brief Redessine la fenêtre.
     */
    void paintGL();
    /**
     * @brief Modifie la fréquence de rendu de la fenêtre.
     * @param frequency La nouvelle fréquence
     */
    void setFrequency(int frequency);
    /**
     * @brief Renvoie la fréquence de rendu de la fenêtre.
     */
    int getFrequency();
    /**
     * @brief Renvoie le delta de la frame.
     */
    int getDeltaTime();

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void resizeGL(int w, int h);

    /**
     * @brief Renvoie la camera actuelle de la fenêtre.
     */
    Camera* getCamera() const;
    /**
     * @brief Change la caméra de la fenêtre.
     * Si (camera == nullptr) une nouvelle caméra est créée.
     */
    void setCamera(Camera* camera);
    /**
     * @brief Renvoie la matrice de projection.
     */
    const QMatrix4x4& getProjectionMatrix();

    /**
     * @brief Renvoie le nombre de frames par seconde.
     */
    float getFPS() const;

private:
    HeightMap m_heightmap;
    float m_rotationSpeed;
    bool m_rotate;

    // La caméra actuelle
    Camera* m_camera;
    // La matrice de projection
    QMatrix4x4 m_projMatrix;

    bool m_isProjMatrixDirty;
    bool m_ownCamera;
    bool m_isInitialized;

    int m_frequency;
    int m_lastDelta;
    QTimer m_timer;
    QTime m_deltaTimer;
    int m_lastDeltas[10];
    int m_currentDeltaIndex;
};

#endif // GAMEWIDGET_H
