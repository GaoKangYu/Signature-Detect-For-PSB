# Signature-Detect-For-PSB
“指纹检测”项目
## 背景/需求
-   检出文件中的指纹与印章，建立客户端演示系统展示结果。
-   本项目包含可执行演示系统和模型训练部分配置文件
![ui](https://github.com/GaoKangYu/Signature-Detect-For-PSB/blob/main/demo_ui/ui.png)
## 环境需求
-   Windows(demo) and Linux(model training)
-   Python 3.5+
-   CMake >= 3.12
-   CUDA >= 8.0
-   OpenCV >= 2.4
-   cuDNN >= 7.0
-   on Linux  **GCC or Clang**, on Windows  **MSVC 2017/2019**  
## 文件结构

文件结构及其作用如下：

```
Signature-Detect-For-PSB
├── SignatureDemo(基于QT C++开发的客户端演示系统，系统源码、系统可执行文件存于网盘)
├── VOC2007(数据集，部分来源于网络)
├── darknet(采用YOLO v3作为目标检测模型)
├── demo_ui(演示系统截屏)
├── labeled(甲方提供的示例数据)
├── 2007_test.txt(测试集列表)
├── 2007_train.txt(训练集列表)
├── 2007_val.txt(验证集集列表)
├── voc_label.txt(voc格式转yolo格式脚本)
```
## 功能示意
- 选取并导入图片
![load_img](https://github.com/GaoKangYu/Signature-Detect-For-PSB/blob/main/demo_ui/load_img.png)
- 显示导入的图片
![show_img](https://github.com/GaoKangYu/Signature-Detect-For-PSB/blob/main/demo_ui/show_img.png)
- 检测结果
![result_1](https://github.com/GaoKangYu/Signature-Detect-For-PSB/blob/main/demo_ui/result_1.png)
![result_2](https://github.com/GaoKangYu/Signature-Detect-For-PSB/blob/main/demo_ui/result_2.png)
