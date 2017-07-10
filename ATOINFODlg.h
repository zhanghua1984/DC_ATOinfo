// ATOINFODlg.h : header file
//

#if !defined(AFX_ATOINFODLG_H__8BDEAE62_2FE4_485D_BE19_1D6A6212CC75__INCLUDED_)
#define AFX_ATOINFODLG_H__8BDEAE62_2FE4_485D_BE19_1D6A6212CC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CATOINFODlg dialog

class CATOINFODlg : public CDialog
{
// Construction
public:
	BOOL CRC16CHECK(unsigned char *pchMsg,unsigned int wDataLen ,unsigned int wCrcLen);
	void SeeData();
	CString VariantToCString(const _variant_t &var);
	void SearchCode(WORD m_wordcode);
	void SearchCode(BYTE m_bytecode);
	void SearchCode(int m_ncode);
	BOOL OpenRecordSet(_RecordsetPtr &recPtr ,CString &strSQL);
	void FrameParse(byte * m_frame);
	BOOL ConnectionDB();
	void InitFrame();
	void Init();
	LRESULT OnThreadRXMessage(WPARAM wParam,LPARAM lParam);
	CATOINFODlg(CWnd* pParent = NULL);	// standard constructor
	int	m_nmsgstatus;
	int m_ntargetaddr;
	CString m_strshowRX;
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	int m_nFramestyle;
	int m_nDataByte;
	byte m_nSeeDataAA1;
	byte m_nSeeDataAA2;
	byte m_nSeeDataAA3;
	byte m_nSeeDataAA4;
	int m_nSeeDataB;



// Dialog Data
	//{{AFX_DATA(CATOINFODlg)
	enum { IDD = IDD_ATOINFO_DIALOG };
	CEdit	m_CSeeDataAA4;
	CEdit	m_CSeeDataAA3;
	CEdit	m_CSeeDataAA2;
	CEdit	m_CSeeDataAA1;
	CComboBox	m_CBByte4;
	CComboBox	m_CBByte3;
	CComboBox	m_CBByte2;
	CComboBox	m_CBByte1;
	CComboBox	m_CAByte4;
	CComboBox	m_CAByte3;
	CComboBox	m_CAByte2;
	CComboBox	m_CAByte1;
	CComboBox	m_CDataBByte4;
	CComboBox	m_CDataBByte3;
	CComboBox	m_CDataBByte2;
	CComboBox	m_CDataBByte1;
	CComboBox	m_CDataAByte4;
	CComboBox	m_CDataAByte3;
	CComboBox	m_CDataAByte2;
	CComboBox	m_CDataAByte1;
	CEdit	m_CSeeDataB;
	CEdit	m_CData1;
	CEdit	m_CDecData;
	CEdit	m_CHexData;
	CRichEditCtrl	m_Cdebuginfo;
	CComboBox	m_CDataBit;
	CComboBox	m_CFrameStyle;
	CRichEditCtrl	m_CTX;
	CButton	m_CcomButton;
	CEdit	m_CRXCounterByte;
	CRichEditCtrl	m_CRX;
	CComboBox	m_CLocalAddr;
	CComboBox	m_CTargetAddr;
	CComboBox	m_Cmode;
	CComboBox	m_Ccomportnumber;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATOINFODlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CATOINFODlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnReceiveAComPort(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnWriteComPortMSG(WPARAM wParam, LPARAM lParam); 
	afx_msg void OnButtoncom();
	afx_msg void OnSelchangeCombocomportnumber();
	afx_msg void OnSelchangeCombomode();
	afx_msg void OnSelchangeCombotarget();
	afx_msg void OnSelchangeCombolocal();
	afx_msg void OnSelchangeComboframestyle();
	afx_msg void OnSelchangeCombodatabit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATOINFODLG_H__8BDEAE62_2FE4_485D_BE19_1D6A6212CC75__INCLUDED_)
