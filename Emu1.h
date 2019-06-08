//---------------------------------------------------------------------------

#ifndef Emu1H
#define Emu1H
#define SYMBOLSNUM 255                             //���-�� �������� �� ��� �������� �� ����� .emu
#define SIGNALSMAX 8192                            //������������ ���������� ������������ �� ��������� �������� ���������
#define OPERMAX 16                                 //������������ ���������� �������� �� .emu �������� ����������� ��� ������� ������
#define PROTMAX 10000                                   //������������ ����� ����� .emu ����� � ����� ���������
#define SETSCOUNT 3                                     //���������� ������������� ���������� ���������
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
#include <syncobjs.hpp>                       //��������� ��� ������ TEvent
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
        void StartStrProc(AnsiString);             //��������� ������ .emu �����, ���������� ����������� ������� ������ ������
        void ProtStrProc(AnsiString);              //�������� ������ .emu �����, ���������� �������� ��������
        void SaveSet(AnsiString);
        void DelSet(AnsiString);
        void StartNull();                       //������� ��������� ��� �������� �����
public:		// User declarations
        TEvent *event;
        HANDLE hCommBKS;       //���������� COM-����� ������ ���
        HANDLE hCommBKI;       //���������� COM-����� ������ ���
        HANDLE BKSRead;       //������������ ��� �������� ������� ������ COM-�����.
        HANDLE BKSWrite;      //������������ ��� �������� ������� ������ COM-�����.
        HANDLE BKIRead;        //������������ ��� �������� ������� ������ COM-�����.
        HANDLE BKIWrite;      //������������ ��� �������� ������� ������ COM-�����.
        HANDLE MessageSendBKI;     //���������� ������� ��������� �������� ��������� ���� <...> �� ������ ���
        HANDLE MessageSendBKS;     //���������� ������� ��������� �������� ��������� ���� <...> �� ������ ���
        HANDLE AllSigBKI;          //���������� ������� ���������� �������� �������� �� ������ ���
        HANDLE AllSigBKS;          //���������� ������� ���������� �������� �������� �� ������ ���
        HANDLE LitBKSReceived;     //���������� ������� ��������� ������ �� ������ ���
        HANDLE LitBKIReceived;     //���������� ������� ��������� ������ �� ������ ���
        AnsiString outBKSBuff, outBKIBuff;      //������ ������ � COM-�����
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
       bool FirstTestFlag;                  //������� ������ ����� ������� ��������� ��������
       bool OpenFileFlag;
       unsigned int StartMessBKIFlag;
       unsigned int StartMessBKSFlag;
       DWORD CloseCountBKI;
       DWORD CloseCountBKS;
       DWORD SetCount;                    //������� ����������� ����������
       FILE *fileDevice;

       struct SystemInfo
       {
                AnsiString SystemName;
                AnsiString ModName;
       };
       SystemInfo *sysinf;
       char LiteraBKS;             //����������� ������, ���������� �� ������ ��� �� ��������� ��������
       char LiteraBKI;             //����������� ������, ���������� �� ������ ��� �� ��������� ��������

       DWORD SigCountBKS;              //������� ��������(��������), ������������ �� ������ ���
       DWORD SigCountBKI;              //������� ��������(��������), ������������ �� ������ ���
       DWORD OpCountBKS;                //������� �������� ������� ���
       DWORD OpCountBKI;                //������� �������� ������� ���
       DWORD PrCountBKS;              //������� ����� � ����� ��������� ���
       DWORD PrCountBKI;              //������� ����� � ����� ��������� ���
       struct SignalsStrucBKS          //���������, �������� �������� ��������, ������������ �� ��������� �������� �� ������ ���
       {
               DWORD SigTime;
               AnsiString SigMessage;
       };
       SignalsStrucBKS *sigbks;

       struct SignalsStrucBKI          //���������, �������� �������� ��������, ������������ �� ��������� �������� �� ������ ���
       {
               DWORD SigTime;
               AnsiString SigMessage;
       };
       SignalsStrucBKI *sigbki;
       struct StartOpBKI               //���������, �������� ������ �� ��������� ������� ������ ���
       {
                AnsiString OpStatus;   //����� ��� ���������� ���������
                AnsiString OpMessage;
                bool OpDone;           //�������� ���������
       };
       StartOpBKI *operbki;

        struct StartOpBKS               //���������, �������� ������ �� ��������� ������� ������ ��C
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
