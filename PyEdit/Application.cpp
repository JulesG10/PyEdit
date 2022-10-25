#include "Application.h"
#include "Console.h"

bool Application::OnInit()
{
    std::string arg(argc > 1 ? argv[1] : "");

    MainFrame* frame = new MainFrame();
    return frame->Start(arg);
}

/*
IMPLEMENT_APP(Application)
*/

IMPLEMENT_APP_NO_MAIN(Application);
IMPLEMENT_WX_THEME_SUPPORT;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(std::wstring(pCmdLine).c_str(), &argc);
    
    wchar_t* exePath = new wchar_t[MAX_PATH];
    GetModuleFileNameW(0, exePath, MAX_PATH);
    std::wstring wexe(exePath);
    delete exePath;

    if (argc >= 1)
    {
        std::wstring warg0(argv[0]);
        if (warg0 != wexe &&  FGetExtension(warg0) != ".pyedit")
        {
            Console app = Console();
            return app.Run(argc, argv);
        }
    }

    wxEntryStart(hInstance, hPrevInstance, wxCmdLineArgType(pCmdLine), nCmdShow);
    wxTheApp->CallOnInit();
    return wxTheApp->OnRun();
}