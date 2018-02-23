// winspool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include <windows.h> 
#include "WinSpool.h"
#include "string.h"

bool Print(LPWSTR szPrinterName, char* szBuffer)// szBuffer是我们希望发送到打印机的数据缓冲
{
	HANDLE hPrinter; //打印机句柄
	int iLen = strlen(szBuffer);// 发送数据的长度
	DWORD dwWritten; // 已经向打印机写了多少
	if(OpenPrinter(szPrinterName, &hPrinter, NULL))
	//Open打印机并赋给一个句柄（已经保存在hPrinter)
	{
		DOC_INFO_1 Info;//文档信息
		memset(&Info, 0, sizeof(Info));
		Info.pDocName = TEXT("My PrintJob");
		//文档名(which is shown in the PrintManager)
		Info.pOutputFile = NULL;
		//不须要向一个文件写，设为NULL
		Info.pDatatype = TEXT("RAW");
		//数据以RAW格式发送(Win9x和WinNT都支持RAW和EMF格式，WinNT还支持TEXT格式
		if(!StartDocPrinter(hPrinter, 1, (LPBYTE)&Info))//文档开始
			return false;
		// 告诉打印机开始新页
		StartPagePrinter(hPrinter);
		// 向打印机写数据
		if(!WritePrinter(hPrinter, szBuffer, iLen, &dwWritten))
		{
			//当发送数据时出现错误
			//用最后的错误指出发生了什么错误
		}
		EndPagePrinter(hPrinter);//告诉打印机已经完成当前页
		if(!EndDocPrinter(hPrinter))//文档结束
			return false;
		ClosePrinter(hPrinter);//关闭打印机
		return true;
	}
	else
	{
		return false;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//printf("dddd\n");
	char* cmd = "^XA^LH30,30^FO20,10^AD^FDEXAMPLE^FS^FO20,60^B3^FDAAA001^FS^PQ1,0,0,Y^XZ";
	TCHAR szPrinterName[] = TEXT("ZDesigner GK888t"); 

	Print(szPrinterName,cmd);

	getch();
	return 0;
}

