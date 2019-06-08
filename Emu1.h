//---------------------------------------------------------------------------

#ifndef Emu1H
#define Emu1H
#define SYMBOLSNUM 255                             //кол-во читаемых за раз символов из файла .emu
#define SIGNALSMAX 8192                            //максимальное количество отправляемых на программу проверки сообщений
#define OPERMAX 16                                 //максимальное количество читаемых из .emu операций необходимых для запуска режима
#define PROTMAX 10000                                   //максимальное число строк .emu файла в блоке протокола
#define SETSCOUNT 3                                     //количество настраиваемых параметров программы
//---------------------------------------------------------------------------
/*
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <stdio.h>
#include <Registry.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
*/

#include <Registry.hpp>
#include <stdio.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//#include <ComCtrls.hpp>
#include <syncobjs.hpp>                       //Необходим для класса TEvent
//#include <Buttons.hpp>
//#include <ComObj.hpp>
//#include <utilcls.h>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//#include <Inifiles.hpp>
//#include <Grids.hpp>
//#include <CheckLst.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Tabnotbk.hpp>
#include <FileCtrl.hpp>
#include "MakeDeviceFile.h"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TButton *PitButt;
        TGroupBox *Info;
        TLabel *SystemNameLabel;
        TLabel *ModNameLabel;
        TProgressBar *SigBarBKS;
        TProgressBar *SigBarBKI;
        TLabel *BarBKSLabel;
        TLabel *BarBKILabel;
        TButton *PauseButt;
        TButton *NextButt;
        TRichEdit *RichBKS;
        TRichEdit *RichBKI;
        TGroupBox *TimerGroup;
        TLabel *TimerBKSLabel;
        TLabel *TimerBKILabel;
        TGroupBox *SignalsGroup;
        TLabel *SentBKSLabel;
        TLabel *SentBKILabel;
        TTimer *SigDelayBKS;
        TTimer *SigDelayBKI;
        TLabel *StatusLabel;
        TGroupBox *SystemSettings;
        TImage *FileOpenImg;
        TImage *InitBKIImg;
        TImage *InitBKSImg;
        TComboBox *COMPortBKI;
        TEdit *DeviceFilePath;
        TButton *DeviceFileButt;
        TComboBox *COMPortBKS;
        TCheckBox *TimerBox;
        TCheckBox *RoundBox;
        TButton *ApplySets;
        TButton *DefaultSets;
        TOpenDialog *DeviceFile;
        TButton *MakeDeviceFile;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall DeviceFileButtClick(TObject *Sender);
        void __fastcall PitButtClick(TObject *Sender);
        void __fastcall NextButtClick(TObject *Sender);
        void __fastcall PauseButtClick(TObject *Sender);
        void __fastcall ApplySetsClick(TObject *Sender);
        void __fastcall MakeDeviceClick(TObject *Sender);
        void __fastcall DefaultSetsClick(TObject *Sender);
        void __fastcall COMPortBKIDrop(TObject *Sender);
        void __fastcall COMPortBKSDrop(TObject *Sender);
        void __fastcall COMPortBKISelect(TObject *Sender);
        void __fastcall COMPortBKSSelect(TObject *Sender);
        void __fastcall SigDelayBKIEvents(TObject *Sender);
        void __fastcall SigDelayBKSEvents(TObject *Sender);

private:	// User declarations
        AnsiString CurrDir;
        void InitBKS(AnsiString portname);
        void InitBKI(AnsiString portname);
        void GetSystemInfo(FILE*);
        void StartModBKS(FILE*);
        void StartModBKI(FILE*);
        void SignalsModBKS(FILE*);
        void SignalsModBKI(FILE*);
        void ProtocolModBKI(FILE*);
        void ProtocolModBKS(FILE*);
        void SignalsLoadBKS(AnsiString str);
        void SignalsLoadBKI(AnsiString str);
        void StartStrProc(AnsiString);             //обработка строки .emu файла, содержащей управляющие команды старта режима
        void ProtStrProc(AnsiString);              //обрботка строки .emu файла, содержащей протокол проверки
        void SaveSet(AnsiString);
        void DelSet(AnsiString);
        void StartNull();                       //функция обнуления при открытии формы
public:		// User declarations
        TEvent *event;
        HANDLE hCommBKS;       //дескриптор COM-порта канала БКС
        HANDLE hCommBKI;       //дескриптор COM-порта канала БКИ
        HANDLE BKSRead;       //используются для создания потоков чтения COM-порта.
        HANDLE BKSWrite;      //используются для создания потоков записи COM-порта.
        HANDLE BKIRead;        //используются для создания потоков чтения COM-порта.
        HANDLE BKIWrite;      //используются для создания потоков записи COM-порта.
        HANDLE MessageSendBKI;     //дескриптор события окончания передачи сообщения типа <...> по каналу БКИ
        HANDLE MessageSendBKS;     //дескриптор события окончания передачи сообщения типа <...> по каналу БКС
        HANDLE AllSigBKI;          //дескриптор события завершения передачи сигналов по каналу БКИ
        HANDLE AllSigBKS;          //дескриптор события завершения передачи сигналов по каналу БКС
        HANDLE LitBKSReceived;     //дескриптор события получения литеры по каналу БКС
        HANDLE LitBKIReceived;     //дескриптор события получения литеры по каналу БКИ
        AnsiString outBKSBuff, outBKIBuff;      //буферы записи в COM-порты
        AnsiString DeviceFileName;
        __fastcall TForm1(TComponent* Owner);
        void SendStringBKS(AnsiString);
        void SendStringBKI(AnsiString);
       void OpenDeviceFile(void);
       void CheckingDeviceFile(AnsiString filename);
       void GetSerialPortsList(AnsiString port);
       void NullAll(DWORD num);
       void FileNull();
       bool SignalsSendBKI;
       bool SignalsSendBKS;
       bool StopFlagBKI;
       bool StopFlagBKS;
       bool CloseFlagBKS;
       bool CloseFlagBKI;
       bool PitButtFlagBKI;
       bool PitButtFlagBKS;
       bool NextButtFlagBKI;
       bool NextButtFlagBKS;
       bool TestEndFlagBKS;
       bool TestEndFlagBKI;
       bool FirstTestFlag;                  //Признак первой после запуска программы проверки
       bool OpenFileFlag;
       unsigned int StartMessBKIFlag;
       unsigned int StartMessBKSFlag;
       DWORD CloseCountBKI;
       DWORD CloseCountBKS;
       DWORD SetCount;                    //Счетчик настроенных параметров
       FILE *fileDevice;

       struct SystemInfo
       {
                AnsiString SystemName;
                AnsiString ModName;
       };
       SystemInfo *sysinf;
       char LiteraBKS;             //управляющая литера, полученная по каналу БКС от программы проверки
       char LiteraBKI;             //управляющая литера, полученная по каналу БКИ от программы проверки

       DWORD SigCountBKS;              //счетчик сигналов(структур), отправляемых по каналу БКС
       DWORD SigCountBKI;              //счетчик сигналов(структур), отправляемых по каналу БКИ
       DWORD OpCountBKS;                //счетчик операций запуска БКС
       DWORD OpCountBKI;                //счетчик операций запуска БКИ
       DWORD PrCountBKS;              //счетчик строк в блоке протокола БКС
       DWORD PrCountBKI;              //счетчик строк в блоке протокола БКИ
       struct SignalsStrucBKS          //структура, хранящая свойства сигналов, отправляемых на программу проверки по каналу БКС
       {
               DWORD SigTime;
               AnsiString SigMessage;
       };
       SignalsStrucBKS *sigbks;

       struct SignalsStrucBKI          //структура, хранящая свойства сигналов, отправляемых на программу проверки по каналу БКИ
       {
               DWORD SigTime;
               AnsiString SigMessage;
       };
       SignalsStrucBKI *sigbki;
       struct StartOpBKI               //структура, хранящие данные об операциях запуска режима БКИ
       {
                AnsiString OpStatus;   //ждать или отправлять сообщение
                AnsiString OpMessage;
                bool OpDone;           //операция выполнена
       };
       StartOpBKI *operbki;

        struct StartOpBKS               //структура, хранящие данные об операциях запуска режима БКC
       {
                AnsiString OpStatus;
                AnsiString OpMessage;
                bool OpDone;
       };
       StartOpBKS *operbks;

       struct PrBKI
       {
                AnsiString PrStatus;
                AnsiString PrMessage;
                bool PrDone;
       };

       PrBKI *protbki;

       struct PrBKS
       {
                AnsiString PrStatus;
                AnsiString PrMessage;
                bool PrDone;
       };
       PrBKS *protbks;
       struct ProgramSettings
       {
                AnsiString SettingName;
                AnsiString SettingStatus;
                void SetSet(AnsiString name) {
                        SettingName = name;
                        SettingStatus = "DONE";
                }
                ProgramSettings() {
                        SettingName = "";
                        SettingStatus = "";
                }
       };
       ProgramSettings *prsets;





};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
