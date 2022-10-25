#include "ProjectManager.h"

ProjectManager::ProjectManager(wxWindow* parent, std::function<void(std::string)> OnOpenFile) : wxTreeCtrl(parent)
{
    this->OpenFile = OnOpenFile;
}

ProjectManager::~ProjectManager()
{
}

void ProjectManager::CreateFileTree(std::string path, wxTreeItemId root)
{
    if (root == NULL)
    {
        root = this->AddRoot(FGetName(path));
    }
    std::vector<std::string> files = FListDir(path);
    for (std::string file : files)
    {
        if (GetFileAttributesA(file.c_str()) & FILE_ATTRIBUTE_DIRECTORY)
        {
            wxTreeItemId childRoot = this->AppendItem(root, FGetName(file));
            this->CreateFileTree(file, childRoot);
        }
        else {
            wxTreeItemId id = this->AppendItem(root, FGetName(file, true));
            this->files[id] = file;
        }
    }
}

bool ProjectManager::LoadProject(std::string file)
{
    std::string ext = PathFindExtensionA(file.c_str());
    if (ext != ".pyedit")
    {
        this->active = false;
        return false;
    }

    std::ifstream proj(file);
    if (!proj.good())
    {
        return false;
    }

    std::string line;
    while (std::getline(proj, line))
    {
        if (wxFileExists(line))
        {
            this->main_path = line;
            break;
        }
    }


    this->DeleteAllItems();
    this->CreateFileTree(FGetParent(file));
    this->ExpandAll();

    this->name = FGetName(file);
    this->pyedit_path = file;
    this->path = FGetParent(file);
    this->active = true;

    return true;
}

bool ProjectManager::CreateProject(std::string name)
{
    std::string doc(wxStandardPaths::Get().GetDocumentsDir());
    std::string dirpath = doc + "\\" + name;
    std::string projfile_path = doc + "\\" + name + "\\" + name + ".pyedit";
    std::string mainpath = doc + "\\" + name + "\\main.py";

    if (!CreateDirectoryA(dirpath.c_str(), nullptr))
    {
        return false;
    }

    HANDLE hpyedit = CreateFileA(projfile_path.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hpyedit == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD  written = 0;
    if (!WriteFile(hpyedit, (LPCVOID)(mainpath.c_str()),(DWORD)mainpath.size(), &written, NULL))
    {
        return false;
    }
    
    if (written != mainpath.size())
    {
        return false;
    }

    if (!CloseHandle(hpyedit))
    {
        return false;
    }

    if (CreateFileA(mainpath.c_str(), NULL, NULL, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    return true;
}

void ProjectManager::OnItemClick(wxTreeEvent& evt)
{
    wxTreeItemId id = evt.GetItem();
    std::string file = this->files[id];

    if (this->OpenFile != nullptr)
    {
        this->OpenFile(file);
    }
}

BEGIN_EVENT_TABLE(ProjectManager, wxTreeCtrl)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, ProjectManager::OnItemClick)
END_EVENT_TABLE()