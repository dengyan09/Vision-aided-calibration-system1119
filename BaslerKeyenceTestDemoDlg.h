
// BaslerKeyenceTestDemoDlg.h: 头文件
//
#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/ImagePersistence.h>

#pragma once

/*初始化接收端socket的线程*/
UINT Recv_Th(LPVOID p);
/*初始化发送socket的线程*/
UINT Send_Th(LPVOID p);

// CBaslerKeyenceTestDemoDlg 对话框
class CBaslerKeyenceTestDemoDlg : public CDialogEx
{
// 构造
public:
	CBaslerKeyenceTestDemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BASLERKEYENCETESTDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	/*邓20211110初始化代码*/

public:

	CEdit* send_edit;
	void  Update(CString str);

	void DL_EN1_DeviceInitial();//初始化基恩士DL_EN1参数
	//设置相机
	void Cam_DeviceInitial();//初始化相机参数
	
	/***********2021.11.15邓******相机设置*/
	double CAMexpose = 16000;//定义相机曝光的参数

	//定义是否保存图片
	#define saveImages 1
	//定义是否记录视频
	#define recordVideo 0
	
	

private:
	CEdit* show_edit;
	//CEdit* send_edit;
	CEdit* edit_ip;
	CEdit* edit_port;
	CButton* btn_conn;



public:
	afx_msg void OnBnClickedSend2();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSendcontinue();
	afx_msg void OnBnClickedChangeexposebtn();
	afx_msg void OnBnClickedStopgrabBtn();
	afx_msg void OnBnClickedConcamBtn();
	afx_msg void OnBnClickedSendStopcontinue();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedDelaytimeBtn();
};
