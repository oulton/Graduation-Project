
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "opencv2/video/video.hpp"
#include <Windows.h>

#define Roi 0
#define N 10 //边缘跟踪宽度

//标定数据
float Row[20] = { 199.784, 197.699, 196.245, 194.81, 193.688, 392.606, 391.131, 389.437, 388.058, 385.681, 585.32, 584.094, 582.881, 580.971, 578.525, 776.681, 775.216, 773.607, 772.545, 770.286 };
float Col[20] = { 127.517, 319.141, 511.448, 704.252, 896.429, 128.496, 320.039, 512.104, 705.058, 896.768, 128.672, 320.768, 512.682, 705.221, 897.512, 130.894, 322.131, 513.916, 705.925, 897.175 };

float qx[20] = { 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300 };
float qy[20] = { 200, 200, 200, 200, 200, 250, 250, 250, 250, 250, 300, 300, 300, 300, 300, 350, 350, 350, 350, 350 };

using namespace cv;
using namespace std;

Mat Calibrate(float Col[], float Row[], float qx[], float qy[], int Length);

Mat _affine_calibrate_;
//主函数
int main()
{
#if 0  //预处理
	//Mat srcImage = imread("2345.bmp", 1);
	//Mat thredImage;
	//
	//cvtColor(srcImage, thredImage, CV_RGB2GRAY);
	//threshold(thredImage, thredImage, 190, 255, CV_THRESH_BINARY);

	////进行腐蚀操作 
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	//Mat dstImage1;
	//erode(thredImage, dstImage1, element1);

	////进行膨胀操作 
	//Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(27, 27));
	//Mat dstImage2;
	//dilate(dstImage1, dstImage2, element2);

	////进行腐蚀操作 
	//Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(23, 23));
	//Mat dstOut;
	//erode(dstImage2, dstOut, element3);

	//imwrite("ero_dil_image.bmp",dstOut);
#endif

	_affine_calibrate_ = Calibrate(Col,Row,qx,qy,20);

	Mat ero_dil_image = imread("ero_dil_image.bmp", 0);
	Mat dstOut = ero_dil_image(Rect(Roi, 0, ero_dil_image.cols - Roi, ero_dil_image.rows));

	//double time0 = static_cast<double>(getTickCount());   //开始计时
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
		return -1;
	}
	QueryPerformanceCounter(&start); //开始计时

	//先找到上下边缘的第一个点
	int upPoint = 0, downPoint = 0;
	uchar *image = dstOut.data;
	int step = dstOut.step;
	for (size_t i = 0; i < dstOut.rows; i++)
	{
		int pix[2];
		pix[0] = image[0];
		image += step;   //下一行
		pix[1] = image[0];
		if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
		{
			upPoint = i;
		}
		if (pix[0] == 0 && pix[1] == 255) //黑-白
		{
			downPoint = i;
		}
		if (upPoint != 0 && downPoint != 0)
		{
			if (downPoint > upPoint)
			{
				if (downPoint - upPoint >= 60)
				{
					break;
				}				
				else
				{
					upPoint = 0;
					downPoint = 0;
				}

			}
			else
			{
				downPoint = 0;
			}
		}		
	}

	vector <Point> upVector, downVector;
	int last_upPoint = upPoint;
	int last_downPoint = downPoint;

	//先找上边缘点，边缘跟踪算法
	for (size_t lie = 0; lie < dstOut.cols; lie++)   //x方向
	{
		uchar *image1 = dstOut.data;
		image1 += step * (last_upPoint - N);
		int get_w2b_flag = 0;
		for (size_t i = 0; i < 2*N; i++)
		{
			int pix[2];
			pix[0] = image1[lie];
			image1 += step;   //下一行
			pix[1] = image1[lie];
			if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
			{
				get_w2b_flag = 1;
				upVector.push_back(Point(lie + Roi, last_upPoint - N + i));
				last_upPoint = last_upPoint - N + i;
			}
		}
		if (get_w2b_flag == 0)   //如果小区间内没有找到，将区间上下平移继续跟踪
		{
			if (upVector[upVector.size() - 1].y > upVector[upVector.size() - 2].y) //区间往下移动
			{
				uchar *image2 = dstOut.data;
				image2 += step * (last_upPoint - N + 2*N);
				int get_w2b_flag1 = 0;
				for (size_t i = 0; i < 2 * N; i++)
				{
					int pix[2];
					pix[0] = image2[lie];
					image2 += step;   //下一行
					pix[1] = image2[lie];
					if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
					{
						get_w2b_flag1 = 1;
						upVector.push_back(Point(lie + Roi, last_upPoint - N + 2*N + i));
						last_upPoint = last_upPoint - N + 2*N + i;
					}
				}
				if (get_w2b_flag1 == 0) //如果还没找到，继续向下移动
				{
					uchar *image3 = dstOut.data;
					image3 += step * (last_upPoint - N + 4 * N);
					for (size_t i = 0; i < 2 * N; i++)
					{
						int pix[2];
						pix[0] = image3[lie];
						image3 += step;   //下一行
						pix[1] = image3[lie];
						if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
						{
							upVector.push_back(Point(lie + Roi, last_upPoint - N + 4 * N + i));
							last_upPoint = last_upPoint - N + 4 * N + i;
						}
					}
				}
			}
			if (upVector[upVector.size() - 1].y < upVector[upVector.size() - 2].y) //区间往上移动
			{
				uchar *image2 = dstOut.data;
				image2 += step * (last_upPoint - N - 2 * N);
				int get_w2b_flag1 = 0;
				for (size_t i = 0; i < 2 * N; i++)
				{
					int pix[2];
					pix[0] = image2[lie];
					image2 += step;   //下一行
					pix[1] = image2[lie];
					if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
					{
						get_w2b_flag1 = 1;
						upVector.push_back(Point(lie + Roi, last_upPoint - N - 2 * N + i));
						last_upPoint = last_upPoint - N - 2 * N + i;
					}
				}
				if (get_w2b_flag1 == 0) //如果还没找到，继续向下移动
				{
					uchar *image3 = dstOut.data;
					image3 += step * (last_upPoint - N - 4 * N);
					for (size_t i = 0; i < 2 * N; i++)
					{
						int pix[2];
						pix[0] = image3[lie];
						image3 += step;   //下一行
						pix[1] = image3[lie];
						if (pix[0] == 255 && pix[1] == 0)  //白-黑跳变
						{
							upVector.push_back(Point(lie + Roi, last_upPoint - N - 4 * N + i));
							last_upPoint = last_upPoint - N - 4 * N + i;
						}
					}
				}
			}
		}
	}

	//再找下边缘，边缘跟踪
	for (size_t lie = 0; lie < dstOut.cols; lie ++)   //x方向
	{
		uchar *image1 = dstOut.data;
		image1 += step * (last_downPoint - N);
		int get_w2b_flag = 0;
		for (size_t i = 0; i < 2 * N; i++)
		{
			int pix[2];
			pix[0] = image1[lie];
			image1 += step;   //下一行
			pix[1] = image1[lie];
			if (pix[0] == 0 && pix[1] == 255)  //黑-白跳变
			{
				get_w2b_flag = 1;
				downVector.push_back(Point(lie + Roi, last_downPoint - N + i));
				last_downPoint = last_downPoint - N + i;
			}
		}

		if (get_w2b_flag == 0)   //如果小区间内没有找到，将区间上下平移继续跟踪
		{
			if (downVector[downVector.size() - 1].y > downVector[downVector.size() - 2].y) //区间往下移动
			{
				uchar *image2 = dstOut.data;
				image2 += step * (last_downPoint - N + 2 * N);
				int get_w2b_flag1 = 0;
				for (size_t i = 0; i < 2 * N; i++)
				{
					int pix[2];
					pix[0] = image2[lie];
					image2 += step;   //下一行
					pix[1] = image2[lie];
					if (pix[0] == 0 && pix[1] == 255)  //黑-白跳变
					{
						get_w2b_flag1 = 1;
						downVector.push_back(Point(lie + Roi, last_downPoint - N + 2 * N + i));
						last_downPoint = last_downPoint - N + 2 * N + i;
					}
				}
				if (get_w2b_flag1 == 0) //如果还没找到，继续向下移动
				{
					uchar *image3 = dstOut.data;
					image3 += step * (last_downPoint - N + 4 * N);
					for (size_t i = 0; i < 2 * N; i++)
					{
						int pix[2];
						pix[0] = image3[lie];
						image3 += step;   //下一行
						pix[1] = image3[lie];
						if (pix[0] == 0 && pix[1] == 255)  //黑 - 白跳变
						{
							downVector.push_back(Point(lie + Roi, last_downPoint - N + 4 * N + i));
							last_downPoint = last_downPoint - N + 4 * N + i;
						}
					}
				}
			}
			if (downVector[downVector.size() - 1].y < downVector[downVector.size() - 2].y) //区间往上移动
			{
				uchar *image2 = dstOut.data;
				image2 += step * (last_downPoint - N - 2 * N);
				int get_w2b_flag1 = 0;
				for (size_t i = 0; i < 2 * N; i++)
				{
					int pix[2];
					pix[0] = image2[lie];
					image2 += step;   //下一行
					pix[1] = image2[lie];
					if (pix[0] == 0 && pix[1] == 255)  //黑-白跳变
					{
						get_w2b_flag1 = 1;
						downVector.push_back(Point(lie + Roi, last_downPoint - N - 2 * N + i));
						last_downPoint = last_downPoint - N - 2 * N + i;
					}
				}
				if (get_w2b_flag1 == 0) //如果还没找到，继续向下移动
				{
					uchar *image3 = dstOut.data;
					image3 += step * (last_downPoint - N - 4 * N);
					for (size_t i = 0; i < 2 * N; i++)
					{
						int pix[2];
						pix[0] = image3[lie];
						image3 += step;   //下一行
						pix[1] = image3[lie];
						if (pix[0] == 0 && pix[1] == 255)  //黑-白跳变
						{
							downVector.push_back(Point(lie + Roi, last_downPoint - N - 4 * N + i));
							last_downPoint = last_downPoint - N - 4 * N + i;
						}
					}
				}
			}
		}
	}

	QueryPerformanceCounter(&end); //结束计时
	printf("QueryPerformanceCounter 运行时间为: %f\n", (double)(end.QuadPart - start.QuadPart) / (double)frequency.QuadPart);

	Mat srcImage = imread("2345.bmp", 1);//在原图3通道图中画出边缘
	Mat srcImage1 = imread("2345.bmp", 1);
	for (int i = 1; i < upVector.size(); i++)
	{
		line(srcImage, Point(upVector[i - 1].x, upVector[i - 1].y + 25), Point(upVector[i].x, upVector[i].y + 25), Scalar(0, 0, 255), 3);
	}
	for (int i = 1; i < downVector.size(); i++)
	{
		line(srcImage, Point(downVector[i - 1].x, downVector[i - 1].y - 25), Point(downVector[i].x, downVector[i].y - 25), Scalar(0, 0, 255), 3);
	}


	//机械坐标转换
	FILE *upFile;
	FILE *downFile;
	fopen_s(&upFile,"Calibrate_up.txt", "w");
	fopen_s(&downFile,"Calibrate_down.txt", "w");
	vector <Point2f> upVector_Real, downVector_Real;
	for (int i = 0; i < upVector.size(); i++)
	{
		Point2f temp;
		temp.x = _affine_calibrate_.at<double>(0)* upVector[i].x + _affine_calibrate_.at<double>(1)* upVector[i].y + _affine_calibrate_.at<double>(2);
		temp.y = _affine_calibrate_.at<double>(3)* upVector[i].x + _affine_calibrate_.at<double>(4)* upVector[i].y + _affine_calibrate_.at<double>(5);
		upVector_Real.push_back(temp);
		fprintf(upFile, "[%.3f, %.3f]\n", temp.x, temp.y);
	}
	for (int i = 0; i < downVector.size(); i++)
	{
		Point2f temp;
		temp.x = _affine_calibrate_.at<double>(0)* downVector[i].x + _affine_calibrate_.at<double>(1)* downVector[i].y + _affine_calibrate_.at<double>(2);
		temp.y = _affine_calibrate_.at<double>(3)* downVector[i].x + _affine_calibrate_.at<double>(4)* downVector[i].y + _affine_calibrate_.at<double>(5);
		downVector_Real.push_back(temp);
		fprintf(downFile, "[%.3f, %.3f]\n", temp.x, temp.y);
	}
	fclose(upFile);
	fclose(downFile);

	


	
	//间隔0.5mm差分提取
	for (int i = 0; i < upVector_Real.size()-1; i++)
	{
		while (abs(upVector_Real[i].x - upVector_Real[i + 1].x) < 0.5)
		{
			upVector_Real.erase(upVector_Real.begin() + i + 1);
			if (i == upVector_Real.size() - 1)
				break;
		}
	}
	for (int i = 0; i < downVector_Real.size()-1; i++)
	{	
		while (abs(downVector_Real[i].x - downVector_Real[i + 1].x) < 0.5)
		{
			downVector_Real.erase(downVector_Real.begin() + i + 1);
			if (i == downVector_Real.size() - 1)
				break;
		}
	}

	vector<Point2f> _Real_Point_;
	for (int i = 0; i < upVector_Real.size(); i++)
	{
		_Real_Point_.push_back(upVector_Real[i]);
	}
	for (int i = 0; i < downVector_Real.size(); i++)
	{
		_Real_Point_.push_back(downVector_Real[downVector_Real.size() - 1 - i]);
	}

	FILE *finalFile;
	fopen_s(&finalFile, "totlePoint.txt", "w");
	for (size_t i = 0; i < _Real_Point_.size(); i++)
	{
		fprintf(finalFile, "[%.3f, %.3f]\n", _Real_Point_[i].x, _Real_Point_[i].y);
	}
	fclose(finalFile);

	//element1.release();
	//element2.release();
	//element3.release();
	//dstImage1.release();
	//dstImage2.release();
	//srcImage.release();
	//thredImage.release();

	return 0;
}

Mat Calibrate(float Col[], float Row[], float qx[], float qy[],int Length)
{
	vector<Point2f> _image_piont_;
	vector<Point2f> _real_piont_;
	for (size_t i = 0; i < Length; i++)
	{
		_image_piont_.push_back(Point2f(Col[i], Row[i]));
		_real_piont_.push_back(Point2f(qx[i], qy[i]));
	}

	Mat _affine_matrix_ = estimateRigidTransform(_image_piont_, _real_piont_, true);
	return _affine_matrix_;
}
