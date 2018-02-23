// winspool.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include <windows.h> 
#include "WinSpool.h"
#include "string.h"

bool Print(LPWSTR szPrinterName, char* szBuffer)// szBuffer������ϣ�����͵���ӡ�������ݻ���
{
	HANDLE hPrinter; //��ӡ�����
	int iLen = strlen(szBuffer);// �������ݵĳ���
	DWORD dwWritten; // �Ѿ����ӡ��д�˶���
	if(OpenPrinter(szPrinterName, &hPrinter, NULL))
	//Open��ӡ��������һ��������Ѿ�������hPrinter)
	{
		DOC_INFO_1 Info;//�ĵ���Ϣ
		memset(&Info, 0, sizeof(Info));
		Info.pDocName = TEXT("My PrintJob");
		//�ĵ���(which is shown in the PrintManager)
		Info.pOutputFile = NULL;
		//����Ҫ��һ���ļ�д����ΪNULL
		Info.pDatatype = TEXT("RAW");
		//������RAW��ʽ����(Win9x��WinNT��֧��RAW��EMF��ʽ��WinNT��֧��TEXT��ʽ
		if(!StartDocPrinter(hPrinter, 1, (LPBYTE)&Info))//�ĵ���ʼ
			return false;
		// ���ߴ�ӡ����ʼ��ҳ
		StartPagePrinter(hPrinter);
		// ���ӡ��д����
		if(!WritePrinter(hPrinter, szBuffer, iLen, &dwWritten))
		{
			//����������ʱ���ִ���
			//�����Ĵ���ָ��������ʲô����
		}
		EndPagePrinter(hPrinter);//���ߴ�ӡ���Ѿ���ɵ�ǰҳ
		if(!EndDocPrinter(hPrinter))//�ĵ�����
			return false;
		ClosePrinter(hPrinter);//�رմ�ӡ��
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

