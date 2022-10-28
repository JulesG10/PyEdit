#pragma once

#include "stdafx.h"

#define PYTHON_TEMPLATE "import sys\n\ndef main(args):\n\tprint(args)\n\treturn 0\n\nif __name__ == \"__main__\":\n\tsys.exit(main(sys.argv[1:]))"
#define PYTHON_FILES "Fichier Python (*.py, *.pyc, *.pyo, *.pyw, *.pyx, *.pyd, *.pxd, *.pxi, *.pyi, *.pyz, *.pywz, *.pth)|*.py;*.pyc;*.pyo;*.pyw;*.pyx;*.pyd;*.pxd;*.pxi;*.pyi;*.pyz;*.pywz;*.pth"
#define PROJECT_FILE "Fichier PyEdit (*.pyedit)|*.pyedit"

#define wxFOLD_ID 1
#define wxNUM_ID 2

static const std::map<std::string, int> themeNames = {

		{"foreground-default", wxSTC_P_DEFAULT},
		{"foreground-comment", wxSTC_P_COMMENTLINE},
		{"foreground-number", wxSTC_P_NUMBER},
		{"foreground-string", wxSTC_P_STRING},
		{"foreground-char", wxSTC_P_CHARACTER},
		{"foreground-word", wxSTC_P_WORD},
		{"foreground-triple", wxSTC_P_TRIPLE},
		{"foreground-tripledb", wxSTC_P_TRIPLEDOUBLE},
		{"foreground-classname", wxSTC_P_CLASSNAME},
		{"foreground-defname", wxSTC_P_DEFNAME},
		{"foreground-operator", wxSTC_P_OPERATOR},
		{"foreground-identifier", wxSTC_P_IDENTIFIER},
		{"foreground-blockcomment", wxSTC_P_COMMENTBLOCK},
		{"foreground-stringeol", wxSTC_P_STRINGEOL},
		{"foreground-word2", wxSTC_P_WORD2},
		{"foreground-decorator", wxSTC_P_DECORATOR},

		{"background-default", wxSTC_P_DEFAULT},
		{"background-comment", wxSTC_P_COMMENTLINE},
		{"background-number", wxSTC_P_NUMBER},
		{"background-string", wxSTC_P_STRING},
		{"background-char", wxSTC_P_CHARACTER},
		{"background-word", wxSTC_P_WORD},
		{"background-triple", wxSTC_P_TRIPLE},
		{"background-tripledb", wxSTC_P_TRIPLEDOUBLE},
		{"background-classname", wxSTC_P_CLASSNAME},
		{"background-defname", wxSTC_P_DEFNAME},
		{"background-operator", wxSTC_P_OPERATOR},
		{"background-identifier", wxSTC_P_IDENTIFIER},
		{"background-blockcomment", wxSTC_P_COMMENTBLOCK},
		{"background-stringeol", wxSTC_P_STRINGEOL},
		{"background-word2", wxSTC_P_WORD2},
		{"background-decorator", wxSTC_P_DECORATOR},

		{"foreground-bracelight",wxSTC_STYLE_BRACELIGHT},
		{"background-bracelight",wxSTC_STYLE_BRACELIGHT},
		{"foreground-bracebad",wxSTC_STYLE_BRACEBAD},
		{"background-bracebad",wxSTC_STYLE_BRACEBAD},


		{"background-indentguide",wxSTC_STYLE_INDENTGUIDE},
		{"background-global", wxSTC_STYLE_DEFAULT}
};

class Editor : public wxStyledTextCtrl
{
public:
	Editor(wxWindow* window, std::string theme);
	~Editor();

	void OnCharAdded(wxStyledTextEvent& e);
	void OnChange(wxStyledTextEvent& e);
	void OnMarginClick(wxStyledTextEvent& e);

	bool LoadTheme(std::string themePath);
	void LoadFile(const wxString& file);
	
	bool SaveFile();
	bool IsSave();
	void SetSaveAuto(bool save);
	void noTitle(bool notitle);
	bool SaveAs();

	wxString GetFile();
	static bool CreateThemeFile(std::string path);
private:
	bool compChange = true;
	std::vector<std::string> completionData = {};
	std::string comp;

	wxString file = wxEmptyString;
	bool notitle = false;
	bool isSave = false;
	bool saveAuto = false;

	void SetupFold(wxColor fg, wxColor bg);
	void SetupLineNum(wxColor fg, wxColor bg);
	void SetupLexer();

	

	DECLARE_EVENT_TABLE()
};

