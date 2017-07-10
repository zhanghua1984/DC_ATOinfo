// ATOINFODlg.cpp : implementation file
//

#include "stdafx.h"
#include "ATOINFO.h"
#include "ATOINFODlg.h"
#include "Includes.h"
#include "COMPORT.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL	m_bWorking;
BOOL	m_bHighSpeed;
extern BOOL	m_bThreadRXrunning;
extern BOOL	m_bThreadTXrunning;
extern HANDLE hCom; //串口
extern  BYTE	m_byteRXbuffer[BUFFERLENTH];
BYTE	m_byteFrame[24];
extern BYTE	m_byteWriteFrame1[24];
extern BYTE	m_byteWriteFrame2[24];
extern BYTE	m_byteWriteFrame3[24];
extern BYTE	m_byteWriteFrame4[24];
BOOL	m_bConnectCom;	//串口是否连接
BOOL	m_bDealing;
extern BOOL	m_bSendPackage;
#define MAXQSIZE 10000




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CATOINFODlg dialog

CATOINFODlg::CATOINFODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CATOINFODlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CATOINFODlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CATOINFODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CATOINFODlg)
	DDX_Control(pDX, IDC_EDITSEEDATAA4, m_CSeeDataAA4);
	DDX_Control(pDX, IDC_EDITSEEDATAA3, m_CSeeDataAA3);
	DDX_Control(pDX, IDC_EDITSEEDATAA2, m_CSeeDataAA2);
	DDX_Control(pDX, IDC_EDITSEEDATAA1, m_CSeeDataAA1);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEBBYTE4, m_CBByte4);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEBBYTE3, m_CBByte3);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEBBYTE2, m_CBByte2);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEBBYTE1, m_CBByte1);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEABYTE4, m_CAByte4);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEABYTE3, m_CAByte3);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEABYTE2, m_CAByte2);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLEABYTE1, m_CAByte1);
	DDX_Control(pDX, IDC_COMBODATABITBBYTE4, m_CDataBByte4);
	DDX_Control(pDX, IDC_COMBODATABITBBYTE3, m_CDataBByte3);
	DDX_Control(pDX, IDC_COMBODATABITBBYTE2, m_CDataBByte2);
	DDX_Control(pDX, IDC_COMBODATABITBBYTE1, m_CDataBByte1);
	DDX_Control(pDX, IDC_COMBODATABITABYTE4, m_CDataAByte4);
	DDX_Control(pDX, IDC_COMBODATABITABYTE3, m_CDataAByte3);
	DDX_Control(pDX, IDC_COMBODATABITABYTE2, m_CDataAByte2);
	DDX_Control(pDX, IDC_COMBODATABITABYTE1, m_CDataAByte1);
	DDX_Control(pDX, IDC_EDITSEEDATAB, m_CSeeDataB);
	DDX_Control(pDX, IDC_DATA1, m_CData1);
	DDX_Control(pDX, IDC_DECDATA, m_CDecData);
	DDX_Control(pDX, IDC_HEXDATA, m_CHexData);
	DDX_Control(pDX, IDC_RICHEDITDEBUGINFO, m_Cdebuginfo);
	DDX_Control(pDX, IDC_COMBODATABIT, m_CDataBit);
	DDX_Control(pDX, IDC_COMBOFRAMESTYLE, m_CFrameStyle);
	DDX_Control(pDX, IDC_RICHEDITTX, m_CTX);
	DDX_Control(pDX, IDC_BUTTONCOM, m_CcomButton);
	DDX_Control(pDX, IDC_EDITRX, m_CRXCounterByte);
	DDX_Control(pDX, IDC_RICHEDITRX, m_CRX);
	DDX_Control(pDX, IDC_COMBOLOCAL, m_CLocalAddr);
	DDX_Control(pDX, IDC_COMBOTARGET, m_CTargetAddr);
	DDX_Control(pDX, IDC_COMBOMODE, m_Cmode);
	DDX_Control(pDX, IDC_COMBOCOMPORTNUMBER, m_Ccomportnumber);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CATOINFODlg, CDialog)
	//{{AFX_MSG_MAP(CATOINFODlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UCOM_WRITE,OnWriteComPortMSG)
	ON_BN_CLICKED(IDC_BUTTONCOM, OnButtoncom)
	ON_CBN_SELCHANGE(IDC_COMBOCOMPORTNUMBER, OnSelchangeCombocomportnumber)
	ON_CBN_SELCHANGE(IDC_COMBOMODE, OnSelchangeCombomode)
	ON_CBN_SELCHANGE(IDC_COMBOTARGET, OnSelchangeCombotarget)
	ON_CBN_SELCHANGE(IDC_COMBOLOCAL, OnSelchangeCombolocal)
	ON_CBN_SELCHANGE(IDC_COMBOFRAMESTYLE, OnSelchangeComboframestyle)
	ON_CBN_SELCHANGE(IDC_COMBODATABIT, OnSelchangeCombodatabit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RX,OnThreadRXMessage)
	ON_MESSAGE(WM_FINDAVACOMPORT,OnReceiveAComPort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CATOINFODlg message handlers

BOOL CATOINFODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	Init();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CATOINFODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CATOINFODlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CATOINFODlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
LRESULT CATOINFODlg::OnThreadRXMessage(WPARAM wParam,LPARAM lParam)		//接收串口消息
{
	static int m_snc=0;
	m_snc+=wParam;
	CString m_temp;
	m_temp.Format("%d",m_snc);
	m_CRXCounterByte.SetWindowText(m_temp);

	for (int i=0;i<24;i++)
	{
		m_byteFrame[i]=m_byteRXbuffer[i];
		m_temp.Format("%02X",m_byteRXbuffer[i]);
		m_strshowRX+=m_temp;
	}

	//命令解析
	FrameParse(m_byteFrame);
	m_strshowRX+="\r\n";
	m_CRX.SetSel(-1, -1);
	m_CRX.ReplaceSel( (LPCTSTR)m_strshowRX);
	m_CRX.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	m_strshowRX.Empty();

	/*

	//判断缓冲区内是否够一帧数据
	for (int m_nc=QueueLenth(q);m_nc>=24;)
	{
		//出队 检查是否收到0xfc
		if(DeQueue(q,m_byteFrame[0])!=0)
		{
			AfxMessageBox("读缓冲区失败");
		}
		if(DeQueue(q,m_byteFrame[1])!=0)
		{
			AfxMessageBox("读缓冲区失败");
		}

		if ((m_byteFrame[0]==0xfc)&&(m_byteFrame[1]==0x0c))
		{
			m_strshowRX+="FC0C";
			for (int j=2;j<23;j++)
			{
				DeQueue(q,m_byteFrame[j]);
				m_temp.Format("%02X",m_byteFrame[j]);
				m_strshowRX+=m_temp;
			}
			//命令解析
			FrameParse(m_byteFrame);
			m_strshowRX+="\r\n";
			m_CRX.SetSel(-1, -1);
			m_CRX.ReplaceSel( (LPCTSTR)m_strshowRX);
			m_CRX.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
			m_nc-=24;
			m_strshowRX.Empty();
		}
		else
		{
			m_nc=m_nc-2;
		}
	}

	if (m_strshowRX.GetLength()>1024)
	{
		m_strshowRX.Empty();
	}
	*/
	//以上算法中用到内存拷贝，速度缓慢

	/*
	//指针算法
	byte * m_bytep;
	m_bytep=&m_byteRXbuffer[0];

	m_bDealing=TRUE;	//屏蔽COM口再次写入数据
	for (int i=0;i<(int)wParam;i++)
	{

		if (0xFC==*m_bytep)		//找到帧开始头
		{
			m_byteFrame[0]=0xFC;
			i++;
			m_bytep++;
			if (0x0C==*m_bytep)		//帧头确认
			{
				m_byteFrame[1]=0x0C;
				i++;
				m_bytep++;
				m_strshowRX+="FC0C";
				for (int j=2;j<24;j++)	//连续读取22个字节
				{
					m_byteFrame[j]=*m_bytep;


					i++;
					m_bytep++;
				
					m_temp.Format("%02X",m_byteFrame[j]);
					m_strshowRX+=m_temp;
				}
				/*
				if (CRC16CHECK(m_byteFrame,22,2))
				{			
					//命令解析
					FrameParse(m_byteFrame);
					m_strshowRX+="\r\n";
					m_CRX.SetSel(-1, -1);
					m_CRX.ReplaceSel( (LPCTSTR)m_strshowRX);
					m_CRX.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
					m_strshowRX.Empty();
				}
				*/
			//	if (0xFC==*m_bytep)
			//	{			
			/*
					//命令解析
					FrameParse(m_byteFrame);
					m_strshowRX+="\r\n";
					m_CRX.SetSel(-1, -1);
					m_CRX.ReplaceSel( (LPCTSTR)m_strshowRX);
					m_CRX.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
					m_strshowRX.Empty();
					m_bytep--;
			//	}
			//	else
			//	{
			//		m_bytep-=22;
			//	}
			}
		}
		m_bytep++;

	}
	m_bDealing=FALSE;
	*/

	return 0;
}

void CATOINFODlg::Init()//初始化数据
{
	CCOMPORT m_comport;
	m_comport.InitPort();	//串口初始化
	m_Cmode.InsertString(0,"应用");
	m_Cmode.InsertString(1,"平台");
	m_Cmode.InsertString(2,"公用");
	m_Cmode.InsertString(3,"解码");
	m_Cmode.InsertString(4,"Flash Clear1");
	m_Cmode.InsertString(5,"Flash Clear2");

	m_Cmode.SetCurSel(0);
	m_nmsgstatus=0;
	// 		ATP_MPM_A地址：0x93
	//		ATP_MPM_B地址：0xA3
	// 		ATP_MPM_C地址：0xB3
	// 		ATP_MPM_D地址：0xC3
	// 		ATO_MPM地址：  0xD3

	m_CTargetAddr.InsertString(0,"ATP_MPM_A");
	m_CTargetAddr.InsertString(1,"ATP_MPM_B");
	m_CTargetAddr.InsertString(2,"ATP_MPM_C");
	m_CTargetAddr.InsertString(3,"ATP_MPM_D");
	m_CTargetAddr.InsertString(4,"ATP_MPM");
	m_CTargetAddr.SetCurSel(0);
	m_ntargetaddr=0;
	
	m_CLocalAddr.InsertString(0,"0x02");
	m_CLocalAddr.SetCurSel(0);

	m_CFrameStyle.InsertString(0,"70");
	m_CFrameStyle.InsertString(1,"E0");
	m_CFrameStyle.InsertString(2,"E1");
	m_CFrameStyle.InsertString(3,"E2");

	m_CFrameStyle.SetCurSel(0);

	m_CDataBit.InsertString(0,"DATABYTE 1");
	m_CDataBit.InsertString(1,"DATABYTE 2");
	m_CDataBit.InsertString(2,"DATABYTE 3");
	m_CDataBit.InsertString(3,"DATABYTE 4");
	m_CDataBit.InsertString(4,"DATABYTE 5");
	m_CDataBit.InsertString(5,"DATABYTE 6");
	m_CDataBit.InsertString(6,"DATABYTE 7");
	m_CDataBit.InsertString(7,"DATABYTE 8");
	m_CDataBit.InsertString(8,"DATABYTE 9");
	m_CDataBit.InsertString(9,"DATABYTE 10");
	m_CDataBit.InsertString(10,"DATABYTE 11");
	m_CDataBit.InsertString(11,"DATABYTE 12");
	m_CDataBit.InsertString(12,"DATABYTE 13");
	m_CDataBit.InsertString(13,"DATABYTE 14");
	m_CDataBit.InsertString(14,"DATABYTE 15");
	m_CDataBit.InsertString(15,"DATABYTE 16");
	m_CDataBit.InsertString(16,"DATABYTE 1+2");
	m_CDataBit.InsertString(17,"DATABYTE 3+4");
	m_CDataBit.InsertString(18,"DATABYTE 5+6");
	m_CDataBit.InsertString(19,"DATABYTE 7+8");
	m_CDataBit.InsertString(20,"DATABYTE 9+10");
	m_CDataBit.InsertString(21,"DATABYTE 11+12");
	m_CDataBit.InsertString(22,"DATABYTE 13+14");
	m_CDataBit.InsertString(23,"DATABYTE 15+16");
	m_CDataBit.InsertString(24,"DATABYTE 2+3");
	m_CDataBit.InsertString(25,"DATABYTE 4+5");
	m_CDataBit.InsertString(26,"DATABYTE 6+7");
	m_CDataBit.InsertString(27,"DATABYTE 8+9");
	m_CDataBit.InsertString(28,"DATABYTE 10+11");
	m_CDataBit.InsertString(29,"DATABYTE 12+13");
	m_CDataBit.InsertString(30,"DATABYTE 14+15");
	m_CDataBit.SetCurSel(0);

	InitFrame();

	if (!ConnectionDB())
	{
		AfxMessageBox("数据库连接失败");
	}


	m_nFramestyle=0;
	m_nDataByte=0;

	m_nSeeDataAA1=0;
	m_nSeeDataB=0;
	CString m_str;
	m_str.Format("%02X",m_nSeeDataAA1);
	m_CSeeDataAA1.SetWindowText(m_str);
	
	m_str.Format("%08X",m_nSeeDataB);
	m_CSeeDataB.SetWindowText(m_str);
	
	m_CAByte1.InsertString(0,"70");
	m_CAByte1.InsertString(1,"E0");
	m_CAByte1.InsertString(2,"E1");
	m_CAByte1.InsertString(3,"E2");
	m_CAByte1.SetCurSel(0);

	m_CAByte2.InsertString(0,"70");
	m_CAByte2.InsertString(1,"E0");
	m_CAByte2.InsertString(2,"E1");
	m_CAByte2.InsertString(3,"E2");
	m_CAByte2.SetCurSel(0);

	m_CAByte3.InsertString(0,"70");
	m_CAByte3.InsertString(1,"E0");
	m_CAByte3.InsertString(2,"E1");
	m_CAByte3.InsertString(3,"E2");
	m_CAByte3.SetCurSel(0);

	m_CAByte4.InsertString(0,"70");
	m_CAByte4.InsertString(1,"E0");
	m_CAByte4.InsertString(2,"E1");
	m_CAByte4.InsertString(3,"E2");
	m_CAByte4.SetCurSel(0);

	m_CBByte1.InsertString(0,"70");
	m_CBByte1.InsertString(1,"E0");
	m_CBByte1.InsertString(2,"E1");
	m_CBByte1.InsertString(3,"E2");
	m_CBByte1.SetCurSel(0);

	m_CBByte2.InsertString(0,"70");
	m_CBByte2.InsertString(1,"E0");
	m_CBByte2.InsertString(2,"E1");
	m_CBByte2.InsertString(3,"E2");
	m_CBByte2.SetCurSel(0);
	
	m_CBByte3.InsertString(0,"70");
	m_CBByte3.InsertString(1,"E0");
	m_CBByte3.InsertString(2,"E1");
	m_CBByte3.InsertString(3,"E2");
	m_CBByte3.SetCurSel(0);
	
	m_CBByte4.InsertString(0,"70");
	m_CBByte4.InsertString(1,"E0");
	m_CBByte4.InsertString(2,"E1");
	m_CBByte4.InsertString(3,"E2");
	m_CBByte4.SetCurSel(0);
	
	

	m_CDataAByte1.InsertString(0,"BYTE 1");
	m_CDataAByte1.InsertString(1,"BYTE 2");
	m_CDataAByte1.InsertString(2,"BYTE 3");
	m_CDataAByte1.InsertString(3,"BYTE 4");
	m_CDataAByte1.InsertString(4,"BYTE 5");
	m_CDataAByte1.InsertString(5,"BYTE 6");
	m_CDataAByte1.InsertString(6,"BYTE 7");
	m_CDataAByte1.InsertString(7,"BYTE 8");
	m_CDataAByte1.InsertString(8,"BYTE 9");
	m_CDataAByte1.InsertString(9,"BYTE 10");
	m_CDataAByte1.InsertString(10,"BYTE 11");
	m_CDataAByte1.InsertString(11,"BYTE 12");
	m_CDataAByte1.InsertString(12,"BYTE 13");
	m_CDataAByte1.InsertString(13,"BYTE 14");
	m_CDataAByte1.InsertString(14,"BYTE 15");
	m_CDataAByte1.InsertString(15,"BYTE 16");
	m_CDataAByte1.SetCurSel(0);

	m_CDataAByte2.InsertString(0,"BYTE 1");
	m_CDataAByte2.InsertString(1,"BYTE 2");
	m_CDataAByte2.InsertString(2,"BYTE 3");
	m_CDataAByte2.InsertString(3,"BYTE 4");
	m_CDataAByte2.InsertString(4,"BYTE 5");
	m_CDataAByte2.InsertString(5,"BYTE 6");
	m_CDataAByte2.InsertString(6,"BYTE 7");
	m_CDataAByte2.InsertString(7,"BYTE 8");
	m_CDataAByte2.InsertString(8,"BYTE 9");
	m_CDataAByte2.InsertString(9,"BYTE 10");
	m_CDataAByte2.InsertString(10,"BYTE 11");
	m_CDataAByte2.InsertString(11,"BYTE 12");
	m_CDataAByte2.InsertString(12,"BYTE 13");
	m_CDataAByte2.InsertString(13,"BYTE 14");
	m_CDataAByte2.InsertString(14,"BYTE 15");
	m_CDataAByte2.InsertString(15,"BYTE 16");
	m_CDataAByte2.SetCurSel(0);

	m_CDataAByte3.InsertString(0,"BYTE 1");
	m_CDataAByte3.InsertString(1,"BYTE 2");
	m_CDataAByte3.InsertString(2,"BYTE 3");
	m_CDataAByte3.InsertString(3,"BYTE 4");
	m_CDataAByte3.InsertString(4,"BYTE 5");
	m_CDataAByte3.InsertString(5,"BYTE 6");
	m_CDataAByte3.InsertString(6,"BYTE 7");
	m_CDataAByte3.InsertString(7,"BYTE 8");
	m_CDataAByte3.InsertString(8,"BYTE 9");
	m_CDataAByte3.InsertString(9,"BYTE 10");
	m_CDataAByte3.InsertString(10,"BYTE 11");
	m_CDataAByte3.InsertString(11,"BYTE 12");
	m_CDataAByte3.InsertString(12,"BYTE 13");
	m_CDataAByte3.InsertString(13,"BYTE 14");
	m_CDataAByte3.InsertString(14,"BYTE 15");
	m_CDataAByte3.InsertString(15,"BYTE 16");
	m_CDataAByte3.SetCurSel(0);

	m_CDataAByte4.InsertString(0,"BYTE 1");
	m_CDataAByte4.InsertString(1,"BYTE 2");
	m_CDataAByte4.InsertString(2,"BYTE 3");
	m_CDataAByte4.InsertString(3,"BYTE 4");
	m_CDataAByte4.InsertString(4,"BYTE 5");
	m_CDataAByte4.InsertString(5,"BYTE 6");
	m_CDataAByte4.InsertString(6,"BYTE 7");
	m_CDataAByte4.InsertString(7,"BYTE 8");
	m_CDataAByte4.InsertString(8,"BYTE 9");
	m_CDataAByte4.InsertString(9,"BYTE 10");
	m_CDataAByte4.InsertString(10,"BYTE 11");
	m_CDataAByte4.InsertString(11,"BYTE 12");
	m_CDataAByte4.InsertString(12,"BYTE 13");
	m_CDataAByte4.InsertString(13,"BYTE 14");
	m_CDataAByte4.InsertString(14,"BYTE 15");
	m_CDataAByte4.InsertString(15,"BYTE 16");
	m_CDataAByte4.SetCurSel(0);


	
	m_CDataBByte1.InsertString(0,"BYTE 1");
	m_CDataBByte1.InsertString(1,"BYTE 2");
	m_CDataBByte1.InsertString(2,"BYTE 3");
	m_CDataBByte1.InsertString(3,"BYTE 4");
	m_CDataBByte1.InsertString(4,"BYTE 5");
	m_CDataBByte1.InsertString(5,"BYTE 6");
	m_CDataBByte1.InsertString(6,"BYTE 7");
	m_CDataBByte1.InsertString(7,"BYTE 8");
	m_CDataBByte1.InsertString(8,"BYTE 9");
	m_CDataBByte1.InsertString(9,"BYTE 10");
	m_CDataBByte1.InsertString(10,"BYTE 11");
	m_CDataBByte1.InsertString(11,"BYTE 12");
	m_CDataBByte1.InsertString(12,"BYTE 13");
	m_CDataBByte1.InsertString(13,"BYTE 14");
	m_CDataBByte1.InsertString(14,"BYTE 15");
	m_CDataBByte1.InsertString(15,"BYTE 16");
	m_CDataBByte1.SetCurSel(0);
	
	m_CDataBByte2.InsertString(0,"BYTE 1");
	m_CDataBByte2.InsertString(1,"BYTE 2");
	m_CDataBByte2.InsertString(2,"BYTE 3");
	m_CDataBByte2.InsertString(3,"BYTE 4");
	m_CDataBByte2.InsertString(4,"BYTE 5");
	m_CDataBByte2.InsertString(5,"BYTE 6");
	m_CDataBByte2.InsertString(6,"BYTE 7");
	m_CDataBByte2.InsertString(7,"BYTE 8");
	m_CDataBByte2.InsertString(8,"BYTE 9");
	m_CDataBByte2.InsertString(9,"BYTE 10");
	m_CDataBByte2.InsertString(10,"BYTE 11");
	m_CDataBByte2.InsertString(11,"BYTE 12");
	m_CDataBByte2.InsertString(12,"BYTE 13");
	m_CDataBByte2.InsertString(13,"BYTE 14");
	m_CDataBByte2.InsertString(14,"BYTE 15");
	m_CDataBByte2.InsertString(15,"BYTE 16");
	m_CDataBByte2.SetCurSel(0);
	
	m_CDataBByte3.InsertString(0,"BYTE 1");
	m_CDataBByte3.InsertString(1,"BYTE 2");
	m_CDataBByte3.InsertString(2,"BYTE 3");
	m_CDataBByte3.InsertString(3,"BYTE 4");
	m_CDataBByte3.InsertString(4,"BYTE 5");
	m_CDataBByte3.InsertString(5,"BYTE 6");
	m_CDataBByte3.InsertString(6,"BYTE 7");
	m_CDataBByte3.InsertString(7,"BYTE 8");
	m_CDataBByte3.InsertString(8,"BYTE 9");
	m_CDataBByte3.InsertString(9,"BYTE 10");
	m_CDataBByte3.InsertString(10,"BYTE 11");
	m_CDataBByte3.InsertString(11,"BYTE 12");
	m_CDataBByte3.InsertString(12,"BYTE 13");
	m_CDataBByte3.InsertString(13,"BYTE 14");
	m_CDataBByte3.InsertString(14,"BYTE 15");
	m_CDataBByte3.InsertString(15,"BYTE 16");
	m_CDataBByte3.SetCurSel(0);
	
	m_CDataBByte4.InsertString(0,"BYTE 1");
	m_CDataBByte4.InsertString(1,"BYTE 2");
	m_CDataBByte4.InsertString(2,"BYTE 3");
	m_CDataBByte4.InsertString(3,"BYTE 4");
	m_CDataBByte4.InsertString(4,"BYTE 5");
	m_CDataBByte4.InsertString(5,"BYTE 6");
	m_CDataBByte4.InsertString(6,"BYTE 7");
	m_CDataBByte4.InsertString(7,"BYTE 8");
	m_CDataBByte4.InsertString(8,"BYTE 9");
	m_CDataBByte4.InsertString(9,"BYTE 10");
	m_CDataBByte4.InsertString(10,"BYTE 11");
	m_CDataBByte4.InsertString(11,"BYTE 12");
	m_CDataBByte4.InsertString(12,"BYTE 13");
	m_CDataBByte4.InsertString(13,"BYTE 14");
	m_CDataBByte4.InsertString(14,"BYTE 15");
	m_CDataBByte4.InsertString(15,"BYTE 16");
	m_CDataBByte4.SetCurSel(0);



}
LRESULT CATOINFODlg::OnReceiveAComPort(WPARAM wParam, LPARAM lParam)	//找到串口并添加
{
	static int m_nindex=0;
	CString m_strport;
	m_strport.Format("COM%d",lParam);
	m_Ccomportnumber.InsertString(m_nindex,m_strport);
	m_nindex++;
	m_Ccomportnumber.SetCurSel(0);
	return 0;
}

void CATOINFODlg::OnButtoncom() //串口操作按钮
{
	// TODO: Add your control notification handler code here
	if (m_bConnectCom==TRUE)	//串口关闭
	{
		m_CcomButton.SetWindowText("打开串口");
		OnSelchangeCombocomportnumber();
	} 
	else
	{
		m_CcomButton.SetWindowText("关闭串口");
		m_bConnectCom=TRUE;
		CString m_strsel;
		int  m_nselcom=m_Ccomportnumber.GetCurSel();
		m_Ccomportnumber.GetLBText(m_nselcom,m_strsel);
		m_strsel.Delete(0,3);
		CCOMPORT m_comport;
		m_comport.OpenPort(atoi(m_strsel));
	}
}

void CATOINFODlg::OnSelchangeCombocomportnumber()//选择串口号
{
	// TODO: Add your control notification handler code here
	//结束线程
	if (m_bConnectCom==TRUE)
	{
		m_bThreadRXrunning=FALSE;
		m_bThreadTXrunning=FALSE;
		CloseHandle(hCom);
		m_bConnectCom=FALSE;
		m_CcomButton.SetWindowText("打开串口");
	}
}

void CATOINFODlg::OnSelchangeCombomode()		//串口功能选择
{
	// TODO: Add your control notification handler code here
	m_nmsgstatus=m_Cmode.GetCurSel();
	switch (m_nmsgstatus)
	{
	case 0:
		{
			char m_char[]=Defcmd_UartAppOpen;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	case 1:
		{
			char m_char[]=Defcmd_UartPlanOpen;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	case 2:
		{
			char m_char[]=Defcmd_UartComOpen;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	case 3:
		{
			char m_char[]=Defcmd_DspBDebugOpen;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	case 4:
		{
			char m_char[]=Defcmd_FlashClear1;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	case 5:
		{
			char m_char[]=Defcmd_FlashClear2;
			for(int i=0;i<COMMANDLENTH;i++)
			{
				m_byteWriteFrame1[i+DATASTART]=m_char[i];
			}
			break;
		}
	}
	//填入第二帧数据
	CString m_str;
	m_CData1.GetWindowText(m_str);
	m_byteWriteFrame2[0x06]=(unsigned char)strtol(m_str,NULL,16);
	m_bSendPackage=TRUE;	//发送数据
}

void CATOINFODlg::InitFrame()//帧值初始化
{
	m_byteWriteFrame1[0x00]=FRAME_HEAD1;
	m_byteWriteFrame2[0x00]=FRAME_HEAD1;
	m_byteWriteFrame3[0x00]=FRAME_HEAD1;
	m_byteWriteFrame4[0x00]=FRAME_HEAD1;
	
	m_byteWriteFrame1[0x01]=FRAME_HEAD2;
	m_byteWriteFrame2[0x01]=FRAME_HEAD2;
	m_byteWriteFrame3[0x01]=FRAME_HEAD2;
	m_byteWriteFrame4[0x01]=FRAME_HEAD2;
	
	m_byteWriteFrame1[0x02]=LOCAL_ADDRESS;
	m_byteWriteFrame2[0x02]=LOCAL_ADDRESS;
	m_byteWriteFrame3[0x02]=LOCAL_ADDRESS;
	m_byteWriteFrame4[0x02]=LOCAL_ADDRESS;
	
	m_byteWriteFrame1[0x05]=FRAME_SEQUENCE_70;
	m_byteWriteFrame2[0x05]=FRAME_SEQUENCE_E0;
	m_byteWriteFrame3[0x05]=FRAME_SEQUENCE_E1;
	m_byteWriteFrame4[0x05]=FRAME_SEQUENCE_E2;

	// 		ATP_MPM_A地址：0x93
	//		ATP_MPM_B地址：0xA3
	// 		ATP_MPM_C地址：0xB3
	// 		ATP_MPM_D地址：0xC3
	// 		ATO_MPM地址：  0xD3
	//		ATO
	//		我是A4 下位机44
	switch (m_CTargetAddr.GetCurSel())
	{
	case 0:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_A;
			m_byteWriteFrame2[0x03]=ATP_MPM_A;
			m_byteWriteFrame3[0x03]=ATP_MPM_A;
			m_byteWriteFrame4[0x03]=ATP_MPM_A;
			break;
		}
	case 1:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_B;
			m_byteWriteFrame2[0x03]=ATP_MPM_B;
			m_byteWriteFrame3[0x03]=ATP_MPM_B;
			m_byteWriteFrame4[0x03]=ATP_MPM_B;
			break;
		}
	case 2:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_C;
			m_byteWriteFrame2[0x03]=ATP_MPM_C;
			m_byteWriteFrame3[0x03]=ATP_MPM_C;
			m_byteWriteFrame4[0x03]=ATP_MPM_C;
			break;
		}
	case 3:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_D;
			m_byteWriteFrame2[0x03]=ATP_MPM_D;
			m_byteWriteFrame3[0x03]=ATP_MPM_D;
			m_byteWriteFrame4[0x03]=ATP_MPM_D;
			break;
		}
	case 4:
		{			
			m_byteWriteFrame1[0x03]=ATP_MPM;
			m_byteWriteFrame2[0x03]=ATP_MPM;
			m_byteWriteFrame3[0x03]=ATP_MPM;
			m_byteWriteFrame4[0x03]=ATP_MPM;
			break;
		}
	}
	OnSelchangeCombomode();
}

void CATOINFODlg::OnSelchangeCombotarget() //目标板
{
	// TODO: Add your control notification handler code here
	switch (m_CTargetAddr.GetCurSel())
	{
	case 0:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_A;
			m_byteWriteFrame2[0x03]=ATP_MPM_A;
			m_byteWriteFrame3[0x03]=ATP_MPM_A;
			m_byteWriteFrame4[0x03]=ATP_MPM_A;
			break;
		}
	case 1:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_B;
			m_byteWriteFrame2[0x03]=ATP_MPM_B;
			m_byteWriteFrame3[0x03]=ATP_MPM_B;
			m_byteWriteFrame4[0x03]=ATP_MPM_B;
			break;
		}
	case 2:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_C;
			m_byteWriteFrame2[0x03]=ATP_MPM_C;
			m_byteWriteFrame3[0x03]=ATP_MPM_C;
			m_byteWriteFrame4[0x03]=ATP_MPM_C;
			break;
		}
	case 3:
		{
			m_byteWriteFrame1[0x03]=ATP_MPM_D;
			m_byteWriteFrame2[0x03]=ATP_MPM_D;
			m_byteWriteFrame3[0x03]=ATP_MPM_D;
			m_byteWriteFrame4[0x03]=ATP_MPM_D;
			break;
		}
	case 4:
		{			
			m_byteWriteFrame1[0x03]=ATP_MPM;
			m_byteWriteFrame2[0x03]=ATP_MPM;
			m_byteWriteFrame3[0x03]=ATP_MPM;
			m_byteWriteFrame4[0x03]=ATP_MPM;
			break;
		}
	}
	m_bSendPackage=TRUE;	//发送数据
}

void CATOINFODlg::OnSelchangeCombolocal() //本机地址
{
	// TODO: Add your control notification handler code here
	switch (m_CLocalAddr.GetCurSel())
	{
	case 0:
		{
			m_byteWriteFrame1[0x02]=LOCAL_ADDRESS;
			m_byteWriteFrame2[0x02]=LOCAL_ADDRESS;
			m_byteWriteFrame3[0x02]=LOCAL_ADDRESS;
			m_byteWriteFrame4[0x02]=LOCAL_ADDRESS;
			
			break;
		}
	}
	m_bSendPackage=TRUE;	//发送数据
}
LRESULT CATOINFODlg::OnWriteComPortMSG(WPARAM wParam, LPARAM lParam)//串口信息发送显示
{
	//显示发送信息
	CString m_str;
	CString m_strf1,m_strf2,m_strf3,m_strf4;
	for (int i=0;i<24;i++)
	{
		m_str.Format("%02X",m_byteWriteFrame1[i]);
		m_strf1+=m_str;
		m_str.Format("%02X",m_byteWriteFrame2[i]);
		m_strf2+=m_str;
		m_str.Format("%02X",m_byteWriteFrame3[i]);
		m_strf3+=m_str;
		m_str.Format("%02X",m_byteWriteFrame4[i]);
		m_strf4+=m_str;
	}
	m_strf1+="\r\n";
	m_strf2+="\r\n";
	m_strf3+="\r\n";
	m_strf4+="\r\n";
	
	m_str=m_strf1+m_strf2+m_strf3+m_strf4;
	//文件写入记录
	m_CTX.ReplaceSel(m_str);
	m_CTX.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	return 0;
}

BOOL CATOINFODlg::ConnectionDB()//数据库连接
{	
	m_pConnection.CreateInstance(__uuidof(Connection));

	CString m_strSQL;
	m_strSQL="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=debuginfo.mdb";
	try
	{
		m_pConnection->Open((_bstr_t)m_strSQL,"","",adModeUnknown);
	}
	catch(_com_error e)
	{
		CString m_strError;
		m_strError.Format("数据库连接异常，异常信息: %s , %s",e.Description(),e.ErrorMessage());
		AfxMessageBox(m_strError);
		return FALSE;
	}
	return TRUE;
}

void CATOINFODlg::FrameParse(byte *m_frame)//帧解析
{
	//帧序列
	switch (m_nFramestyle)
	{
	case 0:
		{
			//70帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_70)
			{
				if ((m_nDataByte>=0)&&(m_nDataByte<=15))
				{
					//第N位数据
					SearchCode((BYTE)m_byteFrame[DATASTART+m_nDataByte]);
				}
				else if ((m_nDataByte>=16)&&(m_nDataByte<=23))
				{
					//第N_N+1位数据 N为基数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-16)*2+1];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-16)*2];
					SearchCode(m_wordcode);
				}
				else if ((m_nDataByte>=24)&&(m_nDataByte<=30))
				{
					//第N_N+1位数据 N为偶数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-24)*2+2];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-24)*2+1];
					SearchCode(m_wordcode);
				}
			}
			break;
		}
	case 1:
		{
			//E0帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E0)
			{
				if ((m_nDataByte>=0)&&(m_nDataByte<=15))
				{
					//第N位数据
					SearchCode((BYTE)m_byteFrame[DATASTART+m_nDataByte]);
				}
				else if ((m_nDataByte>=16)&&(m_nDataByte<=23))
				{
					//第N_N+1位数据 N为基数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-16)*2+1];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-16)*2];
					SearchCode(m_wordcode);
				}
				else if ((m_nDataByte>=24)&&(m_nDataByte<=30))
				{
					//第N_N+1位数据 N为偶数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-24)*2+2];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-24)*2+1];
					SearchCode(m_wordcode);
				}
			}
			break;
		}
	case 2:
		{
			//E1帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E1)
			{
				if ((m_nDataByte>=0)&&(m_nDataByte<=15))
				{
					//第N位数据
					SearchCode((BYTE)m_byteFrame[DATASTART+m_nDataByte]);
				}
				else if ((m_nDataByte>=16)&&(m_nDataByte<=23))
				{
					//第N_N+1位数据 N为基数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-16)*2+1];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-16)*2];
					SearchCode(m_wordcode);
				}
				else if ((m_nDataByte>=24)&&(m_nDataByte<=30))
				{
					//第N_N+1位数据 N为偶数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-24)*2+2];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-24)*2+1];
					SearchCode(m_wordcode);
				}
			}
			break;
		}
	case 3:
		{
			//E2帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E2)
			{
				if ((m_nDataByte>=0)&&(m_nDataByte<=15))
				{
					//第N位数据
					SearchCode((BYTE)m_byteFrame[DATASTART+m_nDataByte]);
				}
				else if ((m_nDataByte>=16)&&(m_nDataByte<=23))
				{
					//第N_N+1位数据 N为基数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-16)*2+1];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-16)*2];
					SearchCode(m_wordcode);
				}
				else if ((m_nDataByte>=24)&&(m_nDataByte<=30))
				{
					//第N_N+1位数据 N为偶数
					WORD m_wordcode=m_byteFrame[DATASTART+(m_nDataByte-24)*2+2];
					m_wordcode=m_wordcode<<8;
					m_wordcode+=m_byteFrame[DATASTART+(m_nDataByte-24)*2+1];
					SearchCode(m_wordcode);
				}
			}
			break;
		}
	}

	//数据提取
	SeeData();
}

BOOL CATOINFODlg::OpenRecordSet(_RecordsetPtr &recPtr, CString &strSQL)//打开记录集
{
//	CATOINFODlg* pApp=(CATOINFODlg*) AfxGetApp();
	m_pRecordset.CreateInstance(__uuidof(Recordset));

	_variant_t sql;
	sql.SetString(strSQL);
	try
	{
		recPtr->Open(sql,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.Description());
	}

	return TRUE;
}

void CATOINFODlg::SearchCode(int m_ncode)//搜索错误码
{
	CString m_strSQL;
	_variant_t m_value;
	m_strSQL="select * from info";
	if (!OpenRecordSet(m_pRecordset,m_strSQL))
	{
		AfxMessageBox("打开表失败!");
	}
	m_pRecordset->MoveFirst();
	try
	{
		while(!m_pRecordset->adoEOF)
		{
			m_value=m_pRecordset->GetFields()->GetItem("VALUEHEX")->Value;
			m_strSQL=VariantToCString(m_value);
			if (m_ncode==(int)strtoul(m_strSQL,NULL,16))
			{
				static int m_sncounter=0;
				CString m_strtemp;
				m_strtemp.Format("%d ",m_sncounter);
				m_value=m_pRecordset->GetFields()->GetItem("CHINADECRIBE")->Value;
				m_strSQL=VariantToCString(m_value);
				m_strSQL+="\r\n";
				m_strSQL.Insert(0,m_strtemp);
				m_Cdebuginfo.ReplaceSel(m_strSQL);
				m_Cdebuginfo.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
				m_sncounter++;
				break;
			}
			m_pRecordset->MoveNext();
		}
	}
	catch(_com_error e)
	{
		AfxMessageBox("查询错误");
	}

	m_strSQL.Format("%08X",m_ncode);
	m_CHexData.SetWindowText(m_strSQL);
	m_strSQL.Format("%d",m_ncode);
	m_CDecData.SetWindowText(m_strSQL);
}

void CATOINFODlg::SearchCode(BYTE m_bytecode)//搜索错误码
{
	CString m_strSQL;
	_variant_t m_value;
	m_strSQL="select * from info";
	if (!OpenRecordSet(m_pRecordset,m_strSQL))
	{
		AfxMessageBox("打开表失败!");
	}
	m_pRecordset->MoveFirst();
	try
	{
		while(!m_pRecordset->adoEOF)
		{
			m_value=m_pRecordset->GetFields()->GetItem("VALUEHEX")->Value;
			m_strSQL=VariantToCString(m_value);
			if (m_bytecode==strtoul(m_strSQL,NULL,16))
			{
				static int m_sncounter=0;
				CString m_strtemp;
				m_strtemp.Format("%d ",m_sncounter);
				m_value=m_pRecordset->GetFields()->GetItem("CHINADECRIBE")->Value;
				m_strSQL=VariantToCString(m_value);
				m_strSQL+="\r\n";
				m_strSQL.Insert(0,m_strtemp);
				m_Cdebuginfo.ReplaceSel(m_strSQL);
				m_Cdebuginfo.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
				m_sncounter++;
				break;
			}
			m_pRecordset->MoveNext();
		}
		
	}
	catch(_com_error e)
	{
		AfxMessageBox("查询错误");
	}
	m_strSQL.Format("%02X",m_bytecode);
	m_CHexData.SetWindowText(m_strSQL);
	m_strSQL.Format("%d",m_bytecode);
	m_CDecData.SetWindowText(m_strSQL);
}

void CATOINFODlg::SearchCode(WORD m_wordcode)//搜索错误码
{
	CString m_strSQL;
	_variant_t m_value;
	m_strSQL="select * from info";
	if (!OpenRecordSet(m_pRecordset,m_strSQL))
	{
		AfxMessageBox("打开表失败!");
	}
	m_pRecordset->MoveFirst();
	try
	{
		while(!m_pRecordset->adoEOF)
		{
			m_value=m_pRecordset->GetFields()->GetItem("VALUEHEX")->Value;
			m_strSQL=VariantToCString(m_value);
			if (m_wordcode==strtoul(m_strSQL,NULL,16))
			{
				static int m_sncounter=0;
				CString m_strtemp;
				m_strtemp.Format("%d ",m_sncounter);
				m_value=m_pRecordset->GetFields()->GetItem("CHINADECRIBE")->Value;
				m_strSQL=VariantToCString(m_value);
				m_strSQL+="\r\n";
				m_strSQL.Insert(0,m_strtemp);
				m_Cdebuginfo.ReplaceSel(m_strSQL);
				m_Cdebuginfo.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
				m_sncounter++;
				break;
			}
			m_pRecordset->MoveNext();
		}
		
	}
	catch(_com_error e)
	{
		AfxMessageBox("查询错误");
	}
	m_strSQL.Format("%04X",m_wordcode);
	m_CHexData.SetWindowText(m_strSQL);
	m_strSQL.Format("%d",m_wordcode);
	m_CDecData.SetWindowText(m_strSQL);
}

CString CATOINFODlg::VariantToCString(const _variant_t &var)//数值转换
{
	CString m_strValue;
	switch(var.vt)
	{
	case VT_BSTR:
		{
			m_strValue=var.bstrVal;
			break;
		}
	case VT_LPSTR:
		{
			break;
		}
	case VT_LPWSTR:
		{
			m_strValue=(LPCTSTR)(_bstr_t)var;
			break;
		}
	case VT_I1:
		{
			break;
		}
	case VT_UI1:
		{
			m_strValue.Format("%d",var.bVal);
			break;
		}
	case VT_I2:
		{
			m_strValue.Format("%d",var.iVal);
			break;
		}
	case VT_UI2:
		{
			m_strValue.Format("%d",var.uiVal);
			break;
		}
	case VT_INT:
		{
			m_strValue.Format("%d",var.intVal);
			break;
		}
	case VT_I4:
		{
			break;
		}
	case VT_I8:
		{
			m_strValue.Format("%d",var.lVal);
			break;
		}
	case VT_UINT:
		{
			m_strValue.Format("%d",var.uintVal);
			break;
		}
	case VT_UI4:
		{
			break;
		}
	case VT_UI8:
		{
			m_strValue.Format("%d",var.ulVal);
			break;
		}
	case VT_VOID:
		{
			m_strValue.Format("%8x",var.byref);
			break;
		}
	case VT_R4:
		{
			m_strValue.Format("%.4f",var.fltVal);
			break;
		}
	case VT_R8:
		{
			m_strValue.Format("%.8f",var.dblVal);
			break;
		}
	case VT_DECIMAL:
		{
			m_strValue.Format("%.8f",(double)var);
			break;
		}
	case VT_CY:
		{
			COleCurrency cy=var.cyVal;
			m_strValue=cy.Format();
			break;
		}
	case VT_BLOB:
		{
			break;
		}
	case VT_BLOB_OBJECT:
		{
			break;
		}
	case VT_BOOL:
		{
			m_strValue=var.boolVal?"TRUE":"FALSE";
			break;
		}
	case VT_DATE:
		{
			DATE dt=var.date;
			COleDateTime da=COleDateTime(dt);
			m_strValue=da.Format("%Y-%m-%d %H:%M:%S");
			break;
		}
	case VT_NULL:
		{
			break;
		}
	case VT_EMPTY:
		{
			m_strValue="";
			break;
		}
	case VT_UNKNOWN:
		{
			m_strValue="VT_UNKNOWN";
			break;
		}
	}
	return m_strValue;
}

void CATOINFODlg::OnSelchangeComboframestyle() //帧序列号选择
{
	// TODO: Add your control notification handler code here
	m_nFramestyle=m_CFrameStyle.GetCurSel();
}

void CATOINFODlg::OnSelchangeCombodatabit() //解析位选择
{
	// TODO: Add your control notification handler code here
	m_nDataByte=m_CDataBit.GetCurSel();
}

void CATOINFODlg::SeeData()
{

	//提取数据A的字节
	//字节1
	switch (m_CAByte1.GetCurSel())
	{
	case 0:
		{
			//70帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_70)
			{
				m_nSeeDataAA1=m_byteFrame[DATASTART+m_CDataAByte1.GetCurSel()];
			}
			break;
		}
	case 1:
		{
			//E0帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E0)
			{
				m_nSeeDataAA1=m_byteFrame[DATASTART+m_CDataAByte1.GetCurSel()];
			}
			break;
		}
	case 2:
		{
			//E1帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E1)
			{
				m_nSeeDataAA1=m_byteFrame[DATASTART+m_CDataAByte1.GetCurSel()];
			}
			break;
		}
	case 3:
		{
			//E2帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E2)
			{
				m_nSeeDataAA1=m_byteFrame[DATASTART+m_CDataAByte1.GetCurSel()];
			}
			break;
		}
	}
	CString m_str;
	m_str.Format("%02X",m_nSeeDataAA1);
	m_CSeeDataAA1.SetWindowText(m_str);
	//字节2

	switch (m_CAByte2.GetCurSel())
	{
	case 0:
		{
			//70帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_70)
			{
				m_nSeeDataAA2=m_byteFrame[DATASTART+m_CDataAByte2.GetCurSel()];
			}
			break;
		}
	case 1:
		{
			//E0帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E0)
			{
				m_nSeeDataAA2=m_byteFrame[DATASTART+m_CDataAByte2.GetCurSel()];
			}
			break;
		}
	case 2:
		{
			//E1帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E1)
			{
				m_nSeeDataAA2=m_byteFrame[DATASTART+m_CDataAByte2.GetCurSel()];
			}
			break;
		}
	case 3:
		{
			//E2帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E2)
			{
				m_nSeeDataAA2=m_byteFrame[DATASTART+m_CDataAByte2.GetCurSel()];
			}
			break;
		}
	}
	m_str.Format("%02X",m_nSeeDataAA2);
	m_CSeeDataAA2.SetWindowText(m_str);
	//字节3
	
	switch (m_CAByte3.GetCurSel())
	{
	case 0:
		{
			//70帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_70)
			{
				m_nSeeDataAA3=m_byteFrame[DATASTART+m_CDataAByte3.GetCurSel()];
			}
			break;
		}
	case 1:
		{
			//E0帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E0)
			{
				m_nSeeDataAA3=m_byteFrame[DATASTART+m_CDataAByte3.GetCurSel()];
			}
			break;
		}
	case 2:
		{
			//E1帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E1)
			{
				m_nSeeDataAA3=m_byteFrame[DATASTART+m_CDataAByte3.GetCurSel()];
			}
			break;
		}
	case 3:
		{
			//E2帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E2)
			{
				m_nSeeDataAA3=m_byteFrame[DATASTART+m_CDataAByte3.GetCurSel()];
			}
			break;
		}
	}

	m_str.Format("%02X",m_nSeeDataAA3);
	m_CSeeDataAA3.SetWindowText(m_str);
	//字节4
	
	switch (m_CAByte4.GetCurSel())
	{
	case 0:
		{
			//70帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_70)
			{
				m_nSeeDataAA4=m_byteFrame[DATASTART+m_CDataAByte4.GetCurSel()];
			}
			break;
		}
	case 1:
		{
			//E0帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E0)
			{
				m_nSeeDataAA4=m_byteFrame[DATASTART+m_CDataAByte4.GetCurSel()];
			}
			break;
		}
	case 2:
		{
			//E1帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E1)
			{
				m_nSeeDataAA4=m_byteFrame[DATASTART+m_CDataAByte4.GetCurSel()];
			}
			break;
		}
	case 3:
		{
			//E2帧
			if (m_byteFrame[FRAMEPOS]==FRAME_SEQUENCE_E2)
			{
				m_nSeeDataAA4=m_byteFrame[DATASTART+m_CDataAByte4.GetCurSel()];
			}
			break;
		}
	}
	
	m_str.Format("%02X",m_nSeeDataAA4);
	m_CSeeDataAA4.SetWindowText(m_str);

}

BOOL CATOINFODlg::CRC16CHECK(unsigned char *pchMsg, unsigned int wDataLen,unsigned int wCrcLen)
{
	unsigned  int wCRCTalbeAbs[] =
	{
		0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 
			0x3C00, 0x2800, 0xE401, 0xA001, 0x6C00, 
			0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 
			0x4400
	};
	unsigned  int wCRC = 0xFFFF;
	unsigned  int i = 0;
	unsigned char chChar = 0;
	unsigned  int wDataCrc;
	for (i = 0; i < wDataLen; i++)
	{
		chChar = *pchMsg++;
		wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
		wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
	}
	
	chChar=*pchMsg++;
	wDataCrc=*pchMsg;
	wDataCrc=wDataCrc<<8;
	wDataCrc+=chChar;
	if (wCRC==wDataCrc)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}
