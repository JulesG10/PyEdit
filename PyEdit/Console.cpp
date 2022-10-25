#include "Console.h"

Console::Console()
{
}

Console::~Console()
{
}

int Console::Run(INT argc, LPWSTR* argv)
{
    if (!this->CreateConsole())
    {
        return 1;
    }
    SetConsoleTitleA("PyEdit");

    for (size_t i = 0; i < argc; i++)
    {
        std::wstring warg = std::wstring(argv[i]);
        std::string arg(warg.begin(), warg.end());

      
        if (wxFileExists(arg) && PathFindExtensionA(arg.c_str()) != ".py")
        {
            DWORD fsize = 0;
            LPSTR file;

            LPSTR absolutePath = new CHAR[MAX_PATH];
            if (!GetFullPathNameA(arg.c_str(), MAX_PATH, absolutePath, &file))
            {
                this->Log("Erreur, " + std::system_category().message(GetLastError()), false);
                this->ExitMessage();
                return 1;
            }
            arg = std::string(absolutePath);

            if (std::string(wxGetCwd()) != FGetParent(arg) && !this->nocwd)
            {
                if (!SetCurrentDirectoryA(FGetParent(arg).c_str()))
                {
                    this->Log("Erreur, change du dossier courrant impossible", false);
                    this->ExitMessage();
                    return 1;
                }
                else {
                    this->Log("Chagement du dossier courrant vers " + FGetParent(arg), true);
                }
            }
            
            //this->Log("Démarage dans le thread: " + std::string(std::this_thread::get_id());

            py::scoped_interpreter guard{};
            this->Log("Initialisation de l'interpreter", true);

            try {
                py::eval_file(arg);
            }
            catch (py::error_already_set& err)
            {
                this->Log(err.what(), false);
            }
            
            /*catch (py::error_already_set& pyerr)
            {
                pyerr.discard_as_unraisable(__func__);
            }
            catch (std::exception& err)
            {
                std::cout << err.what() << std::endl;
            }
            */

            this->Log("Fermeture de l'interpreter", true);
            break;
        }
        else if (arg == "--ncwd" || arg == "-ncwd")
        {
            this->nocwd = true;
            this->Log("Changement du dossier courrant desactiver.", true);
        }
        else if(arg == "--debug" || arg == "-d") {
            this->debug = true;
            this->Log("Activation des logs", true);
        }
        else {
            this->Log("Fichier introuvable " + arg + " !", false);
        }
    }
    this->ExitMessage();
    return 0;
}

BOOL Console::CreateConsole()
{
    if (!AllocConsole())
    {
        return FALSE;
    }

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();


    HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hConOut == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hConIn == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetStdHandle(STD_ERROR_HANDLE, hConOut);
    SetStdHandle(STD_INPUT_HANDLE, hConIn);

    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();
    
    return TRUE;
}

void Console::ExitMessage()
{
    if (this->debug)
    {
        std::cout << "[+] Appuyer sur n'importe quelle touche pour quitter...";
        std::cin.get();
    }
}

void Console::Log(std::string msg, bool sign)
{
    if (this->debug)
    {
        std::cout << "[" << (sign ? "+" : "-") << "] " << msg << std::endl;
    }
}
