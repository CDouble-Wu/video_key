#include<opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace cv;
//�ҶȶԱ�
//����
int  xmin=10, ymin=320, xmax=400, ymax=500;
//֡��������ز�
int abs_differ=15;
//�����������ռ����ͼƬ�����İٷֱ�
float scale = 0.19;
//����
bool gats = true;
float alpha =1.3;
float beat = 20;
//��ת
bool rotation = true;
int angle1 = 20;
int angle2 = 30;
#define random(a,b) (rand()%(b-a)+a)
//�˲�
bool blured = true;
int ksize = 3;
int type=0;
//�Ƿ��ں�
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
  //���������Ҫ��ȡ�ؼ��ֵ���Ƶ
  cap.open("1.mp4");
  if (!cap.isOpened())//�����Ƶ�����������򷵻�
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
    //��ͼ���BGRɫ�ʿռ�ת���� HSVɫ�ʿռ�
    cvtColor(srcImage_base, previousImage, CV_BGR2GRAY);
    cvtColor(srcImage_test1, currentImage, CV_BGR2GRAY);
    //printf(">>>>>>>>>>>>>2");
    absdiff(currentImage, previousImage, resultImage);  //֡������
    //printf(">>>>>>>>>>>>>3");
    threshold(resultImage, resultImage, abs_differ, 255.0, CV_THRESH_BINARY); //��ֵ��������ֵ������20����Ϊ255������Ϊ0
    printf(">>>>>>>>>>>>>1\n");
    float counter = 0;
    float num = 0;
    // ͳ����֡�����ͼ����
    for (int i = 0; i < resultImage.rows; i++)
    {
      uchar *data = resultImage.ptr<uchar>(i); //��ȡÿһ�е�ָ��
      for (int j = 0; j < resultImage.cols; j++)
      {
        num = num + 1;
        if (data [j] == 255) //���ʵ�����ֵ
        {
          counter = counter + 1;
        }
      }
    }
    p = counter / num;
    // counter  num  p �ֱ�Ϊ  �б仯�����ص���  �����ص���Ŀ  ����
    printf(">>>>>>counter>>>>num>>>>p: %f  %f  %f  \n", counter, num, p);
    //float t = 1.05-float(thread) / float(100);
    if (p > scale) //�ﵽ��ֵ���������ﵽһ���������򱣴��ͼ��
    {
      //printf(">>>>>>>>>>>>>6");
      cout << ">>>>>>>>>>>>>>>>>>>>.>>>>>>>.this frame is keyframe!" << endl;
     // frame.convertTo(frame, frame.type(), alpha, beat);
      frame_key = frame;
      cout << "����д��" << currentFrame << "֡" << endl;
      stringstream str;
      stringstream str2;
      stringstream str3;
      stringstream str4;
      //д��Ƶ����Ŀ¼,�ҵ���  ./keyframes_pixels_cha/xx.jpg  xxΪ��ǰ֡�����
      //str << "./keyframes_pixels_cha/" << currentFrame << ".jpg";
      str << "F:/����/΢��/key/key_" << currentFrame << ".jpg";
      cout << str.str() << endl;
      Mat src = frame_key(Rect(xmin, ymin, xmax, ymax));
      Mat des;
      //�Ƿ���ת
      if (rotation)
      {
        if (blend)
        {
           rotate_arbitrarily_angle(src, des, angle1,angle2);
        }
        else
        {
          str2 << "F:/����/΢��/key/rotation/key_rotation_" << currentFrame << ".jpg";
          imwrite(str2.str(), des);
        }
      }
      //�Ƿ��˲�ģ��
      if (blured)
      {
        if (blend)
        {
          image_blur(des, des, ksize, type);
        }
        else
        {
          image_blur(src, des, ksize, type);
          str3 << "F:/����/΢��/key/blur/key_blur_" << currentFrame << ".jpg";
          imwrite(str3.str(), des);
        }
      }
      //�Ƿ�����
      if (gats)
      {
        if (blend)
        {
          Gats(des, alpha, beat);
        }
        else
        {
          Gats(src, alpha, beat);
          str4 << "F:/����/΢��/key/gats/key_gats_" << currentFrame << ".jpg";
          imwrite(str4.str(), src);
        }
      }
      if (blend)
      {
        str2.clear();
        str2 << "F:/����/΢��/key/blend/key_blend_" << currentFrame << ".jpg";
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
  srand((int)time(0));  // �����������  ��0����NULLҲ��
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

  //���ͼ��
  int maxBorder = (int)(max(src.cols, src.rows)* 1.414); //��Ϊsqrt(2)*max
  int dx = (maxBorder - src.cols) / 2;
  int dy = (maxBorder - src.rows) / 2;
  copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

  //��ת
  Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
  Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//�����ת����
  warpAffine(dst, dst, affine_matrix, dst.size());

  //����ͼ����ת֮�����ͼ������ľ���
  float sinVal = abs(sin(radian));
  float cosVal = abs(cos(radian));
  Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
    (int)(src.cols * sinVal + src.rows * cosVal));

  //��������߿�
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