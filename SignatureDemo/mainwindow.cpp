#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yolo_v2_class.hpp>
#ifdef _WIN32
#define OPENCV
#define GPU
#endif


using namespace cv;

#pragma comment(lib,"F:/3_SignatureDetect/SignatureDemo/SignatureDemo/yolo_cpp_dll.lib")
//#pragma comment(lib, "D:/software/opencv/opencv/build/x64/vc15/lib/opencv_world349d.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(this->width(),this->height());
    connect(ui->LoadIMG, SIGNAL(clicked(bool)), this, SLOT(OpenImg()));
    connect(ui->Detect, SIGNAL(clicked(bool)), this, SLOT(StartDetect()));
}



void MainWindow::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx, y()+dy);
}
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//载入图片按钮槽函数
void MainWindow::OpenImg()
{
    QString OpenIMGFile, OpenIMGPath;
    QImage open_image;
    //打开文件夹中的图片文件
    OpenIMGFile = QFileDialog::getOpenFileName(this,
                                              "选取图片文件",
                                              "",
                                              "Image Files(*.jpg *.png);");
    if( OpenIMGFile != "" )
    {
        if( open_image.load(OpenIMGFile) )
        {
            QPixmap pixmap = QPixmap::fromImage(open_image);
            if(pixmap.height() >= pixmap.width()){
                QPixmap fitpixmap = pixmap.scaledToHeight(600);  // 按比例缩放
                ui->IMG_Loaded->setAlignment(Qt::AlignCenter);
                ui->IMG_Loaded->setPixmap(fitpixmap);
                ui->wider_img_1->hide();
                ui->wider_img_2->hide();
                ui->higher_img_1->hide();
                ui->higher_img_2->hide();
                ui->IMG_Loaded->show();
                flag =1;
            }
            if(pixmap.height() < pixmap.width()){
                QPixmap fitpixmap = pixmap.scaledToWidth(500);  // 按比例缩放
                ui->IMG_Loaded->setAlignment(Qt::AlignCenter);
                ui->IMG_Loaded->setPixmap(fitpixmap);
                ui->higher_img_1->hide();
                ui->higher_img_2->hide();
                ui->wider_img_1->hide();
                ui->wider_img_2->hide();
                ui->IMG_Loaded->show();
                flag =1;
            }
        }
    }

    //显示所示图片的路径
    QFileInfo OpenIMGInfo;
    OpenIMGInfo = QFileInfo(OpenIMGFile);
    OpenIMGPath = OpenIMGInfo.filePath();
    IMGPath = OpenIMGPath;
}


QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

//框选区颜色
static cv::Scalar obj_id_to_color(int obj_id) {
    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
    int const offset = obj_id * 123457 % 6;
    int const color_scale = 150 + (obj_id * 123457) % 100;
    cv::Scalar color(colors[offset][0], colors[offset][1], colors[offset][2]);
    color *= color_scale;
    return color;
}

//生成选框
void draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names,
    int current_det_fps = -1, int current_cap_fps = -1)
{
    for (auto &i : result_vec) {

        cv::Scalar color = CV_RGB(34,34,255);
        if(i.obj_id==1){
                   color =CV_RGB(153,0,196);
        }
        if (obj_names.size() > i.obj_id) {
            std::string obj_name = obj_names[i.obj_id];
            if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
            cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX, 1, 1, 0);
            int const max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
            cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 30, 0)),
                cv::Point2f(std::min((int)i.x + max_width, mat_img.cols-1), std::min((int)i.y, mat_img.rows-1)),
               CV_RGB(216,216,216), CV_FILLED, 8, 0);
            putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 10), cv::FONT_HERSHEY_COMPLEX, 0.8, CV_RGB(9,142,235), 2);
        }
        cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);

    }
}

//读取配置文件
std::vector<std::string> objects_names_from_file(std::string const filename) {
    std::ifstream file(filename);
    std::vector<std::string> file_lines;
    if (!file.is_open()) return file_lines;
    for(std::string line; getline(file, line);) file_lines.push_back(line);
    std::cout << "object names loaded \n";
    return file_lines;
}

//格式转换
static QImage cvMat_to_QImage(const cv::Mat &mat ) {
  switch ( mat.type() )
  {
     // 8-bit, 4 channel
     case CV_8UC4:
     {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
        return image;
     }

     // 8-bit, 3 channel
     case CV_8UC3:
     {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
     }

     // 8-bit, 1 channel
     case CV_8UC1:
     {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
           for ( int i = 0; i < 256; ++i )
              sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
     }

     default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
        break;
  }

  return QImage();
}

//格式转换2
static QPixmap cvMatToQPixmap(const cv::Mat &inMat)
{
    return QPixmap::fromImage(cvMat_to_QImage(inMat));
}

//开始检测槽函数
void MainWindow::StartDetect()
{
    if(flag == 1){
        Image=imread(IMGPath.filePath().toLocal8Bit().toStdString());
        Raw_Image=imread(IMGPath.filePath().toLocal8Bit().toStdString());
        std::string names_file = "F:/3_SignatureDetect/SignatureDemo/build-SignatureDemo-Desktop_Qt_5_14_1_MSVC2017_64bit-Release/release/war.names";
        std::string cfg_file = "F:/3_SignatureDetect/SignatureDemo/build-SignatureDemo-Desktop_Qt_5_14_1_MSVC2017_64bit-Release/release/yolov3-war.cfg";
        std::string weights_file = "F:/3_SignatureDetect/SignatureDemo/build-SignatureDemo-Desktop_Qt_5_14_1_MSVC2017_64bit-Release/release/yolov3-war_4900.weights";
        Detector detector(cfg_file,weights_file,0);
        std::vector<std::string> obj_names = objects_names_from_file(names_file); //调用获得分类对象名称
        std::vector<bbox_t> result_img = detector.detect(Image);
        draw_boxes(Image, result_img, obj_names);
        QPixmap pixmap = QPixmap::fromImage(cvMat_to_QImage(Image));
        QPixmap raw_pixmap = QPixmap::fromImage(cvMat_to_QImage(Raw_Image));
        if(pixmap.height() >= pixmap.width()){
            ui->IMG_Loaded->hide();
            QPixmap raw_fitpixmap = raw_pixmap.scaledToHeight(600);  // 按比例缩放
            ui->higher_img_1->setAlignment(Qt::AlignCenter);
            ui->higher_img_1->setPixmap(raw_fitpixmap);
            ui->higher_img_1->show();
            QPixmap fitpixmap = pixmap.scaledToHeight(600);  // 按比例缩放
            ui->higher_img_2->setAlignment(Qt::AlignCenter);
            ui->higher_img_2->setPixmap(fitpixmap);
            ui->higher_img_2->show();

        }
        if(pixmap.height() < pixmap.width()){
            ui->IMG_Loaded->hide();
            QPixmap raw_fitpixmap = raw_pixmap.scaledToWidth(500);  // 按比例缩放
            ui->wider_img_1->setAlignment(Qt::AlignCenter);
            ui->wider_img_1->setPixmap(raw_fitpixmap);
            ui->wider_img_1->show();
            QPixmap fitpixmap = pixmap.scaledToWidth(500);  // 按比例缩放
            ui->wider_img_2->setAlignment(Qt::AlignCenter);
            ui->wider_img_2->setPixmap(fitpixmap);
            ui->wider_img_2->show();
        }
    }
}


