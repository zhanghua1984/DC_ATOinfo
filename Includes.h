// Includes.h: interface for the Includes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INCLUDES_H__0A319C65_3AFD_49AD_8C84_09E646AE9D35__INCLUDED_)
#define AFX_INCLUDES_H__0A319C65_3AFD_49AD_8C84_09E646AE9D35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WM_FINDAVACOMPORT (WM_USER+1)		//�ҵ�һ����Ч���ں�
#define WM_RX	WM_USER+2
#define BUFFERLENTH	24
#define WM_UCOM_WRITE	(WM_USER+102) 
#define WM_UDISPLAY_MSG (WM_USER+103)

#define	Defcmd_UartAppOpen  		"cmd/Uart AppOp  "  /*     cmd 1         */
#define	Defcmd_UartPlanOpen 		"cmd/Uart PlanOp "	/*     cmd 2         */
#define	Defcmd_UartComOpen  		"cmd/Uart ComOp  "	/*     cmd 3         */
#define	Defcmd_DspBDebugOpen		"cmd/DspB DebugOp"	//����
#define	Defcmd_FlashClear1  		"cmd/Flash Clear1"	/*     cmd 4         */
#define	Defcmd_FlashClear2  		"cmd/Flash Clear2"	/*     cmd 5         */

// 		ATP_MPM_A��ַ��0x93;
//		ATP_MPM_B��ַ��0xA3
// 		ATP_MPM_C��ַ��0xB3
// 		ATP_MPM_D��ַ��0xC3
// 		ATO_MPM��ַ��  0xD3
//		ATO
//		����A4 ��λ��44
#define ATP_MPM_A	0x93
#define ATP_MPM_B	0xA3
#define ATP_MPM_C	0xB3
#define ATP_MPM_D	0xC3
#define ATP_MPM		0xD3
#define FRAME_HEAD1	0xFC
#define FRAME_HEAD2	0x0C

#define FRAME_SEQUENCE_70	0x70
#define FRAME_SEQUENCE_E0	0xE0
#define FRAME_SEQUENCE_E1	0xE1
#define FRAME_SEQUENCE_E2	0xE2

#define COMMANDLENTH	16
#define DATASTART	6
#define FRAMEPOS	5
#define LOCAL_ADDRESS	0x02

class Includes  
{
public:
	Includes();
	virtual ~Includes();

};

#endif // !defined(AFX_INCLUDES_H__0A319C65_3AFD_49AD_8C84_09E646AE9D35__INCLUDED_)
