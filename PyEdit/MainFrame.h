#pragma once
#include "stdafx.h"

#include "Editor.h"
#include "Terminal.h"
#include "ProjectManager.h"
#include "CanvasScene.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();

    bool Start(std::string arg);
private:
    std::string themePath;
    wxExecuteEnv* pyeditEnv;

    bool warmPyedit = false;
    bool saveAuto = false;
    
    wxAuiManager paneManager;
    wxAuiNotebook* noteBook;
    ProjectManager* projectManager;



    wxImage pythonFileImg;
    wxImage cythonFileImg;
    wxImage editFileImg;

    void OnExit(wxCommandEvent& event);
    void OnClose(wxCloseEvent&);

    void LoadPyImages();
    void CreateMenu();
    int isEditorOpened(std::string path);
 
    void OnNewProject(wxCommandEvent& event);
    void OnOpenProject(wxCommandEvent& event);

    void OnOpenFile(wxCommandEvent& event);
    void OnNewFile(wxCommandEvent& event);

    void OnSaveAs(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);

    void OnCloseFile(wxCommandEvent& event);
    void OnClosePage(wxAuiNotebookEvent& event);

    void OnSaveAuto(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);

    void OnOpenSettings(wxCommandEvent& event);
    void OnReloadSettings(wxCommandEvent& event);
    void CheckSettingsReload(Editor*);

    wxProcess* CreateConsole(std::string command, wxExecuteEnv* env);
    void StartCurrentFile(std::string param = "");
    void OnOpenTerm(wxCommandEvent& event);
    
    void OnSelectAll(wxCommandEvent& event);
    void OnSelectLine(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);

    void RunFile(wxCommandEvent& event);
    void TestFile(wxCommandEvent& event);
    
    void RunProject(wxCommandEvent& event);
    void TestProject(wxCommandEvent& event);
    void ReloadProject(wxCommandEvent& event);
    
    
    void DiaError(std::string);
    void DiaOk(std::string);
    void DiaWarm(std::string);

    DECLARE_EVENT_TABLE()
};

enum MainMenuID {
    NEW_F = 0x100,
    NEW_P,
    NEW_W,
    OPEN_F,
    OPEN_P,
    SAVE_F,
    SAVE_AS,
    SAVE_AUTO,
    SETTING,
    SETTING_RELOAD,
    CLOSE_F,
    EXIT_APP,
    SEL_L,
    SEL_ALL,
    COPY,
    CUT,
    PASTE,
    START_F,
    START_TF,
    START_P,
    START_TP,
    RELOAD_P,
    OPEN_TERM,
};

