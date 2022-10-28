#include "MainFrame.h"


MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _T("PyEdit"), wxDefaultPosition, wxSize(800, 600))
{
	this->CreateMenu();
	this->paneManager.SetManagedWindow(this);

	this->noteBook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_MIDDLE_CLICK_CLOSE);
	this->projectManager = new ProjectManager(this, [&](std::string file) {
		int res = this->isEditorOpened(file);
		if (res == -1)
		{
			Editor* editor = new Editor(this, this->themePath);
			std::string path = FAbsolute(file);
			//editor->SetToolTip(path);
			editor->LoadFile(path);
			if (this->saveAuto)
			{
				editor->SetSaveAuto(true);
			}

			wxBitmapBundle bitbundle = wxBitmapBundle();
			std::string ext = FGetExtension(path);
			if (ext == ".py")
			{
				bitbundle = wxBitmapBundle::FromImage(this->pythonFileImg);
			}else if (ext == ".pyedit")
			{
				bitbundle = wxBitmapBundle::FromImage(this->editFileImg);
			}
			
			this->noteBook->AddPage(editor, FGetName(file, true), true, bitbundle);
			this->paneManager.Update();
		}
		else {
			this->noteBook->SetSelection(res);
			this->paneManager.Update();
		}

		if (FGetExtension(file) == ".pyedit" && !this->warmPyedit)
		{
			this->DiaWarm("Vous devrez recharger le projet si vous modifier ce fichier !");
			this->warmPyedit = true;
		}
	});
	CanvasScene* scene = new CanvasScene(this);
	//Terminal* terminal = new Terminal(this);

	this->paneManager.AddPane(this->projectManager, wxAuiPaneInfo().Name(_("project")).Caption(_("Projets")).Left().Show(false));
	this->paneManager.AddPane(this->noteBook, wxAuiPaneInfo().Name(_("notebook")).CaptionVisible(false).Center().PaneBorder(false));
	this->paneManager.AddPane(scene, wxAuiPaneInfo().Name(_("canvas")).Caption(_("Scene")).Right());
	//this->paneManager.AddPane(terminal, wxAuiPaneInfo().Name(_("terminal")).CaptionVisible(false).Bottom().CloseButton(false));

	this->noteBook->SetFocus();
	this->paneManager.Update();


	this->CreateStatusBar(2);
	this->SetStatusText("PyEdit v1.0", 0);

	wxString pyEditInfoText = _("PyEdit 2022 (c) - JulesG10");
	int widths[2] = { -1, GetTextExtent(pyEditInfoText).GetWidth() };
	this->SetStatusWidths(2, widths);
	this->SetStatusText(pyEditInfoText, 1);

	this->CenterOnScreen();
	this->Maximize();

	wxExecuteEnv* env = new wxExecuteEnv();
	env->cwd = wxGetCwd();
	wxGetEnvMap(&env->env);

	/*	
	std::vector<std::string> key, value;
	for (wxEnvVariableHashMap::iterator it = env->env.begin(); it != env->env.end(); ++it) {
		key.push_back(std::string(it->first));
		value.push_back(std::string(it->second));
	}
	*/

	std::string exePath(wxStandardPaths::Get().GetExecutablePath());
	env->env["PYEDIT_INFO"] = "PyEdit Console - version 1.0 JulesG10";
	env->env["pyedit"] = exePath;
	env->env["Path"] = std::string(env->env["Path"]) + ";" + FGetParent(exePath) + ";";

	this->pyeditEnv = env;
	this->themePath = FGetParent(exePath) + "\\theme.ini";
}
MainFrame::~MainFrame()
{

}


bool MainFrame::Start(std::string arg)
{
	this->Show(true);

	wxIcon winIcon = wxICON(IDI_ICON1);
	if (winIcon.IsOk())
	{
		this->SetIcon(winIcon);
	}

	this->LoadPyImages();
	if (!wxFileExists(this->themePath))
	{
		if (!Editor::CreateThemeFile(this->themePath))
		{
			this->DiaError("La réparation du thème (" + this->themePath + ") à échouer !");
		}
	}

	if (wxFileExists(arg) && FGetExtension(arg) == ".pyedit")
	{
		if (!this->paneManager.GetPane("project").IsShown())
		{
			this->paneManager.GetPane("project").Show(true);
			this->paneManager.Update();
		}

		if (!this->projectManager->LoadProject(FAbsolute(arg)))
		{
			this->DiaError("Projet non valide, le chargement a échouer !" + std::system_category().message(GetLastError()));
		}
		else if(wxFileExists(this->projectManager->main_path)) {
			Editor* editor = new Editor(this, this->themePath);
			
			std::string path = FAbsolute(this->projectManager->main_path);
			//editor->SetToolTip(path);
			editor->LoadFile(path);

			if (this->saveAuto)
			{
				editor->SetSaveAuto(true);
			}

			wxBitmapBundle bitbundle = wxBitmapBundle();
			std::string ext = FGetExtension(path);
			if (ext == ".py")
			{
				bitbundle = wxBitmapBundle::FromImage(this->pythonFileImg);
			}
			else if (ext == ".pyedit")
			{
				bitbundle = wxBitmapBundle::FromImage(this->editFileImg);
			}
			this->noteBook->AddPage(editor, FGetName(this->projectManager->main_path, true), true, bitbundle);
			this->paneManager.Update();
		}
	}

	return true;
}

void MainFrame::LoadPyImages()
{
	wxInitAllImageHandlers();
	//wxImage::AddHandler(new wxICOHandler());

	std::string ico = FGetParent(std::string(wxStandardPaths::Get().GetExecutablePath()));

	std::string icoPy = ico + "\\DLLs\\py.ico";
	std::string icoCy = ico + "\\DLLs\\pyc.ico";
	std::string icoEd = ico + "\\DLLs\\pyd.ico";

	if (wxFileExists(icoPy))
	{
		this->pythonFileImg.LoadFile(icoPy, wxBITMAP_TYPE_ICO, PY_ICO);
		if (this->pythonFileImg.IsOk())
		{
			this->pythonFileImg = this->pythonFileImg.Rescale(20, 20, wxIMAGE_QUALITY_NEAREST);
		}
	}

	if (wxFileExists(icoCy))
	{
		this->editFileImg.LoadFile(icoCy, wxBITMAP_TYPE_ICO, PYC_ICO);
		if (this->editFileImg.IsOk())
		{
			this->editFileImg = this->editFileImg.Rescale(20, 20, wxIMAGE_QUALITY_NEAREST);
		}
	}

	if (wxFileExists(icoEd))
	{
		this->cythonFileImg.LoadFile(icoEd, wxBITMAP_TYPE_ICO, PYD_ICO);
		if (this->cythonFileImg.IsOk())
		{
			this->cythonFileImg = this->cythonFileImg.Rescale(20, 20, wxIMAGE_QUALITY_NEAREST);
		}
	}

	wxImageList* limg = new wxImageList(20, 20);
	limg->Add(this->pythonFileImg);
	limg->Add(this->editFileImg);
	limg->Add(this->cythonFileImg);

	this->projectManager->SetImageList(limg);
}
void MainFrame::CreateMenu()
{
	wxMenuBar* menuBar = new wxMenuBar();
	wxAcceleratorEntry entries[1];
	
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(NEW_F, _("Nouveau fichier\tCtrl+N"), _("Nouveau fichier"));
	fileMenu->Append(NEW_P, _("Nouveau projet\tCtrl+P"), _("Nouveau projet"));
	fileMenu->Append(NEW_W, _("Nouvelle fenêtre\tCtrl+Shift+N"), _("Nouvelle fenêtre"));
	fileMenu->AppendSeparator();
	fileMenu->Append(OPEN_F, _("Ouvrir un fichier\tCtrl+O"), _("Ouvrir un fichier"));
	fileMenu->Append(OPEN_P, _("Ouvrir un projet\tCtrl+Shift+O"), _("Ouvrir un projet/dossier"));
	fileMenu->AppendSeparator();
	fileMenu->Append(SAVE_F, _("Sauvegarder\tCtrl+S"), _("Sauvegarder le fichier/projet ouvert"));
	fileMenu->Append(SAVE_AS, _("Sauvegarder une copie\tCtrl+Shift+S"), _("Sauvegarder une copie du fichier ouvert"));
	fileMenu->AppendCheckItem(SAVE_AUTO, _("Sauvegarde automatique\tCtrl+Alt+s"), _("Sauvegarder automatiquement"));
	fileMenu->AppendSeparator();
	fileMenu->Append(SETTING, _("Thème\tCtrl+T"), _("Ouvrir le fichier de thème"));
	fileMenu->Append(SETTING, _("Recharger le thème\tCtrl+Alt+T"), _("Recharger le thème"));
	fileMenu->Append(CLOSE_F, _("Fermer le fichier courrant\tCtrl-W"), _("Fermer le fichier ouvert"));
	fileMenu->Append(EXIT_APP, _("Quitter PyEdit\tAlt-F4"), _("Quitter PyEdit"));
	menuBar->Append(fileMenu, _("&File"));
	
	
	
	wxMenu* editMenu = new wxMenu();
	editMenu->Append(SEL_ALL, _("Selectionner tout\tCtrl+A"), _("Selectionner tout"));
	editMenu->Append(SEL_L, _("Selectionner la ligne\tCtrl+L"), _("Selectionner la ligne"));
	editMenu->AppendSeparator();
	editMenu->Append(COPY, _("Copier\tCtrl+C"), _("Copier"));
	editMenu->Append(CUT, _("Couper\tCtrl+X"), _("Couper"));
	editMenu->Append(PASTE, _("Coller\tCtrl+V"), _("Coller"));
	menuBar->Append(editMenu, _("&Edition"));


	wxMenu* runMenu = new wxMenu();
	runMenu->Append(START_F, _("Démarer le fichier\tCtrl+1"), _("Démarer le fichier"));
	runMenu->Append(START_TF, _("Tester le fichier\tCtrl+Shift+1"), _("Tester le fichier"));
	runMenu->AppendSeparator();
	runMenu->Append(START_P, _("Démarer le projet\tCtrl+2"), _("Démarer le projet"));
	runMenu->Append(START_TP, _("Tester le projet\tCtrl+Shift+2"), _("Tester le projet"));
	runMenu->Append(RELOAD_P, _("Recharger le projet\tCtrl+Alt+2"), _("Recharger le projet"));
	runMenu->AppendSeparator();
	runMenu->Append(OPEN_TERM, _("Ouvrir la console\tCtrl+3"), _("Ouvrir la console"));
	menuBar->Append(runMenu, _("&Projet"));


	this->SetMenuBar(menuBar);
	wxAcceleratorTable menuAccelerator(1, entries);
	this->SetAcceleratorTable(menuAccelerator);
}


void MainFrame::OnNewProject(wxCommandEvent& event)
{
	wxTextEntryDialog* dialog = new wxTextEntryDialog(this, _("Nom du projet:"), _("Nouveau Projet"));
	if (dialog->ShowModal() == wxID_OK)
	{
		wxString val = dialog->GetValue();
		if (val != wxEmptyString)
		{
			if (!this->projectManager->CreateProject(std::string(val)))
			{
				this->DiaError("La création du projet a échouer ! "+ std::system_category().message(GetLastError()));
			}
			else {
				std::string doc(wxStandardPaths::Get().GetDocumentsDir());
				std::string path = doc + "\\" + std::string(val);
				this->DiaOk("Le projet a bien été créé dans " + path);

				wxMessageDialog* diag = new wxMessageDialog(this, _("Voulez-vous charger le nouveau projet ?"), _("Changer de projet"), wxYES_NO | wxICON_QUESTION);
				if (diag->ShowModal() == wxID_YES)
				{
					if (!this->paneManager.GetPane("project").IsShown())
					{
						this->paneManager.GetPane("project").Show(true);
						this->paneManager.Update();
					}

					if (!this->projectManager->LoadProject(FAbsolute(path + "\\" + std::string(val) + ".pyedit")))
					{
						this->DiaError("Projet non valide, le chargement a échouer !" + std::system_category().message(GetLastError()));
					}
				}
			}
		}
	}
	dialog->Destroy();
}
void MainFrame::OnOpenProject(wxCommandEvent& event)
{
	if (!this->paneManager.GetPane("project").IsShown())
	{
		this->paneManager.GetPane("project").Show(true);
		this->paneManager.Update();
	}


	wxFileDialog* dialog = new wxFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, PROJECT_FILE, wxFD_OPEN);
	if (dialog->ShowModal() == wxID_OK)
	{
		if (!this->projectManager->LoadProject(std::string(dialog->GetPath())))
		{
			this->DiaError("Projet non valide, le chargement a échouer !" + std::system_category().message(GetLastError()));
		}
	}
	dialog->Destroy();
}


void MainFrame::OnCloseFile(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (editor->IsSave())
	{
		this->CheckSettingsReload(editor);
		this->noteBook->DeletePage(this->noteBook->GetSelection());
		return;
	}

	wxMessageDialog* dialog = new wxMessageDialog(this, "Sauvegarder le fichier avant de fermer ?", _("Fichier non sauvegarder"), wxYES_NO);
	if (dialog->ShowModal() == wxID_NO)
	{
		this->noteBook->DeletePage(this->noteBook->GetSelection());
		return;
	}

	if (!editor->SaveFile())
	{
		this->DiaError("La sauvegarde du fichier a échouer");
	}
	else {
		this->CheckSettingsReload(editor);
		this->noteBook->DeletePage(this->noteBook->GetSelection());
	}
}
void MainFrame::OnClosePage(wxAuiNotebookEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (editor->IsSave())
	{
		this->CheckSettingsReload(editor);
		return;
	}

	wxMessageDialog* dialog = new wxMessageDialog(this, "Sauvegarder le fichier avant de fermer ?", _("Fichier non sauvegarder"), wxYES_NO);
	if (dialog->ShowModal() == wxID_NO)
	{
		return;
	}

	if (!editor->SaveFile())
	{
		this->DiaError("La sauvegarde du fichier a échouer");
		event.Veto();
		event.Skip();
	}
	else {
		this->CheckSettingsReload(editor);
	}
}

void MainFrame::OnOpenFile(wxCommandEvent& event)
{


	wxFileDialog* dialog = new wxFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, PYTHON_FILES, wxFD_OPEN);
	if (dialog->ShowModal() == wxID_OK)
	{
		int res = this->isEditorOpened(std::string(dialog->GetPath()));
		if (res == -1)
		{
			Editor* editor = new Editor(this, this->themePath);
			std::string path = FAbsolute(std::string(dialog->GetPath()));
			//editor->SetToolTip(path);
			editor->LoadFile(path);

			wxBitmapBundle bitbundle = wxBitmapBundle();
			std::string ext = FGetExtension(path);
			if (ext == ".py")
			{
				bitbundle = wxBitmapBundle::FromImage(this->pythonFileImg);
			}
			else if (ext == ".pyedit")
			{
				bitbundle = wxBitmapBundle::FromImage(this->editFileImg);
			}
			this->noteBook->AddPage(editor, FGetName(std::string(dialog->GetPath()), true), true, bitbundle);
			this->paneManager.Update();
		}
		else {
			this->noteBook->SetSelection(res);
			this->paneManager.Update();
		}
	}

	dialog->Destroy();
}
void MainFrame::OnNewFile(wxCommandEvent& event)
{
	Editor* editor = new Editor(this, this->themePath);
	editor->SetText(PYTHON_TEMPLATE);
	editor->noTitle(true);

	this->noteBook->AddPage(editor, wxString("[main.py]"), true, wxBitmapBundle::FromImage(this->pythonFileImg));
	if (this->saveAuto)
	{
		editor->SetSaveAuto(true);
	}
}


void MainFrame::OnSaveAs(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	editor->SaveAs();
}
void MainFrame::OnSaveFile(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (!editor->SaveFile())
	{
		this->DiaError("La sauvegarde du fichier a échouer");
	}
}
void MainFrame::OnSaveAuto(wxCommandEvent& event)
{
	this->saveAuto = !this->saveAuto;
	for (int i = 0; i < this->noteBook->GetPageCount(); i++)
	{
		Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
		if (this->saveAuto && !editor->SaveFile())
		{
			this->saveAuto = !this->saveAuto;
			this->DiaError("La sauvegarde automatique à échouer pour le fichier "+std::string(editor->GetFile()));
		}
		else {
			editor->SetSaveAuto(this->saveAuto);
		}
	}
}
int MainFrame::isEditorOpened(std::string path)
{
	for (int i = 0; i < this->noteBook->GetPageCount(); i++)
	{
		Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
		if (editor->GetFile() == path)
		{
			return i;
		}
	}

	return -1;
}


void MainFrame::OnOpenSettings(wxCommandEvent& event)
{
	int res = this->isEditorOpened(this->themePath);
	if (res == -1)
	{
		Editor* editor = new Editor(this, this->themePath);
		editor->LoadFile(this->themePath);

		this->noteBook->AddPage(editor, "[theme.ini]", true, wxBitmapBundle::FromImage(this->editFileImg));
		this->paneManager.Update();
	}
	else {
		this->noteBook->SetSelection(res);
		this->paneManager.Update();
	}
}
void MainFrame::OnReloadSettings(wxCommandEvent& event)
{
	bool isOK = true;
	for (int i = 0; i < this->noteBook->GetPageCount(); i++)
	{
		Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
		isOK = editor->LoadTheme(this->themePath);
	}

	if (!isOK)
	{
		this->DiaError("Une erreur est survenue durant le chargement des thèmes !");
	}
}
void MainFrame::CheckSettingsReload(Editor* editor)
{
	if (std::string(editor->GetFile()) == this->themePath)
	{
		bool isOK = true;
		for (int i = 0; i < this->noteBook->GetPageCount(); i++)
		{
			Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
			isOK = editor->LoadTheme(this->themePath);
		}

		if (!isOK)
		{
			this->DiaError("Une erreur est survenue durant le chargement des thèmes !");
		}
	}
}


wxProcess* MainFrame::CreateConsole(std::string command, wxExecuteEnv* env)
{
	wxProcess* process = new wxProcess();
	if (wxExecute(command, wxEXEC_SHOW_CONSOLE, process, env) != -1)
	{
		return process;
	}
	else {
		return nullptr;
	}
}
void MainFrame::OnOpenTerm(wxCommandEvent& event)
{

	if (this->noteBook->GetPageCount() >= 1)
	{
		std::string file(static_cast<Editor*>(this->noteBook->GetCurrentPage())->GetFile());
		this->pyeditEnv->cwd = FGetParent(file);
	}
	else {
		this->pyeditEnv->cwd = wxGetCwd();
	}
	wxProcess* cmd = this->CreateConsole(std::string(wxGetOSDirectory()) + "\\System32\\cmd.exe /K \"echo PyEdit Console(c) 2022 - JulesG10 && echo: && echo Usage: && echo: && echo Run: pyedit.exe file.py && echo Test: pyedit.exe --debug file.py && echo Current Directory: pyedit --ncwd [--debug] file.py\"",this->pyeditEnv);
}


void MainFrame::OnNewWindow(wxCommandEvent& event)
{
	this->CreateConsole(std::string(wxStandardPaths::Get().GetExecutablePath()), this->pyeditEnv);
}
void MainFrame::OnExit(wxCommandEvent& event)
{
	bool isOk = true;
	for (int i = 0; i < this->noteBook->GetPageCount(); i++)
	{
		Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
		if (!editor->IsSave())
		{
			wxMessageDialog* dialog = new wxMessageDialog(this, "Sauvegarder le fichier " + std::string(editor->GetFile()) + " avant de fermer ?", _("Fichier non sauvegarder"), wxYES_NO);
			if (dialog->ShowModal() == wxID_YES && !editor->SaveFile())
			{
				this->DiaError("La sauvegarde du fichier a échouer");
				isOk = false;
			}
		}
	}

	if (isOk)
	{
		this->Destroy();
	}
}
void MainFrame::OnClose(wxCloseEvent& event)
{
	bool isOk = true;
	for (int i = 0; i < this->noteBook->GetPageCount(); i++)
	{
		Editor* editor = static_cast<Editor*>(this->noteBook->GetPage(i));
		if (!editor->IsSave())
		{
			wxMessageDialog* dialog = new wxMessageDialog(this, "Sauvegarder le fichier " + std::string(editor->GetFile()) + " avant de fermer ?", _("Fichier non sauvegarder"), wxYES_NO);
			if (dialog->ShowModal() == wxID_YES && !editor->SaveFile())
			{
				this->DiaError("La sauvegarde du fichier a échouer");
				isOk = false;
			}
		}
	}

	if (isOk)
	{
		this->Destroy();
	}
}


void MainFrame::OnSelectAll(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	editor->SelectAll();
}
void MainFrame::OnSelectLine(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	editor->SetSelection(editor->GetCurrentLine(), editor->GetLineEndPosition(editor->GetCurrentLine()));
}
void MainFrame::OnCopy(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (editor->GetSelectedText() == wxEmptyString)
	{
		editor->LineCopy();
	}
	else {
		editor->Copy();
	}
}
void MainFrame::OnPaste(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	editor->Paste();
}
void MainFrame::OnCut(wxCommandEvent& event)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (editor->GetSelectedText() == wxEmptyString)
	{
		editor->LineCut();
	}
	else {
		editor->Cut();
	}
}


void MainFrame::StartCurrentFile(std::string param)
{
	if (this->noteBook->GetPageCount() == 0)
	{
		return;
	}

	Editor* editor = static_cast<Editor*>(this->noteBook->GetCurrentPage());
	if (!editor->IsSave())
	{
		wxMessageDialog* diag = new wxMessageDialog(this, "Le fichier n'a pas été sauvegarder, voulez-vous appliquer les modifications ?", "Sauvegarde", wxYES_NO | wxICON_QUESTION);
		if (diag->ShowModal() == wxID_YES)
		{
			if (!editor->SaveFile())
			{
				this->DiaError("La sauvegarde a échouer");
				return;
			}
		}
	}

	if (editor->GetFile() == wxEmptyString)
	{
		this->DiaError("Le fichier n'existe pas sur le lecteur");
		return;
	}
	std::string exePath(wxStandardPaths::Get().GetExecutablePath());

	this->pyeditEnv->cwd = FGetParent(std::string(editor->GetFile()));
	this->CreateConsole(exePath + " " + param + " \"" + std::string(editor->GetFile() + "\""), this->pyeditEnv);
}
void MainFrame::RunFile(wxCommandEvent& event)
{
	this->StartCurrentFile();
}
void MainFrame::TestFile(wxCommandEvent& event)
{
	this->StartCurrentFile("--debug");
}


void MainFrame::RunProject(wxCommandEvent& event)
{
	if (this->paneManager.GetPane("project").IsShown())
	{
		if (!wxFileExists(this->projectManager->main_path))
		{
			this->DiaError("Aucun fichier de démarrage, ajouter en un dans le fichier *.pyedit");
		}
		else {
			std::string exePath(wxStandardPaths::Get().GetExecutablePath());
			this->pyeditEnv->cwd = this->projectManager->path;
			this->CreateConsole(exePath + "  \"" + this->projectManager->main_path + "\"", this->pyeditEnv);
		}
	}
	else {
		this->DiaWarm("Aucun projet n'est ouvert !");
	}
}
void MainFrame::ReloadProject(wxCommandEvent& event)
{
	if (this->paneManager.GetPane("project").IsShown())
	{
		if (!wxFileExists(this->projectManager->pyedit_path))
		{
			this->DiaError("Aucun fichier de projet *.pyedit !");
		}
		else if (!this->projectManager->LoadProject(this->projectManager->pyedit_path))
		{
			this->DiaError("Projet non valide, le chargement a échouer !" + std::system_category().message(GetLastError()));
		}
		else {
			this->DiaOk("Le projet a bien été recharger !");
		}
	}
	else {
		this->DiaWarm("Aucun projet n'est ouvert !");
	}
}
void MainFrame::TestProject(wxCommandEvent& event)
{
	if (this->paneManager.GetPane("project").IsShown())
	{
		if (!wxFileExists(this->projectManager->main_path))
		{
			this->DiaError("Aucun fichier de démarrage, ajouter en un dans le fichier *.pyedit");
		}
		else {
			std::string exePath(wxStandardPaths::Get().GetExecutablePath());
			this->pyeditEnv->cwd = this->projectManager->path;
			this->CreateConsole(exePath + " --debug \"" + this->projectManager->main_path + "\"", this->pyeditEnv);
		}
	}
	else {
		this->DiaWarm("Aucun projet n'est ouvert !");
	}
}


void MainFrame::DiaError(std::string msg)
{
	wxMessageDialog* diag = new wxMessageDialog(this, wxString(msg), wxString("ERREUR"), wxOK | wxICON_ERROR);
	diag->ShowModal();
	diag->Destroy();
}
void MainFrame::DiaOk(std::string msg)
{
	wxMessageDialog* diag = new wxMessageDialog(this, wxString(msg), wxString("INFO"), wxOK | wxICON_INFORMATION);
	diag->ShowModal();
	diag->Destroy();
}
void MainFrame::DiaWarm(std::string msg)
{
	wxMessageDialog* diag = new wxMessageDialog(this, wxString(msg), wxString("ATTENTION"), wxOK | wxICON_EXCLAMATION);
	diag->ShowModal();
	diag->Destroy();
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_CLOSE(MainFrame::OnClose)
	EVT_MENU(EXIT_APP, MainFrame::OnExit)

	EVT_MENU(NEW_F, MainFrame::OnNewFile)
	EVT_MENU(NEW_P, MainFrame::OnNewProject)
	EVT_MENU(NEW_W, MainFrame::OnNewWindow)
	EVT_MENU(OPEN_F, MainFrame::OnOpenFile)
	EVT_MENU(OPEN_P, MainFrame::OnOpenProject)

	EVT_MENU(SAVE_F, MainFrame::OnSaveFile)
	EVT_MENU(SAVE_AS, MainFrame::OnSaveAs)
	EVT_MENU(SAVE_AUTO, MainFrame::OnSaveAuto)

	EVT_MENU(SETTING, MainFrame::OnOpenSettings)
	EVT_MENU(SETTING_RELOAD, MainFrame::OnReloadSettings)

	EVT_MENU(CLOSE_F, MainFrame::OnCloseFile)

	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MainFrame::OnClosePage)

	

	EVT_MENU(SEL_L, MainFrame::OnSelectLine)
	EVT_MENU(SEL_ALL, MainFrame::OnSelectAll)
	EVT_MENU(COPY, MainFrame::OnCopy)
	EVT_MENU(CUT, MainFrame::OnCut)
	EVT_MENU(PASTE, MainFrame::OnPaste)

	EVT_MENU(START_F, MainFrame::RunFile)
	EVT_MENU(START_TF, MainFrame::TestFile)
	EVT_MENU(START_P, MainFrame::RunProject)
	EVT_MENU(START_TP, MainFrame::TestProject)
	EVT_MENU(RELOAD_P, MainFrame::ReloadProject)

	EVT_MENU(OPEN_TERM, MainFrame::OnOpenTerm)

END_EVENT_TABLE()