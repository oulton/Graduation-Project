
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <Windows.h>

#define N 60

using namespace cv;
using namespace std;

Mat Hist_Together(Mat srcImage_left, Mat srcImage_right);

//������
int main()
{
	Mat srcImage_left = imread("2.bmp", 1);
	Mat srcImage_right = imread("3.bmp", 1);

	//double time0 = static_cast<double>(getTickCount());   //��ʼ��ʱ
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
		return -1;
	}
	QueryPerformanceCounter(&start); //��ʼ��ʱ

	Mat dst_torgether;
	dst_torgether = Hist_Together(srcImage_left, srcImage_right);

	QueryPerformanceCounter(&end); //������ʱ
	printf("QueryPerformanceCounter ����ʱ��Ϊ: %fs\n", (double)(end.QuadPart - start.QuadPart) / (double)frequency.QuadPart);
	//imwrite("2345.bmp", dst_torgether);
	system("pause");
	return 0;
}

Mat Hist_Together(Mat srcImage_left, Mat srcImage_right)
{
	//��1����������HSVͼ��ľ���
	Mat hsvImage_left, hsvImage_right;

	//��2����ʼ������ֱ��ͼ��Ҫ��ʵ��
	const int nimages = 1;
	int channels[] = { 0 };
	int dims = 1;
	int histSize[] = { 256 };
	float hranges[] = { 0.0, 255.0 };
	const float* ranges[] = { hranges };//һ��Ҫ��const
	MatND hist_right, hist_left;

	//��3��������ͼ�Ҷ�ֱ��ͼ
	Mat Right_5Col = srcImage_right(Rect(0, 0, N, srcImage_right.rows)); //�ұ�ͼǰN��
	cvtColor(Right_5Col, hsvImage_right, CV_RGB2GRAY);
	calcHist(&hsvImage_right, 1, channels, Mat(), hist_right, dims, histSize, ranges);

	//��4��ƽ�Ƽ�����ͼֱ��ͼ����ƥ�� �ҵ�����ƥ��
	int Index = -1;
	double maxHist = 0;
	for (size_t i = 0; i < 100; i++)
	{
		Mat Left_5Col = srcImage_left(Rect(srcImage_left.cols - i - N, 0, N, srcImage_left.rows));
		cvtColor(Left_5Col, hsvImage_left, CV_RGB2GRAY);
		calcHist(&hsvImage_left, 1, channels, Mat(), hist_left, dims, histSize, ranges);
		double base_hist = compareHist(hist_right, hist_left, 0);
		//printf(" ���� [%d] ��ƥ�������£�\n\n����׼ͼ - ��׼ͼ���� %f \n ", i, base_hist);
		if (base_hist > maxHist)
		{
			maxHist = base_hist;
			Index = i;
		}
	}

	//��5����λ��ط����� ��ƥ��õ���λ��
	Mat src1, src2;
	Mat dst1, dst2;
	src1 = srcImage_left(Rect(srcImage_left.cols - Index - N, 0, N, srcImage_left.rows));
	src2 = srcImage_right(Rect(0, 0, N, srcImage_right.rows));
	cvtColor(src1, src1, CV_BGR2GRAY);     //ת��Ϊ�Ҷ�ͼ��
	src1.convertTo(dst1, CV_32FC1);        //ת��Ϊ32λ������
	cvtColor(src2, src2, CV_BGR2GRAY);
	src2.convertTo(dst2, CV_32FC1);
	Point2d phase_shift;
	phase_shift = phaseCorrelate(dst1, dst2);
	//cout << endl << "warp :" << endl << "\tX shift : " << phase_shift.x << "\tY shift : " << phase_shift.y << endl;

	//��6������ƴ��
	Mat Left_Gray, Right_Gray;
	cvtColor(srcImage_left, Left_Gray, CV_RGB2GRAY);
	cvtColor(srcImage_right, Right_Gray, CV_RGB2GRAY);
	Mat Show_Image = Mat::zeros(Left_Gray.rows, Right_Gray.cols + Left_Gray.cols - (int)(N + Index + phase_shift.x + 0.5), CV_8UC1);        //����һ���������ͼ�� 

	Mat image_roi_left = Left_Gray(Rect(0, 0, Left_Gray.cols - (int)(N + Index + phase_shift.x + 0.5), Left_Gray.rows));
	image_roi_left.copyTo(Show_Image(Rect(0, 0, Left_Gray.cols - (int)(N + Index + phase_shift.x + 0.5), Left_Gray.rows))); //��roicopy����ͼ�������
	Mat image_roi_right = Right_Gray(Rect(0, (int)(phase_shift.y + 0.5), Right_Gray.cols, Right_Gray.rows - (int)(phase_shift.y + 0.5)));
	image_roi_right.copyTo(Show_Image(Rect(Left_Gray.cols - (int)(N + Index + phase_shift.x + 0.5), 0, image_roi_right.cols, image_roi_right.rows)));

	return Show_Image;
}
