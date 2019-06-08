//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MakeDeviceFile.h"
#include "Emu1.h";
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMakeFileForm *MakeFileForm;
char *symbols;

double round(double x)                                       //������� round() � C++98, ������� ������ ��
{
        return floor(x + 0.5);
}

//---------------------------------------------------------------------------
__fastcall TMakeFileForm::TMakeFileForm(TComponent* Owner)
        : TForm(Owner)
{
         newfilebks = new NewFileData;
         newfilebki = new NewFileData;
         SystemName = new char[128];
         ModName =  new char[128];
         unsigned int k = 0;
         unsigned int i = 0;
         AnsiString temp;
         CurrDir=GetCurrentDir();
         symbols = new char[SYMNUMBER];            //������ �������������� � ASCII ��������: 'a' - 'z', 'A' - 'Z', 'BKCSP', '-', '-' � ���������, '�' - '�', '�' - '�', ' '
         SystemNameFlag = false;
         ModNameFlag = false;
         BKIFileFlag = false;
         BKSFileFlag = false;
         ResultFileFlag = false;
         ApplySetsButt->Enabled = false;
         while(i != 26) {                          //'A' - 'Z'
                symbols[i] = k + 65;
                k++;
                i++;
         }
         k = 0;
         while(i != 52) {
                symbols[i] = k + 97;             //'a' - 'z'
                k++;
                i++;
         }
         k = 0;
         while(i != 63) {
                symbols[i] = k + 48;            //'0'- '9'
                k++;
                i++;
         }
         k = 0;
         while(i != 64) {                      //'-' ��������
                symbols[i] = k + 45;
                k++;
                i++;
         }
         k = 0;
         while(i != 127) {                   //'�' - '�', '�' - '�'
                symbols[i] = k + 192;
                k++;
                i++;
         }
         k = 0;
         while(i != SYMNUMBER) {           //SPACE
                symbols[i] = k + 32;
                k++;
                i++;
         }

}

void TMakeFileForm::ChkOpt()                                                    //�������, ����������� ������������ ���� ��������� ����������
{
        if(SystemNameFlag && ModNameFlag && BKIFileFlag && BKSFileFlag && ResultFileFlag)
                MakeFileForm->ApplySetsButt->Enabled = true;
        else
                MakeFileForm->ApplySetsButt->Enabled = false;
}




void __fastcall TMakeFileForm::SystemNameChange(TObject *Sender)                                  //�������� ������������ ������������ ����� ��������� ���������� ����
{
        int i = 0;
        int j = 0;
        if(SystemNameEdit->Text.Length() != 0)
                for(i=1; i<=SystemNameEdit->Text.Length(); i++) {
                        for(j=0; j<SYMNUMBER; j++)
                                if(SystemNameEdit->Text[i] == symbols[j])
                                        break;
                        if(j == SYMNUMBER) {
                                MakeFileForm->SystemNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                                SystemNameFlag = false;
                                ChkOpt();
                                break;
                        }
                }
                if(i > SystemNameEdit->Text.Length()) {
                        MakeFileForm->SystemNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                        SystemNameFlag = true;
                        ChkOpt();
                }
        else {
                MakeFileForm->SystemNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                SystemNameFlag = false;
                ChkOpt();
        }

}






void __fastcall TMakeFileForm::ModNameChange(TObject *Sender)                                  //�������� ������������ ������������ ����� ��������� ���������� ���� ������������ ������
{
        int i = 0;
        int j = 0;
        if(ModNameEdit->Text.Length() != 0)
                for(i=1; i<=ModNameEdit->Text.Length(); i++) {
                        for(j=0; j<SYMNUMBER; j++)
                                if(ModNameEdit->Text[i] == symbols[j])
                                        break;
                        if(j == SYMNUMBER) {
                                MakeFileForm->ModNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                                ModNameFlag = false;
                                ChkOpt();
                                break;
                        }
                }
                if(i > ModNameEdit->Text.Length()) {
                        MakeFileForm->ModNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                        ModNameFlag = true;
                        ChkOpt();
                }
        else {
                MakeFileForm->ModNameImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                ModNameFlag = false;
                ChkOpt();
        }

}
void __fastcall TMakeFileForm::BKSFileButtClick(TObject *Sender)
{
        OpenFile("BKS");
}

void __fastcall TMakeFileForm::BKIFileButtClick(TObject *Sender)
{
        OpenFile("BKI");
}



void TMakeFileForm::OpenFile(AnsiString Line)
{
        if(Line == "BKS") {
                if(OpenFileBKS->Execute())
                        FileBKSName = OpenFileBKS->FileName;
                BKSFilePath->Text = FileBKSName;
                if((FileBKS = fopen(FileBKSName.c_str(), "a+b")) == NULL) {
                        Application->MessageBox(FileBKSName.c_str(), "���� ����������!", MB_OK);
                        MakeFileForm->BKSFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                        BKSFileFlag = false;
                        ChkOpt();
                }
                else
                        if(CheckingFile(FileBKS, FileBKSName, "BKS")) {
                                MakeFileForm->BKSFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                                BKSFileFlag = true;
                                ChkOpt();
                        }
        }
        if(Line == "BKI") {
                if(OpenFileBKI->Execute())
                        FileBKIName = OpenFileBKI->FileName;
                BKIFilePath->Text = FileBKIName;
                if((FileBKI = fopen(FileBKIName.c_str(), "a+b")) == NULL) {
                        Application->MessageBox(FileBKIName.c_str(), "���� ����������!", MB_OK);
                        MakeFileForm->BKIFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                        BKIFileFlag = false;
                        ChkOpt();
                }
                else
                        if(CheckingFile(FileBKI, FileBKIName, "BKI")) {
                                MakeFileForm->BKIFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                                BKIFileFlag = true;
                                ChkOpt();
                        }

        }
}

void TMakeFileForm::IncorFileMsg(AnsiString name, AnsiString line, unsigned int strnum)
{
       AnsiString temp;
       temp = name + "\n������ � ������: " + IntToStr(strnum);
       Application->MessageBox(temp.c_str(), "������������ ����!", MB_OK);
       if(line == "BKS") {
                MakeFileForm->BKSFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                BKSFileFlag = false;
                ChkOpt();
       }
       else if(line == "BKI") {
                MakeFileForm->BKIFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                BKIFileFlag = false;
                ChkOpt();
       }
}


int TMakeFileForm::CheckingFile(FILE *filep, AnsiString name, AnsiString line)           //�������� ������������ ������� ������ ����� ��� � ���
{
    AnsiString temp;
    unsigned int strcount = 0;
    char emptys[2][5] = {"\n", "\r\n"};                      //������ ������������ �����



    char buffer[MAXSYMINSTR];
    unsigned int i = 0;
    unsigned int k = 0;
    char sym;


        while(fgets(buffer, MAXSYMINSTR, filep) != NULL) {                    //������ �������� ���� ���������
                i = 0;
                strcount++;
                for(unsigned int j=0; j<2; j++)                             //���������� ������ �� ������� ������������
                        while(buffer[k] != '\0') {
                                if(buffer[k] != emptys[j][k])
                                        break;
                                k++;
                        }
                if(buffer[k] == '\0')
                        continue;

                if(isdigit(buffer[i++])) {                                         //������ ������ ������ ���� ������
                        while(isdigit(buffer[i]) && i<strlen(buffer))
                                i++;
                        if(i<=strlen(buffer)) {
                                if(buffer[i++] != ':') {
                                        IncorFileMsg(name, line, strcount);
                                        return 0;
                                }
                                else {
                                        while(isdigit(buffer[i]) && i<strlen(buffer))
                                                i++;
                                        if(i<=strlen(buffer)) {
                                                if(buffer[i++] != ':') {
                                                        IncorFileMsg(name, line,strcount);
                                                        return 0;
                                                }
                                                else {
                                                        while(isdigit(buffer[i]) && i<strlen(buffer))
                                                                i++;
                                                        if(i<=strlen(buffer)) {
                                                                if(buffer[i++] !=':') {
                                                                        IncorFileMsg(name, line, strcount);
                                                                        return 0;
                                                                }
                                                                else {
                                                                        while(isdigit(buffer[i])&& i<strlen(buffer))
                                                                                i++;
                                                                }
                                                        }
                                                        else {
                                                                IncorFileMsg(name, line, strcount);
                                                                return 0;
                                                        }
                                                }
                                        }
                                        else {
                                                IncorFileMsg(name, line, strcount);
                                                return 0;
                                        }
                                }
                        }
                        else {
                                IncorFileMsg(name, line, strcount);
                                return 0;
                        }
                }
                else {
                        IncorFileMsg(name, line, strcount);
                        return 0;
                }                                                                //������ ����� ��������� �����
                if(buffer[i++] == ' ') {
                        while(buffer[i] == ' ' && i<strlen(buffer))
                                i++;
                        if(i<=strlen(buffer)) {
                                while((strstr(buffer+i, "->") != buffer+i) && (strstr(buffer+i, "<-") != buffer + i) && i<strlen(buffer)) {                       //���������� �������� � ����� ����������� '
                                        if(buffer[i] == ' ') {
                                                i++;
                                                continue;
                                        }
                                        if((sym = buffer[i]) != '<' && sym != '!' && sym != '?' && sym != '#' && i<strlen(buffer)){
                                                i++;
                                                if(buffer[i] == ' ')
                                                        continue;
                                                else {
                                                        IncorFileMsg(name, line, strcount);
                                                        return 0;
                                                }
                                        }
                                        else {
                                                switch(sym) {
                                                case '<':
                                                        while(buffer[i] != '>' && i < strlen(buffer))
                                                                i++;
                                                        if(i < strlen(buffer)) {
                                                                i++;
                                                                break;
                                                        }
                                                        else {
                                                                IncorFileMsg(name, line, strcount);
                                                                return 0;
                                                        }
                                                case '!':
                                                        if(strstr(buffer+i, "RDY>")) {
                                                                i+=5;
                                                                break;
                                                        }
                                                        if(strstr(buffer+i, "SYS>")) {
                                                                i+=5;
                                                                break;
                                                        }
                                                        if(strstr(buffer+i, "PIT>")) {
                                                                i+=5;
                                                                break;
                                                        }
                                                        else {
                                                                IncorFileMsg(name, line, strcount);
                                                                return 0;
                                                        }
                                                case '?':
                                                        if(strstr(buffer+i, "OK>")) {
                                                                i+=4;
                                                                break;
                                                        }
                                                        if(strstr(buffer+i, "KS+>")) {
                                                                i+=5;
                                                                break;
                                                        }
                                                        if(strstr(buffer+i, "START>")) {
                                                                i+=7;
                                                                break;
                                                        }
                                                        if(strstr(buffer+i, "END>")) {
                                                                i+=5;
                                                                break;
                                                        }
                                                        else {
                                                                IncorFileMsg(name, line, strcount);
                                                                return 0;
                                                        }
                                                case '#':
                                                        while((strstr(buffer+i, "->") != buffer+i) && (strstr(buffer+i, "<-") != buffer+i) && (i < strlen(buffer)))
                                                                i++;
                                                        if(i < strlen(buffer))
                                                                break;
                                                        else {
                                                                IncorFileMsg(name, line, strcount);
                                                                return 0;
                                                        }

                                                default:
                                                        IncorFileMsg(name, line, strcount);
                                                        return 0;
                                                }
                                        }


                                }
                                if(i > strlen(buffer)) {
                                        IncorFileMsg(name, line, strcount);
                                        return 0;
                                }

                        }
                        else {
                                IncorFileMsg(name, line, strcount);
                                return 0;
                        }
                }
                else {
                        IncorFileMsg(name, line, strcount);
                        return 0;
                }
        }
        strcount = 0;
        rewind(filep);
        return 1;
}

void TMakeFileForm::ProcessingFile(FILE *filep, AnsiString line)                //��������� ������������ ����� � ���������� ���������� ������ � ���������
{
        char *temp;
        char emptys[2][5] = {"\n", "\r\n"};                     //������ ������������ �����
        char buffer[MAXSYMINSTR];
        char sym;
        unsigned int i = 0;                                    //������� �������� � ������ ���-�����
        unsigned int n = 0;
        unsigned int strcount = 0;                             //������� �������� �����
        bool SignalsFlag = false;



        if(line == "BKS") {
                while(fgets(buffer, MAXSYMINSTR, filep) != NULL) {
                        i = 0;
      //---------------------------------------------------------------------------------------------------------------------
                        for(unsigned int j=0; j<2; j++)                             //���������� ������ �� ������� ������������
                                while(buffer[i] != '\0') {
                                        if(buffer[i] != emptys[j][i])
                                                break;
                                        i++;
                                }
                        if(buffer[i] == '\0')
                                continue;
       //-----------------------------------------------------------------------------------------------------------------------
                        if(strstr(buffer, "!SYS>"))                     //����������� ������ "!SYS>"
                                continue;



                        i = 0;
                        while(buffer[i] != ' ') {
                                strncat(newfilebks->TimeStrValue[strcount], buffer+i, 1);
                                i++;
                        }
                        strcount++;

                        while(buffer[i] == ' ')
                                i++;

                        if((buffer[i] != '<') && (buffer[i] != '#')) {                   //��������� � ��������� ��������� ������� � �������� ��������� �����
                                if(!SignalsFlag) {                                       //���� �� ������ ���������������� ���������
                                        while(buffer[i] != ' ') {
                                                strncat(newfilebks->ControlStr[newfilebks->opcount], buffer+i, 1);        //������������ ���� ������ �� ������ � ������-�������� ���������
                                                i++;
                                        }
                                        while(buffer[i] == ' ')                         //���������� �������
                                                i++;
                                        if(strstr(buffer+i, "->") == buffer+i) {                   //���� � ����� ������ ���������� ������������������ "->"
                                               /* while(newfilebks->inmass[n])            //���������� ������ ����������� ������ �� ������� ������� ��������
                                                        n++;
                                                newfilebks->inmass[n] = newfilebks->opcount; */      //����������� �������� ���������� ����� ������������ ���������
                                                newfilebks->inmass[n] = newfilebks->opcount;
                                                n++;
                                        }
                                        newfilebks->opcount++;
                                        continue;
                                }
                        }

                        else if(buffer[i] == '<') {                                     //��������� � ��������� ���������� ���������
                                SignalsFlag = true;
                                while(buffer[i] != ' ') {
                                        if(buffer[i] == '<') {
                                                while(buffer[i] != '>') {
                                                        strncat(newfilebks->SignalStr[newfilebks->sigcount], buffer+i, 1);
                                                        i++;
                                                }
                                                if(buffer[i] == '>')
                                                        strncat(newfilebks->SignalStr[newfilebks->sigcount], buffer+i, 1);
                                                i++;
                                                newfilebks->sigcount++;
                                                newfilebks->inrow[newfilebks->opcount+newfilebks->sigcount] = true;
                                        }
                                        else if(buffer[i] == '?')
                                                if(strstr(buffer+i, "?START>") == buffer+i) {
                                                        strcpy(newfilebks->SignalStr[newfilebks->sigcount], "?START>");
                                                        i+=7;
                                                        newfilebks->sigcount++;
                                                        newfilebks->inrow[newfilebks->opcount+newfilebks->sigcount] = true;
                                                }


                                        else {
                                                Application->MessageBox(itoa(newfilebks->opcount+newfilebks->sigcount, temp, 10),  "������������ ������ ���������� ���������!", MB_OK);
                                                break;
                                        }

                                }
                                newfilebks->inrow[newfilebks->opcount+newfilebks->sigcount] = false;
                                newfilebks->sigstrcount++;
                                continue;
                        }
                        else if(buffer[i] == '#') {                                     //��������� � ��������� ������ ���������
                                while(buffer[i] != '>') {
                                        strncat(newfilebks->ProtStr[newfilebks->protcount], buffer+i, 1);
                                        i++;
                                }
                                if(buffer[i] == '>')
                                        strncat(newfilebks->ProtStr[newfilebks->protcount], buffer+i, 1);
                                newfilebks->protcount++;
                                continue;
                        }

                }
        }

        if(line == "BKI") {
                while(fgets(buffer, MAXSYMINSTR, filep) != NULL) {
                        i = 0;
        //-------------------------------------------------------------------------------------------------------------------------------
                        for(unsigned int j=0; j<2; j++)                             //���������� ������ �� ������� ������������
                                while(buffer[i] != '\0') {
                                        if(buffer[i] != emptys[j][i])
                                                break;
                                        i++;
                                }
                        if(buffer[i] == '\0')
                                continue;
       //------------------------------------------------------------------------------------------------------------------------------------
                        if(strstr(buffer, "!SYS>"))
                                continue;

                        i = 0;

                        while(buffer[i] != ' ') {
                                strncat(newfilebki->TimeStrValue[strcount], buffer+i, 1);
                                i++;
                        }
                        strcount++;
                        while(buffer[i] == ' ')
                                i++;

                        if((buffer[i] != '<') && (buffer[i] != '#')) {                   //��������� � ��������� ��������� ������� � �������� ��������� �����
                                if(!SignalsFlag) {
                                        while(buffer[i] != ' ') {
                                                strncat(newfilebki->ControlStr[newfilebki->opcount], buffer+i, 1);        //������������ ���� ������ �� ������ � ������-�������� ���������
                                                i++;
                                        }
                                        while(buffer[i] == ' ')
                                                i++;
                                        if(strstr(buffer+i, "->") == buffer+i) {
                                               /* while(newfilebki->inmass[n])
                                                        n++;
                                                newfilebki->inmass[n] = newfilebki->opcount; */
                                                newfilebki->inmass[n] = newfilebki->opcount;
                                                n++;
                                        }
                                        newfilebki->opcount++;
                                        continue;
                                }
                        }

                        else if(buffer[i] == '<') {                                     //��������� � ��������� ���������� ���������
                                SignalsFlag = true;
                                while(buffer[i] != ' ') {
                                        if(buffer[i] == '<') {
                                                while(buffer[i] != '>') {
                                                        strncat(newfilebki->SignalStr[newfilebki->sigcount], buffer+i, 1);
                                                        i++;

                                                }
                                                if(buffer[i] == '>')
                                                        strncat(newfilebki->SignalStr[newfilebki->sigcount], buffer+i, 1);
                                                i++;
                                                newfilebki->sigcount++;
                                                newfilebki->inrow[newfilebki->opcount+newfilebki->sigcount] = true;         //��������� ���� "<...>" ����������� �� ����� ������ � ����������
                                        }                                                                                   //�������������� �������� ��������� inrow[] ���������� � ������ ������, ������� ������� ������ ������������ ���������� newfilebki->opcount+newfilebki->sigcount
                                       else if(buffer[i] =='?')
                                                if(strstr(buffer+i, "?START>") == buffer+i) {
                                                        strcpy(newfilebki->SignalStr[newfilebki->sigcount], "?START>");
                                                        i+=7;
                                                        newfilebki->sigcount++;
                                                        newfilebki->inrow[newfilebki->opcount+newfilebki->sigcount] = true;
                                                }
                                        else {
                                                Application->MessageBox(itoa(newfilebki->opcount+newfilebki->sigcount, temp, 10),  "������������ ������ �������������������!", MB_OK);
                                                break;
                                        }

                                }
                                newfilebki->sigstrcount++;
                                newfilebki->inrow[newfilebki->opcount+newfilebki->sigcount] = false;                        //��������� ��������� ���� "<...>" ����� ����������� �� ����� ������, ������������� ������� �������
                                continue;
                        }
                        else if(buffer[i] == '#') {                                     //��������� � ��������� ������ ���������
                                while(buffer[i] != '>') {
                                        strncat(newfilebki->ProtStr[newfilebki->protcount], buffer+i, 1);
                                        i++;
                                }
                                if(buffer[i] == '>')
                                        strncat(newfilebki->ProtStr[newfilebki->protcount], buffer+i, 1);
                                newfilebki->protcount++;
                                continue;
                        }

                }
        }


}

void TMakeFileForm::ProcessingTime(NewFileData *obj)
{
        unsigned int i = obj->opcount;
        unsigned int j = obj->opcount;
        unsigned int count = obj->opcount + obj->sigstrcount;

        char *strtmp = new char[100];                 //��������������� ����������
        char *strnum = new char[100];
        struct TimeFormat
        {

                unsigned int min;
                unsigned int sec;
                unsigned int msec;
                unsigned int hour;
                TimeFormat(char *start) {
                        unsigned int i = 0;
                        unsigned int k = 0;
                        char temp[10];
                        while(start[i] != ':')
                                temp[k++] = start[i++];
                        temp[k] = '\0';
                        k = 0;
                        hour = atoi(temp);
                        i++;
                        while(start[i] != ':')
                                temp[k++] = start[i++];
                        temp[k] = '\0';
                        k = 0;
                        min = atoi(temp);
                        i++;
                        while(start[i] != ':')
                                temp[k++] = start[i++];
                        temp[k] = '\0';
                        k = 0;
                        sec = atoi(temp);
                        i++;
                        while(start[i] != '\0')
                                temp[k++] = start[i++];
                        temp[k] = '\0';
                        k = 0;
                        msec = atoi(temp);

                }
                int CalculateTime(char *timestr) {
                        unsigned int i = 0;
                        unsigned int k = 0;
                        char temp[10] = "";
                        char timetmp[10] = "";
                        unsigned int newhour = 0;
                        unsigned int newmin = 0;
                        unsigned int newsec = 0;
                        unsigned int newmsec = 0;
                        float mscalc;                   //��������������� ���������� ��� ������� ������� ����� �� �����������
                        int scalc;                      //��������������� ���������� ��� ������� ������� ����� �� ������
                        int mcalc;
                        int hcalc;
                        while(timestr[i] != ':') {           //�������� ������ � ��������� � ������������� ����������
                                temp[k++] = timestr[i];
                                i++;
                        }
                        temp[k] = '\0';
                        k = 0;
                        newhour = atoi(temp);
                        i++;
                        while(timestr[i] != ':') {           //�������� ������ � ��������� � ������������� ����������
                                temp[k++] = timestr[i];
                                i++;
                        }
                        temp[k] = '\0';
                        k = 0;
                        newmin = atoi(temp);
                        i++;
                        while(timestr[i] != ':') {            //�������� ������� � ��������� � ������������� ����������
                                temp[k++] = timestr[i];
                                i++;
                        }
                        temp[k] = '\0';
                        k = 0;
                        newsec = atoi(temp);
                        i++;
                        while(timestr[i] != '\0') {           //�������� ������������ � ��������� � ������������� ����������
                                temp[k++] = timestr[i];
                                i++;
                        }
                        temp[k] = '\0';
                        k = 0;
                        newmsec = atoi(temp);
                        if(newhour > hour) {
                                if(newmin >= min) {
                                        hcalc = newhour - hour;
                                        if(newsec >= sec) {
                                                mcalc = newmin - min;
                                                if(newmsec >= msec){
                                                        scalc = newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else {
                                                        scalc = newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }

                                        else {
                                                mcalc = newmin - min - 1;
                                                if(newmsec > msec) {
                                                        scalc = 60 + newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else    {
                                                        scalc = 60 + newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }
                                }
                                else {
                                       hcalc = newhour - hour - 1;
                                       if(newsec >= sec) {
                                                mcalc = 60 + newmin - min;
                                                if(newmsec >= msec){
                                                        scalc = newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else {
                                                        scalc = newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }

                                        else {
                                                mcalc = 60 + newmin - min - 1;
                                                if(newmsec >= msec) {
                                                        scalc = 60 + newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else    {
                                                        scalc = 60 + newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }
                                }
                        }



                        else if(newhour == hour) {
                                hcalc = 0;
                                if(newmin > min) {
                                        if(newsec >= sec) {
                                                mcalc = newmin - min;
                                                if(newmsec >= msec) {
                                                        scalc = newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else {
                                                        scalc = newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }
                                        else {
                                                mcalc = newmin - min - 1;
                                                if(newmsec >= msec) {
                                                        scalc = 60 + newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else {
                                                        scalc = 60 + newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }
                                }





                                else if(newmin == min) {
                                        mcalc = 0;
                                        if(newsec > sec) {
                                                if(newmsec >= msec) {
                                                        scalc = newsec - sec;
                                                        mscalc = (float)(newmsec - msec)/100;
                                                }
                                                else {
                                                        scalc = newsec - sec - 1;
                                                        mscalc = (float)(1000 + newmsec - msec)/100;
                                                }
                                        }
                                        else if(newsec == sec) {
                                                scalc = 0;
                                                if(newmsec > msec)
                                                        mscalc = (float)(newmsec - msec)/100;
                                                else if(newmsec == msec)
                                                        mscalc = 0;
                                                else
                                                        return 0;
                                        }
                                        else
                                                return 0;
                                }
                                else
                                        return 0;
                        }
                        else
                                return 0;






                //        mscalc = (newmsec - msec)/100;         //����������� ����� � ������� ����� �� �������������� ������������
                        strcpy(timestr, itoa(hcalc*3600 + mcalc*600 + scalc*10 + round(mscalc), timetmp , 10));                //��������� �����, ������������ � ������� ����� ������� � �������� ������
                        return 1;
              }



        } *newformat;                             //�� ������ ������� ��������� ����� ��������� ���������� ������������ �������

       newformat = new TimeFormat(obj->TimeStrValue[obj->opcount-1]);

        for(unsigned int j=obj->opcount; j<obj->sigstrcount+obj->opcount; j++)                         //������������ � ��������� ��������� ���������� ��� ���������� �����
                if(!newformat->CalculateTime(obj->TimeStrValue[j])) {
                        strcpy(strtmp, "� ������: ");
                        strcat(strtmp, itoa(j+1,strnum, 10));
                        Application->MessageBox(strtmp, "������ ������� �������!", MB_OK);
                }


        while(i <= count) {
                strcpy(obj->TimeValue[j], obj->TimeStrValue[i]);              //�� ��������� ����������� ����� .log ����� �������� ��������� ���������� ��� ������� ������� (� ������ �������� ���������� ���������� �������� �� ����� ������)
                j++;
                if(obj->inrow[j] != true)
                        i++;
        }






 /*  unsigned int k = 0;

        for(unsigned int j=obj->opcount; j<obj->sigcount+obj->opcount; j++) {
                MakeFileForm->RichEdit1->Lines->Add(AnsiString(obj->TimeValue[j], strlen(obj->TimeValue[j])) + AnsiString(obj->SignalStr[k], strlen(obj->SignalStr[k])));
                k++;
        }  */
      /*  for(unsigned int j=obj->opcount; j<obj->sigcount+obj->opcount; j++)
                 MakeFileForm->RichEdit1->Lines->Add(IntToStr(obj->inrow[j]));   */
    /*  for(unsigned int j=0; j<obj->opcount; j++) {
                MakeFileForm->RichEdit1->Lines->Add(AnsiString(obj->ControlStr[j], strlen(obj->ControlStr[j])));
                k++;
        } */
    /*    for(unsigned int i=0; i<5 ; i++)
                MakeFileForm->RichEdit1->Lines->Add(AnsiString(IntToStr(obj->inmass[i])));   */


}







void __fastcall TMakeFileForm::ApplySetsButtClick(TObject *Sender)
{
        strcpy(SystemName, SystemNameEdit->Text.c_str());
        strcpy(ModName, ModNameEdit->Text.c_str());
        ProcessingFile(FileBKI, "BKI");
        ProcessingFile(FileBKS, "BKS");
        ProcessingTime(newfilebki);
        ProcessingTime(newfilebks);
        ProcResultFile(ResultFile);
        Application->MessageBox(ResultFileName.c_str(), "���� ������� ������!", MB_OK);
        MakeFileForm->Close();

        
       /* j = 0;
        while(strcmp(newfilebki->ProtStr[j], "") && newfilebki->ProtStr[j] != NULL) {
                MakeFileForm->RichEdit1->Lines->Add(AnsiString(newfilebki->ProtStr[j], strlen(newfilebki->ProtStr[j])));
                j++;
        }  */
       // MakeFileForm->RichEdit1->Lines->Add(AnsiString(newfilebki->ProtStr[1], strlen(newfilebki->ProtStr[j])));
}


void __fastcall TMakeFileForm::ResultFileButtClick(TObject *Sender)
{
        MakeResultFile();
}




void TMakeFileForm::MakeResultFile()
{
        if(SaveResultFile->Execute())
                ResultFileName = SaveResultFile->FileName;
        ResultFilePath->Text = ResultFileName;
        if((ResultFile = fopen(ResultFileName.c_str(), "wb")) == NULL) {
                Application->MessageBox(ResultFileName.c_str(), "���������� ������� ����!", MB_OK);
                MakeFileForm->ResultFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\ERR.jpg");
                ResultFileFlag = false;
                ChkOpt();
        }
        else {
                MakeFileForm->ResultFileImg->Picture->LoadFromFile(CurrDir+"\\Pics\\OK.jpg");
                ResultFileFlag = true;
                ChkOpt();
        }


}

void TMakeFileForm::ProcResultFile(FILE* filep)
{
        DWORD strcount = 0;                 //������� ����� �� ��������� NewFileData(������� newfilebki, newfilebks), ���������� � .emu ����
        DWORD lastsigtime = 0;              //��������� �������� ���������� �������
        unsigned int ind = 0;            //������ �������� ������� inmass[]
        unsigned int ssbki = newfilebki->opcount; //���������� ����� ������, ��������������� ������� �������, � ������� ����� ��������� ������ (��� ���)
        unsigned int ssbks = newfilebks->opcount; //���������� ����� ������, ��������������� ������� �������, � ������� ����� ��������� ������ (��� ��C)
        char *temp = new char[1000];    //��������� ���������� ��� �������� ������������� ���� �����
        strcat(strcpy(temp, "�������: "), SystemName);
        strcat(temp, "\n");
        fputs(temp, filep);                    //������� fputs() � ������� ��   pust() �� ��������� ������!
        strcat(strcpy(temp, "�����: "), ModName);
        strcat(temp, "\n");
        fputs(temp, filep);                    //���������� � .emu ���� ������������ ������

//--------------������ ����� � .emu ���� ��� ������ ���-----------------------------------------------------------------
        fputs("[���]\n", filep);
        fputs("[������ ������]\n", filep);
        for(unsigned int i=0; i<newfilebks->opcount; i++) {                //������ � .emu ���� �������� ����������� �������� ��������� ��������� ����
                if(strcount == newfilebks->inmass[ind]) {                   //���� ����� ���������� ������ ����� �������� ������� inmass[], ��������� ������ ����� � ��������� �����������
                        strcat(strcpy(temp, "BKS||"), newfilebks->ControlStr[i]);      //�������� � .emu ���� ������ �������� �������
                        strcat(temp, "\n");
                        fputs(temp, filep);
                        strcount++;
                        ind++;
                }
                else {
                        strcat(strcpy(temp, "BKS<<"), newfilebks->ControlStr[i]);      //�������� � .emu ���� ������ �������� ���������
                        strcat(temp, "\r\n");                                          //'\r' ��������� ��� ���������� ������ ��� ������ ���������� ����� .emu �����
                        fputs(temp, filep);
                        strcount++;
                }
        }


        fputs("[/������ ������]\n", filep);
        fputs("[�������]\n", filep);
        for(unsigned int i=0; i<newfilebks->sigcount; i++) {                //������ � .emu ���� ���������� ��������� � ���������� �����������
                strcat(strcpy(temp, "BKS:"), newfilebks->TimeValue[ssbks]);
                strcat(temp, ":");
                strcat(temp, newfilebks->SignalStr[i]);
                strcat(temp, "\r\n");
                fputs(temp,filep);
                ssbks++;
        }
        lastsigtime = atoi(newfilebks->TimeValue[newfilebks->sigcount-1+newfilebks->opcount]);      //�������� ��������� �������� ���������� �������
        strcpy(temp,"BKS:");                    //��������� ������ ������
        itoa(lastsigtime+10, temp+4, 10);       //� ������� ���������� ����������� ��������� ���������� 1�, �������� � ��������� ��� � ���������� �� ��������������� ��������� ���������� ����� ��� ��������������� ������
        strcat(temp, ":?OK>\r\n");
        fputs(temp, filep);
        strcpy(temp, "BKS:");
        itoa(lastsigtime+20, temp+4, 10);
        strcat(temp, ":!RDY>\r\n");
        fputs(temp, filep);
        fputs("[/�������]\n", filep);
        fputs("[��������]\n", filep);             //������ ����� ��������� � .emu ����
        fputs("BKS||;\n", filep);
        for(unsigned int i=0; i<newfilebks->protcount; i++) {
                strcat(strcpy(temp, "BKS<<"), newfilebks->ProtStr[i]);
                strcat(temp, "\r\n");
                fputs(temp, filep);
        }
        fputs("BKS<<!RDY>\r\n", filep);
        fputs("[/��������]\n", filep);
        fputs("[/���]\n", filep);

        ind = 0;
        strcount = 0;
        lastsigtime = 0;

//--------------������ ����� � .emu ���� ��� ������ ���-----------------------------------------------------------------
        fputs("[���]\n", filep);
        fputs("[������ ������]\n", filep);
        for(unsigned int i=0; i<newfilebki->opcount; i++) {                //������ � .emu ���� �������� ����������� �������� ��������� ��������� ����
                if(strcount == newfilebki->inmass[ind]) {                   //���� ����� ���������� ������ ����� �������� ������� inmass[], ��������� ������ ����� � ��������� �����������
                        strcat(strcpy(temp, "BKI||"), newfilebki->ControlStr[i]);      //�������� � .emu ���� ������ �������� �������
                        strcat(temp, "\n");
                        fputs(temp, filep);
                        strcount++;
                        ind++;
                }
                else {
                        strcat(strcpy(temp, "BKI<<"), newfilebki->ControlStr[i]);      //�������� � .emu ���� ������ �������� ���������
                        strcat(temp, "\r\n");
                        fputs(temp, filep);
                        strcount++;
                }
        }

        fputs("[/������ ������]\n", filep);
        fputs("[�������]\n", filep);
        for(unsigned int i=0; i<newfilebki->sigcount; i++) {                //������ � .emu ���� ���������� ��������� � ���������� �����������
                strcat(strcpy(temp, "BKI:"), newfilebki->TimeValue[ssbki]);
                strcat(temp, ":");
                strcat(temp, newfilebki->SignalStr[i]);
                strcat(temp, "\r\n");
                fputs(temp,filep);
                ssbki++;
        }
        lastsigtime = atoi(newfilebki->TimeValue[newfilebki->sigcount-1+newfilebki->opcount]);      //�������� ��������� �������� ���������� �������
        strcpy(temp,"BKI:");                    //��������� ������ ������
        itoa(lastsigtime+10, temp+4, 10);       //� ������� ���������� ����������� ��������� ���������� 1�, �������� � ��������� ��� � ���������� �� ��������������� ��������� ���������� ����� ��� ��������������� ������
        strcat(temp, ":?OK>\r\n");
        fputs(temp, filep);
        strcpy(temp, "BKI:");
        itoa(lastsigtime+20, temp+4, 10);
        strcat(temp, ":!RDY>\r\n");
        fputs(temp, filep);
        fputs("[/�������]\n", filep);
        fputs("[��������]\n", filep);             //������ ����� ��������� � .emu ����
        fputs("BKI||;\n", filep);
        for(unsigned int i=0; i<newfilebki->protcount; i++) {
                strcat(strcpy(temp, "BKI<<"), newfilebki->ProtStr[i]);
                strcat(temp, "\r\n");
                fputs(temp, filep);
        }
        fputs("BKI<<!RDY>\r\n", filep);
        fputs("[/��������]\n", filep);
        fputs("[/���]\n", filep);
//-------------------------------------------------------------------------------------------------------------------------------------------

        fclose(filep);

}

void __fastcall TMakeFileForm::FormClose(TObject *Sender, TCloseAction &Action)
{
        delete newfilebks;
        delete newfilebki;
        delete []SystemName;
        delete []ModName;
        delete []symbols;
}

