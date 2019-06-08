#define NUMSIMBOLS 255
#define DATA_OK 0xACF0
#define DATA_NULL 0xACF1
#define DATA_SEND_ERROR 0xACF2
#define DATA_RECEIVE_ERROR 0xACF3

#include <stdio.h>
#include <string.h>
//#include <windows.h>

//OVERLAPPED ovrlRead;
//OVERLAPPED ovrlWrite;

class CComUse
{
private:

	bool ComInOutBuffSize(HANDLE, DWORD, DWORD);
	bool ComConfig(HANDLE, DWORD, BYTE, BYTE, BYTE, char, char);
	bool ComTimeOuts(HANDLE, DWORD, DWORD, DWORD, DWORD, DWORD);


public:
        //HANDLE hComm;
        bool ComMask(HANDLE, DWORD);
	HANDLE ComInit(LPCTSTR, unsigned long);
	//unsigned int GetData(HANDLE, char*, OVERLAPPED*);
        //unsigned int SetData(HANDLE, char*, OVERLAPPED*);
        bool ComWait(HANDLE, LPDWORD);
        bool ComClear(HANDLE, DWORD);
	bool ComClose(HANDLE);

}CComUse;

//Установка внутреннего буфера ввода/вывода ком-порта в байтах
//Буфер ввода/вывода ком-порта успешно установлен функция возвращает true.
//Если есть ошибки функция возвращает false
bool CComUse::ComInOutBuffSize(HANDLE hComm, DWORD in, DWORD out)
{
	if (!SetupComm(hComm, in, out))
	{
		return false;
	}
	return true;
}
//Установка конфигурации ком-порта
//Конфигурация ком-порта успешно установлена функция возвращает true.
//Если есть ошибки функция возвращает false
bool CComUse::ComConfig(HANDLE hComm, DWORD baudrate, BYTE parity, BYTE stopbits,
					   BYTE bytesize, char eofchar, char eventchar)
{
	COMMCONFIG cConf;
	DWORD size;
	size = sizeof(cConf);
	if (!GetCommConfig(hComm, &cConf, &size))
	{
		return false;
	}
	cConf.dcb.fBinary = TRUE;
	cConf.dcb.BaudRate = baudrate;
	cConf.dcb.Parity = parity;
	cConf.dcb.StopBits = stopbits;
	cConf.dcb.ByteSize = bytesize;
	cConf.dcb.EofChar = eofchar;
	cConf.dcb.EvtChar = eventchar;
        cConf.dcb.fNull=FALSE;
        cConf.dcb.fOutxCtsFlow = FALSE;
 	cConf.dcb.fOutxDsrFlow = FALSE;
 	cConf.dcb.fDtrControl = DTR_CONTROL_ENABLE;
 	cConf.dcb.fDsrSensitivity = FALSE;
 	cConf.dcb.fRtsControl = RTS_CONTROL_ENABLE;
        cConf.dcb.fAbortOnError = FALSE;
	if (!SetCommConfig(hComm, &cConf, sizeof(cConf)))
	{
		return false;
	}
	return true;
}
//Установка таймаутов ком-порта
//Таймауты ком-порта успешно установленны функция возвращает true.
//Если есть ошибки функция возвращает false
bool CComUse::ComTimeOuts(HANDLE hComm, DWORD read_interval, DWORD read_constant, DWORD read_multiplier,
						 DWORD write_constant, DWORD write_multiplier)
{
	COMMTIMEOUTS  cTime;
	if (!GetCommTimeouts(hComm, &cTime))
	{
		return false;
	}
	cTime.ReadIntervalTimeout = read_interval;
	cTime.ReadTotalTimeoutMultiplier = read_constant;
	cTime.ReadTotalTimeoutConstant = read_multiplier;
	cTime.WriteTotalTimeoutConstant = write_constant;
	cTime.WriteTotalTimeoutMultiplier = write_multiplier;
	if (!SetCommTimeouts(hComm, &cTime))
	{
		return false;
	}
	return true;
}
//Установка маски событий срабатывания ком-порта
//Маска ком-порта успешно установленна функция возвращает true.
//Если есть ошибки функция возвращает false
bool CComUse::ComMask(HANDLE hComm, DWORD mask)
{
	if (!SetCommMask(hComm, mask))
	{
		return false;
	}
	return true;
}
//Функция удаляет все символы в буфере обмена ком-порта или сбрасывает операции
//чтения/записи в ком-порт
//Ком-порт успешно очищен функция возвращает true.
//Если есть ошибки функция возвращает false
bool CComUse::ComClear(HANDLE hComm, DWORD flag)
{
	if (!PurgeComm(hComm,flag))
	{
		return false;
	}
	return true;
}
//Функция ожидает события из компорта
//Функция возвращает true при успешном выполнении.
//Если есть ошибки функция возвращает false
bool CComUse::ComWait(HANDLE hComm, LPDWORD rx_state)
{
	if (!WaitCommEvent(hComm, rx_state, NULL))
	{
		return false;
	}
	return true;
}
//Инициализация ком-порта для пирометра.
//Если ком-порт успешно открыт функция возвращает true.
//Если есть ошибки функция возвращает false
HANDLE CComUse::ComInit(LPCTSTR com, unsigned long ovrl)
{
	HANDLE hComm = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,
	0, OPEN_EXISTING, ovrl, NULL);          //FILE_FLAG_OVERLAPPED
	if(hComm != INVALID_HANDLE_VALUE)
	{
		if (!ComInOutBuffSize(hComm, 1024,1024)) return 0;
		if (!ComConfig(hComm, 115200, NOPARITY, ONESTOPBIT, 8, NULL, NULL)) return 0;
		if (!ComTimeOuts(hComm, MAXDWORD, 0, 0, 1, 1)) return 0;   //MAXDWORD
		if (!ComMask(hComm, EV_RXCHAR)) return 0;
		if (!ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR)) return 0;

		return hComm;
	}
	return 0;
}
bool CComUse::ComClose(HANDLE hComm)
{
        if (!CloseHandle(hComm))
	{
		return false;
	}
	return true;
}

/*unsigned int CComUse::GetData(HANDLE hComm, char* buffer, OVERLAPPED* ovrl)
{
        unsigned long bytesread=0;
        if (!ReadFile(hComm, buffer, 1, &bytesread, ovrl))     //&ovrlRead
	{
		ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_RECEIVE_ERROR;
	}
        if(0==bytesread)
	{
		ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_NULL;
	}
       	//ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
	return bytesread;
}

unsigned int CComUse::SetData(HANDLE hComm, char* buffer, OVERLAPPED* ovrl)
{
        unsigned long byteswrite=0;
        DWORD len=strlen(buffer);
        WriteFile(hComm, buffer, len, &byteswrite, ovrl);
        if (!WriteFile(hComm, buffer, len, &byteswrite, ovrl))
        {
                ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_SEND_ERROR;
        }
        if(0==byteswrite)
	{
		ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_NULL;
	}*/
        /*if (ovrlWrite.InternalHigh==0)
        {
                ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_SEND_ERROR;
        }
        if(0==byteswrite)
	{
		ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
		return DATA_NULL;
	}
        //ComClear(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
        return DATA_OK;
}*/
