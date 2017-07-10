// COMPORT.cpp: implementation of the CCOMPORT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "COMPORT.h"
#include "Includes.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
HANDLE hCom; //����
extern BOOL m_bReady;
BYTE	m_byteWriteFrame1[24];
BYTE	m_byteWriteFrame2[24];
BYTE	m_byteWriteFrame3[24];
BYTE	m_byteWriteFrame4[24];
UINT ThreadReadCOM(LPVOID lpParam);
UINT ThreadSendCOM(LPVOID lpParam);
UINT ThreadDealCOM(LPVOID lpParam);
CWinThread* pThreadRX;
CWinThread* pThreadTX;
CWinThread* pThreadDeal;
BOOL	m_bThreadRXrunning;
BOOL	m_bThreadTXrunning;
BOOL	m_bThreadDealrunning;
BYTE	m_byteRXbuffer[BUFFERLENTH];
OVERLAPPED m_overlap;
OVERLAPPED m_osRead,m_osWrite; 
BYTE	m_bytecounter;
BOOL	m_bSendPackage;
extern BOOL	m_bWorking;
extern	BOOL	m_bHighSpeed;
extern BOOL	m_bConnectCom;
extern BOOL	m_bDealing;
#define MAXQSIZE 10000
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*����ѭ������ʵ��ѭ������*/

typedef BYTE QElemType ; //������������ 

typedef struct
{
	QElemType *base;  //ѭ�����еĴ洢�ռ� �� ������ָ��ָʾ�洢ʵ�ʳ���10�ֽڵ��ַ���) 
	int front;  //��ͷԪ���±꣨ȡԪ�أ� 
	int rear;  //��βԪ���±꣨����Ԫ�أ� 
}SqQueue;            

//����ѭ�����У�ѭ����������
int InitQueue(SqQueue &Q)
{
    Q.base=(QElemType *)malloc(MAXQSIZE*sizeof(QElemType));
    if(!Q.base) 
		return -1;  //����ʧ�ܣ�����-1 
    Q.front=0;
    Q.rear=0;
    return 0;    //�����ɹ�������0 
}

//����Ԫ�أ�����У�
int EnQueue(SqQueue &Q,QElemType e)
{
    if((Q.rear+1)%MAXQSIZE==Q.front)	//��һ���Ƿ�Ͷ�ȡָ���ػ�
		return -1;  //����
    //memcpy(Q.base[Q.rear],e);  //��ֵ
	Q.base[Q.rear]=e;
    Q.rear=(Q.rear+1)%MAXQSIZE;  //�±��һ 
    return 0;  //��ӳɹ�
}

//ȡԪ��(����ָ�룬ָ��ȡ����ֵ�� 
int DeQueue(SqQueue &Q,QElemType &e)
{
    if(Q.rear==Q.front)
        return -1;   //�ӿ�
    //memcpy(e,Q.base[Q.front]);
	e=Q.base[Q.front];
    Q.front=(Q.front+1)%MAXQSIZE; //�±��һ 
    return 0;  //�ɹ�
}  

int QueueLenth(SqQueue Q)
{
	return (Q.rear-Q.front+MAXQSIZE)%MAXQSIZE;
}

SqQueue q;  //���ݽṹ ���λ���������
int flag; 
QElemType strIn,strOut;


CCOMPORT::CCOMPORT()
{
				InitQueue(q);
}

CCOMPORT::~CCOMPORT()
{
	
}
void CCOMPORT::InitPort()
{
	m_nMinPortNumber=-1;
	//���ڳ�ʼ��,�������������д��ں�,Ĭ�ϴ򿪴��ں���С�Ĵ���
	if (SearchPort())
	{
		if (OpenPort(m_nMinPortNumber))
		{
			m_bConnectCom=TRUE;

		}
		else
		{
			m_bConnectCom=FALSE;
		}
	}
	else
	{
		AfxMessageBox("û���ҵ�����!");
	}	
}
BOOL CCOMPORT::SearchPort()
{
	BOOL m_BHaveComPort=FALSE;
	//һ����Ϊ�������ںŲ�����256
	CString m_strport;

	for (int i=1;i<=256;i++)
	{
		m_strport.Format("\\\\.\\COM%d",i);
		hCom=CreateFile(m_strport,//COM��
			GENERIC_READ|GENERIC_WRITE, //��������д
			0, //��ռ��ʽ
			NULL,
			OPEN_EXISTING, //�򿪶����Ǵ���
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //�ص���ʽ
			NULL);
		//�򿪳ɹ�
		if(hCom!=(HANDLE)-1)
		{
			if (m_BHaveComPort==FALSE)
			{
				m_BHaveComPort=TRUE;
				m_nMinPortNumber=i;
			}
			
			::PostMessage((HWND)AfxGetMainWnd()->m_hWnd, WM_FINDAVACOMPORT, 0, (LPARAM)i);
			CloseHandle(hCom);	//�ҵ����ȹرմ����Ա���һ�����ڵĲ���
		}
	}
	return m_BHaveComPort;
}

BOOL CCOMPORT::OpenPort(int portnumber)
{
	BOOL m_bOpenSucceed=TRUE;
	CString m_strport;
	m_strport.Format("\\\\.\\COM%d",portnumber);
	if (portnumber!=-1)
	{
		hCom=CreateFile(m_strport,//COM��
			GENERIC_READ|GENERIC_WRITE, //��������д
			0, //��ռ��ʽ
			NULL,
			OPEN_EXISTING, //�򿪶����Ǵ���
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //�ص���ʽ
			NULL);
		//�򿪳ɹ�
		if(hCom==(HANDLE)-1)
		{
			AfxMessageBox("��ָ������ʧ��!");
			m_bOpenSucceed=FALSE;
			return m_bOpenSucceed;
		}

		SetupComm(hCom,BUFFERLENTH,BUFFERLENTH); //���뻺����������������Ĵ�С����10000
		COMMTIMEOUTS TimeOuts;
		//�趨����ʱ
		TimeOuts.ReadIntervalTimeout=0;
		TimeOuts.ReadTotalTimeoutMultiplier=0;
		TimeOuts.ReadTotalTimeoutConstant=0;
		//�ڶ�һ�����뻺���������ݺ���������������أ�
		//�������Ƿ������Ҫ����ַ���
		
		//�趨д��ʱ
		TimeOuts.WriteTotalTimeoutMultiplier=0;
		TimeOuts.WriteTotalTimeoutConstant=0;
		SetCommTimeouts(hCom,&TimeOuts); //���ó�ʱ
		DCB dcb;
		GetCommState(hCom,&dcb);
		dcb.BaudRate=115200; //������Ϊ115200
		dcb.ByteSize=8; //ÿ���ֽ���8λ
		dcb.Parity=NOPARITY; //����żУ��λ
		dcb.StopBits=ONESTOPBIT; //һ��ֹͣλ
		dcb.fDtrControl=0;
		// set DCB to configure the serial port
		SetCommState(hCom,&dcb);
		PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

		m_bThreadRXrunning=TRUE;
		m_bThreadTXrunning=TRUE;
		m_bThreadDealrunning=TRUE;
		//	�����߳�
		pThreadRX=AfxBeginThread(ThreadReadCOM,0);
		pThreadRX->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
		//  �����߳�
		pThreadTX=AfxBeginThread(ThreadSendCOM,0);
		pThreadTX->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
		//  �����߳�
		pThreadDeal=AfxBeginThread(ThreadDealCOM,0);
		pThreadDeal->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
	}
	return m_bOpenSucceed;
}

void CCOMPORT::CloseComPort()
{
	CloseHandle(hCom);
}
UINT ThreadReadCOM(LPVOID lpParam)
{	
	//�жϷ�ʽ��������
	static m_srxcounter=0;
	DWORD nBytesRead,dwEvent,dwError;  
	COMSTAT cs;
	BYTE	m_bytebuffer[22];
	cs.cbInQue=0;
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	SetCommMask(hCom,EV_RXCHAR); 
	while(m_bThreadRXrunning==TRUE)
	{
		if (WaitCommEvent(hCom,&dwEvent,NULL))	//������������
		{
			//ִ�гɹ����ط���ֵ
			ClearCommError(hCom,&dwError,&cs);	//�鿴�������л��ж�������û�ж�ȡ
			if (dwEvent&EV_RXCHAR&&cs.cbInQue)
			{
				//���������ݴ���ԭ������ǻ�������С�����
				/*
				if (m_bDealing==FALSE)
				{
					if (ReadFile(hCom,m_byteRXbuffer,cs.cbInQue,&nBytesRead,&m_osRead))
					{
						::PostMessage(AfxGetMainWnd()->m_hWnd, WM_RX, cs.cbInQue, 0);
					}
				}
				*/
				//dwCounter=cs.cbInQue;
				
				//��������������
				//�����յĴ�������ȫ�����
				for (int i=0;i<(int)cs.cbInQue;i++)
				{
					ReadFile(hCom,m_bytebuffer,1,&nBytesRead,&m_osRead);
					if (EnQueue(q,m_bytebuffer[0])==-1)	//����ʧ��
					{
						do 
						{
							Sleep(1);
						} while (EnQueue(q,m_bytebuffer[0])!=0);
						
					}
				}
				
				
				//��ʼѭ����ȡ����
				/*
				do 
				{
					//��黺�������Ƿ��Ѿ�����֡ͷ����
					if (0xFC==m_bytebuffer[0])
					{

					} 
					else
					{
					}
					if ((ReadFile(hCom,m_bytebuffer,1,&nBytesRead,&m_osRead))&&dwCounter)	//��1�������ж��Ƿ���֡ͷ
					{
						dwCounter--;
						//�����֡ͷ��ô�ٴζ�ȡ
						if (0xFC==m_bytebuffer[0])
						{
							m_byteRXbuffer[0]=m_bytebuffer[0];
							if ((ReadFile(hCom,m_bytebuffer,1,&nBytesRead,&m_osRead))&&dwCounter)
							{
								dwCounter--;
								if (0x0C==m_bytebuffer[0])
								{
									m_byteRXbuffer[1]=m_bytebuffer[0];
								}
								//֡ͷȷ����ϣ���ʼ��ȡ������
								if (dwCounter>=22)
								{
									if (ReadFile(hCom,m_bytebuffer,22,&nBytesRead,&m_osRead))
									{
										dwCounter-=22;
										for (int i=0;i<22;i++)
										{
											m_byteRXbuffer[i+2]=m_bytebuffer[i];
										}
										::PostMessage(AfxGetMainWnd()->m_hWnd, WM_RX, 24, 0); //����һ֡����
									} 
									else	//��ȡ����
									{

										break;
									}
								}
								else
								{

								}

							} 
							else	//��ȡ����
							{
								break;
							}
						}
					} 
					else	//��ȡ����
					{
						break;
					}
				} while (dwCounter);
				*/
			}
		}
		else
		{
			//ִ��ʧ��
			int m_nError;
			m_nError=GetLastError();
			PurgeComm(hCom,PURGE_RXCLEAR);
		}
	}
	return 0;
}
BOOL WriteChar(BYTE* m_szWriteBuffer,DWORD m_nToSend)
{
	OVERLAPPED osWrite={0};
	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;
	osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	if (osWrite.hEvent==NULL)
	{
		return FALSE;
	}
	if(!WriteFile(hCom,m_szWriteBuffer,m_nToSend,&dwWritten,&osWrite))
	{
		int m_nError=GetLastError();
		if (m_nError!=ERROR_IO_PENDING)
		{
			fRes=FALSE;
		}
		else
		{
			dwRes=WaitForSingleObject(osWrite.hEvent,INFINITE);
			switch (dwRes)
			{
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(hCom,&osWrite,&dwWritten,FALSE))
				{
					fRes=FALSE;
				} 
				else
				{
					if (dwWritten!=m_nToSend)
					{
						fRes=FALSE;
					} 
					else
					{
						fRes=TRUE;
					}
				}
				break;
			default:
				{
					fRes=FALSE;
					break;
				}
			}
		}
	}
	else
	{
		//д�������
		if (dwWritten!=m_nToSend)
		{
			fRes=FALSE;
		} 
		else
		{
			fRes=TRUE;
		}
	}
	//PurgeComm(hCom, PURGE_TXABORT|PURGE_TXCLEAR);
	CloseHandle(osWrite.hEvent);
	return fRes;
}

UINT ThreadSendCOM(LPVOID lpParam)
{
	while(m_bThreadTXrunning==TRUE)
	{
		//����һ������
		if (m_bSendPackage==TRUE)
		{
			WORD	m_ncrch16;
			//�������кź�У��
			//��һ֡
			m_byteWriteFrame1[0x04]=m_bytecounter++;
			m_ncrch16=crc16(m_byteWriteFrame1,22);
			m_byteWriteFrame1[0x16]=m_ncrch16&0x00ff;
			m_ncrch16=m_ncrch16&0xff00;
			m_ncrch16=m_ncrch16>>8;
			m_byteWriteFrame1[0x17]=(unsigned char)m_ncrch16;
			WriteChar(m_byteWriteFrame1,24);
			if (m_bHighSpeed==FALSE)
			{
				Sleep(SLEEPTIME);
			} 
			else
			{
				Delayms(DELAYTIMES);
			}
			
			//�ڶ�֡
			m_byteWriteFrame2[0x04]=m_bytecounter++;
			m_ncrch16=crc16(m_byteWriteFrame2,22);
			m_byteWriteFrame2[0x16]=m_ncrch16&0x00ff;
			m_ncrch16=m_ncrch16&0xff00;
			m_ncrch16=m_ncrch16>>8;
			m_byteWriteFrame2[0x17]=(unsigned char)m_ncrch16;
			WriteChar(m_byteWriteFrame2,24);
			if (m_bHighSpeed==FALSE)
			{
				Sleep(SLEEPTIME);
			} 
			else
			{
				Delayms(DELAYTIMES);
			}
			//����֡
			m_byteWriteFrame3[0x04]=m_bytecounter++;
			m_ncrch16=crc16(m_byteWriteFrame3,22);
			m_byteWriteFrame3[0x16]=m_ncrch16&0x00ff;
			m_ncrch16=m_ncrch16&0xff00;
			m_ncrch16=m_ncrch16>>8;
			m_byteWriteFrame3[0x17]=(unsigned char)m_ncrch16;
			WriteChar(m_byteWriteFrame3,24);
			if (m_bHighSpeed==FALSE)
			{
				Sleep(SLEEPTIME);
			} 
			else
			{
				Delayms(DELAYTIMES);
			}
			//����֡
			m_byteWriteFrame4[0x04]=m_bytecounter++;
			m_ncrch16=crc16(m_byteWriteFrame4,22);
			m_byteWriteFrame4[0x16]=m_ncrch16&0x00ff;
			m_ncrch16=m_ncrch16&0xff00;
			m_ncrch16=m_ncrch16>>8;
			m_byteWriteFrame4[0x17]=(unsigned char)m_ncrch16;
			WriteChar(m_byteWriteFrame4,24);
			if (m_bHighSpeed==FALSE)
			{
				Sleep(SLEEPTIME);
			} 
			else
			{
				Delayms(DELAYTIMES);
			}
			//������Ϣ�����Ի�����ʾ������Ϣ
			::PostMessage(AfxGetMainWnd()->m_hWnd,WM_UCOM_WRITE,0,0);
			m_bSendPackage=FALSE;
		}
		if (m_bWorking==FALSE)
		{
			Sleep(SLEEPTIME);
		}
	}
	return 0;
}
int crc16(unsigned char *pchMsg,unsigned int wDataLen)
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
	
	for (i = 0; i < wDataLen; i++)
	{
		chChar = *pchMsg++;
		wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
		wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
	}
	return wCRC;
}
void Delayms(int m_ns)
{
	LARGE_INTEGER litmp;
	LONGLONG QPart1,QPart2;
	double dfMinus, dfFreq, dfTim;
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;// ��ü�������ʱ��Ƶ��
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;// ��ó�ʼֵ
	do
	{
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;//�����ֵֹ
		dfMinus = (double)(QPart2-QPart1);
		dfTim = dfMinus / dfFreq;// ��ö�Ӧ��ʱ��ֵ����λΪ��
	}while(dfTim <0.0000173575*m_ns);
}

UINT ThreadDealCOM(LPVOID lpParam)
{
	WORD	m_wcrc16c,m_wcrc16d;
	
	while(m_bThreadRXrunning==TRUE)
	{
		do 
		{
			//����������ȡ����
			 while (DeQueue(q,m_byteRXbuffer[0])!=0)
			 {
				 Sleep(1);	//��ȡ����ʧ��
			 }
			if (m_byteRXbuffer[0]==0xFC)
			{
				while(DeQueue(q,m_byteRXbuffer[1])!=0)
				{
					Sleep(1);	//��ȡ����ʧ��
				}
				if (m_byteRXbuffer[1]==0x0C)
				{
					//֡ͷȷ����ɣ�������ȡ22���ֽ�
					for (int i=0;i<22;i++)
					{
						while(DeQueue(q,m_byteRXbuffer[i+2])!=0)
						{
							Sleep(1);	//��ȡ����ʧ��
						}
					}
					//CRC16У�飬���������ȷ���͸���������
					m_wcrc16c=crc16(m_byteRXbuffer,22);
					m_wcrc16d=m_byteRXbuffer[0x17];
					m_wcrc16d=m_wcrc16d<<8;
					m_wcrc16d+=m_byteRXbuffer[0x16];
					if (m_wcrc16c==m_wcrc16d)
					{
						::PostMessage(AfxGetMainWnd()->m_hWnd, WM_RX, 24, 0); //����һ֡����
					}
				}
			}
		} while (QueueLenth(q)>=24);
		Sleep(1);
	}
	return 0;
}
