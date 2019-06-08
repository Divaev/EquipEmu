//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Emu1.h"
#include "com.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
OVERLAPPED ovrlBKSRead;                                    //��������� ��� ���������� ������������ ���������� �����/������
DWORD WINAPI ReadBKSThread(LPVOID);
OVERLAPPED ovrlBKSWrite;
DWORD WINAPI WriteBKSThread(LPVOID);

OVERLAPPED ovrlBKIRead;                                     //��������� OVERLAPPED ������������ ��� ����������� ������ ������ � ������ ������
DWORD WINAPI ReadBKIThread(LPVOID);
OVERLAPPED ovrlBKIWrite;
DWORD WINAPI WriteBKIThread(LPVOID);
DWORD tickbks, tickbki;                                 //�������� �������� ��� � ���
DWORD signumbks, signumbki;                             //������ ������������ ��������� ��������
DWORD nextcount;                                        //������� ��������� �������� 1, ���� �� ������ �� ������� ������� ��������� �� ������� ������
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
        StartNull();
}



void TForm1::StartNull()
{
         CurrDir=GetCurrentDir();
        event=new TEvent(NULL, false, true, "OBJ1");
        sysinf=new SystemInfo;
        sigbki=new SignalsStrucBKI[SIGNALSMAX];
        sigbks=new SignalsStrucBKS[SIGNALSMAX];
        operbki=new StartOpBKI[OPERMAX];
        operbks=new StartOpBKS[OPERMAX];
        protbki=new PrBKI[PROTMAX];
        protbks=new PrBKS[PROTMAX];
        prsets=new ProgramSettings[SETSCOUNT];
        SigCountBKS = 0;
        SigCountBKI = 0;
        OpCountBKS = 0;
        OpCountBKI = 0;
        PrCountBKI = 0;
        PrCountBKS = 0;
        LiteraBKS = NULL;
        LiteraBKI = NULL;
        SignalsSendBKI = false;
        SignalsSendBKS = false;
        OpenFileFlag = false;
        StopFlagBKI = false;
        StopFlagBKS = false;
        PitButtFlagBKS = false;
        PitButtFlagBKI = false;
        NextButtFlagBKS = false;
        NextButtFlagBKI = false;
        TestEndFlagBKS = false;
        TestEndFlagBKI = false;
        FirstTestFlag = true;
        StartMessBKIFlag = 0;
        StartMessBKSFlag = 0;
        CloseCountBKI = 0;
        CloseCountBKS = 0;
        SetCount = 0;
        Form1->ApplySets->Enabled = false;
        Form1->DefaultSets->Enabled = false;
        Form1->PitButt->Enabled = false;
        Form1->NextButt->Enabled = false;
        Form1->PauseButt->Enabled = false;
        Form1->TimerBox->State = cbUnchecked;
        Form1->RoundBox->State = cbUnchecked;
        Form1->RichBKI->Lines->Clear();
        Form1->RichBKS->Lines->Clear();
        Form1->RichBKI->SelAttributes->Style = Form1->RichBKI->SelAttributes->Style << fsBold;                             //��������� �������� ��������� ����� RichBKI � RichBKS
        Form1->RichBKS->SelAttributes->Style = Form1->RichBKS->SelAttributes->Style << fsBold;
        Form1->RichBKI->Lines->Add("����� ���");
        Form1->RichBKS->Lines->Add("����� ���");
        Form1->RichBKI->SelAttributes->Style = Form1->RichBKI->SelAttributes->Style >> fsBold;
        Form1->RichBKS->SelAttributes->Style = Form1->RichBKS->SelAttributes->Style >> fsBold;
        Form1->StatusLabel->Caption = "���������: ��������� ���� ������";
}



//---------------------------------------------------------------------------

void TForm1::OpenDeviceFile()                                                    //������� �������� ����� �������� ������
{
                DeviceFileName = "�������� ���� ������";
                if(DeviceFile->Execute())
                        DeviceFileName = DeviceFile->FileName;
                DeviceFilePath->Text = DeviceFileName;
                FileNull();
                DelSet("FileNameSet");

                if(Form1->COMPortBKI->Text != "COM-���� ���")
                        InitBKI(Form1->COMPortBKI->Text);
                if(Form1->COMPortBKS->Text != "COM-���� ���")
                        InitBKS(Form1->COMPortBKS->Text);

}

void __fastcall TForm1::DeviceFileButtClick(TObject *Sender)
{
        OpenDeviceFile();
        if(DeviceFileName != "�������� ���� ������")                           //���� ������ ����
                CheckingDeviceFile(DeviceFileName);



}

void __fastcall TForm1::PauseButtClick(TObject *Sender)
{
        if(Form1->PauseButt->Caption == "�������������") {
                if(Form1->TimerBox->State == cbChecked) {
                        Form1->StopFlagBKS = true;
                        Form1->StopFlagBKI = true;
                }
                else {
                        Form1->SigDelayBKI->Enabled = false;
                        Form1->SigDelayBKS->Enabled = false;
                }
                Form1->NextButt->Enabled = true;
                Form1->PauseButt->Caption = "����������";
                Form1->StatusLabel->Caption = "���������: ������ ��������������";
        }
        else if(Form1->PauseButt->Caption == "����������") {
                if(Form1->TimerBox->State == cbChecked) {
                Form1->StopFlagBKS = false;
                Form1->SignalsSendBKS = true;
                Form1->StopFlagBKI = false;
                Form1->SignalsSendBKI = true;
                ResumeThread(Form1->BKIWrite);
                ResumeThread(Form1->BKSWrite);
                }
                else {
                        Form1->SigDelayBKI->Enabled = true;
                        Form1->SigDelayBKS->Enabled = true;
                }
                Form1->NextButt->Enabled = false;
                Form1->PauseButt->Caption = "�������������";

        }
}

void __fastcall TForm1::NextButtClick(TObject *Sender)
{
        Form1->SignalsSendBKS = true;
        Form1->NextButtFlagBKS = true;
        Form1->outBKSBuff = "";
        ResumeThread(Form1->BKSWrite);
        Form1->SignalsSendBKI = true;
        Form1->NextButtFlagBKI = true;
        Form1->outBKIBuff = "";
        ResumeThread(Form1->BKIWrite);
         if(Form1->TimerBox->State == cbUnchecked) {
                nextcount = 0;
                Form1->NextButt->Enabled = false;
                Form1->SigDelayBKS->Enabled = true;
                Form1->SigDelayBKI->Enabled =true;
                Form1->StatusLabel->Caption = "���������: �������� �������";
        }
}





 void TForm1::CheckingDeviceFile(AnsiString filename)                            //�������� ����� ��������, �������� ������
{
        AnsiString temp = "";
        if((fileDevice = fopen(filename.c_str(), "a+b")) == NULL) {
                temp = "�� ������� ������� ���� " + filename;
                Application->MessageBox(temp.c_str(), "���� ����������!", MB_OK);
                Form1->FileOpenImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
        }
        else    {
                FileOpenImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                GetSystemInfo(fileDevice);                                      //�������� ��������� �� ���� � ������� ��������� ���������� � �������
                StartModBKI(fileDevice);                                        //������ �� ����� �������� ������� ������ ���
                StartModBKS(fileDevice);                                        //������ �� ����� �������� ������� ������ ���
                StartMessBKIFlag = 0;                                            //�������� ���� ������� ��������� "?START>" � ���������� ����� ���
                StartMessBKSFlag = 0;                                            //�������� ���� ������� ��������� "?START>" � ���������� ����� ���
                SignalsModBKI(fileDevice);                                      //������ �� ����� ��������� �� ���
                SignalsModBKS(fileDevice);                                      //������ �� ����� ��������� �� ���
                ProtocolModBKS(fileDevice);
                ProtocolModBKI(fileDevice);
                SaveSet("FileNameSet");
               OpenFileFlag = true;
               Form1->SentBKSLabel->Caption = "���: 0/" + IntToStr(SigCountBKS-2-StartMessBKSFlag);               //���������� ��������, �������� ��������� ����� "!RDY>", "?OK>"
               Form1->SentBKILabel->Caption = "���: 0/" + IntToStr(SigCountBKI-2-StartMessBKIFlag);
               Form1->StatusLabel->Caption = "���������: ��������� �������";
        }
}

//------------------------------������� ��������� ������ COM-������ �� ��������---------------------------------
void TForm1::GetSerialPortsList(AnsiString port)
{
    AnsiString KeyName = "\\Hardware\\DeviceMap\\SerialComm";
    TStringList *SerialCommValues = new TStringList();

    TRegistry *Registry = new TRegistry;
    try{
        if(port == "BKI") {
                Form1->COMPortBKI->Items->Clear();
                Registry->RootKey = HKEY_LOCAL_MACHINE;
                Registry->OpenKeyReadOnly( KeyName );
                Registry->GetValueNames( SerialCommValues );
                for(int i=0; i<SerialCommValues->Count; i++)
                {
                //if(Registry->ReadString(SerialCommValues->Strings[i]) != Form1->COMPortBKS->Text)     ������ ���� ������� �������� �������!
                Form1->COMPortBKI->Items->Add(Registry->ReadString(SerialCommValues->Strings[i]));
                }
        }
        else if(port == "BKS") {
                Form1->COMPortBKS->Items->Clear();
                Registry->RootKey = HKEY_LOCAL_MACHINE;
                Registry->OpenKeyReadOnly( KeyName );
                Registry->GetValueNames( SerialCommValues );
                for(int i=0; i<SerialCommValues->Count; i++)
                {
                //if(Registry->ReadString(SerialCommValues->Strings[i]) != Form1->COMPortBKI->Text)     ������ ���� ������� �������� �������!
                Form1->COMPortBKS->Items->Add(Registry->ReadString(SerialCommValues->Strings[i]));
                }
        }
    }
    __finally                                   //����������� ���������� �� ���������� �������������� ��������
    {
        delete Registry;
        delete SerialCommValues;
    }

}
//-------------------------------------------------------------------------------------------------------------------------

void __fastcall TForm1::COMPortBKIDrop(TObject *Sender)
{
        GetSerialPortsList("BKI");
}

void __fastcall TForm1::COMPortBKSDrop(TObject *Sender)
{
        GetSerialPortsList("BKS");
}

void __fastcall TForm1::COMPortBKISelect(TObject *Sender)
{
        AnsiString temp = "";
        temp = Form1->COMPortBKI->Text + " ��� ������.";
        if(Form1->COMPortBKI->ItemIndex > 0)                                    //���� �������� ������ COM-������ (� ���������� ������ >0 ��-��)
                if(Form1->COMPortBKI->Text != Form1->COMPortBKS->Text)          //���� COM-���� �� ��������� � ��������� ����� �� �������� ������
                        InitBKI(Form1->COMPortBKI->Text);

                else    {
                       Application->MessageBox(temp.c_str(), "������ COM-�����!", MB_OK);
                       Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                       DelSet("ComBKISet");
               }
}
void __fastcall TForm1::COMPortBKSSelect(TObject *Sender)
{
        AnsiString temp = "";
        temp = Form1->COMPortBKS->Text + " ��� ������.";
        if(Form1->COMPortBKS->ItemIndex > 0)
                if(Form1->COMPortBKS->Text != Form1->COMPortBKI->Text)           //���� COM-���� �� ��������� � ��������� ����� �� �������� ������
                        InitBKS(Form1->COMPortBKS->Text);
                else    {
                       Application->MessageBox(temp.c_str(), "������ COM-�����!", MB_OK);
                       Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                       DelSet("ComBKSSet");
                }
}



void TForm1::InitBKS(AnsiString portname)

{
        AnsiString temp = "";
        temp = "���������� ������� " + portname;
        if (hCommBKS) CComUse.ComClose(hCommBKS);
        CComUse.ComClear(hCommBKS, PURGE_TXCLEAR|PURGE_RXCLEAR);
        Form1->event->WaitFor(200);
        hCommBKS=CComUse.ComInit(portname.c_str(), FILE_FLAG_OVERLAPPED);
         if(!hCommBKS) {
                Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                Application->MessageBox(temp.c_str() , "������ COM-�����!", MB_OK);
                DelSet("ComBKSSet");
         }
        else {
                Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                if(BKSRead)
                        TerminateThread(BKSRead, 0);
                BKSRead=CreateThread(NULL, 0, ReadBKSThread, NULL, 0, NULL);
                BKSWrite=CreateThread(NULL, 0, WriteBKSThread, NULL, CREATE_SUSPENDED, NULL);                     //���������� ������ ������ �� ������ ���
                //-------------���������� ��������� � �������� ���� ��������-----------------------
                SaveSet("ComBKSSet");
        }
}
void TForm1::InitBKI(AnsiString portname)
{
        AnsiString temp = "";
        temp = "���������� ������� " + portname;
        if (hCommBKI) CComUse.ComClose(hCommBKI);
        CComUse.ComClear(hCommBKI, PURGE_TXCLEAR|PURGE_RXCLEAR);
        Form1->event->WaitFor(200);                                                     //����� �������� ����������� COM-����� ����� ��������� ���������, �.�. ���������� ����� ��� �������������� � ���������� ������
        hCommBKI=CComUse.ComInit(portname.c_str(), FILE_FLAG_OVERLAPPED);
        if(!hCommBKI) {
                Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                Application->MessageBox(temp.c_str() , "������ COM-�����!", MB_OK);
                DelSet("ComBKISet");
        }
        else {
                Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                if(BKIRead)
                        TerminateThread(BKIRead, 0);
                BKIRead=CreateThread(NULL, 0, ReadBKIThread, NULL, 0, NULL);
                BKIWrite=CreateThread(NULL, 0, WriteBKIThread, NULL, CREATE_SUSPENDED, NULL);                         //���������� ������ ������ �� ������ ���
                SaveSet("ComBKISet");
        }


        /*
        if (!hCommBKI)
        {
                LogMemoBKI->Lines->Add("������ ������������� "+temp+".");
                Skpa_5->DeviceComboBox->Enabled=false;
                Skpa_5->Vst05ComboBox->Enabled=false;
                Skpa_5->ProtokolBKSBitBtn->Enabled=false;
                Skpa_5->ProtokolBKIBitBtn->Enabled=false;
                Skpa_5->StageLabel->Caption=Skpa_5->ChangeCap("�������: "+cmdsets->SystemName, "�����: ������.", "������ ������������� �����: "+temp, "");
        }
        else
        {
                BKIRead=CreateThread(NULL, 0, ReadBKIThread, NULL, 0, NULL);
                BKIWrite=CreateThread(NULL, 0, WriteBKIThread, NULL, CREATE_SUSPENDED, NULL);
        }
        */
}




//----------------------------------------------������ � �����������-------------------------------------------------------------
void TForm1::SaveSet(AnsiString name)                                           //���������� ����� ���������� � ��������� ������������
{
        DWORD i = 0;
        while((prsets[i].SettingName != name) && (i != SetCount))               //���� �������� � ����� ������������� ����������, �������� ��� ������ �� "DONE"
                       i++;
        if(i < SetCount)
                prsets[i].SettingStatus = "DONE";
        else
                if(SetCount <= SETSCOUNT - 1) {                                        //���� ��������� �� ����������, � ���� ����� ��������� ������ ����. ���-��, ������ ����� ��������
                        prsets[SetCount].SetSet(name);
                        if(SetCount != SETSCOUNT - 1)                                 //�������������� ����� ���������, ���� �� ���������� ������������ ��������
                        SetCount++;
                }
        if(SetCount == SETSCOUNT - 1) {
                for(i=0; i<=SetCount; i++)                                              //���� ��� ���������(���������) ���������� ������������ ������ "������ �������"
                        if(prsets[i].SettingStatus != "DONE")
                                        break;

                if(i > SetCount)
                        Form1->ApplySets->Enabled = true;
                else {
                        Form1->ApplySets->Enabled = false;
                        Form1->PitButt->Enabled = false;
                }
        }



}

void TForm1::DelSet(AnsiString name)                                            //������ ������������ ����������
{
        DWORD i;
        for(i=0; i<=SetCount; i++)
                if(prsets[i].SettingName == name) {
                        prsets[i].SettingStatus = "UNDONE";
                        Form1->PitButt->Enabled = false;
                        Form1->ApplySets->Enabled = false;
                        Form1->StatusLabel->Caption = "���������: ��������� �������";
                }
}


void __fastcall TForm1::ApplySetsClick(TObject *Sender)                                             //������� ������ "���������" � ����� ��������
{

        Form1->PitButt->Enabled = true;                                         //������������ ������ "������ �������"
        Form1->COMPortBKI->Enabled = false;
        Form1->COMPortBKS->Enabled = false;
        Form1->DefaultSets->Enabled = true;
        Form1->DeviceFilePath->Enabled = false;
        Form1->DeviceFileButt->Enabled = false;
        MakeDeviceFile->Enabled = false;                                        //������������ ������ �������� ����� ������
        if(OpenFileFlag == false) {                                             //���� ����� ���������� �������� �� ������ ����� ����������
                FileNull();
                CheckingDeviceFile(DeviceFileName);                             //�������� � ��������� ������ �� �������� �����
                if(Form1->COMPortBKI->Text != "COM-���� ���")
                        InitBKI(Form1->COMPortBKI->Text);
                if(Form1->COMPortBKS->Text != "COM-���� ���")
                        InitBKS(Form1->COMPortBKS->Text);
        }
        Form1->StatusLabel->Caption = "���������: ��������� ������� ���������";
        Form1->ApplySets->Enabled = false;

}

void __fastcall TForm1::DefaultSetsClick(TObject *Sender)
{
        DelSet("FileNameSet");
        DelSet("ComBKISet");
        DelSet("ComBKSSet");
        DeviceFilePath->Text = "�������� ���� ������";
        DeviceFilePath->Enabled = true;
        DeviceFileButt->Enabled = true;
        MakeDeviceFile->Enabled =true;
        Form1->TimerBox->State = cbUnchecked;
        Form1->RoundBox->State = cbUnchecked;
        Form1->TimerBox->Enabled = true;
        Form1->RoundBox->Enabled = true;
        Form1->COMPortBKI->Text = "COM-���� ���";
        Form1->COMPortBKS->Text = "COM-���� ���";
        Form1->COMPortBKI->Enabled = true;
        Form1->COMPortBKS->Enabled = true;
        Form1->PitButt->Enabled = false;
        Form1->DefaultSets->Enabled = false;
}

void __fastcall TForm1::MakeDeviceClick(TObject *Sender)
{
        if(!MakeFileForm)
                MakeFileForm = new TMakeFileForm(this);                     //�������� ������(�������) �����, Auto-create ��������
        MakeFileForm->Show();                                       //���������� �����
}






//--------------��������� ����������  ��� ��������� ��������-------------------------------------------------------------------------
void TForm1::NullAll(DWORD num)
{
        static int rdycount = 0;                                        //������� ����������� ��������(�� �������), ���� =2  �������� ��������� � �� ���, � �� ���
        if(num == 1) {                                                    //��������� ����� ������ ���������
                if(rdycount == 1) {                                                       //���� �������� �� ��������� ������ ����� ���������
                        if(Form1->RoundBox->State == cbUnchecked){                        //���� �� ����������� ����� ����������� ��������
                               Form1->PitButt->Enabled = true;                                   //���������� ������ ������ �������
                                Form1->PitButt->Caption = "������ �������";
                                Form1->NextButt->Enabled = false;
                                Form1->PauseButt->Enabled = false;
                                Form1->PauseButt->Caption = "�������������";
                                 Form1->COMPortBKI->Enabled = true;
                                Form1->COMPortBKS->Enabled = true;
                                Form1->TimerBox->Enabled = true;
                                Form1->RoundBox->Enabled = true;
                                Form1->DeviceFileButt->Enabled = true;
                                Form1->MakeDeviceFile->Enabled = true;
                           //     DeviceFilePath->Text = "�������� ���� ������";
                                Form1->ApplySets->Enabled = true;
                                OpenFileFlag = false;
                           //     tickbks = tickbki = 0;                           //�������� �������
                           //     signumbks = signumbki = 0;                         //�������� ������ ��������
                        }

                        Form1->PitButt->Caption = "������ �������";
                        Form1->PitButt->Enabled = false;
                        Form1->NextButt->Enabled = false;
                        Form1->PauseButt->Enabled = false;
                        Form1->PauseButt->Caption = "�������������";
                        Form1->SigDelayBKI->Enabled = false;
                        Form1->SigDelayBKS->Enabled = false;
                        tickbks = tickbki = 0;
                        signumbks = signumbki = 0;                              //�������� �������
                        rdycount = 0;                                           //�������� ������ ��������

                       if(Form1->TimerBox->State == cbChecked) {
                                Form1->SignalsSendBKI = false;
                                Form1->SignalsSendBKS = false;
                        }
                        Form1->StatusLabel->Caption = "���������: �������� ���������";

                }

                else
                        rdycount++;
        }
        if(num == 2) {                                                          //��������� ����� ������ ������
                Form1->PitButt->Enabled = true;
                Form1->PitButt->Caption = "�������� ������";
                Form1->PauseButt->Enabled = true;
                Form1->PauseButt->Caption = "�������������";
                Form1->DeviceFileButt->Enabled = false;
                Form1->ApplySets->Enabled = false;
                Form1->TimerBox->Enabled = false;
                Form1->RoundBox->Enabled = false;
                Form1->COMPortBKI->Enabled = false;
                Form1->COMPortBKS->Enabled = false;
                Form1->DeviceFilePath->Enabled = false;
                Form1->DefaultSets->Enabled = false;

        }
          if(num == 3) {                                                        //��������� ����� ������� ���������
                        Form1->PitButt->Enabled = false;
                        Form1->PitButt->Caption = "������ �������";
                        Form1->NextButt->Enabled = false;
                        Form1->PauseButt->Enabled = false;
                        Form1->PauseButt->Caption = "�������������";
                        Form1->DeviceFileButt->Enabled = false;
                        Form1->StatusLabel->Caption = "���������: ����� ���������";
                      //  Form1->SigDelayBKI->Enabled = false;
                      //  Form1->SigDelayBKS->Enabled = false;

        }
        if(num == 4) {                                       //������ ��������� ������ ������� "�������� ������"
                    Form1->PitButt->Enabled = false;
                    Form1->PitButt->Caption = "������ �������";
                    Form1->NextButt->Enabled = false;
                    Form1->PauseButt->Enabled = false;
                    Form1->PauseButt->Caption = "�������������";
                    if(Form1->RoundBox->State == cbChecked)
                        Form1->RoundBox->State = cbUnchecked;
                     Form1->TimerBKILabel->Caption = "���: 0";
                     Form1->TimerBKSLabel->Caption = "���: 0";
                    /*
                    Form1->DeviceFileButt->Enabled = true;
                    Form1->COMPortBKI->Enabled = true;
                    Form1->COMPortBKS->Enabled = true;
                    Form1->TimerBox->Enabled = true;
                    Form1->RoundBox->Enabled = true;
                    tickbks = tickbki = 0;                           //�������� �������
                    signumbks = signumbki = 0;                         //�������� ������ ��������
                    */
        }

}

//----------------------------��������� ��������� � �������� �������� ��� ������ ������ ����� ��������-------------------------------------

void TForm1::FileNull()
{

        SigCountBKS = 0;
        SigCountBKI = 0;
        OpCountBKS = 0;
        OpCountBKI = 0;
        PrCountBKI = 0;
        PrCountBKS = 0;

        delete sysinf;
        delete []sigbki;
        delete []sigbks;
        delete []operbki;
        delete []operbks;
        delete []protbki;
        delete []protbks;

        sysinf=new SystemInfo;
        sigbki=new SignalsStrucBKI[SIGNALSMAX];
        sigbks=new SignalsStrucBKS[SIGNALSMAX];
        operbki=new StartOpBKI[OPERMAX];
        operbks=new StartOpBKS[OPERMAX];
        protbki = new PrBKI[PROTMAX];
        protbks = new PrBKS[PROTMAX];



}


//-------------------� ���� ����� ������������ ����������� ������������� �������� ������ �� COM-������----------------------------------

DWORD WINAPI WriteBKSThread(LPVOID lpParam)
{
        static bool startflag = false;
        unsigned int perc = 0;
        DWORD stopcount;
        DWORD sigcount,closecount;
        char snd[1];
        char sndstr[1024];
        DWORD temp;
        ovrlBKSWrite.hEvent=CreateEvent(NULL, true, true, NULL);
        Form1->MessageSendBKS=CreateEvent(NULL, true, true, NULL);
        Form1->AllSigBKS=CreateEvent(NULL, true,true, NULL);
        if(Form1->TimerBox->State == false)                            //���� �������� "������� ��� ��������" �� ������� ��������, ��������� �������� ��������� �������� �������� 1
                        Form1->StopFlagBKS = true;
        while(1)
        {
                ResetEvent(Form1->MessageSendBKS);                             //�������� ������� "��������� ����������"
                ResetEvent(Form1->AllSigBKS);                                  //�������� ������� "��� ��������� ����������"
                if(Form1->outBKSBuff != "")
                       /*
                        while(Form1->outBKSBuff.Length()>0)
                        {
                                snd[0]=Form1->outBKSBuff[1];
                                Form1->outBKSBuff.Delete(1, 1);
                                CComUse.ComClear(Form1->hCommBKS, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKS, snd, 1, &temp, &ovrlBKSWrite);
                                WaitForSingleObject(ovrlBKSWrite.hEvent, INFINITE);
                                Form1->event->WaitFor(1);
                        } */
                        while(Form1->outBKSBuff.Length()>0)
                         {
                                strcpy(sndstr, Form1->outBKSBuff.c_str());
                                Form1->outBKSBuff = "";
                                Form1->RichBKS->Lines->Add(sndstr);             //������ ������������ ������ � ��������� ���� RichBKS
                                CComUse.ComClear(Form1->hCommBKS, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKS, sndstr, strlen(sndstr), &temp, &ovrlBKSWrite);
                                WaitForSingleObject(ovrlBKSWrite.hEvent, INFINITE);

                         }

                SetEvent(Form1->MessageSendBKS);                                //������������� ������� "��������� ����������"
                if(Form1->TimerBox->State == true)
                        Sleep(1);                      //����������� �������� ����� ������������ ������ �� ������ ���
                //-----------------------------�������� ��������� (���. � ��������) �� ������ ��� ��� �������--------------------------
                 if(Form1->SignalsSendBKS == true && sigcount <= Form1->SigCountBKS) {                            //���� ���� ������� �������� ��������� � ��������, � ��������� �� ��� ���������
                        ResetEvent(Form1->MessageSendBKS);                                                       //�������� ������� "�������������������"
                        if(Form1->StopFlagBKS == false && Form1->CloseFlagBKS == false) {                                                        //���� ������� ��������������� ���������� �����������
                                Form1->outBKSBuff = Form1->sigbks[sigcount].SigMessage;                          //��������� ��������� ������ �������� ���������� ��������� �� .emu �����
                                if(Form1->sigbks[sigcount].SigMessage == "?START>")
                                        startflag = true;                                                    //���� ������ ��������� "?START>", ���������� ��������������� ����

                                if(sigcount <=Form1->SigCountBKS - 2) {
                                        if(Form1->StartMessBKSFlag == 0)
                                                Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                        Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }


                                if(Form1->sigbks[sigcount].SigMessage == "?OK>")
                                                Sleep(300);
                                if(sigcount > 0 && sigcount <= Form1->SigCountBKS-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKS-2) - (sigcount-1)*100/(Form1->SigCountBKS-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                                        Form1->SigBarBKS->StepIt();
                                perc = 0;
                                sigcount++;
                        }

                        else if(Form1->CloseFlagBKS == true) {                        //���� ����� ��������� �������� �������
                                Form1->outBKSBuff = "<ERROR! Emulator program has been closed. Pls close this program.>";
                                Form1->SignalsSendBKS = false;                        //����������� 0 �������� �������� ���������
                        }
                        else if(Form1->PitButtFlagBKS == true) {                      //����� ������� ������ "�������� ������"
                                if(++stopcount != 4) {                                //���� ������� ���������� ������ 1, ���������� ��������� �� ��������� �������� ��������������� ���������
                                        switch(stopcount) {
                                        case 1:
                                                Form1->outBKSBuff = "<���������� ������ ���������>";
                                                break;
                                        case 2:
                                                Form1->outBKSBuff = "?ERR>";
                                                break;
                                        case 3:
                                                Form1->outBKSBuff = "!RDY>";
                                                break;
                                        default:
                                                break;
                                        }
                                }
                                else {
                                        Form1->SignalsSendBKS = false;
                                        Form1->PitButtFlagBKS = false;
                                        Form1->outBKSBuff = "";
                                        stopcount = 0;
                                }
                        }

                        else                                                            //���� ������� ��������� �������� Form1->StopFlagBKS = 1
                                Form1->SignalsSendBKS = false;
                        if(Form1->NextButtFlagBKS == true && Form1->TimerBox->State == cbChecked) {                    //����� ������� ������ "��������� ������"
                                Form1->outBKSBuff = Form1->outBKSBuff + Form1->sigbks[sigcount].SigMessage;
                                if(Form1->sigbks[sigcount].SigMessage == "?START>")
                                        startflag = true;                                                    //���� ������ ��������� "?START>", ���������� ��������������� ����

                                if(sigcount<= Form1->SigCountBKS - 2) {
                                        if(Form1->StartMessBKSFlag == 0)
                                                Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                        Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }

                                if(sigcount > 0 && sigcount <= Form1->SigCountBKS-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKS-2) - (sigcount-1)*100/(Form1->SigCountBKS-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                                        Form1->SigBarBKS->StepIt();
                                perc = 0;
                                sigcount++;
                                Form1->NextButtFlagBKS = false;                //��������� �������� �������� ���������� ������� �������� 0
                                }
                        continue;
                }



                SetEvent(Form1->AllSigBKS);                                     //���������� ������� "��� ��������� ����������"
//-----------------------------------------------------------------------------------------------------------------------
                ResumeThread(Form1->BKSRead);                                    //������������ ����� ������ ���
                SuspendThread(Form1->BKSWrite);                                  //������������� ����� ������ ���

        }
}

DWORD WINAPI WriteBKIThread(LPVOID lpParam)
{
        static bool startflag = false;
        unsigned int perc = 0;
        DWORD stopcount;
        DWORD sigcount, closecount;
        char snd[1];
        char sndstr[1024];
        DWORD temp;
        ovrlBKIWrite.hEvent=CreateEvent(NULL, true, true, NULL);
        Form1->MessageSendBKI=CreateEvent(NULL, true, true, NULL);
        Form1->AllSigBKI=CreateEvent(NULL, true,true, NULL);
        if(Form1->TimerBox->State == false)
                Form1->StopFlagBKI = true;
        while(1)
        {
                ResetEvent(Form1->MessageSendBKI);
                ResetEvent(Form1->AllSigBKI);
                if(Form1->outBKIBuff != "")                      //���� ����� �������� �������� �� � �������� ����������

//----------------------------------� ������ ����� ����������� ������������ ������, ������� �� �������� ����� ������������ ������ �������------------------
                        /*
                         while(Form1->outBKIBuff.Length()>0)
                         {
                                snd[0]=Form1->outBKIBuff[1];
                                Form1->outBKIBuff.Delete(1, 1);
                                CComUse.ComClear(Form1->hCommBKI, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKI, snd, 1, &temp, &ovrlBKIWrite);
                                WaitForSingleObject(ovrlBKIWrite.hEvent, INFINITE);
                                Form1->event->WaitFor(1);
                         } */
//-------------------------����� ���������� ����� ������ � COM-����---------------------------------------------------------------------------------
                         while(Form1->outBKIBuff.Length()>0)
                         {
                                strcpy(sndstr, Form1->outBKIBuff.c_str());
                                Form1->RichBKI->Lines->Add(sndstr);                           //������ ������������ ������ � ��������� ���� RichBKI
                                Form1->outBKIBuff = "";
                                CComUse.ComClear(Form1->hCommBKI, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKI, sndstr, strlen(sndstr), &temp, &ovrlBKIWrite);
                                WaitForSingleObject(ovrlBKIWrite.hEvent, INFINITE);

                         }
//----------------------------------------------------------------------------------------------------------------------------


                SetEvent(Form1->MessageSendBKI);
                if(Form1->TimerBox->State == true)
                        Sleep(1);                                                      //����������� �������� ����� ������������ ������ �� ������ ���
//-----------------------------�������� ��������� (���. � ��������) �� ������ ��� ��� �������--------------------------
                if(Form1->SignalsSendBKI == true && sigcount <= Form1->SigCountBKI) {
                        ResetEvent(Form1->MessageSendBKI);
                        if(Form1->StopFlagBKI == false && Form1->CloseFlagBKI == false) {
                                Form1->outBKIBuff = Form1->sigbki[sigcount].SigMessage;
                                if(Form1->sigbki[sigcount].SigMessage == "?START>")
                                        startflag = true;

                                if(sigcount <= Form1->SigCountBKI - 2) {
                                        if(Form1->StartMessBKIFlag == 0)
                                                Form1->SentBKILabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKI-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKILabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                                else
                                                        Form1->SentBKILabel->Caption = "���: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                        }
                                }



                                if(Form1->sigbki[sigcount].SigMessage == "?OK>")                                 // �������� ����� "?OK>" � ����� "!RDY>"
                                        Sleep(300);
                                if(sigcount > 0 && sigcount <= Form1->SigCountBKI-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKI-2) - (sigcount-1)*100/(Form1->SigCountBKI-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                                        Form1->SigBarBKI->StepIt();
                                perc = 0;
                                sigcount++;

                        }

                       else if(Form1->CloseFlagBKI == true) {                                   //�������� ����� ��������� �� ����� �������� (������������ ����������)
                                Form1->outBKIBuff = "<ERROR! Emulator program has been closed. Pls close this program.>";
                                Form1->SignalsSendBKI = false;
                       }


                       else if(Form1->PitButtFlagBKI == true) {
                                if(++stopcount != 4) {
                                        switch(stopcount) {
                                        case 1:
                                                Form1->outBKIBuff = "<���������� ������ ���������>";
                                                break;
                                        case 2:

                                                Form1->outBKIBuff = "?ERR>";
                                                break;
                                        case 3:
                                                Form1->outBKIBuff = "!RDY>";
                                                break;
                                        default:
                                                break;
                                        }
                                }
                                else {
                                        Form1->SignalsSendBKI = false;
                                        Form1->PitButtFlagBKI = false;
                                        Form1->outBKIBuff = "";
                                        stopcount = 0;
                                }

                        }

                        else
                                Form1->SignalsSendBKI = false;
                        if(Form1->NextButtFlagBKI == true && Form1->TimerBox->State == true) {
                                Form1->outBKIBuff = Form1->outBKIBuff + Form1->sigbki[sigcount].SigMessage;
                                if(Form1->sigbks[sigcount].SigMessage == "?START>")
                                        startflag = true;
                                if(sigcount <= Form1->SigCountBKS - 2) {
                                        if(Form1->StartMessBKSFlag == 0)
                                               Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                     Form1->SentBKSLabel->Caption = "���: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }

                                if(sigcount > 0 && sigcount <= Form1->SigCountBKI-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKI-2) - (sigcount-1)*100/(Form1->SigCountBKI-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                                        Form1->SigBarBKI->StepIt();
                                perc = 0;
                                sigcount++;
                                Form1->NextButtFlagBKI = false;
                        }
                        continue;
                }
//-----------------------------------------------------------------------------------------------------------------------
                SetEvent(Form1->AllSigBKI);
                ResumeThread(Form1->BKIRead);                                   //������������ ����� ������ ���
                SuspendThread(Form1->BKIWrite);                                 //������������� ����� ������ ���

        }
}

DWORD WINAPI ReadBKSThread(LPVOID lpParam)
{
        DWORD mask, signal, tmp, num, realnum;
        DWORD beginSymbStr, endSymbStr;
        DWORD count = 0;
        DWORD countprot = 0;
        char bufferBKS[2];
        AnsiString RichStr = "";
        COMSTAT comstat;                                                     //��������� COMSTAT �������� ���������� � ���������������� ����������
        ovrlBKSRead.hEvent=CreateEvent(NULL, true, true, NULL);
        Form1->LitBKSReceived=CreateEvent(NULL, true, true, NULL);
        while(1)
        {

                WaitCommEvent(Form1->hCommBKS, &mask, &ovrlBKSRead);
                signal=WaitForSingleObject(ovrlBKSRead.hEvent, INFINITE);
                if(signal==WAIT_OBJECT_0)
                {
                        if(GetOverlappedResult(Form1->hCommBKS, &ovrlBKSRead, &tmp, true))
                        {

                                if ((mask&EV_RXCHAR)==EV_RXCHAR)
                                {
                                        ClearCommError(Form1->hCommBKS, &tmp, &comstat);
                                        num=comstat.cbInQue;                                            //cbInQue - �������� ��������� COMSTAT - ���������� ����������,
                                        if (num)                                                        //�� ��� �� ����������� �������� ReadFile ����
                                        {
                                                Form1->event->WaitFor(100);        //?
                                                memset(bufferBKS, NULL, 2);
                                                ReadFile(Form1->hCommBKS, bufferBKS, num, &realnum, &ovrlBKSRead);
                                                Form1->LiteraBKS=bufferBKS[0];              //��������� ����������� ������ �� ������ ���
                                                if(bufferBKS[0] != NULL)
                                                        RichStr += bufferBKS[0];
                                                Form1->RichBKS->Lines->Add("<--- " + RichStr); //������ ���������� ������� � ��������� ���� RichBKS
                                                RichStr = "";
                                                SetEvent(Form1->LitBKSReceived);
                                                if(count < Form1->OpCountBKS) {                               //���� ���-�� ���������� ����� ������ ����� ����������� �� .emu ����� ����������� ��������
                                                        if(count == 0 || (count == Form1->OpCountBKS - 1 && Form1->FirstTestFlag == false))              //���� �������� ������ ����������� ������� ��� �������, �������������� ������ �������� ��������, ��� ����������� ����������
                                                                Form1->NullAll(2);
                                                        if(Form1->LiteraBKS == Form1->operbks[count].OpMessage[1] && Form1->operbks[count].OpStatus == "WAIT" ){                //���� ���������� ������ ����� ���������, � ��� ������� ����������� �������� - "��������"
                                                                Form1->operbks[count].OpDone = true;               //����������� 1 �������� ����������� �������� "���������"
                                                                count++;
                                                        }
                                                   //     else
                                                   //             continue;
                                                        while(Form1->operbks[count].OpStatus == "SEND") {          //���� ������� ����������� �������� ����� ��� "��������"
                                                                Sleep(500);                                                  //�������� 0.5 � ����� ���������, ������������� �� ���
                                                                Form1->SendStringBKS(Form1->operbks[count].OpMessage);        //���������� �������, ����������� �� .emu ����� � ���������� � �����. ���������
                                                                count++;
                                                                SuspendThread(Form1->BKSRead);                               //����� �������� ��������� �� ��������� �������� ������������� ����� ������ ���

                                                        }
                                                }
                                                if(count == Form1->OpCountBKS) {                       //���� ���������� ��������� ����������� ��������
                                                        if(Form1->TimerBox->State == cbUnchecked)
                                                                Form1->SigDelayBKS->Enabled = true;                         //��������� ������ ���
                                                        else {
                                                        Form1->SignalsSendBKS = true;                                       //����������� 1 �������� �������� ���������
                                                        Form1->outBKSBuff = "";                                           //������������ ����� ������ �� ������ ���
                                                        ResumeThread(Form1->BKSWrite);                                      //����������� �������, ����� ������������� ������� �� �����������, ���������� �����. ��. ���
                                                        }
                                                        Form1->SigBarBKS->Position = 0;
                                                        Form1->StatusLabel->Caption = "���������: ���������� ��������";
                                                        count++;
                                                }


                                                if(countprot < Form1->PrCountBKS) {

                                                        if(Form1->LiteraBKS == Form1->protbks[countprot].PrMessage[1] && Form1->protbks[countprot].PrStatus == "WAIT" ){                //���� ���������� ������ ����� ���������, � ��� ������� ������ ��������� - "��������"
                                                                Form1->protbks[countprot].PrDone = true;               //����������� 1 �������� "���������" ������ ���������
                                                                if(countprot == 0)                                    //�������� ���������, ���������� �������� ���������� � �������� ���������
                                                                        Form1->NullAll(3);
                                                                countprot++;

                                                        }
                                                        while(Form1->protbks[countprot].PrStatus == "SEND") {          //���� ������� ������ ��������� ����� ��� "��������"
                                                                WaitForSingleObject(Form1->AllSigBKS, INFINITE);
                                                                Sleep(10);                                      //�������� ����� ����������� ���������, ������������� �� ��������� ��������
                                                                Form1->SendStringBKS(Form1->protbks[countprot].PrMessage);        //���������� ������ ���������, ����������� �� .emu ����� � ���������� � �����. ���������
                                                                countprot++;
                                                                SuspendThread(Form1->BKSRead);                               //����� �������� ��������� �� ��������� �������� ������������� ����� ������ ���

                                                        }


                                                }

                                                if(countprot == Form1->PrCountBKS) {                                       //����� ������ ��������� �������� �������� ��� ��������� ��������
                                                        countprot = 0;
                                                        count = Form1->OpCountBKS - 1;                                    //��������� � ��������� ����������� ��������
                                                        Form1->NullAll(1);
                                                        Form1->StopFlagBKS = false;
                                                        Form1->PitButtFlagBKS = false;
                                                        Form1->FirstTestFlag = false;
                                                        TerminateThread(Form1->BKSWrite, 0);                               //����� ���������� ��������(������ ���������) ������� �����
                                                        Form1->BKSWrite=CreateThread(NULL, 0, WriteBKSThread, NULL, CREATE_SUSPENDED, NULL);      //� ������� ������

                                                }
                                                //------------------��������� ����������� ������, ����������� �� ����� ������ ��������� �� ����� ���-------------------------------------
                                               if(Form1->LiteraBKS == '~') {

                                                        WaitForSingleObject(Form1->AllSigBKS, INFINITE);                         //���� ��������� �������� ��������� �� ���
                                                     //   SuspendThread(Form1->BKSWrite);                                               //������������� ����� ������ ���
                                                        Form1->StopFlagBKS = true;                                                    //������������� ������� ��������������� ����������
                                                        Sleep(200);                                                                   //�������� ����� ��������� ������� ����������
                                                        Form1->SendStringBKS("<3 0 50>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("?ERR>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("!RDY>");
                                                        Form1->LiteraBKS = NULL;                                                       //������� ������� ����� ���
                                                        ResumeThread(Form1->BKSWrite);                                                 //������������ ����� ������ ���
                                                        SuspendThread(Form1->BKSRead);                                                //������������� ����� ������ ���
                                               }
                                               if(Form1->LiteraBKS == '!') {
                                                        WaitForSingleObject(Form1->MessageSendBKS, INFINITE);
                                                        SuspendThread(Form1->BKSWrite);
                                                        Form1->StopFlagBKS = true;
                                                        Sleep(200);
                                                        Form1->SendStringBKS("<Stopped by Timer>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("?ERR>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("!RDY>");
                                                        Form1->LiteraBKS = NULL;
                                                        ResumeThread(Form1->BKSWrite);
                                                        SuspendThread(Form1->BKSRead);
                                               }
                                               if(Form1->LiteraBKS == '0') {
                                                        Application->MessageBox("��������� �������� ������� ���" , "����������!", MB_OK);
                                                        SuspendThread(Form1->BKSRead);
                                               }


                                               ResetEvent(Form1->LitBKSReceived);
                                               //-----------------------------------------------------------------------------------------

                                        }
                                }
                        }
                }
        }
        //return 0;
}

DWORD WINAPI ReadBKIThread(LPVOID lpParam)
{
        DWORD mask, signal, tmp, num, realnum;
        DWORD beginSymbStr, endSymbStr;
        DWORD count = 0;
        DWORD countprot = 0;
        char bufferBKI[2];
        AnsiString RichStr = "";                                            //������ ��� ������ � ��������� ���� RichBKI
        COMSTAT comstat;                                                       //��������� COMSTAT �������� ���������� � ���������������� ����������
        ovrlBKIRead.hEvent=CreateEvent(NULL, true, true, NULL);
        Form1->LitBKIReceived=CreateEvent(NULL, true,true, NULL);
        while(1)
        {
               WaitCommEvent(Form1->hCommBKI, &mask, &ovrlBKIRead);
                signal=WaitForSingleObject(ovrlBKIRead.hEvent, INFINITE);
                if(signal==WAIT_OBJECT_0)
                {
                        if(GetOverlappedResult(Form1->hCommBKI, &ovrlBKIRead, &tmp, true))
                        {

                                if ((mask&EV_RXCHAR)==EV_RXCHAR)
                                {
                                        ClearCommError(Form1->hCommBKI, &tmp, &comstat);
                                        num=comstat.cbInQue;                                            //cbInQue - �������� ��������� COMSTAT - ���������� ����������,
                                        if (num)                                                        //�� ��� ������������� �������� ReadFile ����
                                        {
                                                Form1->event->WaitFor(100);          //?
                                                memset(bufferBKI, NULL, 2);
                                                ReadFile(Form1->hCommBKI, bufferBKI, num, &realnum, &ovrlBKIRead);
                                                Form1->LiteraBKI=bufferBKI[0];
                                                if(bufferBKI[0] != NULL)
                                                        RichStr += bufferBKI[0];

                                                Form1->RichBKI->Lines->Add("<--- " + RichStr); //������ ���������� ������� � ��������� ���� RichBKI
                                                RichStr = "";
                                                SetEvent(Form1->LitBKIReceived);
                                                if(count < Form1->OpCountBKI) {
                                                        if(count == 0 || (count == Form1->OpCountBKI - 1 && Form1->FirstTestFlag == false))      //���� �������� �������, ����������� ���� �������� ���������, ��� ��������� ��������
                                                                Form1->NullAll(2);
                                                        if(Form1->LiteraBKI == Form1->operbki[count].OpMessage[1] && Form1->operbki[count].OpStatus == "WAIT" ){
                                                                Form1->operbki[count].OpDone = true;
                                                                count++;
                                                        }
                                                        while(Form1->operbki[count].OpStatus == "SEND") {
                                                                Sleep(500);                                    //�������� 0.5 � ����� ���������, ������������� �� ���
                                                                Form1->SendStringBKI(Form1->operbki[count].OpMessage);
                                                                count++;
                                                                SuspendThread(Form1->BKIRead);                 //����� �������� ��������� �� ��������� �������� ������������� ����� ������ ���

                                                        }
                                                }
                                                if(count == Form1->OpCountBKI) {
                                                        if(Form1->TimerBox->State == cbUnchecked)
                                                                Form1->SigDelayBKI->Enabled = true;                    //��������� ������ ���
                                                        else {
                                                        Form1->SignalsSendBKI = true;
                                                        Form1->outBKIBuff = "";
                                                        ResumeThread(Form1->BKIWrite);
                                                        }
                                                        Form1->SigBarBKI->Position = 0;
                                                        Form1->StatusLabel->Caption = "���������: ���������� ��������";
                                                        count++;
                                                }
                                                if(countprot < Form1->PrCountBKI) {

                                                        if(Form1->LiteraBKI == Form1->protbki[countprot].PrMessage[1] && Form1->protbki[countprot].PrStatus == "WAIT" ){                //���� ���������� ������ ����� ���������, � ��� ������� ������ ��������� - "��������"
                                                                Form1->protbki[countprot].PrDone = true;               //����������� 1 �������� "���������" ������ ���������
                                                                if(countprot == 0)
                                                                        Form1->NullAll(3);
                                                                countprot++;
                                                        }
                                                        while(Form1->protbki[countprot].PrStatus == "SEND") {          //���� ������� ������ ��������� ����� ��� "��������"
                                                                WaitForSingleObject(Form1->AllSigBKI, INFINITE);
                                                                Sleep(10);
                                                                if(Form1->protbki[countprot].PrMessage == "!RDY>")
                                                                        Sleep(100);
                                                                Form1->SendStringBKI(Form1->protbki[countprot].PrMessage);        //���������� ������ ���������, ����������� �� .emu ����� � ���������� � �����. ���������
                                                                countprot++;
                                                                SuspendThread(Form1->BKIRead);                               //����� �������� ��������� �� ��������� �������� ������������� ����� ������ ���
                                                        }
                                                }
                                                if(countprot == Form1->PrCountBKI) {
                                                        countprot = 0;
                                                        count = Form1->OpCountBKI - 1;
                                                      // count = 0;
                                                        Form1->NullAll(1);
                                                        Form1->StopFlagBKI = false;
                                                        Form1->PitButtFlagBKI = false;
                                                        Form1->FirstTestFlag = false;
                                                        TerminateThread(Form1->BKIWrite, 0);
                                                        Form1->BKIWrite=CreateThread(NULL, 0, WriteBKIThread, NULL, CREATE_SUSPENDED, NULL);
                                                }
                                                //------------------��������� ����������� ������, ����������� �� ����� ������ ��������� �� ����� ���-------------------------------------
                                               if(Form1->LiteraBKI == '~') {
                                                        WaitForSingleObject(Form1->AllSigBKI, INFINITE);
                                                       // SuspendThread(Form1->BKIWrite);
                                                        Form1->StopFlagBKI = true;
                                                        Sleep(200);
                                                        Form1->SendStringBKI("<3 0 50>");
                                                        SuspendThread(Form1->BKIRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKI("?ERR>");
                                                        SuspendThread(Form1->BKIRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKI("!RDY>");
                                                        Form1->LiteraBKI = NULL;
                                                        ResumeThread(Form1->BKIWrite);
                                                        SuspendThread(Form1->BKIRead);
                                               }
                                               if(Form1->LiteraBKI == '!') {
                                                        WaitForSingleObject(Form1->MessageSendBKI, INFINITE);
                                                        SuspendThread(Form1->BKIWrite);
                                                        Form1->StopFlagBKI = true;
                                                        Sleep(200);
                                                        Form1->SendStringBKI("<Stopped by Timer>");
                                                         SuspendThread(Form1->BKIRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKI("?ERR>");
                                                        SuspendThread(Form1->BKIRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKI("!RDY>");
                                                        Form1->LiteraBKI = NULL;
                                                        ResumeThread(Form1->BKIWrite);
                                                        SuspendThread(Form1->BKIRead);
                                               }
                                               if(Form1->LiteraBKI == '0') {
                                                        Application->MessageBox("��������� �������� ������� ���" , "����������!", MB_OK);
                                                        SuspendThread(Form1->BKIRead);
                                               }

                                               ResetEvent(Form1->LitBKIReceived);
                                               //-----------------------------------------------------------------------------------------
                                        }
                                }
                        }
                }
        }
        //return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------

void TForm1::SendStringBKS(AnsiString sendbuf)                              //������� ������ � COM-���� �� ������ ���
{
        Form1->outBKSBuff=Form1->outBKSBuff + sendbuf;
        ResumeThread(Form1->BKSWrite);                                       //������������ ����� ������ ���
}

void TForm1::SendStringBKI(AnsiString sendbuf)                              //������� ������ � COM-���� �� ������ ���
{
        Form1->outBKIBuff=Form1->outBKIBuff + sendbuf;
        ResumeThread(Form1->BKIWrite);                                       //������������ ����� ������ ���
}

void __fastcall TForm1::PitButtClick(TObject *Sender)
{
        if(PitButt->Caption == "������ �������"){
                Form1->SendStringBKI("!SYS>");
                Form1->SendStringBKS("!SYS>");

                Form1->SigBarBKS->Position = 0;
                Form1->SigBarBKI->Position = 0;
                Form1->DefaultSets->Enabled = false;
                PitButt->Caption = "�������� ������";
                Form1->StatusLabel->Caption = "���������: ������� ����������";
                NullAll(2);
        }
        else if(PitButt->Caption == "�������� ������") {

                if(Form1->SigCountBKS > 0) {
                        Form1->PitButtFlagBKS = true;

                }
                else if(Form1->SigCountBKI > 0) {
                        Form1->PitButtFlagBKI = true;

                }
                Form1->StopFlagBKS = true;
                Form1->SigDelayBKS->Enabled = false;
                Form1->SignalsSendBKS = true;
                ResumeThread(Form1->BKSWrite);
                Form1->StopFlagBKI = true;
                Form1->SigDelayBKI->Enabled = false;
                Form1->SignalsSendBKI = true;
                ResumeThread(Form1->BKIWrite);
                Form1->NullAll(4);
                PitButt->Enabled = false;
                PitButt->Caption = "������ �������";
                Form1->StatusLabel->Caption = "���������: ������ ��������";
        }



}

void TForm1::GetSystemInfo(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if(temp[temp.Length()] == '\n')                           //���� ��������� ������ � ������ '\n', �������
                        temp.Delete(temp.Length(), 1);
                if(temp.Pos("�������:"))
                        sysinf->SystemName=temp;
                if(temp.Pos("�����:"))
                        sysinf->ModName=temp;
        }
        Form1->SystemNameLabel->Caption = sysinf->SystemName;
        Form1->ModNameLabel->Caption = sysinf->ModName;
        rewind(filename);                                              //���������� ��������� ����� � ������

}

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        //-------���������� ���������� � ���������� ��������� �������� � ��������

        WaitForSingleObject(Form1->MessageSendBKI, INFINITE);
        CloseFlagBKI = true;
        WaitForSingleObject(Form1->MessageSendBKS, INFINITE);
        CloseFlagBKS = true;
        WaitForSingleObject(Form1->AllSigBKI, INFINITE);
        WaitForSingleObject(Form1->AllSigBKS, INFINITE);



        CComUse.ComClear(Form1->hCommBKS, PURGE_TXCLEAR|PURGE_RXCLEAR);
        CComUse.ComClear(Form1->hCommBKI, PURGE_TXCLEAR|PURGE_RXCLEAR);
        if (BKSRead)
        {
                TerminateThread(BKSRead, 0);
                CloseHandle(ovrlBKSRead.hEvent);
                CloseHandle(BKSRead);
        }
        if (BKSWrite)
        {
                TerminateThread(BKSWrite, 0);
                CloseHandle(ovrlBKSWrite.hEvent);
                CloseHandle(BKSWrite);
        }
        if (hCommBKS) CComUse.ComClose(hCommBKS);
        if (BKIRead)
        {
                TerminateThread(BKIRead, 0);
                CloseHandle(ovrlBKIRead.hEvent);
                CloseHandle(BKIRead);
        }
        if (BKIWrite)
        {
                TerminateThread(BKIWrite, 0);
                CloseHandle(ovrlBKIWrite.hEvent);
                CloseHandle(BKIWrite);
        }
        if (hCommBKI) CComUse.ComClose(hCommBKI);
        event->~TEvent();
       // SigDelayBKS->~TTimer();
       // SigDelayBKI->~TTimer();
        Form1->SigDelayBKI->Enabled = false;
        Form1->SigDelayBKS->Enabled = false;
}


//-------------------������� ������ ����� ��������� .emu �����-------------------------------------------------------------------------------------

void TForm1::StartModBKI(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if (temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[������ ������]"))
                                        while(!temp.Pos("[/������ ������]")) {
                                                fgets(buffer, SYMBOLSNUM, filename);
                                                temp=buffer;
                                                StartStrProc(temp);
                                        }
                        }
               }
        }
        rewind(filename);
}

void TForm1::StartModBKS(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if (temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[������ ������]"))
                                        while(!temp.Pos("[/������ ������]")) {
                                                fgets(buffer, SYMBOLSNUM, filename);
                                                temp=buffer;
                                                StartStrProc(temp);
                                        }
                        }
               }
        }
        rewind(filename);
}

void TForm1::SignalsModBKS(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if(temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[�������]")) {
                                        while(!temp.Pos("[/�������]")) {
                                                fgets(buffer,SYMBOLSNUM, filename);
                                                temp=buffer;
                                                if(!temp.Pos("[/�������]"))
                                                        SignalsLoadBKS(temp);
                                        }
                                }
                        }
                }
        }
        rewind(filename);
}

void TForm1::SignalsModBKI(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if(temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[�������]")) {
                                        while(!temp.Pos("[/�������]")) {
                                                fgets(buffer,SYMBOLSNUM, filename);
                                                temp=buffer;
                                                if(!temp.Pos("[/�������]"))
                                                        SignalsLoadBKI(temp);
                                        }
                                }
                        }
                }
        }
        rewind(filename);
}

void TForm1::ProtocolModBKI(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if (temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[��������]"))
                                        while(!temp.Pos("[/��������]")) {
                                                fgets(buffer, SYMBOLSNUM, filename);
                                                temp=buffer;
                                                ProtStrProc(temp);
                                        }
                        }
               }
        }
        rewind(filename);
}
void TForm1::ProtocolModBKS(FILE* filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if (temp.Pos("[���]")) {
                        while(!temp.Pos("[/���]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[��������]"))
                                        while(!temp.Pos("[/��������]")) {
                                                fgets(buffer, SYMBOLSNUM, filename);
                                                temp=buffer;
                                                ProtStrProc(temp);
                                        }
                        }
               }
        }
        rewind(filename);
}








//-----------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------������� ��������� ����������� ����� .emu �����-------------------------------------------------------------------

void TForm1::StartStrProc(AnsiString str)             //��������� ����� � ������������ ����������
{

        for(int i=1; i<=str.Length(); i++)
                if(str[i] == '\r')
                        str[i] = '\0';                        //�������� '\r' � ��� ��� �����

        if(str.Pos("BKS||")) {                                //�������� ����������� ������ � .emu ����� � ��������� �� ������ ��� �������
                str.Delete(1, 5);                              //������� �������� ����������� �� ������
                operbks[OpCountBKS].OpMessage = str[1];                         //��������� ��������� ��������
                operbks[OpCountBKS].OpStatus = "WAIT";               //��������� ������ ��������
                OpCountBKS++;
        }
        if(str.Pos("BKI||")) {
                str.Delete(1, 5);
                operbki[OpCountBKI].OpMessage = str[1];
                operbki[OpCountBKI].OpStatus = "WAIT";
                OpCountBKI++;
        }

        if(str.Pos("BKS<<")) {                                  //�������� ����������� ������ � .emu ����� � ������������ �� ��������� �������� ��������
                str.Delete(1, 5);                               //������� �.�.
                operbks[OpCountBKS].OpMessage = str;
                operbks[OpCountBKS].OpStatus = "SEND";
                OpCountBKS++;
        }
        if(str.Pos("BKI<<")) {
                str.Delete(1, 5);                               //������� �.�.
                operbki[OpCountBKI].OpMessage = str;
                operbki[OpCountBKI].OpStatus = "SEND";
                OpCountBKI++;
        }

}

void TForm1::ProtStrProc(AnsiString str)             //��������� ����� ���������, ���������� �� .emu �����
{                                                    //�������� ��� �������� �������� ���������� ��!!!

        for(int i=1; i<=str.Length(); i++)
                if(str[i] == '\r')
                        str[i] = '\0';                        //�������� '\r' � ��� ��� �����
        if(str.Pos("BKS||")) {                                //�������� ����������� ������ � .emu ����� � ��������� �� ������ ��� �������
                str.Delete(1, 5);                              //������� �������� ����������� �� ������
                protbks[PrCountBKS].PrMessage = str[1];                         //��������� �������� ����� ������ �� ����� ���������
                protbks[PrCountBKS].PrStatus = "WAIT";               //��������� ������ ������ �� ����� ���������
                PrCountBKS++;
        }
        if(str.Pos("BKI||")) {
                str.Delete(1, 5);
                protbki[PrCountBKI].PrMessage = str[1];
                protbki[PrCountBKI].PrStatus = "WAIT";
                PrCountBKI++;
        }

        if(str.Pos("BKS<<")) {                                  //�������� ����������� ������ � .emu ����� � ������������ �� ��������� �������� ���������� ���������
                str.Delete(1, 5);                               //������� �.�.
                protbks[PrCountBKS].PrMessage = str;
                protbks[PrCountBKS].PrStatus = "SEND";
                PrCountBKS++;
        }

        if(str.Pos("BKI<<")) {
                str.Delete(1, 5);                               //������� �.�.
             //   Form1->BuffBKIMemo->Text = Form1->BuffBKIMemo->Text + "\n" + str;
                protbki[PrCountBKI].PrMessage = str;
                protbki[PrCountBKI].PrStatus = "SEND";
                PrCountBKI++;
        }

}

void TForm1::SignalsLoadBKS(AnsiString str)                //��������� ����� � ����������� ��� �������� �� ��������� �������� �� ������ ���
{
        int i;
        AnsiString time = "";
        AnsiString message = "";
        for(i=1; i<=str.Length(); i++) {
                if(str[i] == ':') {
                        while(str[++i] != ':')
//                                time[k++] = str[i];             �������!� ��������� ������ AnsiString ����������� �������������������� �������� ������!
                                time = time + str[i];          // ������������ ���������� ������ ���� AnsiString. ������ �������.
                        while(str[++i] != '>')
                                message = message + str[i];
                }
        }
        message = message + '>';
        if(message == "?START>")
                StartMessBKSFlag = 1;
        try
        {
        sigbks[SigCountBKS].SigTime = StrToInt(time);
        }
        catch(...) {
                Application->MessageBox(IntToStr(SigCountBKS).c_str(), "���� ����������!", MB_OK);
        }
        sigbks[SigCountBKS].SigMessage = message;
        SigCountBKS++;

}

void TForm1::SignalsLoadBKI(AnsiString str)               //��������� ����� � ����������� ��� �������� �� ��������� �������� �� ������ ���
{
        int i;
        AnsiString time = "";
        AnsiString message = "";
        for(i=1; i<=str.Length(); i++) {
                if(str[i] == ':') {
                        while(str[++i] != ':')
                                time = time + str[i];
                        while(str[++i] != '>')
                                message = message + str[i];
                }

        }
         message = message + '>';
         if(message == "?START>")
                StartMessBKIFlag = 1;
        sigbki[SigCountBKI].SigTime = StrToInt(time);
        sigbki[SigCountBKI].SigMessage = message;
        SigCountBKI++;
}

//--------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------������� �������� ��������� �� ��������� �������� �� ������� ��� � ���-------------------------------

void __fastcall TForm1::SigDelayBKSEvents(TObject *Sender)
{
        static bool startflag = false;
        unsigned int perc = 0;
        if((sigbks[signumbks].SigTime < tickbks) && (signumbks < SigCountBKS)) {
                Form1->SignalsSendBKS = true;
           //     Form1->NextButtFlagBKS = true;
                Form1->outBKSBuff = Form1->outBKSBuff + Form1->sigbks[signumbks].SigMessage;
                ResumeThread(Form1->BKSWrite);
                if(signumbks > 0 && signumbks <= Form1->SigCountBKS-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                        perc = signumbks*100/(Form1->SigCountBKS-2) - (signumbks-1)*100/(Form1->SigCountBKS-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                        Form1->SigBarBKS->StepIt();
                perc = 0;
                if(sigbks[signumbks].SigMessage == "?START>")
                                        startflag = true;
                if(startflag == true){
                        if(signumbks <= Form1->SigCountBKS - 3)
                                Form1->SentBKSLabel->Caption = "���: " + IntToStr(signumbks-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                }
                else {
                        if((signumbks <= Form1->SigCountBKS - 2) && (Form1->StartMessBKSFlag == 0))
                                Form1->SentBKSLabel->Caption = "���: " + IntToStr(signumbks) + "/" + IntToStr(Form1->SigCountBKS-2);
                        else if(Form1->StartMessBKSFlag == 1)
                                Form1->SentBKSLabel->Caption = "���: " + IntToStr(signumbks-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);              //����� ���������� ������������ �������� � ������ ����������
                }
                if(Form1->PauseButt->Caption == "����������") {                      //���� ������ ��������� �������������� ��������� ������
                        Form1->SigDelayBKS->Enabled = false;
                        if(nextcount == 1) {
                                Form1->NextButt->Enabled = true;
                                Form1->StatusLabel->Caption = "���������: ������ ��������������";
                        }
                        else
                        nextcount++;
                }
                signumbks++;
        }
        else if(signumbks == SigCountBKS) {
                signumbks = 0;
                tickbks = 0;
                Form1->SigDelayBKS->Enabled = false;
        }

        Form1->TimerBKSLabel->Caption = "���: " + IntToStr(tickbks);
        tickbks++;
}

void __fastcall TForm1::SigDelayBKIEvents(TObject *Sender)
{
        unsigned int perc = 0;
        static bool startflag = false;
        if((sigbki[signumbki].SigTime < tickbki) && (signumbki < SigCountBKI)) {
                Form1->SignalsSendBKI = true;
          //      Form1->NextButtFlagBKI = true;
                Form1->outBKIBuff = Form1->outBKIBuff + Form1->sigbki[signumbki].SigMessage;
                if(signumbki > 0 && signumbki <= Form1->SigCountBKI-2)                                 //���� ������� ������ �� ����� ������ � ������� ������ ������ ���������� �������� �� ����������� "?OK>", "!RDY>"
                        perc = signumbki*100/(Form1->SigCountBKI-2) - (signumbki-1)*100/(Form1->SigCountBKI-2);           //������������ ��������� ��������� ���������� ���� ������������ �������� ����� ����������� ���������� �������
                for(unsigned int i=0; i<perc; i++)           //���� ����� ���� �� ������� �� ������ ���������� ��������, ��������� ��� � ������-����
                        Form1->SigBarBKI->StepIt();
                perc = 0;
                if(sigbki[signumbki].SigMessage == "?START>")
                        startflag = true;

                if(signumbki <=Form1->SigCountBKI - 2) {
                        if(Form1->StartMessBKIFlag == 0)
                                Form1->SentBKILabel->Caption = "���: " + IntToStr(signumbki) + "/" + IntToStr(Form1->SigCountBKI-2);
                        else {
                                if(startflag == false)
                                        Form1->SentBKILabel->Caption = "���: " + IntToStr(signumbki) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                else
                                        Form1->SentBKILabel->Caption = "���: " + IntToStr(signumbki-1) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                        }
                }


                ResumeThread(Form1->BKIWrite);
                if(Form1->PauseButt->Caption == "����������") {
                        Form1->SigDelayBKI->Enabled = false;
                        if(nextcount == 1) {
                                Form1->NextButt->Enabled = true;
                                Form1->StatusLabel->Caption = "���������: ������ ��������������";
                        }
                        else
                                nextcount++;

                }
                signumbki++;
        }
        else if(signumbki == SigCountBKI) {
                signumbki = 0;
                tickbki = 0;
                Form1->SigDelayBKI->Enabled = false;
        }

        Form1->TimerBKILabel->Caption = "���: " + IntToStr(tickbki);
        tickbki++;
}










