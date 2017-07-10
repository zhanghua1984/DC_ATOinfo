// ATOINFO.h : main header file for the ATOINFO application
//

#if !defined(AFX_ATOINFO_H__37CE23F2_4D35_4AF8_8E25_9112E6BF79FA__INCLUDED_)
#define AFX_ATOINFO_H__37CE23F2_4D35_4AF8_8E25_9112E6BF79FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "ATOINFODlg.h"
/////////////////////////////////////////////////////////////////////////////
// CATOINFOApp:
// See ATOINFO.cpp for the implementation of this class
//

class CATOINFOApp : public CWinApp
{
public:
	CATOINFOApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATOINFOApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CATOINFOApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATOINFO_H__37CE23F2_4D35_4AF8_8E25_9112E6BF79FA__INCLUDED_)
