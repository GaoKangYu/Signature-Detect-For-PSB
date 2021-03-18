#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yolo_v2_class.hpp>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int flag;
    QFileInfo IMGPath;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OpenImg();
    void StartDetect();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    Ui::MainWindow *ui;
    cv::Mat Image;
    cv::Mat Raw_Image;
    QPoint last;
};
#endif // MAINWINDOW_H
