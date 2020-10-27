
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "opencv2/video/video.hpp"

using namespace cv;
using namespace std;

//������
int main()
{
	Mat image_source = imread("5.bmp", 0);
	Mat image_template = imread("tempnew.bmp", 0);
	Mat image_matched;
#if 0 //����ģ���õ�
	//Mat image_color;
	//cvtColor(image_source, image_color, CV_GRAY2BGR);
	//rectangle(image_color, cvPoint(160, 380), cvPoint(410, 540), Scalar(255, 0, 0), 2, 1, 0);
	//Mat Temp = image_source(Rect(160, 380, 250, 160)); //�ұ�ͼǰN��
	//imwrite("tempnew.bmp",Temp);
#endif
	matchTemplate(image_source, image_template, image_matched, TM_CCOEFF_NORMED);
	double minVal, maxVal;
	Point minLoc, maxLoc;
	//Ѱ�����ƥ��λ��  
	minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);
	// ģ������
	int template_x = maxLoc.x + image_template.cols / 2, template_y = maxLoc.y + image_template.rows / 2;  //ģ������

	Mat image_color;
	cvtColor(image_source, image_color, CV_GRAY2BGR);
	circle(image_color, Point(template_x, template_y), 2, Scalar(0, 0, 255), 2, 8, 0);
	rectangle(image_color, cvPoint(template_x - image_template.cols / 2, template_y - image_template.rows / 2), cvPoint(template_x + image_template.cols / 2, template_y + image_template.rows / 2), Scalar(255, 0, 0), 2, 1, 0);

	return 0;
}
