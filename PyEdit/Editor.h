#pragma once

#include "stdafx.h"

#define PYTHON_TEMPLATE "import sys\n\ndef main(args):\n\tprint(args)\n\treturn 0\n\nif __name__ == \"__main__\":\n\tsys.exit(main(sys.argv[1:]))"
#define PYTHON_FILES "Fichier Python (*.py, *.pyc, *.pyo, *.pyw, *.pyx, *.pyd, *.pxd, *.pxi, *.pyi, *.pyz, *.pywz, *.pth)|*.py;*.pyc;*.pyo;*.pyw;*.pyx;*.pyd;*.pxd;*.pxi;*.pyi;*.pyz;*.pywz;*.pth"
#define PROJECT_FILE "Fichier PyEdit (*.pyedit)|*.pyedit"

#define wxFOLD_ID 1
#define wxNUM_ID 2

class Editor : public wxStyledTextCtrl
{
public:
	Editor(wxWindow* window);
	~Editor();

	void OnCharAdded(wxStyledTextEvent& e);
	void OnChange(wxStyledTextEvent& e);
	void OnMarginClick(wxStyledTextEvent& e);

	bool LoadTheme(wxString themePath);
	void LoadFile(const wxString& file);
	
	bool SaveFile();
	bool IsSave();
	void SetSaveAuto(bool save);
	void noTitle(bool notitle);
	bool SaveAs();

	wxString GetFile();
private:
	wxString file = wxEmptyString;
	bool notitle = false;
	bool saveAuto = false;
	bool isSave = false;

	void SetupFold(wxColor fg, wxColor bg);
	void SetupLineNum(wxColor fg, wxColor bg);
	void SetupLexer();

	DECLARE_EVENT_TABLE()
};

