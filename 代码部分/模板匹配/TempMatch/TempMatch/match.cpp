
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "opencv2/video/video.hpp"

using namespace cv;
using namespace std;

//主函数
int main()
{
	Mat image_source = imread("5.bmp", 0);
	Mat image_template = imread("tempnew.bmp", 0);
	Mat image_matched;
#if 0 //创建模板用的
	//Mat image_color;
	//cvtColor(image_source, image_color, CV_GRAY2BGR);
	//rectangle(image_color, cvPoint(160, 380), cvPoint(410, 540), Scalar(255, 0, 0), 2, 1, 0);
	//Mat Temp = image_source(Rect(160, 380, 250, 160)); //右边图前N列
	//imwrite("tempnew.bmp",Temp);
#endif
	matchTemplate(image_source, image_template, image_matched, TM_CCOEFF_NORMED);
	double minVal, maxVal;
	Point minLoc, maxLoc;
	//寻找最佳匹配位置  
	minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);
	// 模板中心
	int template_x = maxLoc.x + image_template.cols / 2, template_y = maxLoc.y + image_template.rows / 2;  //模板中心

	Mat image_color;
	cvtColor(image_source, image_color, CV_GRAY2BGR);
	circle(image_color, Point(template_x, template_y), 2, Scalar(0, 0, 255), 2, 8, 0);
	rectangle(image_color, cvPoint(template_x - image_template.cols / 2, template_y - image_template.rows / 2), cvPoint(template_x + image_template.cols / 2, template_y + image_template.rows / 2), Scalar(255, 0, 0), 2, 1, 0);

	return 0;
}
