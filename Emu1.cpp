//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Emu1.h"
#include "com.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
OVERLAPPED ovrlBKSRead;                                    //структура для управления асинхронными операциями ввода/вывода
DWORD WINAPI ReadBKSThread(LPVOID);
OVERLAPPED ovrlBKSWrite;
DWORD WINAPI WriteBKSThread(LPVOID);

OVERLAPPED ovrlBKIRead;                                     //структура OVERLAPPED используется при асинхронном режиме чтения и записи файлов
DWORD WINAPI ReadBKIThread(LPVOID);
OVERLAPPED ovrlBKIWrite;
DWORD WINAPI WriteBKIThread(LPVOID);
DWORD tickbks, tickbki;                                 //счетчики таймеров БКС и БКИ
DWORD signumbks, signumbki;                             //номера отправленных таймерами сигналов
DWORD nextcount;                                        //счетчик принимает значение 1, если по одному из каналов получен следующий по порядку сигнал
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
        Form1->RichBKI->SelAttributes->Style = Form1->RichBKI->SelAttributes->Style << fsBold;                             //Начальное значение тесктовых полей RichBKI и RichBKS
        Form1->RichBKS->SelAttributes->Style = Form1->RichBKS->SelAttributes->Style << fsBold;
        Form1->RichBKI->Lines->Add("Канал БКИ");
        Form1->RichBKS->Lines->Add("Канал БКС");
        Form1->RichBKI->SelAttributes->Style = Form1->RichBKI->SelAttributes->Style >> fsBold;
        Form1->RichBKS->SelAttributes->Style = Form1->RichBKS->SelAttributes->Style >> fsBold;
        Form1->StatusLabel->Caption = "Состояние: загрузите файл режима";
}



//---------------------------------------------------------------------------

void TForm1::OpenDeviceFile()                                                    //Функция открытия файла эмуляции режима
{
                DeviceFileName = "Выберите файл режима";
                if(DeviceFile->Execute())
                        DeviceFileName = DeviceFile->FileName;
                DeviceFilePath->Text = DeviceFileName;
                FileNull();
                DelSet("FileNameSet");

                if(Form1->COMPortBKI->Text != "COM-порт БКИ")
                        InitBKI(Form1->COMPortBKI->Text);
                if(Form1->COMPortBKS->Text != "COM-порт БКС")
                        InitBKS(Form1->COMPortBKS->Text);

}

void __fastcall TForm1::DeviceFileButtClick(TObject *Sender)
{
        OpenDeviceFile();
        if(DeviceFileName != "Выберите файл режима")                           //если выбран файл
                CheckingDeviceFile(DeviceFileName);



}

void __fastcall TForm1::PauseButtClick(TObject *Sender)
{
        if(Form1->PauseButt->Caption == "Приостановить") {
                if(Form1->TimerBox->State == cbChecked) {
                        Form1->StopFlagBKS = true;
                        Form1->StopFlagBKI = true;
                }
                else {
                        Form1->SigDelayBKI->Enabled = false;
                        Form1->SigDelayBKS->Enabled = false;
                }
                Form1->NextButt->Enabled = true;
                Form1->PauseButt->Caption = "Продолжить";
                Form1->StatusLabel->Caption = "Состояние: работа приостановлена";
        }
        else if(Form1->PauseButt->Caption == "Продолжить") {
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
                Form1->PauseButt->Caption = "Приостановить";

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
                Form1->StatusLabel->Caption = "Состояние: ожидание сигнала";
        }
}





 void TForm1::CheckingDeviceFile(AnsiString filename)                            //Проверка файла эмуляции, создание потока
{
        AnsiString temp = "";
        if((fileDevice = fopen(filename.c_str(), "a+b")) == NULL) {
                temp = "Не удалось открыть файл " + filename;
                Application->MessageBox(temp.c_str(), "Файл недоступен!", MB_OK);
                Form1->FileOpenImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
        }
        else    {
                FileOpenImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                GetSystemInfo(fileDevice);                                      //Передаем указатель на файл в функцию получения информации о системе
                StartModBKI(fileDevice);                                        //Читаем из файла операции запуска режима БКИ
                StartModBKS(fileDevice);                                        //Читаем из файла операции запуска режима БКС
                StartMessBKIFlag = 0;                                            //обнуляем флаг наличия сообщения "?START>" в сигнальном блоке БКИ
                StartMessBKSFlag = 0;                                            //обнуляем флаг наличия сообщения "?START>" в сигнальном блоке БКС
                SignalsModBKI(fileDevice);                                      //Читаем из файла сообщения на БКИ
                SignalsModBKS(fileDevice);                                      //Читаем из файла сообщения на БКС
                ProtocolModBKS(fileDevice);
                ProtocolModBKI(fileDevice);
                SaveSet("FileNameSet");
               OpenFileFlag = true;
               Form1->SentBKSLabel->Caption = "БКС: 0/" + IntToStr(SigCountBKS-2-StartMessBKSFlag);               //Количество сигналов, исключая служебные слова "!RDY>", "?OK>"
               Form1->SentBKILabel->Caption = "БКИ: 0/" + IntToStr(SigCountBKI-2-StartMessBKIFlag);
               Form1->StatusLabel->Caption = "Состояние: настройка системы";
        }
}

//------------------------------функиця получения списка COM-портов из регистра---------------------------------
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
                //if(Registry->ReadString(SerialCommValues->Strings[i]) != Form1->COMPortBKS->Text)     Скрыть порт занятый соседним каналом!
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
                //if(Registry->ReadString(SerialCommValues->Strings[i]) != Form1->COMPortBKI->Text)     Скрыть порт занятый соседним каналом!
                Form1->COMPortBKS->Items->Add(Registry->ReadString(SerialCommValues->Strings[i]));
                }
        }
    }
    __finally                                   //выполняется независимо от переданной исключительной ситуации
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
        temp = Form1->COMPortBKI->Text + " уже выбран.";
        if(Form1->COMPortBKI->ItemIndex > 0)                                    //Если загружен список COM-портов (в выпадающем списке >0 эл-ов)
                if(Form1->COMPortBKI->Text != Form1->COMPortBKS->Text)          //Если COM-порт не совпадает с выбранным ранее на соседнем канале
                        InitBKI(Form1->COMPortBKI->Text);

                else    {
                       Application->MessageBox(temp.c_str(), "Ошибка COM-порта!", MB_OK);
                       Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                       DelSet("ComBKISet");
               }
}
void __fastcall TForm1::COMPortBKSSelect(TObject *Sender)
{
        AnsiString temp = "";
        temp = Form1->COMPortBKS->Text + " уже выбран.";
        if(Form1->COMPortBKS->ItemIndex > 0)
                if(Form1->COMPortBKS->Text != Form1->COMPortBKI->Text)           //Если COM-порт не совпадает с выбранным ранее на соседнем канале
                        InitBKS(Form1->COMPortBKS->Text);
                else    {
                       Application->MessageBox(temp.c_str(), "Ошибка COM-порта!", MB_OK);
                       Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                       DelSet("ComBKSSet");
                }
}



void TForm1::InitBKS(AnsiString portname)

{
        AnsiString temp = "";
        temp = "Невозможно открыть " + portname;
        if (hCommBKS) CComUse.ComClose(hCommBKS);
        CComUse.ComClear(hCommBKS, PURGE_TXCLEAR|PURGE_RXCLEAR);
        Form1->event->WaitFor(200);
        hCommBKS=CComUse.ComInit(portname.c_str(), FILE_FLAG_OVERLAPPED);
         if(!hCommBKS) {
                Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                Application->MessageBox(temp.c_str() , "Ошибка COM-порта!", MB_OK);
                DelSet("ComBKSSet");
         }
        else {
                Form1->InitBKSImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                if(BKSRead)
                        TerminateThread(BKSRead, 0);
                BKSRead=CreateThread(NULL, 0, ReadBKSThread, NULL, 0, NULL);
                BKSWrite=CreateThread(NULL, 0, WriteBKSThread, NULL, CREATE_SUSPENDED, NULL);                     //Дескриптор потока вывода по каналу БКС
                //-------------Сохранение параметра и проверка всех настроек-----------------------
                SaveSet("ComBKSSet");
        }
}
void TForm1::InitBKI(AnsiString portname)
{
        AnsiString temp = "";
        temp = "Невозможно открыть " + portname;
        if (hCommBKI) CComUse.ComClose(hCommBKI);
        CComUse.ComClear(hCommBKI, PURGE_TXCLEAR|PURGE_RXCLEAR);
        Form1->event->WaitFor(200);                                                     //После закрытия дескриптора COM-порта перед повторным открытием, т.к. необходимо время для взаимодействия с аппаратной частью
        hCommBKI=CComUse.ComInit(portname.c_str(), FILE_FLAG_OVERLAPPED);
        if(!hCommBKI) {
                Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                Application->MessageBox(temp.c_str() , "Ошибка COM-порта!", MB_OK);
                DelSet("ComBKISet");
        }
        else {
                Form1->InitBKIImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                if(BKIRead)
                        TerminateThread(BKIRead, 0);
                BKIRead=CreateThread(NULL, 0, ReadBKIThread, NULL, 0, NULL);
                BKIWrite=CreateThread(NULL, 0, WriteBKIThread, NULL, CREATE_SUSPENDED, NULL);                         //Дескриптор потока вывода по каналу БКИ
                SaveSet("ComBKISet");
        }


        /*
        if (!hCommBKI)
        {
                LogMemoBKI->Lines->Add("Ошибка инициализации "+temp+".");
                Skpa_5->DeviceComboBox->Enabled=false;
                Skpa_5->Vst05ComboBox->Enabled=false;
                Skpa_5->ProtokolBKSBitBtn->Enabled=false;
                Skpa_5->ProtokolBKIBitBtn->Enabled=false;
                Skpa_5->StageLabel->Caption=Skpa_5->ChangeCap("Система: "+cmdsets->SystemName, "Режим: Начало.", "ОШИБКА инициализации порта: "+temp, "");
        }
        else
        {
                BKIRead=CreateThread(NULL, 0, ReadBKIThread, NULL, 0, NULL);
                BKIWrite=CreateThread(NULL, 0, WriteBKIThread, NULL, CREATE_SUSPENDED, NULL);
        }
        */
}




//----------------------------------------------Работа с настройками-------------------------------------------------------------
void TForm1::SaveSet(AnsiString name)                                           //сохранение новых параметров и установка существующих
{
        DWORD i = 0;
        while((prsets[i].SettingName != name) && (i != SetCount))               //если параметр с таким наименованием существует, изменить его статус на "DONE"
                       i++;
        if(i < SetCount)
                prsets[i].SettingStatus = "DONE";
        else
                if(SetCount <= SETSCOUNT - 1) {                                        //если параметра не существует, и если номер параметра меньше макс. кол-ва, задать новый параметр
                        prsets[SetCount].SetSet(name);
                        if(SetCount != SETSCOUNT - 1)                                 //инкрементируем номер параметра, если не достигнуто максимальное значение
                        SetCount++;
                }
        if(SetCount == SETSCOUNT - 1) {
                for(i=0; i<=SetCount; i++)                                              //если все параметры(настройки) определены активировать кнопку "Подать питание"
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

void TForm1::DelSet(AnsiString name)                                            //снятие существующих параметров
{
        DWORD i;
        for(i=0; i<=SetCount; i++)
                if(prsets[i].SettingName == name) {
                        prsets[i].SettingStatus = "UNDONE";
                        Form1->PitButt->Enabled = false;
                        Form1->ApplySets->Enabled = false;
                        Form1->StatusLabel->Caption = "Состояние: настройка системы";
                }
}


void __fastcall TForm1::ApplySetsClick(TObject *Sender)                                             //Нажатие кнопки "Применить" в блоке настроек
{

        Form1->PitButt->Enabled = true;                                         //Активировать кнопку "Подать питание"
        Form1->COMPortBKI->Enabled = false;
        Form1->COMPortBKS->Enabled = false;
        Form1->DefaultSets->Enabled = true;
        Form1->DeviceFilePath->Enabled = false;
        Form1->DeviceFileButt->Enabled = false;
        MakeDeviceFile->Enabled = false;                                        //деактивируем кнопку создания файла режима
        if(OpenFileFlag == false) {                                             //если после завершения проверки не выбран новый файлрежима
                FileNull();
                CheckingDeviceFile(DeviceFileName);                             //проверка и обработка данных из текущего файла
                if(Form1->COMPortBKI->Text != "COM-порт БКИ")
                        InitBKI(Form1->COMPortBKI->Text);
                if(Form1->COMPortBKS->Text != "COM-порт БКС")
                        InitBKS(Form1->COMPortBKS->Text);
        }
        Form1->StatusLabel->Caption = "Состояние: настройки успешно применены";
        Form1->ApplySets->Enabled = false;

}

void __fastcall TForm1::DefaultSetsClick(TObject *Sender)
{
        DelSet("FileNameSet");
        DelSet("ComBKISet");
        DelSet("ComBKSSet");
        DeviceFilePath->Text = "Выберите файл режима";
        DeviceFilePath->Enabled = true;
        DeviceFileButt->Enabled = true;
        MakeDeviceFile->Enabled =true;
        Form1->TimerBox->State = cbUnchecked;
        Form1->RoundBox->State = cbUnchecked;
        Form1->TimerBox->Enabled = true;
        Form1->RoundBox->Enabled = true;
        Form1->COMPortBKI->Text = "COM-порт БКИ";
        Form1->COMPortBKS->Text = "COM-порт БКС";
        Form1->COMPortBKI->Enabled = true;
        Form1->COMPortBKS->Enabled = true;
        Form1->PitButt->Enabled = false;
        Form1->DefaultSets->Enabled = false;
}

void __fastcall TForm1::MakeDeviceClick(TObject *Sender)
{
        if(!MakeFileForm)
                MakeFileForm = new TMakeFileForm(this);                     //выделяем память(создаем) форму, Auto-create отключен
        MakeFileForm->Show();                                       //отображаем форму
}






//--------------Обнуление параметров  при различных событиях-------------------------------------------------------------------------
void TForm1::NullAll(DWORD num)
{
        static int rdycount = 0;                                        //счетчик завершенных проверок(по каналам), если =2  проверка завершена и по БКИ, и по БКС
        if(num == 1) {                                                    //обнуление после вывода протокола
                if(rdycount == 1) {                                                       //если проверка по соседнему каналу также завершена
                        if(Form1->RoundBox->State == cbUnchecked){                        //если не активирован режим циклической проверки
                               Form1->PitButt->Enabled = true;                                   //активируем кнопку подачи питания
                                Form1->PitButt->Caption = "Подать питание";
                                Form1->NextButt->Enabled = false;
                                Form1->PauseButt->Enabled = false;
                                Form1->PauseButt->Caption = "Приостановить";
                                 Form1->COMPortBKI->Enabled = true;
                                Form1->COMPortBKS->Enabled = true;
                                Form1->TimerBox->Enabled = true;
                                Form1->RoundBox->Enabled = true;
                                Form1->DeviceFileButt->Enabled = true;
                                Form1->MakeDeviceFile->Enabled = true;
                           //     DeviceFilePath->Text = "Выберите файл режима";
                                Form1->ApplySets->Enabled = true;
                                OpenFileFlag = false;
                           //     tickbks = tickbki = 0;                           //обнуляем таймеры
                           //     signumbks = signumbki = 0;                         //обнуляем номера сигналов
                        }

                        Form1->PitButt->Caption = "Подать питание";
                        Form1->PitButt->Enabled = false;
                        Form1->NextButt->Enabled = false;
                        Form1->PauseButt->Enabled = false;
                        Form1->PauseButt->Caption = "Приостановить";
                        Form1->SigDelayBKI->Enabled = false;
                        Form1->SigDelayBKS->Enabled = false;
                        tickbks = tickbki = 0;
                        signumbks = signumbki = 0;                              //обнуляем таймеры
                        rdycount = 0;                                           //обнуляем номера сигналов

                       if(Form1->TimerBox->State == cbChecked) {
                                Form1->SignalsSendBKI = false;
                                Form1->SignalsSendBKS = false;
                        }
                        Form1->StatusLabel->Caption = "Состояние: проверка выполнена";

                }

                else
                        rdycount++;
        }
        if(num == 2) {                                                          //обнуление после старта режима
                Form1->PitButt->Enabled = true;
                Form1->PitButt->Caption = "Прервать работу";
                Form1->PauseButt->Enabled = true;
                Form1->PauseButt->Caption = "Приостановить";
                Form1->DeviceFileButt->Enabled = false;
                Form1->ApplySets->Enabled = false;
                Form1->TimerBox->Enabled = false;
                Form1->RoundBox->Enabled = false;
                Form1->COMPortBKI->Enabled = false;
                Form1->COMPortBKS->Enabled = false;
                Form1->DeviceFilePath->Enabled = false;
                Form1->DefaultSets->Enabled = false;

        }
          if(num == 3) {                                                        //обнуление перед выводом протокола
                        Form1->PitButt->Enabled = false;
                        Form1->PitButt->Caption = "Подать питание";
                        Form1->NextButt->Enabled = false;
                        Form1->PauseButt->Enabled = false;
                        Form1->PauseButt->Caption = "Приостановить";
                        Form1->DeviceFileButt->Enabled = false;
                        Form1->StatusLabel->Caption = "Состояние: вывод протокола";
                      //  Form1->SigDelayBKI->Enabled = false;
                      //  Form1->SigDelayBKS->Enabled = false;

        }
        if(num == 4) {                                       //полное обнуление помсле нажатия "Прервать работу"
                    Form1->PitButt->Enabled = false;
                    Form1->PitButt->Caption = "Подать питание";
                    Form1->NextButt->Enabled = false;
                    Form1->PauseButt->Enabled = false;
                    Form1->PauseButt->Caption = "Приостановить";
                    if(Form1->RoundBox->State == cbChecked)
                        Form1->RoundBox->State = cbUnchecked;
                     Form1->TimerBKILabel->Caption = "БКИ: 0";
                     Form1->TimerBKSLabel->Caption = "БКС: 0";
                    /*
                    Form1->DeviceFileButt->Enabled = true;
                    Form1->COMPortBKI->Enabled = true;
                    Form1->COMPortBKS->Enabled = true;
                    Form1->TimerBox->Enabled = true;
                    Form1->RoundBox->Enabled = true;
                    tickbks = tickbki = 0;                           //обнуляем таймеры
                    signumbks = signumbki = 0;                         //обнуляем номера сигналов
                    */
        }

}

//----------------------------Обнуление счетчиков и удаление объектов при выборе нового файла эмуляции-------------------------------------

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


//-------------------В этом блоке организована асинхронная многопоточная передача данных по COM-портам----------------------------------

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
        if(Form1->TimerBox->State == false)                            //Если параметр "Сигналы без задержек" не отмечен галочкой, присвоить признаку остановки проверки значение 1
                        Form1->StopFlagBKS = true;
        while(1)
        {
                ResetEvent(Form1->MessageSendBKS);                             //обнуляем событие "Сообщение отправлено"
                ResetEvent(Form1->AllSigBKS);                                  //обнуляем событие "Все сообщения отправлены"
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
                                Form1->RichBKS->Lines->Add(sndstr);             //запись отправленных данных в текстовое поле RichBKS
                                CComUse.ComClear(Form1->hCommBKS, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKS, sndstr, strlen(sndstr), &temp, &ovrlBKSWrite);
                                WaitForSingleObject(ovrlBKSWrite.hEvent, INFINITE);

                         }

                SetEvent(Form1->MessageSendBKS);                                //устанавливаем событие "Сообщение отправлено"
                if(Form1->TimerBox->State == true)
                        Sleep(1);                      //минимальная задержка между транзакциями записи по каналу БКС
                //-----------------------------отправка сообщений (инф. о сигналах) по каналу БКС без таймера--------------------------
                 if(Form1->SignalsSendBKS == true && sigcount <= Form1->SigCountBKS) {                            //Если есть признак отправки сообщений о сигналах, и отпралены не все сообщения
                        ResetEvent(Form1->MessageSendBKS);                                                       //Обнуляем событие "Сообщениеотправлено"
                        if(Form1->StopFlagBKS == false && Form1->CloseFlagBKS == false) {                                                        //Если признак принудительного прерывания отсутствует
                                Form1->outBKSBuff = Form1->sigbks[sigcount].SigMessage;                          //Присвоить выходному буферу значение очередного сообщение из .emu файла
                                if(Form1->sigbks[sigcount].SigMessage == "?START>")
                                        startflag = true;                                                    //если выдано сообщение "?START>", установить соответствующий флаг

                                if(sigcount <=Form1->SigCountBKS - 2) {
                                        if(Form1->StartMessBKSFlag == 0)
                                                Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                        Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }


                                if(Form1->sigbks[sigcount].SigMessage == "?OK>")
                                                Sleep(300);
                                if(sigcount > 0 && sigcount <= Form1->SigCountBKS-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKS-2) - (sigcount-1)*100/(Form1->SigCountBKS-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                                        Form1->SigBarBKS->StepIt();
                                perc = 0;
                                sigcount++;
                        }

                        else if(Form1->CloseFlagBKS == true) {                        //Если форма программы проверки закрыта
                                Form1->outBKSBuff = "<ERROR! Emulator program has been closed. Pls close this program.>";
                                Form1->SignalsSendBKS = false;                        //Присваиваем 0 признаку отправки сообщений
                        }
                        else if(Form1->PitButtFlagBKS == true) {                      //После нажатия кнопки "Прервать работу"
                                if(++stopcount != 4) {                                //Если признак прерывания работы 1, поочередно отправить на программу проверки соответствующие сообщения
                                        switch(stopcount) {
                                        case 1:
                                                Form1->outBKSBuff = "<Прерывание работы эмулятора>";
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

                        else                                                            //Если признак остановки проверки Form1->StopFlagBKS = 1
                                Form1->SignalsSendBKS = false;
                        if(Form1->NextButtFlagBKS == true && Form1->TimerBox->State == cbChecked) {                    //После нажатия кнопки "Следующий сигнал"
                                Form1->outBKSBuff = Form1->outBKSBuff + Form1->sigbks[sigcount].SigMessage;
                                if(Form1->sigbks[sigcount].SigMessage == "?START>")
                                        startflag = true;                                                    //если выдано сообщение "?START>", установить соответствующий флаг

                                if(sigcount<= Form1->SigCountBKS - 2) {
                                        if(Form1->StartMessBKSFlag == 0)
                                                Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                        Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }

                                if(sigcount > 0 && sigcount <= Form1->SigCountBKS-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKS-2) - (sigcount-1)*100/(Form1->SigCountBKS-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                                        Form1->SigBarBKS->StepIt();
                                perc = 0;
                                sigcount++;
                                Form1->NextButtFlagBKS = false;                //Присвоить признаку отправки следующего сигнала значение 0
                                }
                        continue;
                }



                SetEvent(Form1->AllSigBKS);                                     //установить событие "Все сообщения отправлены"
//-----------------------------------------------------------------------------------------------------------------------
                ResumeThread(Form1->BKSRead);                                    //возобновляем поток чтения БКС
                SuspendThread(Form1->BKSWrite);                                  //останавливаем поток записи БКС

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
                if(Form1->outBKIBuff != "")                      //если режим отправки сигналов не в процессе выполнения

//----------------------------------в данном блоке реализована посимвольная запись, которая не поволяет точно иммитировать работу системы------------------
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
//-------------------------Новая реализация блока записи в COM-порт---------------------------------------------------------------------------------
                         while(Form1->outBKIBuff.Length()>0)
                         {
                                strcpy(sndstr, Form1->outBKIBuff.c_str());
                                Form1->RichBKI->Lines->Add(sndstr);                           //запись отправленных данных в текстовое поле RichBKI
                                Form1->outBKIBuff = "";
                                CComUse.ComClear(Form1->hCommBKI, PURGE_TXCLEAR);
                                WriteFile(Form1->hCommBKI, sndstr, strlen(sndstr), &temp, &ovrlBKIWrite);
                                WaitForSingleObject(ovrlBKIWrite.hEvent, INFINITE);

                         }
//----------------------------------------------------------------------------------------------------------------------------


                SetEvent(Form1->MessageSendBKI);
                if(Form1->TimerBox->State == true)
                        Sleep(1);                                                      //минимальная задержка между транзакциями записи по каналу БКИ
//-----------------------------отправка сообщений (инф. о сигналах) по каналу БКИ без таймера--------------------------
                if(Form1->SignalsSendBKI == true && sigcount <= Form1->SigCountBKI) {
                        ResetEvent(Form1->MessageSendBKI);
                        if(Form1->StopFlagBKI == false && Form1->CloseFlagBKI == false) {
                                Form1->outBKIBuff = Form1->sigbki[sigcount].SigMessage;
                                if(Form1->sigbki[sigcount].SigMessage == "?START>")
                                        startflag = true;

                                if(sigcount <= Form1->SigCountBKI - 2) {
                                        if(Form1->StartMessBKIFlag == 0)
                                                Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKI-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                                else
                                                        Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                        }
                                }



                                if(Form1->sigbki[sigcount].SigMessage == "?OK>")                                 // Задержка после "?OK>" и перед "!RDY>"
                                        Sleep(300);
                                if(sigcount > 0 && sigcount <= Form1->SigCountBKI-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKI-2) - (sigcount-1)*100/(Form1->SigCountBKI-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                                        Form1->SigBarBKI->StepIt();
                                perc = 0;
                                sigcount++;

                        }

                       else if(Form1->CloseFlagBKI == true) {                                   //Закрытие формы эмулятора во время проверки (некорректное завершение)
                                Form1->outBKIBuff = "<ERROR! Emulator program has been closed. Pls close this program.>";
                                Form1->SignalsSendBKI = false;
                       }


                       else if(Form1->PitButtFlagBKI == true) {
                                if(++stopcount != 4) {
                                        switch(stopcount) {
                                        case 1:
                                                Form1->outBKIBuff = "<Прерывание работы эмулятора>";
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
                                               Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2);
                                        else {
                                                if(startflag == false)
                                                        Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                                else
                                                     Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(sigcount-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                                        }
                                }

                                if(sigcount > 0 && sigcount <= Form1->SigCountBKI-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                                        perc = sigcount*100/(Form1->SigCountBKI-2) - (sigcount-1)*100/(Form1->SigCountBKI-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                                        Form1->SigBarBKI->StepIt();
                                perc = 0;
                                sigcount++;
                                Form1->NextButtFlagBKI = false;
                        }
                        continue;
                }
//-----------------------------------------------------------------------------------------------------------------------
                SetEvent(Form1->AllSigBKI);
                ResumeThread(Form1->BKIRead);                                   //возобновляем поток чтения БКИ
                SuspendThread(Form1->BKIWrite);                                 //останавливаем поток записи БКИ

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
        COMSTAT comstat;                                                     //структура COMSTAT содержит информацию о коммуникационном устройстве
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
                                        num=comstat.cbInQue;                                            //cbInQue - свойство структуры COMSTAT - количество полученных,
                                        if (num)                                                        //но еще не прочитанных функцией ReadFile байт
                                        {
                                                Form1->event->WaitFor(100);        //?
                                                memset(bufferBKS, NULL, 2);
                                                ReadFile(Form1->hCommBKS, bufferBKS, num, &realnum, &ovrlBKSRead);
                                                Form1->LiteraBKS=bufferBKS[0];              //получение управляющей литеры из буфера БКС
                                                if(bufferBKS[0] != NULL)
                                                        RichStr += bufferBKS[0];
                                                Form1->RichBKS->Lines->Add("<--- " + RichStr); //запись полученной команды в тесктовое поле RichBKS
                                                RichStr = "";
                                                SetEvent(Form1->LitBKSReceived);
                                                if(count < Form1->OpCountBKS) {                               //если кол-во полученных литер меньше числа прочитанных из .emu файла управляющих операций
                                                        if(count == 0 || (count == Form1->OpCountBKS - 1 && Form1->FirstTestFlag == false))              //если получена первая управляющая команда или команда, предшествующая началу передачи сигналов, при последующих провекрках
                                                                Form1->NullAll(2);
                                                        if(Form1->LiteraBKS == Form1->operbks[count].OpMessage[1] && Form1->operbks[count].OpStatus == "WAIT" ){                //если полученная литера равна ожидаемой, и тип текущей управляющей операции - "ОЖИДАНИЕ"
                                                                Form1->operbks[count].OpDone = true;               //присваиваем 1 признаку управляющей операции "Выполнено"
                                                                count++;
                                                        }
                                                   //     else
                                                   //             continue;
                                                        while(Form1->operbks[count].OpStatus == "SEND") {          //если текущая управляющая операция имеет тип "ОТПРАВКА"
                                                                Sleep(500);                                                  //задержка 0.5 с между командами, отправляемыми на БКС
                                                                Form1->SendStringBKS(Form1->operbks[count].OpMessage);        //отправляем команду, прочитанную из .emu файла и записанную в соотв. структуру
                                                                count++;
                                                                SuspendThread(Form1->BKSRead);                               //после отправки сообщения на программу проверки останавливаем поток чтения БКС

                                                        }
                                                }
                                                if(count == Form1->OpCountBKS) {                       //если обработали последнюю управляющую операцию
                                                        if(Form1->TimerBox->State == cbUnchecked)
                                                                Form1->SigDelayBKS->Enabled = true;                         //запускаем таймер БКС
                                                        else {
                                                        Form1->SignalsSendBKS = true;                                       //присваиваем 1 признаку отправки сообщения
                                                        Form1->outBKSBuff = "";                                           //возобновляем поток записи по каналу БКС
                                                        ResumeThread(Form1->BKSWrite);                                      //увеличиваем счетчик, чтобы вышеописанные условия не выполнялись, фактически управ. оп. нет
                                                        }
                                                        Form1->SigBarBKS->Position = 0;
                                                        Form1->StatusLabel->Caption = "Состояние: выполнение проверки";
                                                        count++;
                                                }


                                                if(countprot < Form1->PrCountBKS) {

                                                        if(Form1->LiteraBKS == Form1->protbks[countprot].PrMessage[1] && Form1->protbks[countprot].PrStatus == "WAIT" ){                //если полученная литера равна ожидаемой, и тип текущей строки протокола - "ОЖИДАНИЕ"
                                                                Form1->protbks[countprot].PrDone = true;               //присваиваем 1 признаку "Выполнено" строки протокола
                                                                if(countprot == 0)                                    //Обнуляем параметры, возвращаем элементы интерфейса в исходное состояние
                                                                        Form1->NullAll(3);
                                                                countprot++;

                                                        }
                                                        while(Form1->protbks[countprot].PrStatus == "SEND") {          //если текущая строка протокола имеет тип "ОТПРАВКА"
                                                                WaitForSingleObject(Form1->AllSigBKS, INFINITE);
                                                                Sleep(10);                                      //задержка между сообщениями протокола, отправляемыми на программу проверки
                                                                Form1->SendStringBKS(Form1->protbks[countprot].PrMessage);        //отправляем строку протокола, прочитанную из .emu файла и записанную в соотв. структуру
                                                                countprot++;
                                                                SuspendThread(Form1->BKSRead);                               //после отправки сообщения на программу проверки останавливаем поток чтения БКС

                                                        }


                                                }

                                                if(countprot == Form1->PrCountBKS) {                                       //после вывода протокола обнуляем счетчики для повторной проверки
                                                        countprot = 0;
                                                        count = Form1->OpCountBKS - 1;                                    //переходим к последней управляющей операции
                                                        Form1->NullAll(1);
                                                        Form1->StopFlagBKS = false;
                                                        Form1->PitButtFlagBKS = false;
                                                        Form1->FirstTestFlag = false;
                                                        TerminateThread(Form1->BKSWrite, 0);                               //после завершения проверки(вывода протокола) удаляем поток
                                                        Form1->BKSWrite=CreateThread(NULL, 0, WriteBKSThread, NULL, CREATE_SUSPENDED, NULL);      //и создаем заново

                                                }
                                                //------------------Обработка управляющих команд, поступивших во время выдачи сообщений на канал БКС-------------------------------------
                                               if(Form1->LiteraBKS == '~') {

                                                        WaitForSingleObject(Form1->AllSigBKS, INFINITE);                         //ждем окончания отправки сообщения по БКС
                                                     //   SuspendThread(Form1->BKSWrite);                                               //останавливаем поток записи БКС
                                                        Form1->StopFlagBKS = true;                                                    //устанавливаем признак принудительного прерывания
                                                        Sleep(200);                                                                   //задержка после получения команды прерывания
                                                        Form1->SendStringBKS("<3 0 50>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("?ERR>");
                                                        SuspendThread(Form1->BKSRead);
                                                        Sleep(200);
                                                        Form1->SendStringBKS("!RDY>");
                                                        Form1->LiteraBKS = NULL;                                                       //очищаем входной буфер БКС
                                                        ResumeThread(Form1->BKSWrite);                                                 //возобновляем поток записи БКС
                                                        SuspendThread(Form1->BKSRead);                                                //останавливаем поток чтения БКС
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
                                                        Application->MessageBox("Программа проверки закрыта БКС" , "Оповещение!", MB_OK);
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
        AnsiString RichStr = "";                                            //строка для вывода в текстовое поле RichBKI
        COMSTAT comstat;                                                       //структура COMSTAT содержит информацию о коммуникационном устройстве
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
                                        num=comstat.cbInQue;                                            //cbInQue - свойство структуры COMSTAT - количество полученных,
                                        if (num)                                                        //но еще непрочитанных функцией ReadFile байт
                                        {
                                                Form1->event->WaitFor(100);          //?
                                                memset(bufferBKI, NULL, 2);
                                                ReadFile(Form1->hCommBKI, bufferBKI, num, &realnum, &ovrlBKIRead);
                                                Form1->LiteraBKI=bufferBKI[0];
                                                if(bufferBKI[0] != NULL)
                                                        RichStr += bufferBKI[0];

                                                Form1->RichBKI->Lines->Add("<--- " + RichStr); //запись полученной команды в тесктовое поле RichBKI
                                                RichStr = "";
                                                SetEvent(Form1->LitBKIReceived);
                                                if(count < Form1->OpCountBKI) {
                                                        if(count == 0 || (count == Form1->OpCountBKI - 1 && Form1->FirstTestFlag == false))      //Если получили команду, запускающую цикл отправки сообщений, при повторной проверке
                                                                Form1->NullAll(2);
                                                        if(Form1->LiteraBKI == Form1->operbki[count].OpMessage[1] && Form1->operbki[count].OpStatus == "WAIT" ){
                                                                Form1->operbki[count].OpDone = true;
                                                                count++;
                                                        }
                                                        while(Form1->operbki[count].OpStatus == "SEND") {
                                                                Sleep(500);                                    //задержка 0.5 с между командами, отправляемыми на БКИ
                                                                Form1->SendStringBKI(Form1->operbki[count].OpMessage);
                                                                count++;
                                                                SuspendThread(Form1->BKIRead);                 //после отправки сообщения на программу проверки останавливаем поток чтения БКИ

                                                        }
                                                }
                                                if(count == Form1->OpCountBKI) {
                                                        if(Form1->TimerBox->State == cbUnchecked)
                                                                Form1->SigDelayBKI->Enabled = true;                    //запускаем таймер БКИ
                                                        else {
                                                        Form1->SignalsSendBKI = true;
                                                        Form1->outBKIBuff = "";
                                                        ResumeThread(Form1->BKIWrite);
                                                        }
                                                        Form1->SigBarBKI->Position = 0;
                                                        Form1->StatusLabel->Caption = "Состояние: выполнение проверки";
                                                        count++;
                                                }
                                                if(countprot < Form1->PrCountBKI) {

                                                        if(Form1->LiteraBKI == Form1->protbki[countprot].PrMessage[1] && Form1->protbki[countprot].PrStatus == "WAIT" ){                //если полученная литера равна ожидаемой, и тип текущей строки протокола - "ОЖИДАНИЕ"
                                                                Form1->protbki[countprot].PrDone = true;               //присваиваем 1 признаку "Выполнено" строки протокола
                                                                if(countprot == 0)
                                                                        Form1->NullAll(3);
                                                                countprot++;
                                                        }
                                                        while(Form1->protbki[countprot].PrStatus == "SEND") {          //если текущая строка протокола имеет тип "ОТПРАВКА"
                                                                WaitForSingleObject(Form1->AllSigBKI, INFINITE);
                                                                Sleep(10);
                                                                if(Form1->protbki[countprot].PrMessage == "!RDY>")
                                                                        Sleep(100);
                                                                Form1->SendStringBKI(Form1->protbki[countprot].PrMessage);        //отправляем строку протокола, прочитанную из .emu файла и записанную в соотв. структуру
                                                                countprot++;
                                                                SuspendThread(Form1->BKIRead);                               //после отправки сообщения на программу проверки останавливаем поток чтения БКС
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
                                                //------------------Обработка управляющих команд, поступивших во время выдачи сообщений на канал БКИ-------------------------------------
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
                                                        Application->MessageBox("Программа проверки закрыта БКИ" , "Оповещение!", MB_OK);
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

void TForm1::SendStringBKS(AnsiString sendbuf)                              //функция записи в COM-порт по каналу БКС
{
        Form1->outBKSBuff=Form1->outBKSBuff + sendbuf;
        ResumeThread(Form1->BKSWrite);                                       //возобновляем поток записи БКС
}

void TForm1::SendStringBKI(AnsiString sendbuf)                              //функция записи в COM-порт по каналу БКС
{
        Form1->outBKIBuff=Form1->outBKIBuff + sendbuf;
        ResumeThread(Form1->BKIWrite);                                       //возобновляем поток записи БКИ
}

void __fastcall TForm1::PitButtClick(TObject *Sender)
{
        if(PitButt->Caption == "Подать питание"){
                Form1->SendStringBKI("!SYS>");
                Form1->SendStringBKS("!SYS>");

                Form1->SigBarBKS->Position = 0;
                Form1->SigBarBKI->Position = 0;
                Form1->DefaultSets->Enabled = false;
                PitButt->Caption = "Прервать работу";
                Form1->StatusLabel->Caption = "Состояние: питание подключено";
                NullAll(2);
        }
        else if(PitButt->Caption == "Прервать работу") {

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
                PitButt->Caption = "Подать питание";
                Form1->StatusLabel->Caption = "Состояние: работа прервана";
        }



}

void TForm1::GetSystemInfo(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if(temp[temp.Length()] == '\n')                           //Если последний символ в столке '\n', удаляем
                        temp.Delete(temp.Length(), 1);
                if(temp.Pos("СИСТЕМА:"))
                        sysinf->SystemName=temp;
                if(temp.Pos("РЕЖИМ:"))
                        sysinf->ModName=temp;
        }
        Form1->SystemNameLabel->Caption = sysinf->SystemName;
        Form1->ModNameLabel->Caption = sysinf->ModName;
        rewind(filename);                                              //возвращаем указатель файла в начало

}

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        //-------Корректное прерывание и оповещение программы проверки о закрытии

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


//-------------------функции чтения строк тестового .emu файла-------------------------------------------------------------------------------------

void TForm1::StartModBKI(FILE *filename)
{
        AnsiString temp;
        char buffer[SYMBOLSNUM];
        while(fgets(buffer, SYMBOLSNUM, filename) != NULL) {
                temp=buffer;
                if (temp.Pos("[БКИ]")) {
                        while(!temp.Pos("[/БКИ]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[ЗАПУСК РЕЖИМА]"))
                                        while(!temp.Pos("[/ЗАПУСК РЕЖИМА]")) {
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
                if (temp.Pos("[БКС]")) {
                        while(!temp.Pos("[/БКС]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[ЗАПУСК РЕЖИМА]"))
                                        while(!temp.Pos("[/ЗАПУСК РЕЖИМА]")) {
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
                if(temp.Pos("[БКС]")) {
                        while(!temp.Pos("[/БКС]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[СИГНАЛЫ]")) {
                                        while(!temp.Pos("[/СИГНАЛЫ]")) {
                                                fgets(buffer,SYMBOLSNUM, filename);
                                                temp=buffer;
                                                if(!temp.Pos("[/СИГНАЛЫ]"))
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
                if(temp.Pos("[БКИ]")) {
                        while(!temp.Pos("[/БКИ]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[СИГНАЛЫ]")) {
                                        while(!temp.Pos("[/СИГНАЛЫ]")) {
                                                fgets(buffer,SYMBOLSNUM, filename);
                                                temp=buffer;
                                                if(!temp.Pos("[/СИГНАЛЫ]"))
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
                if (temp.Pos("[БКИ]")) {
                        while(!temp.Pos("[/БКИ]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[ПРОТОКОЛ]"))
                                        while(!temp.Pos("[/ПРОТОКОЛ]")) {
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
                if (temp.Pos("[БКС]")) {
                        while(!temp.Pos("[/БКС]")) {
                                fgets(buffer, SYMBOLSNUM, filename);
                                temp=buffer;
                                if(temp.Pos("[ПРОТОКОЛ]"))
                                        while(!temp.Pos("[/ПРОТОКОЛ]")) {
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

//-----------------------------функции обработки прочитанных строк .emu файла-------------------------------------------------------------------

void TForm1::StartStrProc(AnsiString str)             //обработка строк с управляющими операциями
{

        for(int i=1; i<=str.Length(); i++)
                if(str[i] == '\r')
                        str[i] = '\0';                        //отсекаем '\r' и все что после

        if(str.Pos("BKS||")) {                                //условное обозначение строки в .emu файле с ожидаемой по каналу БКС литерой
                str.Delete(1, 5);                              //удаляем условное обозначение из строки
                operbks[OpCountBKS].OpMessage = str[1];                         //сохраняем сообщение операции
                operbks[OpCountBKS].OpStatus = "WAIT";               //сохраняем статус операции
                OpCountBKS++;
        }
        if(str.Pos("BKI||")) {
                str.Delete(1, 5);
                operbki[OpCountBKI].OpMessage = str[1];
                operbki[OpCountBKI].OpStatus = "WAIT";
                OpCountBKI++;
        }

        if(str.Pos("BKS<<")) {                                  //условное обозначение строки в .emu файле с отправляемой на программу проверки командой
                str.Delete(1, 5);                               //удаляем у.о.
                operbks[OpCountBKS].OpMessage = str;
                operbks[OpCountBKS].OpStatus = "SEND";
                OpCountBKS++;
        }
        if(str.Pos("BKI<<")) {
                str.Delete(1, 5);                               //удаляем у.о.
                operbki[OpCountBKI].OpMessage = str;
                operbki[OpCountBKI].OpStatus = "SEND";
                OpCountBKI++;
        }

}

void TForm1::ProtStrProc(AnsiString str)             //обработка строк протокола, полученных из .emu файла
{                                                    //операции над длинными строками повреждают их!!!

        for(int i=1; i<=str.Length(); i++)
                if(str[i] == '\r')
                        str[i] = '\0';                        //отсекаем '\r' и все что после
        if(str.Pos("BKS||")) {                                //условное обозначение строки в .emu файле с ожидаемой по каналу БКС литерой
                str.Delete(1, 5);                              //удаляем условное обозначение из строки
                protbks[PrCountBKS].PrMessage = str[1];                         //сохраняем значимую часть строки из блока протокола
                protbks[PrCountBKS].PrStatus = "WAIT";               //сохраняем статус строки из блока протокола
                PrCountBKS++;
        }
        if(str.Pos("BKI||")) {
                str.Delete(1, 5);
                protbki[PrCountBKI].PrMessage = str[1];
                protbki[PrCountBKI].PrStatus = "WAIT";
                PrCountBKI++;
        }

        if(str.Pos("BKS<<")) {                                  //условное обозначение строки в .emu файле с отправляемым на программу проверки сообщением протокола
                str.Delete(1, 5);                               //удаляем у.о.
                protbks[PrCountBKS].PrMessage = str;
                protbks[PrCountBKS].PrStatus = "SEND";
                PrCountBKS++;
        }

        if(str.Pos("BKI<<")) {
                str.Delete(1, 5);                               //удаляем у.о.
             //   Form1->BuffBKIMemo->Text = Form1->BuffBKIMemo->Text + "\n" + str;
                protbki[PrCountBKI].PrMessage = str;
                protbki[PrCountBKI].PrStatus = "SEND";
                PrCountBKI++;
        }

}

void TForm1::SignalsLoadBKS(AnsiString str)                //обработка строк с сообщениями для отправки на программу проверки по каналу БКС
{
        int i;
        AnsiString time = "";
        AnsiString message = "";
        for(i=1; i<=str.Length(); i++) {
                if(str[i] == ':') {
                        while(str[++i] != ':')
//                                time[k++] = str[i];             Неверно!В строковом классе AnsiString присваивать неинициализированные элементы НЕЛЬЗЯ!
                                time = time + str[i];          // Посимвольное заполнение строки типа AnsiString. Верный вариант.
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
                Application->MessageBox(IntToStr(SigCountBKS).c_str(), "Файл недоступен!", MB_OK);
        }
        sigbks[SigCountBKS].SigMessage = message;
        SigCountBKS++;

}

void TForm1::SignalsLoadBKI(AnsiString str)               //обработка строк с сообщениями для отправки на программу проверки по каналу БКИ
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

//-------------------------------функции отправки сообщений на программу проверки по каналам БКС и БКИ-------------------------------

void __fastcall TForm1::SigDelayBKSEvents(TObject *Sender)
{
        static bool startflag = false;
        unsigned int perc = 0;
        if((sigbks[signumbks].SigTime < tickbks) && (signumbks < SigCountBKS)) {
                Form1->SignalsSendBKS = true;
           //     Form1->NextButtFlagBKS = true;
                Form1->outBKSBuff = Form1->outBKSBuff + Form1->sigbks[signumbks].SigMessage;
                ResumeThread(Form1->BKSWrite);
                if(signumbks > 0 && signumbks <= Form1->SigCountBKS-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                        perc = signumbks*100/(Form1->SigCountBKS-2) - (signumbks-1)*100/(Form1->SigCountBKS-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                        Form1->SigBarBKS->StepIt();
                perc = 0;
                if(sigbks[signumbks].SigMessage == "?START>")
                                        startflag = true;
                if(startflag == true){
                        if(signumbks <= Form1->SigCountBKS - 3)
                                Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(signumbks-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);
                }
                else {
                        if((signumbks <= Form1->SigCountBKS - 2) && (Form1->StartMessBKSFlag == 0))
                                Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(signumbks) + "/" + IntToStr(Form1->SigCountBKS-2);
                        else if(Form1->StartMessBKSFlag == 1)
                                Form1->SentBKSLabel->Caption = "БКС: " + IntToStr(signumbks-1) + "/" + IntToStr(Form1->SigCountBKS-2-Form1->StartMessBKSFlag);              //вывод количества отправленных сигналов и общего количества
                }
                if(Form1->PauseButt->Caption == "Продолжить") {                      //Если работа эмулятора приостановлена отключить таймер
                        Form1->SigDelayBKS->Enabled = false;
                        if(nextcount == 1) {
                                Form1->NextButt->Enabled = true;
                                Form1->StatusLabel->Caption = "Состояние: работа приостановлена";
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

        Form1->TimerBKSLabel->Caption = "БКС: " + IntToStr(tickbks);
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
                if(signumbki > 0 && signumbki <= Form1->SigCountBKI-2)                                 //если получен второй по счетц сигнал и счетчик меньше общего количества сигналов за исключением "?OK>", "!RDY>"
                        perc = signumbki*100/(Form1->SigCountBKI-2) - (signumbki-1)*100/(Form1->SigCountBKI-2);           //рассчитываем насколько процентов увеличится доля отправленных сигналов после отправления очередного сигнала
                for(unsigned int i=0; i<perc; i++)           //Если выдан хотя бы процент от общего количества сигналов, выполнить шаг в статус-баре
                        Form1->SigBarBKI->StepIt();
                perc = 0;
                if(sigbki[signumbki].SigMessage == "?START>")
                        startflag = true;

                if(signumbki <=Form1->SigCountBKI - 2) {
                        if(Form1->StartMessBKIFlag == 0)
                                Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(signumbki) + "/" + IntToStr(Form1->SigCountBKI-2);
                        else {
                                if(startflag == false)
                                        Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(signumbki) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                                else
                                        Form1->SentBKILabel->Caption = "БКИ: " + IntToStr(signumbki-1) + "/" + IntToStr(Form1->SigCountBKI-2-Form1->StartMessBKIFlag);
                        }
                }


                ResumeThread(Form1->BKIWrite);
                if(Form1->PauseButt->Caption == "Продолжить") {
                        Form1->SigDelayBKI->Enabled = false;
                        if(nextcount == 1) {
                                Form1->NextButt->Enabled = true;
                                Form1->StatusLabel->Caption = "Состояние: работа приостановлена";
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

        Form1->TimerBKILabel->Caption = "БКИ: " + IntToStr(tickbki);
        tickbki++;
}










