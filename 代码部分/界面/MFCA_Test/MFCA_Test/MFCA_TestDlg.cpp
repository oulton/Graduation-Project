
// MFCA_TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCA_Test.h"
#include "MFCA_TestDlg.h"
#include "afxdialogex.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp> 

#include <cmath>
#include <Windows.h> 
#include <iostream>

using namespace std;
using namespace cv; //包含cv命名空间

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//标定数据
Mat _affine_matrix_;
//像素上下边缘坐标
vector <Point> upVector, downVector;
//差分提取后的机械坐标
vector<Point2f> _Real_Point_;

Mat dstEro_Dil;
Mat TorgrtherImage;
Mat SoftImage,GraySoftImage;
int Image_Count = 0;

CRect rect_pic_original;			// wnd控件支持的CRect
Rect rect_pic_original_opencv;		// opencv支持的Rect
CRect rect_pic_roi;					// wnd控件支持的CRect
Rect rect_pic_roi_opencv;			// opencv支持的Rect
CRect rect_pic_original2;			// wnd控件支持的CRect
Rect rect_pic_original_opencv2;		// opencv支持的Rect
CRect rect_pic_roi2;				// wnd控件支持的CRect
Rect rect_pic_roi_opencv2;			// opencv支持的Rect
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

Mat Calibrate(float Col[], float Row[], float qx[], float qy[], int Length);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCA_TestDlg 对话框



CMFCA_TestDlg::CMFCA_TestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCA_TestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCA_TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCA_TestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LoadImage, &CMFCA_TestDlg::OnBnClickedLoadimage)
	ON_BN_CLICKED(IDC_FitLine, &CMFCA_TestDlg::OnBnClickedFitline)
	ON_BN_CLICKED(IDC_Matching, &CMFCA_TestDlg::OnBnClickedMatching)
	ON_BN_CLICKED(IDC_Torgether, &CMFCA_TestDlg::OnBnClickedTorgether)
//	ON_BN_CLICKED(IDC_EroDil, &CMFCA_TestDlg::OnBnClickedErodil)
ON_BN_CLICKED(IDC_Calibrate, &CMFCA_TestDlg::OnBnClickedCalibrate)
ON_BN_CLICKED(IDC_SaveLine, &CMFCA_TestDlg::OnBnClickedSaveline)
ON_BN_CLICKED(IDC_CalSave, &CMFCA_TestDlg::OnBnClickedCalsave)
END_MESSAGE_MAP()


// CMFCA_TestDlg 消息处理程序

BOOL CMFCA_TestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	// 用于在picture控件IDC_SHOWIMAGE中显示mat图像。参见http://lib.csdn.net/article/opencv/24137
	GetDlgItem(IDC_SHOWIMAGE1)->GetClientRect(rect_pic_original);
	namedWindow(IMG_WND_IDC_ORIGINAL_IMAGE, CV_WINDOW_NORMAL);
	resizeWindow(IMG_WND_IDC_ORIGINAL_IMAGE, rect_pic_original.Width(), rect_pic_original.Height());
	HWND hWnd0 = (HWND)cvGetWindowHandle(IMG_WND_IDC_ORIGINAL_IMAGE);
	HWND hParent0 = ::GetParent(hWnd0);
	::SetParent(hWnd0, GetDlgItem(IDC_SHOWIMAGE1)->m_hWnd);
	::ShowWindow(hParent0, SW_HIDE);
	CPoint p0 = rect_pic_original.TopLeft();
	rect_pic_original_opencv.x = p0.x;
	rect_pic_original_opencv.y = p0.y;
	rect_pic_original_opencv.width = rect_pic_original.Width();
	rect_pic_original_opencv.height = rect_pic_original.Height();

	GetDlgItem(IDC_SHOWIMAGE2)->GetClientRect(rect_pic_original2);
	namedWindow(IMG_WND_IDC_ORIGINAL_IMAGE2, CV_WINDOW_NORMAL);
	resizeWindow(IMG_WND_IDC_ORIGINAL_IMAGE2, rect_pic_original2.Width(), rect_pic_original2.Height());
	HWND hWnd2 = (HWND)cvGetWindowHandle(IMG_WND_IDC_ORIGINAL_IMAGE2);
	HWND hParent2 = ::GetParent(hWnd2);
	::SetParent(hWnd2, GetDlgItem(IDC_SHOWIMAGE2)->m_hWnd);
	::ShowWindow(hParent2, SW_HIDE);
	CPoint p2 = rect_pic_original2.TopLeft();
	rect_pic_original_opencv2.x = p2.x;
	rect_pic_original_opencv2.y = p2.y;
	rect_pic_original_opencv2.width = rect_pic_original2.Width();
	rect_pic_original_opencv2.height = rect_pic_original2.Height();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCA_TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCA_TestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCA_TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 按照窗口大小绘制指定图像
//		Mat img_for_show		用于显示的图像
//		Rect rect_wnd			用于显示图像的窗口大小
//		CString wnd_name		用于显示图像的窗口名称
int CMFCA_TestDlg::ShowImageInWnd(Mat img_for_show, int wnd_width, int wnd_height, CString wnd_name)
{
	Mat temp;

	if (wnd_width == 0)
		wnd_width = img_for_show.cols;
	if (wnd_height == 0)
		wnd_height = img_for_show.rows;

	resize(img_for_show, temp, Size(wnd_width, wnd_height), INTER_LINEAR);
	imshow(wnd_name.GetBuffer(), temp);

	return 0;
}

void CMFCA_TestDlg::OnBnClickedLoadimage()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str = "bmp文件(*.bmp)|*.bmp|jpeg文件(*.jpg)|*.jpg|所有文件(*.*)|*.*|";
	CFileDialog dlg(FALSE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, str, NULL);

	CString ImageName;
	if (dlg.DoModal() == IDOK)
	{
		//当你找到文件并确定打开时
		ImageName = dlg.GetPathName();
	}

	//Mat SrcImage;
	Mat m_image_gray;
	// 读图
	SoftImage = imread(ImageName.GetBuffer());

	Image_Count++;

	// 转换为灰度图
	if (3 == SoftImage.channels())
		cvtColor(SoftImage, GraySoftImage, CV_RGB2GRAY);
	else if (1 == SoftImage.channels())
		GraySoftImage = SoftImage.clone();

	// 显示图像
	ShowImageInWnd(SoftImage, rect_pic_original_opencv.width, rect_pic_original_opencv.height, IMG_WND_IDC_ORIGINAL_IMAGE);
}


void CMFCA_TestDlg::OnBnClickedMatching()
{
	// TODO:  在此添加控件通知处理程序代码
	if (GraySoftImage.data != NULL)
	{
		Mat image_template = imread("tempnew.bmp", 0);
		Mat image_matched;
		matchTemplate(GraySoftImage, image_template, image_matched, TM_CCOEFF_NORMED);
		double minVal, maxVal;
		Point minLoc, maxLoc;
		//寻找最佳匹配位置  
		minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);
		// 模板中心
		int template_x = maxLoc.x + image_template.cols / 2, template_y = maxLoc.y + image_template.rows / 2;  //模板中心

		Mat image_color;
		cvtColor(GraySoftImage, image_color, CV_GRAY2BGR);
		circle(image_color, Point(template_x, template_y), 2, Scalar(0, 0, 255), 5, 8, 0);
		rectangle(image_color, cvPoint(template_x - image_template.cols / 2, template_y - image_template.rows / 2), cvPoint(template_x + image_template.cols / 2, template_y + image_template.rows / 2), Scalar(255, 0, 0), 5, 1, 0);
		ShowImageInWnd(image_color, rect_pic_original_opencv.width, rect_pic_original_opencv.height, IMG_WND_IDC_ORIGINAL_IMAGE);
	}
	else
	{
		CString str = "未获取到图像";
		MessageBox(str);
	}
}


void CMFCA_TestDlg::OnBnClickedTorgether()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Image_Count >= 4)
	{
		TorgrtherImage = imread("../picture/2345.bmp");
		ShowImageInWnd(TorgrtherImage, rect_pic_original_opencv2.width, rect_pic_original_opencv2.height, IMG_WND_IDC_ORIGINAL_IMAGE2);
		Image_Count = 0;
	}
	else
	{
		CString str = "图像数量不足";
		MessageBox(str);
	}
}


//void CMFCA_TestDlg::OnBnClickedErodil()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	if (TorgrtherImage.data != NULL)
//	{
//		Mat srcImage;
//		srcImage = imread("../picture/2345.bmp", 1);
//		Mat thredImage;
//
//		cvtColor(srcImage, thredImage, CV_RGB2GRAY);
//		threshold(thredImage, thredImage, 190, 255, CV_THRESH_BINARY);
//
//		//进行腐蚀操作 
//		Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
//		Mat dstImage1;
//		erode(thredImage, dstImage1, element1);
//
//		//进行膨胀操作 
//		Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(27, 27));
//		Mat dstImage2;
//		dilate(dstImage1, dstImage2, element2);
//
//		//进行腐蚀操作 
//		Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(23, 23));
//		erode(dstImage2, dstEro_Dil, element3);
//
//		ShowImageInWnd(dstEro_Dil, rect_pic_original_opencv2.width, rect_pic_original_opencv2.height, IMG_WND_IDC_ORIGINAL_IMAGE2);
//	}
//	else
//	{
//		CString str = "图像未拼接";
//		MessageBox(str);
//	}
//	
//}

void CMFCA_TestDlg::OnBnClickedFitline()
{
	// TODO:  在此添加控件通知处理程序代码
	if (TorgrtherImage.data != NULL)
	{
		Mat ero_dil_image = imread("ero_dil_image.bmp", 0);
		Mat dstOut = ero_dil_image(Rect(Roi, 0, ero_dil_image.cols - Roi, ero_dil_image.rows));


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

		int last_upPoint = upPoint;
		int last_downPoint = downPoint;

		//先找上边缘点，边缘跟踪算法
		for (size_t lie = 0; lie < dstOut.cols; lie++)   //x方向
		{
			uchar *image1 = dstOut.data;
			image1 += step * (last_upPoint - N);
			int get_w2b_flag = 0;
			for (size_t i = 0; i < 2 * N; i++)
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
					image2 += step * (last_upPoint - N + 2 * N);
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
							upVector.push_back(Point(lie + Roi, last_upPoint - N + 2 * N + i));
							last_upPoint = last_upPoint - N + 2 * N + i;
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
		for (size_t lie = 0; lie < dstOut.cols; lie++)   //x方向
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

		Mat srcImage = imread("../picture/2345.bmp", 1);//在原图3通道图中画出边缘
		for (int i = 1; i < upVector.size(); i++)
		{
			line(srcImage, Point(upVector[i - 1].x, upVector[i - 1].y + 25), Point(upVector[i].x, upVector[i].y + 25), Scalar(0, 0, 255), 3);
		}
		for (int i = 1; i < downVector.size(); i++)
		{
			line(srcImage, Point(downVector[i - 1].x, downVector[i - 1].y - 25), Point(downVector[i].x, downVector[i].y - 25), Scalar(0, 0, 255), 3);
		}

		Mat Fshow = srcImage(Rect(/*460*/0, 0, srcImage.cols /*- 460*/, srcImage.rows));
		// 显示图像
		ShowImageInWnd(Fshow, rect_pic_original_opencv2.width, rect_pic_original_opencv2.height, IMG_WND_IDC_ORIGINAL_IMAGE2);
	}
	else
	{
		CString str = "图像未拼接";
		MessageBox(str);
	}
	
}

Mat Calibrate(float Col[], float Row[], float qx[], float qy[], int Length)
{
	vector<Point2f> _image_piont_;
	vector<Point2f> _real_piont_;
	for (size_t i = 0; i < Length; i++)
	{
		_image_piont_.push_back(Point2f(Col[i], Row[i]));
		_real_piont_.push_back(Point2f(qx[i], qy[i]));
	}

	Mat _affine_matrix_temp_ = estimateRigidTransform(_image_piont_, _real_piont_, true);
	return _affine_matrix_temp_;
}

void CMFCA_TestDlg::OnBnClickedCalibrate()
{
	// TODO:  在此添加控件通知处理程序代码
	float Row[20] = { 199.784, 197.699, 196.245, 194.81, 193.688, 392.606, 391.131, 389.437, 388.058, 385.681, 585.32, 584.094, 582.881, 580.971, 578.525, 776.681, 775.216, 773.607, 772.545, 770.286 };
	float Col[20] = { 127.517, 319.141, 511.448, 704.252, 896.429, 128.496, 320.039, 512.104, 705.058, 896.768, 128.672, 320.768, 512.682, 705.221, 897.512, 130.894, 322.131, 513.916, 705.925, 897.175 };

	float qx[20] = { 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300, 100, 150, 200, 250, 300 };
	float qy[20] = { 200, 200, 200, 200, 200, 250, 250, 250, 250, 250, 300, 300, 300, 300, 300, 350, 350, 350, 350, 350 };

	_affine_matrix_ = Calibrate(Col, Row, qx, qy, 20);
	if (_affine_matrix_.data != NULL)
	{
		CString str = "标定成功";
		MessageBox(str);
	}
}

void CMFCA_TestDlg::OnBnClickedCalsave()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_affine_matrix_.data != NULL)
	{
		FILE *calibrateFile;
		fopen_s(&calibrateFile, "calibrateFile.txt", "w");
		for (size_t i = 0; i < 6; i++)
		{
			if (i<2)
				fprintf(calibrateFile, "%f  ", _affine_matrix_.at<double>(i));
			else if (i==2)
				fprintf(calibrateFile, "%f\n", _affine_matrix_.at<double>(i));
			else
				fprintf(calibrateFile, "%f  ", _affine_matrix_.at<double>(i));
		}
		fclose(calibrateFile);
		CString str = "数据保存成功";
		MessageBox(str);
	}
	else
	{
		CString str = "没有标定数据";
		MessageBox(str);
	}
}

void CMFCA_TestDlg::OnBnClickedSaveline()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_affine_matrix_.data != NULL)
	{
		if (upVector.size() != 0 && downVector.size() != 0)
		{
			FILE *upFile;
			FILE *downFile;
			fopen_s(&upFile, "Calibrate_up.txt", "w");
			fopen_s(&downFile, "Calibrate_down.txt", "w");
			vector <Point2f> upVector_Real, downVector_Real;
			for (int i = 0; i < upVector.size(); i++)
			{
				Point2f temp;
				temp.x = _affine_matrix_.at<double>(0)* upVector[i].x + _affine_matrix_.at<double>(1)* upVector[i].y + _affine_matrix_.at<double>(2);
				temp.y = _affine_matrix_.at<double>(3)* upVector[i].x + _affine_matrix_.at<double>(4)* upVector[i].y + _affine_matrix_.at<double>(5);
				upVector_Real.push_back(temp);
				fprintf(upFile, "[%.3f, %.3f]\n", temp.x, temp.y);
			}
			for (int i = 0; i < downVector.size(); i++)
			{
				Point2f temp;
				temp.x = _affine_matrix_.at<double>(0)* downVector[i].x + _affine_matrix_.at<double>(1)* downVector[i].y + _affine_matrix_.at<double>(2);
				temp.y = _affine_matrix_.at<double>(3)* downVector[i].x + _affine_matrix_.at<double>(4)* downVector[i].y + _affine_matrix_.at<double>(5);
				downVector_Real.push_back(temp);
				fprintf(downFile, "[%.3f, %.3f]\n", temp.x, temp.y);
			}
			fclose(upFile);
			fclose(downFile);

			//间隔0.5mm差分提取
			for (int i = 0; i < upVector_Real.size() - 1; i++)
			{
				while (abs(upVector_Real[i].x - upVector_Real[i + 1].x) < 0.5)
				{
					upVector_Real.erase(upVector_Real.begin() + i + 1);
					if (i == upVector_Real.size() - 1)
						break;
				}
			}
			for (int i = 0; i < downVector_Real.size() - 1; i++)
			{
				while (abs(downVector_Real[i].x - downVector_Real[i + 1].x) < 0.5)
				{
					downVector_Real.erase(downVector_Real.begin() + i + 1);
					if (i == downVector_Real.size() - 1)
						break;
				}
			}

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

			CString str = "机械路径保存成功";
			MessageBox(str);
		}
		else
		{
			CString str = "未提取到边缘";
			MessageBox(str);
		}
	}
	else
	{
		CString str = "没有标定数据，先做相机标定";
		MessageBox(str);
	}
}



