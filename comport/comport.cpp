// comport.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include <windows.h>
/*
int
main(int argc, char *argv[])
{
  DCB dcb;
  HANDLE hCom;
  BOOL fSuccess;
  TCHAR *pcCommPort = TEXT("COM2");

  hCom = CreateFile( pcCommPort,
                     GENERIC_READ | GENERIC_WRITE,
                     0,    // comm devices must be opened w/exclusive-access
                     NULL, // no security attributes
                     OPEN_EXISTING, // comm devices must use OPEN_EXISTING
                     0,    // not overlapped I/O
                     NULL  // hTemplate must be NULL for comm devices
                     );

  if (hCom == INVALID_HANDLE_VALUE) {
      // Handle the error.
      printf ("CreateFile failed with error %d.\n", GetLastError());
      return (1);
    }

  // We will build on the current configuration, and skip setting the size
  // of the input and output buffers with SetupComm.

  fSuccess = GetCommState(hCom, &dcb);

  if (!fSuccess) {
      // Handle the error.
      printf ("GetCommState failed with error %d.\n", GetLastError());
      return (2);
    }

  // Fill in the DCB: baud=57,600 bps, 8 data bits, no parity, and 1 stop bit.

  dcb.BaudRate = CBR_57600;     // set the baud rate
  dcb.ByteSize = 8;             // data size, xmit, and rcv
  dcb.Parity = NOPARITY;        // no parity bit
  dcb.StopBits = ONESTOPBIT;    // one stop bit

  fSuccess = SetCommState(hCom, &dcb);

  if (!fSuccess) {
      // Handle the error.
      printf ("SetCommState failed with error %d.\n", GetLastError());
      return (3);
    }

  printf ("Serial port %s successfully reconfigured.\n", pcCommPort);
  return (0);
}
*/
#include "Setupapi.h"
#include "string.h"

#pragma comment(lib, "setupapi.lib")

#define UsbTestDevice_CLASS_GUID \
	{ 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
//{0x36fc9e60L, 0xc465, 0x11cf, {0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}}
//{0x96e73b6eL, 0x7a5a, 0x11d4, 0x9f, 0x24, 0x00, 0x80, 0xc8, 0x27, 0x27, 0xf4}



const GUID  ClassGuid = UsbTestDevice_CLASS_GUID;
HDEVINFO DoDeviceEnum( 
)
/*
Routine Description:
    Retrieves the device information set that contains that contains
    the devices of the specified class. 

Parameters:
    InterfaceClassGuid - The interface class GUID. 

Return Value:
    If the function succeeds, the return value is a handle to the 
    device information set.

    If the function fails, the return value is zero.
*/
{
	DWORD Err;
    HDEVINFO DeviceInfoSet;
    HDEVINFO NewDeviceInfoSet;


	/*GUID hidGuid;
	::HidD_GetHidGuid((LPGUID)&hidGuid);*/

    // Create a device information set that will be the container for 
    // the device interfaces.

    DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
    
    if(DeviceInfoSet == INVALID_HANDLE_VALUE) {
        Err = GetLastError();
        printf( "SetupDiCreateDeviceInfoList failed: %lx.\n", Err );
        return 0;
    }

    // Retrieve the device information set for the interface class.

    NewDeviceInfoSet = SetupDiGetClassDevsEx( &ClassGuid,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE,
		//DIGCF_PRESENT | DIGCF_ALLCLASSES,
        DeviceInfoSet,
        NULL,
        NULL
    );

    if(NewDeviceInfoSet == INVALID_HANDLE_VALUE) 
    {
        Err = GetLastError();
        printf( "SetupDiGetClassDevsEx failed: %lx.\n", Err );
        return 0;
    }

    return NewDeviceInfoSet;
}

BOOL EnablePrivilege(TCHAR * name, BOOL fEnable, HANDLE hToken)
{
    BOOL                bRetVal;
    int                    nError;
    HANDLE                hNewToken = NULL;
    TOKEN_PRIVILEGES    priv = { 1, {0, 0, fEnable ? SE_PRIVILEGE_ENABLED : 0} };

    LookupPrivilegeValue(0, name, &priv.Privileges[0].Luid);
    if(hToken == NULL)
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hNewToken);
    else
        hNewToken = hToken;

    bRetVal = AdjustTokenPrivileges(hNewToken, FALSE, &priv, sizeof(priv), 0, 0);
    if(bRetVal == FALSE)
    {
        nError = ::GetLastError();
        if(hToken == NULL)    
            CloseHandle(hNewToken);

        ::SetLastError(nError);
        return FALSE;
    }

    if(hToken == NULL)    
        CloseHandle(hNewToken);
    return TRUE;
}

/**
斑马打印机的usbpath---\\?\usb#vid_0a5f&pid_00c6#19j120201227#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
*/

int main(int argc, char *argv[])
{
	SP_DEVINFO_DATA DeviceInfoData = {0};
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {0};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = {0};
	CHAR DevicePath[128];
	DWORD RequiredSize = 0;

	HDEVINFO info = DoDeviceEnum();
	if (info == INVALID_HANDLE_VALUE)
    {
        printf("error");
    }

	/*
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	
	BOOL bValue = SetupDiEnumDeviceInfo(info, 0, &DeviceInfoData);
	*/
	
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	BOOL bValue = SetupDiEnumDeviceInterfaces(info, NULL,  &ClassGuid, 0 ,&DeviceInterfaceData);
	
	DWORD error = GetLastError();
	if(error == ERROR_NO_MORE_ITEMS)
	{
		printf("ERROR_NO_MORE_ITEMS\n");
	}



	//for(int i = 0; SetupDiEnumDeviceInfo(info, i, &DeviceInfoData); i++)
	for(int i = 0; SetupDiEnumDeviceInterfaces(info, NULL,  &ClassGuid, i ,&DeviceInterfaceData); i++)
	{
		//printf("%d\n",DeviceInfoData.cbSize);
		//printf("%d\n",DeviceInterfaceData.cbSize);
		
		/*
		SetupDiEnumDeviceInterfaces(
			info, 
			&DeviceInfoData ,
			&ClassGuid,i,
			&DeviceInterfaceData);
			*/

		SetupDiGetDeviceInterfaceDetail(
			info,
			&DeviceInterfaceData,
			NULL,
			0,
			&RequiredSize,
			NULL);

		PSP_INTERFACE_DEVICE_DETAIL_DATA detail 
			= (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(RequiredSize);

		int size = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SP_DEVINFO_DATA did = {sizeof(SP_DEVINFO_DATA)};
		
		SetupDiGetDeviceInterfaceDetail(
			info, 
			&DeviceInterfaceData, 
			detail,RequiredSize, 
			NULL, 
			&did);


		TCHAR fname[256] = {0};
		//wprintf(_T("%s\n"),fname);
		_tcsncpy(fname, detail->DevicePath, 256);

		wprintf(_T("%s\n"),fname);

		//printf("i=%d\n",i);

		HANDLE hDevice = CreateFile(
			TEXT("\\\\?\\usb#vid_0a5f&pid_00c6#19j120201227#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"),
			//TEXT("\\\\?\\usb#vid_148f&pid_761a#1.0#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"),
			GENERIC_READ && GENERIC_WRITE,  
			//0,
            FILE_SHARE_READ && FILE_SHARE_WRITE,  
            NULL,  
            OPEN_EXISTING,  
            0,  
            NULL);

		
		if(INVALID_HANDLE_VALUE == hDevice)
		{
			error = GetLastError();
			printf( "CreateFile failed: %lx.\n", error );
			if(ERROR_FILE_NOT_FOUND == error)
			{
				printf( "The system cannot find the file specified. \n");
			}
			return 0;
		}



		DWORD dwWritenSize = 0;
		//TCHAR cmd[] = {TEXT("^XA^LH30,30^FO20,10^AD^FDEXAMPLE^FS^FO20,60^B3^FDAAA001^FS^PQ1,0,0,Y^XZ")};
		TCHAR cmd[] = {TEXT("oLY")};



		BOOL bRetValue = WriteFile(hDevice,cmd,sizeof(cmd),&dwWritenSize,NULL);

		printf( "write size : %d.\n", dwWritenSize );

		if(bRetValue==FALSE)
		{
			error = GetLastError();
			printf( "CreateFile failed: %lx.\n", error );

			if(ERROR_ACCESS_DENIED == error)
			{
				printf( "Access is denied. : %lx.\n", error );
			}
		}

		
		free(detail);
		CloseHandle(hDevice);
		
	}


	getch();
}