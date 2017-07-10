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
HANDLE hCom; //串口
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
/*利用循环队列实现循环缓冲*/

typedef BYTE QElemType ; //定义新类型名 

typedef struct
{
	QElemType *base;  //循环队列的存储空间 （ 用数组指针指示存储实际长度10字节的字符串) 
	int front;  //队头元素下标（取元素） 
	int rear;  //队尾元素下标（插入元素） 
}SqQueue;            

//建立循环队列（循环缓冲区）
int InitQueue(SqQueue &Q)
{
    Q.base=(QElemType *)malloc(MAXQSIZE*sizeof(QElemType));
    if(!Q.base) 
		return -1;  //创建失败，返回-1 
    Q.front=0;
    Q.rear=0;
    return 0;    //创建成功，返回0 
}

//插入元素（入队列）
int EnQueue(SqQueue &Q,QElemType e)
{
    if((Q.rear+1)%MAXQSIZE==Q.front)	//下一个是否和读取指针重回
		return -1;  //队满
    //memcpy(Q.base[Q.rear],e);  //赋值
	Q.base[Q.rear]=e;
    Q.rear=(Q.rear+1)%MAXQSIZE;  //下标加一 
    return 0;  //入队成功
}

//取元素(数组指针，指向取到的值） 
int DeQueue(SqQueue &Q,QElemType &e)
{
    if(Q.rear==Q.front)
        return -1;   //队空
    //memcpy(e,Q.base[Q.front]);
	e=Q.base[Q.front];
    Q.front=(Q.front+1)%MAXQSIZE; //下标加一 
    return 0;  //成功
}  

int QueueLenth(SqQueue Q)
{
	return (Q.rear-Q.front+MAXQSIZE)%MAXQSIZE;
}

SqQueue q;  //数据结构 环形缓冲区队列
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
	//串口初始化,先搜索本机所有串口号,默认打开串口号最小的串口
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
		AfxMessageBox("没有找到串口!");
	}	
}
BOOL CCOMPORT::SearchPort()
{
	BOOL m_BHaveComPort=FALSE;
	//一般认为本机串口号不大于256
	CString m_strport;

	for (int i=1;i<=256;i++)
	{
		m_strport.Format("\\\\.\\COM%d",i);
		hCom=CreateFile(m_strport,//COM口
			GENERIC_READ|GENERIC_WRITE, //允许读和写
			0, //独占方式
			NULL,
			OPEN_EXISTING, //打开而不是创建
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //重叠方式
			NULL);
		//打开成功
		if(hCom!=(HANDLE)-1)
		{
			if (m_BHaveComPort==FALSE)
			{
				m_BHaveComPort=TRUE;
				m_nMinPortNumber=i;
			}
			
			::PostMessage((HWND)AfxGetMainWnd()->m_hWnd, WM_FINDAVACOMPORT, 0, (LPARAM)i);
			CloseHandle(hCom);	//找到后先关闭串口以便下一个串口的查找
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
		hCom=CreateFile(m_strport,//COM口
			GENERIC_READ|GENERIC_WRITE, //允许读和写
			0, //独占方式
			NULL,
			OPEN_EXISTING, //打开而不是创建
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //重叠方式
			NULL);
		//打开成功
		if(hCom==(HANDLE)-1)
		{
			AfxMessageBox("打开指定串口失败!");
			m_bOpenSucceed=FALSE;
			return m_bOpenSucceed;
		}

		SetupComm(hCom,BUFFERLENTH,BUFFERLENTH); //输入缓冲区和输出缓冲区的大小都是10000
		COMMTIMEOUTS TimeOuts;
		//设定读超时
		TimeOuts.ReadIntervalTimeout=0;
		TimeOuts.ReadTotalTimeoutMultiplier=0;
		TimeOuts.ReadTotalTimeoutConstant=0;
		//在读一次输入缓冲区的内容后读操作就立即返回，
		//而不管是否读入了要求的字符。
		
		//设定写超时
		TimeOuts.WriteTotalTimeoutMultiplier=0;
		TimeOuts.WriteTotalTimeoutConstant=0;
		SetCommTimeouts(hCom,&TimeOuts); //设置超时
		DCB dcb;
		GetCommState(hCom,&dcb);
		dcb.BaudRate=115200; //波特率为115200
		dcb.ByteSize=8; //每个字节有8位
		dcb.Parity=NOPARITY; //无奇偶校验位
		dcb.StopBits=ONESTOPBIT; //一个停止位
		dcb.fDtrControl=0;
		// set DCB to configure the serial port
		SetCommState(hCom,&dcb);
		PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

		m_bThreadRXrunning=TRUE;
		m_bThreadTXrunning=TRUE;
		m_bThreadDealrunning=TRUE;
		//	接收线程
		pThreadRX=AfxBeginThread(ThreadReadCOM,0);
		pThreadRX->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
		//  发送线程
		pThreadTX=AfxBeginThread(ThreadSendCOM,0);
		pThreadTX->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
		//  处理线程
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
	//中断方式串口收数
	static m_srxcounter=0;
	DWORD nBytesRead,dwEvent,dwError;  
	COMSTAT cs;
	BYTE	m_bytebuffer[22];
	cs.cbInQue=0;
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	SetCommMask(hCom,EV_RXCHAR); 
	while(m_bThreadRXrunning==TRUE)
	{
		if (WaitCommEvent(hCom,&dwEvent,NULL))	//串口数据来临
		{
			//执行成功返回非零值
			ClearCommError(hCom,&dwError,&cs);	//查看缓冲区中还有多少数据没有读取
			if (dwEvent&EV_RXCHAR&&cs.cbInQue)
			{
				//读出的数据错误原因可能是缓冲区大小引起的
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
				
				//缓冲区数据入列
				//将接收的串口数，全部入队
				for (int i=0;i<(int)cs.cbInQue;i++)
				{
					ReadFile(hCom,m_bytebuffer,1,&nBytesRead,&m_osRead);
					if (EnQueue(q,m_bytebuffer[0])==-1)	//入列失败
					{
						do 
						{
							Sleep(1);
						} while (EnQueue(q,m_bytebuffer[0])!=0);
						
					}
				}
				
				
				//开始循环读取数据
				/*
				do 
				{
					//检查缓冲区中是否已经存在帧头数据
					if (0xFC==m_bytebuffer[0])
					{

					} 
					else
					{
					}
					if ((ReadFile(hCom,m_bytebuffer,1,&nBytesRead,&m_osRead))&&dwCounter)	//读1个数据判断是否是帧头
					{
						dwCounter--;
						//如果是帧头那么再次读取
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
								//帧头确认完毕，开始读取后方数据
								if (dwCounter>=22)
								{
									if (ReadFile(hCom,m_bytebuffer,22,&nBytesRead,&m_osRead))
									{
										dwCounter-=22;
										for (int i=0;i<22;i++)
										{
											m_byteRXbuffer[i+2]=m_bytebuffer[i];
										}
										::PostMessage(AfxGetMainWnd()->m_hWnd, WM_RX, 24, 0); //完整一帧数据
									} 
									else	//读取错误
									{

										break;
									}
								}
								else
								{

								}

							} 
							else	//读取错误
							{
								break;
							}
						}
					} 
					else	//读取错误
					{
						break;
					}
				} while (dwCounter);
				*/
			}
		}
		else
		{
			//执行失败
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
		//写操作完成
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
		//发送一包数据
		if (m_bSendPackage==TRUE)
		{
			WORD	m_ncrch16;
			//填入序列号和校验
			//第一帧
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
			
			//第二帧
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
			//第三帧
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
			//第四帧
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
			//发送消息给主对话框，显示发送信息
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
	dfFreq = (double)litmp.QuadPart;// 获得计数器的时钟频率
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;// 获得初始值
	do
	{
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;//获得中止值
		dfMinus = (double)(QPart2-QPart1);
		dfTim = dfMinus / dfFreq;// 获得对应的时间值，单位为秒
	}while(dfTim <0.0000173575*m_ns);
}

UINT ThreadDealCOM(LPVOID lpParam)
{
	WORD	m_wcrc16c,m_wcrc16d;
	
	while(m_bThreadRXrunning==TRUE)
	{
		do 
		{
			//缓冲区中提取数据
			 while (DeQueue(q,m_byteRXbuffer[0])!=0)
			 {
				 Sleep(1);	//提取数据失败
			 }
			if (m_byteRXbuffer[0]==0xFC)
			{
				while(DeQueue(q,m_byteRXbuffer[1])!=0)
				{
					Sleep(1);	//提取数据失败
				}
				if (m_byteRXbuffer[1]==0x0C)
				{
					//帧头确认完成，连续提取22个字节
					for (int i=0;i<22;i++)
					{
						while(DeQueue(q,m_byteRXbuffer[i+2])!=0)
						{
							Sleep(1);	//提取数据失败
						}
					}
					//CRC16校验，如果数据正确则发送给处理界面
					m_wcrc16c=crc16(m_byteRXbuffer,22);
					m_wcrc16d=m_byteRXbuffer[0x17];
					m_wcrc16d=m_wcrc16d<<8;
					m_wcrc16d+=m_byteRXbuffer[0x16];
					if (m_wcrc16c==m_wcrc16d)
					{
						::PostMessage(AfxGetMainWnd()->m_hWnd, WM_RX, 24, 0); //完整一帧数据
					}
				}
			}
		} while (QueueLenth(q)>=24);
		Sleep(1);
	}
	return 0;
}

