
// MFCA_TestDlg.h : 头文件
//

#pragma once

/* opencv相关 */
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

#define IMG_WND_IDC_ORIGINAL_IMAGE	"IDC_SHOWIMAGE1"
#define IMG_WND_IDC_ORIGINAL_IMAGE2	"IDC_SHOWIMAGE2"

#define Roi 0
#define N 10 //边缘跟踪宽度

// CMFCA_TestDlg 对话框
class CMFCA_TestDlg : public CDialogEx
{
// 构造
public:
	CMFCA_TestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCA_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;


	// 按照窗口大小绘制指定图像
	int ShowImageInWnd(Mat img_for_show, int wnd_width, int wnd_height, CString wnd_name);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadimage();
	afx_msg void OnBnClickedFitline();
	afx_msg void OnBnClickedMatching();
	afx_msg void OnBnClickedTorgether();
//	afx_msg void OnBnClickedErodil();
	afx_msg void OnBnClickedCalibrate();
	afx_msg void OnBnClickedSaveline();
	
	afx_msg void OnBnClickedCalsave();
};
