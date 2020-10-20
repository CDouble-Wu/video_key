#include<opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace cv;
//灰度对比
//区域
int  xmin=10, ymin=320, xmax=400, ymax=500;
//帧数相聚像素差
int abs_differ=15;
//差距像素数量占整体图片数量的百分比
float scale = 0.19;
//增益
bool gats = true;
float alpha =1.3;
float beat = 20;
//旋转
bool rotation = true;
int angle1 = 20;
int angle2 = 30;
#define random(a,b) (rand()%(b-a)+a)
//滤波
bool blured = true;
int ksize = 3;
int type=0;
//是否融合
bool blend=true;
void Gats(Mat &src, float alpha, int beat);
void rotate_arbitrarily_angle(Mat &src, Mat &dst, int angle1, int angle2);
void image_blur(Mat &src, Mat &dst, int ksize,int type);
int main()
{
  using namespace std;
  system("color 2F");
  long currentFrame = 1;
  float p;
  VideoCapture cap;
  //这里放置需要提取关键字的视频
  cap.open("1.mp4");
  if (!cap.isOpened())//如果视频不能正常打开则返回
  {
    cout << "cannot open video!" << endl;
    return 0 ;
  }
  Mat frame_key;
  cap >> frame_key;
  if (frame_key.empty())
    cout << "frame_key is empty!" << endl;
  stringstream str;
  Mat frame;
  Mat previousImage, currentImage, resultImage;
  while (1)
  {
    currentFrame++;
    Mat frame;
    cap >> frame;
    if (frame.empty())
    {
      cout << "frame is empty!" << endl;
      break;
    }
    Mat srcImage_base, hsvImage_base;
    Mat srcImage_test1, hsvImage_test1;
 /*   if ( gats)
    {
      Gats(frame,alpha,beat);
      Gats(frame_key, alpha, beat);
  
    }*/
    srcImage_base = frame_key(Rect(xmin, ymin, xmax, ymax));
    srcImage_test1 = frame(Rect(xmin, ymin, xmax, ymax));
    //将图像从BGR色彩空间转换到 HSV色彩空间
    cvtColor(srcImage_base, previousImage, CV_BGR2GRAY);
    cvtColor(srcImage_test1, currentImage, CV_BGR2GRAY);
    //printf(">>>>>>>>>>>>>2");
    absdiff(currentImage, previousImage, resultImage);  //帧差法，相减
    //printf(">>>>>>>>>>>>>3");
    threshold(resultImage, resultImage, abs_differ, 255.0, CV_THRESH_BINARY); //二值化，像素值相差大于20则置为255，其余为0
    printf(">>>>>>>>>>>>>1\n");
    float counter = 0;
    float num = 0;
    // 统计两帧相减后图像素
    for (int i = 0; i < resultImage.rows; i++)
    {
      uchar *data = resultImage.ptr<uchar>(i); //获取每一行的指针
      for (int j = 0; j < resultImage.cols; j++)
      {
        num = num + 1;
        if (data [j] == 255) //访问到像素值
        {
          counter = counter + 1;
        }
      }
    }
    p = counter / num;
    // counter  num  p 分别为  有变化的像素点数  总像素点数目  比例
    printf(">>>>>>counter>>>>num>>>>p: %f  %f  %f  \n", counter, num, p);
    //float t = 1.05-float(thread) / float(100);
    if (p > scale) //达到阈值的像素数达到一定的数量则保存该图像
    {
      //printf(">>>>>>>>>>>>>6");
      cout << ">>>>>>>>>>>>>>>>>>>>.>>>>>>>.this frame is keyframe!" << endl;
     // frame.convertTo(frame, frame.type(), alpha, beat);
      frame_key = frame;
      cout << "正在写第" << currentFrame << "帧" << endl;
      stringstream str;
      stringstream str2;
      stringstream str3;
      stringstream str4;
      //写视频保存目录,我的是  ./keyframes_pixels_cha/xx.jpg  xx为当前帧的序号
      //str << "./keyframes_pixels_cha/" << currentFrame << ".jpg";
      str << "F:/桌面/微企/key/key_" << currentFrame << ".jpg";
      cout << str.str() << endl;
      Mat src = frame_key(Rect(xmin, ymin, xmax, ymax));
      Mat des;
      //是否旋转
      if (rotation)
      {
        if (blend)
        {
           rotate_arbitrarily_angle(src, des, angle1,angle2);
        }
        else
        {
          str2 << "F:/桌面/微企/key/rotation/key_rotation_" << currentFrame << ".jpg";
          imwrite(str2.str(), des);
        }
      }
      //是否滤波模糊
      if (blured)
      {
        if (blend)
        {
          image_blur(des, des, ksize, type);
        }
        else
        {
          image_blur(src, des, ksize, type);
          str3 << "F:/桌面/微企/key/blur/key_blur_" << currentFrame << ".jpg";
          imwrite(str3.str(), des);
        }
      }
      //是否增益
      if (gats)
      {
        if (blend)
        {
          Gats(des, alpha, beat);
        }
        else
        {
          Gats(src, alpha, beat);
          str4 << "F:/桌面/微企/key/gats/key_gats_" << currentFrame << ".jpg";
          imwrite(str4.str(), src);
        }
      }
      if (blend)
      {
        str2.clear();
        str2 << "F:/桌面/微企/key/blend/key_blend_" << currentFrame << ".jpg";
        imwrite(str2.str(), des);
      }
      else
      {
        imwrite(str.str(), src);
      }  
    }
    else
    {
      cout << ">>>>>>>>>>>>.this frame is not keyframe!" << endl;
    }
  }
}


void Gats(Mat &src,float alpha,int beat) {
  src.convertTo(src, src.type(), alpha, beat);
}


void rotate_arbitrarily_angle(Mat &src, Mat &dst,  int angle1, int angle2)
{
  srand((int)time(0));  // 产生随机种子  把0换成NULL也行
  float angle;
  if (angle2<angle1)
  {
    int temp = angle2;
    angle2 = angle1;
    angle1 = temp;
    angle = random(angle1, angle2);
  }
  if (angle2= angle1)
  {
   angle = random(angle1);
  }
 
  float radian = (float)(angle / 180.0 * CV_PI);

  //填充图像
  int maxBorder = (int)(max(src.cols, src.rows)* 1.414); //即为sqrt(2)*max
  int dx = (maxBorder - src.cols) / 2;
  int dy = (maxBorder - src.rows) / 2;
  copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

  //旋转
  Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
  Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
  warpAffine(dst, dst, affine_matrix, dst.size());

  //计算图像旋转之后包含图像的最大的矩形
  float sinVal = abs(sin(radian));
  float cosVal = abs(cos(radian));
  Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
    (int)(src.cols * sinVal + src.rows * cosVal));

  //剪掉多余边框
  int x = (dst.cols - targetSize.width) / 2;
  int y = (dst.rows - targetSize.height) / 2;
  Rect rect(x, y, targetSize.width, targetSize.height);
  dst = Mat(dst, rect);
}



void image_blur(Mat &src, Mat &dst, int ksize,int type) {
  switch (type)
  {
  case 0:
    medianBlur(src, dst, ksize);
  case 1:
    blur(src, dst, Size(ksize, ksize));
  case 2:
    GaussianBlur(src, dst, Size(ksize, ksize), 0, 0);
  case 3:
    boxFilter(src, dst, -1, Size(ksize, ksize));
  default:
    break;
  }
  
}