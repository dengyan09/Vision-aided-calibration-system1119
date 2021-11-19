
// BaslerKeyenceTestDemoDlg.cpp: 实现文件
//
#include "pch.h"
#include "framework.h"
#include "BaslerKeyenceTestDemo.h"
#include "BaslerKeyenceTestDemoDlg.h"
#include "afxdialogex.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include <afxsock.h>
#include <string>
#include <iostream>
#include <thread>
// 加载OpenCV API
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
//加载PYLON API.
#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigECamera.h>

#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable: 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;
using namespace Pylon;
using namespace GenApi;
using namespace Basler_GigECameraParams;
//定义在类CBaslerGigEInstantCamera的实例，后续单独封装的时候再考虑
//CBaslerGigEInstantCamera  cameraaa;
using namespace std;
SOCKET sock;//套接字

//新建一个OpenCV image对象.
Mat openCvImage;
// 创建一个Pylonlmage后续将用来创建OpenCV images
CPylonImage pylonImage;
// 新建一个OpenCV video creator对象.
VideoWriter cvVideoCreator;
//声明一个整形变量用来计数抓取的图像，以及创建文件名索引
int grabbedlmages = 0;
//定义抓取的图像数
size_t c_countOfImagesToGrab = 100;
CString TempcountOfImagesToGrab;

CString TempcountOfchangedelaytime;

//抓取结果数据指针
CGrabResultPtr ptrGrabResult;

CImageFormatConverter formatConverter;//转换器，可以设置要转换的格式===0812===邓岩===
CRect rect1;
CInstantCamera camera;

//此标志位来改变"连续发送测量命令"dy
int conflag = 1;

//此标志位来改变"连续发送测量命令"的时间间隔dy
int delaytemp = 1000;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
 
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CBaslerKeyenceTestDemoDlg 对话框

CBaslerKeyenceTestDemoDlg::CBaslerKeyenceTestDemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BASLERKEYENCETESTDEMO_DIALOG, pParent)
{
	EnableActiveAccessibility();//deng20211112
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBaslerKeyenceTestDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaslerKeyenceTestDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_Send2, &CBaslerKeyenceTestDemoDlg::OnBnClickedSend2)
	ON_BN_CLICKED(ID_Connect, &CBaslerKeyenceTestDemoDlg::OnBnClickedConnect)
	ON_BN_CLICKED(ID_Send_continue, &CBaslerKeyenceTestDemoDlg::OnBnClickedSendcontinue)
	ON_BN_CLICKED(IDC_ChangeExposeBtn, &CBaslerKeyenceTestDemoDlg::OnBnClickedChangeexposebtn)
	ON_BN_CLICKED(IDC_StopGrab_Btn, &CBaslerKeyenceTestDemoDlg::OnBnClickedStopgrabBtn)
	ON_BN_CLICKED(IDC_ConCam_Btn, &CBaslerKeyenceTestDemoDlg::OnBnClickedConcamBtn)
	ON_BN_CLICKED(ID_Send_Stopcontinue, &CBaslerKeyenceTestDemoDlg::OnBnClickedSendStopcontinue)
	ON_BN_CLICKED(IDC_ChangeGrabNumBtn, &CBaslerKeyenceTestDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_Delaytime_Btn, &CBaslerKeyenceTestDemoDlg::OnBnClickedDelaytimeBtn)
END_MESSAGE_MAP()

// CBaslerKeyenceTestDemoDlg 消息处理程序

BOOL CBaslerKeyenceTestDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
 
	//设置初始对话框大小
	CRect temprect(0, 0, 1920, 1200);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER | SWP_NOMOVE);
	
	//对话框屏幕居中显示
	CenterWindow();
	
	//通过程序改变Picture Control控件的大小(相机画面)
	CRect rect2;
	GetDlgItem(IDC_PIC_Cam)->GetWindowRect(&rect2);           //IDC_WAVE_DRAW为Picture Control的ID
	ScreenToClient(&rect2);
	GetDlgItem(IDC_PIC_Cam)->MoveWindow(rect2.left, rect2.top, 1360,850, true);    //固定Picture Control控件的大小

	/******D-----20211110基恩士测距仪初始化代码-----******/
	show_edit = (CEdit*)GetDlgItem(IDC_EDITRevwindow);
	send_edit = (CEdit*)GetDlgItem(IDC_EDIT2sendcode);
	btn_conn = (CButton*)GetDlgItem(ID_Connect);
	edit_ip = (CEdit*)GetDlgItem(IDC_EDITIL300IP);
	edit_port = (CEdit*)GetDlgItem(IDC_EDITIL300port);
	send_edit->SetFocus();

	/*====将需要抓取的相机图像数量显示在对话框中======1117===deng==*/
	TempcountOfImagesToGrab.Format("%i", c_countOfImagesToGrab);
	GetDlgItem(IDC_EDIT_CamNum)->SetWindowText(TempcountOfImagesToGrab);

	/*====将需要修改的时间间隔显示在对话框中======1117===deng==*/
	TempcountOfchangedelaytime.Format("%i", delaytemp);
	GetDlgItem(IDC_EDIT_delaytime)->SetWindowText(TempcountOfchangedelaytime);

	edit_ip->SetWindowText("192.168.2.99");//基恩士的IP::192.168.2.99
	edit_port->SetWindowText("64000");//基恩士的端口::64000
	send_edit->SetWindowText("M0");//基恩士的获取测量值指令::M0
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("IDP_SOCKETS_INIT_FAILED"));
		return FALSE;
	}

	/******D-----20211110Basler相关初始化代码-----******/
	CWnd* pWnd1 = GetDlgItem(IDC_PIC_Cam);//CWnd是MFC窗口类的基类,提供了微软基础类库中所有窗口类的基本功能。
	pWnd1->GetClientRect(&rect1);//GetClientRect为获得控件相自身的坐标大小
	cv::namedWindow("PicControl_Cam", WINDOW_AUTOSIZE);//设置窗口名
	HWND hWndl = (HWND)cvGetWindowHandle("PicControl_Cam");//hWnd 表示窗口句柄,获取窗口句柄
	HWND hParent1 = ::GetParent(hWndl);//GetParent函数一个指定子窗口的父窗口句柄
	::SetParent(hWndl, GetDlgItem(IDC_PIC_Cam)->m_hWnd);
	::ShowWindow(hParent1, SW_HIDE);//ShowWindow指定窗口中显示
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBaslerKeyenceTestDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBaslerKeyenceTestDemoDlg::OnPaint()
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
HCURSOR CBaslerKeyenceTestDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBaslerKeyenceTestDemoDlg::Update(CString str)
{
	show_edit->ReplaceSel(str + "\r\n");
}

/*基恩士DL-EN1的接收线程2021.11.15*/
UINT Recv_Th(LPVOID p)
{
	int res;//表示收到的测量数据的总个数
	char msg[1024];//字符数组用来接收测量信息，后续显示合理的距离，需要再拆分此字符数组
	char msg_temp[1024];//拆分测量数据2021.11.12
	CString str_temp;
	CBaslerKeyenceTestDemoDlg* dlg = (CBaslerKeyenceTestDemoDlg*)AfxGetApp()->GetMainWnd();
	dlg->Update("接收数据准备就绪！！！");

	while (1)
	{
		if ((res = recv(sock, msg, 1024, 0)) == -1)//recv函数:从连接的套接字接收数据。
		{
			dlg->Update("失去连接");
			break;
		}
		else
		{
			//拆分测量的数据         2021.11.15       DDDD		
			//1、判断msg[res] 是否包含 'M0' 
			string str = "M0";
			string F_M0_String = msg;//将char[]转为string类型
			if (F_M0_String.find(str) < F_M0_String.length())
			{
				//表示str是F_M0_String(msg)的一个子字符串
				//若存在“M0”,根据DL-EN1的响应格式，将“M0，”拆分出去，
				//只留下当前传感器的测量，例如：“+000012345”对应12.345
				//如下函数是截取第3位到第res位的值，得到将“M0，”去除的测量值----DY
				int i;
				for (i = 0; i + 3 < res; i++)
				{
					msg_temp[i] = msg[i + 3];
				}
			}
			else
			{
				dlg->Update("响应错误，未发现“M0”");
			}
			//2、数据处理
			//string msg_temp1 = msg_temp;//将char[]转为string类型,测量值的显示用
			//float msg_temp11 = stof(msg_temp1); //将字符串string转换为float类型
			float msg_temp11 = atof(msg_temp);//将char[]字符数组转换为float类型
			float msg_temp22 = msg_temp11 / 100; //将测量值正确显示		
			str_temp.Format(_T("%.2f"), msg_temp22);//将float类型msg_temp22转换为CString字符串str_temp，显示小数点后两位
			//msg_temp[res] = '\0';//使用空字符'\0'截断字符串，只要0到“res-1”个字符，也就是实际得到的测量响应信息
			msg_temp[res-3] = '\0';//使用空字符'\0'截断字符串，只要0到“res-1-3”个字符，也就是实际得到的测量响应信息
			dlg->Update("来自IL300的测量结果:" + CString(str_temp));

			/*向文本文件中写入文本（测量数据）2021.11.17*/
			CStdioFile  File;//CFile::modeCreate模式创建和打开一个文件，假如这个文件已存在，则会清空已经存在的文件，而modeNoTruncate，不会清空这个文件
			File.Open(_T("C:\\Users\\46263\\Desktop\\IL300MeaValue.txt"), /*CFile::modeCreate|*/CFile::modeNoTruncate|CFile::modeReadWrite);
			if (File)
			{
				File.Seek(0, CFile::end);//寻找最下端
				File.WriteString(str_temp+ "\n");//写入数据
			}
			File.Close();

		}
	}
	return 0;
}

/* 基恩士DL-EN1的连续发送数据指令的线程函数 2021.11.16*/
UINT Send_Th(LPVOID p)
{
	conflag = 1;
	CString str_temp;
	char* msg;
	
	str_temp = "M0\r\n";//M0的命令格式，需要尾部添加“CRLF”，意思是增加回车"\r"，换行"\n"===2021.11.12
	msg = str_temp.GetBuffer(str_temp.GetLength());

	CBaslerKeyenceTestDemoDlg* dlg = (CBaslerKeyenceTestDemoDlg*)AfxGetApp()->GetMainWnd();	
	dlg->Update("\r\n#####准备连续发送数据#####\r\n");
	while (conflag)
	{
		if (send(sock, msg, strlen(msg), 0) == SOCKET_ERROR)
		{
			dlg->Update(_T("发送失败\r\n"));
		}
		else
		{
			dlg->Update(_T("客户端的连续发送命令:M0\r"));//消息上屏，清空输入，并重获焦点,回车符是"\r"，换行符是"\n".
		}
		Sleep(delaytemp);//延时***ms	
	}	
	return 0;
}

/* 基恩士DL-EN1建立连接 IP*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	DL_EN1_DeviceInitial();
	
}

/* 基恩士DL-EN1 连接 IP初始化*/
void CBaslerKeyenceTestDemoDlg::DL_EN1_DeviceInitial()
{
	WSADATA wsaData;
	SOCKADDR_IN server_addr;
	WORD wVersion;
	wVersion = MAKEWORD(2, 2);
	WSAStartup(wVersion, &wsaData);

	CString ip;
	edit_ip->GetWindowText(ip);//取得服务器的IP地址
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(64000);//基恩士DL-EN1的IP
	//server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.2.99");
	//基恩士DL-EN1支持TCP 套接字1（Sockets：流步套接字（SOCK_STREAM))
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		show_edit->ReplaceSel("create socket error\r\n");
	}
	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		show_edit->ReplaceSel("连接失败\r\n");
	}
	else
	{
		show_edit->ReplaceSel("基恩士DL-EN1--连接成功！！！\r\n");
		AfxBeginThread(&Recv_Th, 0);//开始打开接收线程DDDDDDDDDD
		btn_conn->EnableWindow(FALSE);//按钮变灰
	}
}

/* 基恩士DL-EN1的发送测量数据命令*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedSend2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CString str_temp;
	char* msg;
	send_edit->GetWindowText(str);//获取“IDC_EDIT2sendcode”中的发送信息"str"
	str_temp = str +"\r\n";//M0的命令格式，需要尾部添加“CRLF”，意思是增加回车"\r"，换行"\n"===2021.11.12
	msg = str_temp.GetBuffer(str_temp.GetLength());
	//msg = str.GetBuffer(str.GetLength());
	//msg = (char*)str.GetBuffer(str.GetLength());//添加“(char*)”解决不能将LPWSTR 类型的值分配到“char”类型的实体问题

	if (send(sock, msg, strlen(msg), 0) == SOCKET_ERROR)
	{
		show_edit->ReplaceSel(_T("发送失败\r\n"));
	}

	else if (str == "")
	{
		MessageBox(_T("请输入信息"));
	}
	else
	{
		show_edit->ReplaceSel(_T("客户端的发送命令:" + str + "\r\n"));//消息上屏，清空输入，并重获焦点,回车符是"\r"，换行符是"\n".
		//send_edit->SetWindowText(_T(""));//清空发送编辑框
		//send_edit->SetWindowText((""));//清空发送编辑框
		//send_edit->SetFocus();
	}

}

/*基恩士DL-EN1开始连续发送测量命令 2021.11.16*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedSendcontinue()
{
	// TODO: 在此添加控件通知处理程序代码
	//打开连续发送测量指令的线程DY
	AfxBeginThread(&Send_Th, 0);

}

/*基恩士DL-EN1停止连续发送测量命令 2021.11.16*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedSendStopcontinue()
{
	// TODO: 在此添加控件通知处理程序代码
	//将连续发送测量指令的线程中的标志位置0
	conflag = 0;
	//AfxMessageBox("停止向基恩士DL-EN1连续发送测量命令!!!");
}


/*相机acA1920-48gc 连接 2021.11.16*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedConcamBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	////连接基恩士DL_EN1
	//DL_EN1_DeviceInitial();
	////打开连续发送测量指令的线程DY
	//AfxBeginThread(&Send_Th, 0);
	/*===邓==相机的初始化====11.16====*/
	Cam_DeviceInitial();

}

/*======相机的初始化函数====2021.11.15====*/
void  CBaslerKeyenceTestDemoDlg::Cam_DeviceInitial()
{
	//Pylon自动初始化和终止
	//Pylon::PylonAutoInitTerm autoInitTerm;

	// Before using any pylon methods, the pylon runtime must be initialized. 
	PylonInitialize();
	
	//创建相机对象（以最先识别的相机）
	CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
	// 打印相机的名称
	AfxMessageBox(camera.GetDeviceInfo().GetModelName() + camera.GetDeviceInfo().GetSerialNumber());//使用多字节字符集
	//获取相机节点映射以获得相机参数
	GenApi::INodeMap& nodemap = camera.GetNodeMap();
	//打开相机
	camera.Open();
	//获取相机成像宽度和高度
	GenApi::CIntegerPtr width = nodemap.GetNode("Width");
	GenApi::CIntegerPtr height = nodemap.GetNode("Height");

	//设置相机最大缓冲区,默认为10
	camera.MaxNumBuffer = 500;
	// 新建pylon ImageFormatConverter对象.
	CImageFormatConverter formatConverter;
	//确定输出像素格式  将相机采集的图片改成特定的图像格式==有问题修改此处==11.16===
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;

	// 视频文件名
	//std::string videoFileName = "openCvVideo.avi";
	std::string videoFileName = "openCvVideo.mp4";
	// 定义视频帧大小
	cv::Size frameSize = Size((int)width->GetValue(), (int)height->GetValue());

	//设置视频编码类型和帧率，有三种选择
	//帧率必须小于等于相机成像帧率
	//cvVideoCreator.open(videoFileName, VideoWriter::fourcc('D', 'I', 'V', 'X'), 10, frameSize, true);
	cvVideoCreator.open(videoFileName, VideoWriter::fourcc('M', 'P', '4', '2'), 20, frameSize, true);	
	//cvVideoCreator.open(videoFileName, CV_FOURCC('M', '3', 'P', 'G'), 20, frameSize, true);
	//"CV_F0URCC"在opencv4已经删除此宏，并使用VideoWriter名字空间里的fourcc代替：
	//VideoWriter::fourcc   使用方式与CV_FOURCC宏一样

	//开始抓取"c_countOfImagesToGrab" 张图片
	//相机默认设置连续抓取模式
	camera.StartGrabbing(c_countOfImagesToGrab, GrabStrategy_LatestImages);
	// 当c_countOfImagesToGrab images获取恢复成功时，Camera.StopGrabbing()被RetrieveResult()方法自动调用停止抓取

	while (camera.IsGrabbing())
		//while (1)
	{
		// 等待接收和恢复图像，超时时间设置为5000 ms.
		camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

		//如果图像抓取成功
		if (ptrGrabResult->GrabSucceeded())
		{
			// 获取图像数据
			//cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
			//cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;

			//将抓取的缓冲数据转化成pylon image.
			formatConverter.Convert(pylonImage, ptrGrabResult);

			// 将 pylon image转成OpenCV image.
			openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());

			//如果需要保存图片选择：if (saveImages)，不存图选择if (!saveImages)    #define saveImages 1
			//if (saveImages)
			if (!saveImages)
			{
				std::ostringstream s;
				// 按索引定义文件名存储图片
				s << "image_" << grabbedlmages << ".jpg";
				std::string imageName(s.str());
				//保存OpenCV image.
				imwrite("C:/Users/46263/Desktop/BaslerSaveimage/" + imageName, openCvImage);
				//imwrite(imageName, openCvImage);//直接存在当前目录（例如源文件的文件夹下）
				grabbedlmages++;
			}

			//如果需要记录视频，选择if (!recordVideo)，不记录选择if (recordVideo)      #define recordVideo 0
			//if (!recordVideo)
			if (recordVideo)
			{
				cvVideoCreator.write(openCvImage);
			}

			////新建OpenCV display window.
			//namedWindow("OpenCV Display Window", CV_WINDOW_NORMAL); // other options: CV_AUTOSIZE, CV_FREERATIO
			//   resize这个代码会影响实时显示在Piccontrol中   2021.11.17
			cv::resize(openCvImage, openCvImage, cv::Size(rect1.Width(), rect1.Height()));
			imshow("PicControl_Cam", openCvImage);

			// 在imshow之后如果没有waitKey语句则不会正常显示图像；cvWaitKey(delay)函数的功能是不断刷新图像，频率时间为delay，单位为ms。
			waitKey(1);

		}

	}
	AfxMessageBox("相机抓图结束!!!");
}

/*相机acA1920-48gc “停止采集”响应函数
只需要停止采集和断开连接，如果不断开连接，下次就无法再加载设备了。*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedStopgrabBtn()
{
	// TODO: 在此添加控件通知处理程序代码	
	camera.StopGrabbing();
	camera.Close();
	camera.DetachDevice();
	//PylonTerminate();
	//将连续发送测量指令的线程中的标志位置0
	conflag = 0;

}

/*相机acA1920-48gc 修改曝光值 2021.11.16*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedChangeexposebtn()
{
	// TODO: 在此添加控件通知处理程序代码
}

/*获取当前编辑框中的抓图数量值*/
void CBaslerKeyenceTestDemoDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码IDC_EDIT_CamNum
	
	CString ValueCamNum;
	GetDlgItem(IDC_EDIT_CamNum)->GetWindowText(ValueCamNum);

	if ("" != ValueCamNum)
	{
		int TEMPchangeLineNum = _ttoi(ValueCamNum);	//CString to int
		c_countOfImagesToGrab = TEMPchangeLineNum;	//修改相机的存图数量
		AfxMessageBox("相机的存图数量数量修改成功！！！");
	}
}


void CBaslerKeyenceTestDemoDlg::OnBnClickedDelaytimeBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Valuedelaytime;
	GetDlgItem(IDC_EDIT_delaytime)->GetWindowText(Valuedelaytime);

	if ("" != Valuedelaytime)
	{
		int TEMPchangedelaytime = _ttoi(Valuedelaytime);	//CString to int
		delaytemp = TEMPchangedelaytime;	//修改发送时间间隔
		//AfxMessageBox("发送时间间隔修改成功！！！");
	}
}
