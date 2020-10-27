#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "opencv2/video/video.hpp"

using namespace cv;
using namespace std;

//Ö÷º¯Êý
int main()
{
	float Row[20] = {199.784, 197.699, 196.245, 194.81, 193.688, 392.606, 391.131, 389.437, 388.058, 385.681, 585.32, 584.094, 582.881, 580.971, 578.525, 776.681, 775.216, 773.607, 772.545, 770.286};
	float Col[20] = {127.517, 319.141, 511.448, 704.252, 896.429, 128.496, 320.039, 512.104, 705.058, 896.768, 128.672, 320.768, 512.682, 705.221, 897.512, 130.894, 322.131, 513.916, 705.925, 897.175};

	float qx[20] = { 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300 };
	float qy[20] = { 200, 200, 200, 200, 200, 250, 250, 250, 250, 250, 300, 300, 300, 300, 300, 350, 350, 350, 350, 350 };

	vector<Point2f> _image_piont_;
	vector<Point2f> _real_piont_;
	for (size_t i = 0; i < 20; i++)
	{
		_image_piont_.push_back(Point2f(Col[i], Row[i]));
		_real_piont_.push_back(Point2f(qx[i], qy[i]));
	}

	Mat _affine_matrix_ = estimateRigidTransform(_image_piont_, _real_piont_, true);
	cout << "\n_affine_matrix_ : \n" << _affine_matrix_ << endl;

	Point2d temp;
	temp.x = _affine_matrix_.at<double>(0)* 127.517 + _affine_matrix_.at<double>(1)* 199.784 + _affine_matrix_.at<double>(2);
	temp.y = _affine_matrix_.at<double>(3)* 127.517 + _affine_matrix_.at<double>(4)* 199.784 + _affine_matrix_.at<double>(5);

	return 0;
}