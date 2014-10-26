#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QMainWindow>

class GameWidget;
class QToolBar;

class GeneratorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GeneratorWindow(QWidget *parent = 0);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    void createView();
    void createLayout();

    GameWidget* m_gameWidget;

signals:

public slots:

};

#endif // GENERATORWINDOW_H
