//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("ProjectEmu1.res");
USEFORM("Emu1.cpp", Form1);
USEFORM("MakeDeviceFile.cpp", MakeFileForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "System Emulator";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
