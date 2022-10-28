#include "Editor.h"

// https://wiki.wxwidgets.org/WxStyledTextCtrl
// https://docs.wxwidgets.org/trunk/classwx_styled_text_ctrl.html#Words
// https://proton-ce.sourceforge.net/rc/scintilla/pyframe/www.pyframe.com/stc/index-2.html
Editor::Editor(wxWindow* window, std::string theme) : wxStyledTextCtrl(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
{
	this->SetupLexer();
    this->LoadTheme(theme);
}

Editor::~Editor()
{

}


void Editor::OnChange(wxStyledTextEvent& e)
{
    this->isSave = false;
    if (this->saveAuto)
    {
        this->SaveFile();
    }

    int currentPos = this->GetCurrentPos();
    int wordStartPos = this->WordStartPosition(currentPos, true);

    int lenEntered = currentPos - wordStartPos;

    //this->BraceHighlight(wordStartPos, currentPos);
    if (lenEntered > 0)
    {
        if (this->compChange)
        {
            std::sort(this->completionData.begin(), this->completionData.end());
            this->comp = "";
            std::for_each(this->completionData.begin(), this->completionData.end(), [&](std::string s) { this->comp += s + " ";  });
            this->compChange = false;
        }
        this->AutoCompShow(lenEntered, this->comp);
    }
}

void Editor::OnMarginClick(wxStyledTextEvent& e)
{
    if (e.GetMargin() == wxFOLD_ID)
    {
        int lineClick = this->LineFromPosition(e.GetPosition());
        int levelClick = this->GetFoldLevel(lineClick);

        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
        {
            this->ToggleFold(lineClick);
        }
    }
}

void Editor::OnCharAdded(wxStyledTextEvent& e)
{
    switch (e.GetKey())
    {
    case '"':
        this->InsertText(this->GetCurrentPos(), "\"");
        break;
    case '\'':
        this->InsertText(this->GetCurrentPos(), "'");
        break;
    case '(':
        this->InsertText(this->GetCurrentPos(), ")");
        break;
    case '[':
        this->InsertText(this->GetCurrentPos(), "]");
        break;
    }
    
    // Expand current parent + line if is fold
    //this->SetFoldExpanded(this->GetCurrentLine(), true);
    
    
    // Highlight current char
    //this->BraceHighlight(this->GetCurrentPos()-1, this->GetCurrentPos());
}

bool Editor::CreateThemeFile(std::string path)
{
    std::ofstream ofs(path);
    if (!ofs.good())
    {
        return false;
    }

    for (auto i = themeNames.begin(); i != themeNames.end(); ++i)
    {
        ofs << i->first << "=" << "#FFFFFF" << std::endl;
    }
    ofs << "fold=#FFFFFF;#FFFFFF" << std::endl;
    ofs << "linenum=#FFFFFF;#FFFFFF" << std::endl;
    ofs << "caret=#none;#FFFFFF" << std::endl;
    ofs << "caretline=#none;#FFFFFF" << std::endl;

    ofs.close();

}

bool Editor::LoadTheme(std::string themePath)
{
    SetIndentationGuides(wxSTC_IV_REAL);
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);
    SetIndent(4);

    SetUseTabs(true);
    SetTabWidth(4);

    SetWrapMode(wxSTC_WRAP_WORD);

    SetCaretLineVisible(true);
    SetCaretWidth(2);
    

    wxFont font = wxFont(11, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    StyleSetFont(wxSTC_C_WORD, font);

    
    StyleClearAll();
    std::ifstream ifs(themePath);
    if (!ifs.good())
    {
        ifs.close();
        return false;
    }

   

    std::string line;
    while (std::getline(ifs, line))
    {
        auto splitLine = split(line, "=");
        if (splitLine.size() == 2)
        {
            std::string name = splitLine[0];

            auto it = themeNames.find(name);
            auto type = split(name, "-");
            auto colors = split(splitLine[1], ";");

            if (colors.size() > 0)
            {
               
                if (type.size() == 2 && it != themeNames.end() )
                {
                    std::string bf = type[0];

                    if (bf == "foreground")
                    {
                        StyleSetForeground(it->second, GetColor(colors[0]));
                    }
                    else if (bf == "background")
                    {
                        StyleSetBackground(it->second, GetColor(colors[0]));
                    }
                }
                else if(type.size() == 1)
                {
                    if (type[0] == "fold" && colors.size() > 1)
                    {
                        this->SetupFold(GetColor(colors[0]), GetColor(colors[1]));
                    }
                    else if (type[0] == "linenum" && colors.size() > 1)
                    {
                        this->SetupLineNum(GetColor(colors[0]), GetColor(colors[1]));
                    }
                    else if (type[0] == "caretline")
                    {
                        wxColour color = GetColor(colors[0]);
                        SetCaretLineBackground(color);
                        SetCaretLineBackAlpha(color.Alpha());
                    }
                    else if (type[0] == "caret")
                    {
                        SetCaretForeground(GetColor(colors[0]));
                    }
                    else if (type[0] == "selection" && colors.size()>1)
                    {
                        if (colors[0] != "#none")
                        {
                            SetSelForeground(false, GetColor(colors[0]));
                        }

                        if (colors[1] != "#none")
                        {
                            SetSelBackground(false, GetColor(colors[1]));
                        }

                    }
                }
            }
        }
    }
    ifs.close();
    this->Refresh();
    //StyleSetUnderline(wxSTC_ERR_PYTHON, true);
    return true;
}

void Editor::LoadFile(const wxString& file)
{
    wxFileInputStream fileStream(file);
    wxTextInputStream textStreal(fileStream, _("\x09"), wxConvUTF8);

    while (fileStream.IsOk() && !fileStream.Eof())
    {
        wxString line = textStreal.ReadLine();
        line.Replace("\r", "");
        SetText(GetText() + line + (fileStream.Eof() ? "" : "\n"));
    }

    this->file = file;
    this->isSave = true;
}

bool Editor::SaveFile()
{
    if (this->notitle)
    {
        if (!this->SaveAs())
        {
            return false;
        }
    }

    wxFileOutputStream fileStream(this->file);
    if (!fileStream.IsOk())
    {
        return false;
    }
    wxTextOutputStream textStream(fileStream, wxEOL_NATIVE, wxConvUTF8);
    for (int i = 0; i < GetNumberOfLines(); i++)
    {
        wxString line = GetLine(i);

        line.Replace("\r", "");
        line.Replace("\n", "");

        textStream << line << ((i == GetNumberOfLines() - 1) ? "" : "\n");
    }

    textStream.Flush();
    fileStream.Close();

    this->SetSavePoint();
    this->isSave = true;

    return true;
}

bool Editor::IsSave()
{
    return this->isSave;
}

void Editor::SetSaveAuto(bool save)
{
    this->saveAuto = save;
    if (save)
    {
        this->SaveFile();
    }
}

void Editor::noTitle(bool notitle)
{
    this->notitle = notitle;
}

bool Editor::SaveAs()
{
    wxFileDialog* dialog = new wxFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, "main.py", PYTHON_FILES, wxFD_SAVE);
    if (dialog->ShowModal() == wxID_OK)
    {
        this->file = dialog->GetPath();
    }
    else {
        dialog->Destroy();
        return false;
    }

    dialog->Destroy();
    return true;
}

wxString Editor::GetFile()
{
    return this->file;
}



void Editor::SetupFold(wxColor fg, wxColor bg)
{
    SetMarginType(wxFOLD_ID, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(wxFOLD_ID, 20);
    SetMarginMask(wxFOLD_ID, wxSTC_MASK_FOLDERS);
    StyleSetBackground(wxFOLD_ID, bg);
    SetMarginSensitive(wxFOLD_ID, true);
    

    MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
    MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED);
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED);
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE);
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER);
    
    const int stc_marknum_fold[] = {
        wxSTC_MARKNUM_FOLDEREND,
        wxSTC_MARKNUM_FOLDEROPENMID,
        wxSTC_MARKNUM_FOLDERMIDTAIL,
        wxSTC_MARKNUM_FOLDERTAIL,
        wxSTC_MARKNUM_FOLDERSUB,
        wxSTC_MARKNUM_FOLDER,
        wxSTC_MARKNUM_FOLDEROPEN,
    };

    for (int marker : stc_marknum_fold)
    {
        MarkerSetForeground(marker, fg);
        MarkerSetBackground(marker, bg);
    }

    SetProperty("fold", "1");
    SetProperty("fold.comment", "1");
    SetProperty("fold.compact", "1");
    SetProperty("tab.timmy.whinge.level", "3");
    SetProperty("fold.comment.python", "2");
    SetProperty("fold.quotes.python", "2");
}

void Editor::SetupLineNum(wxColor fg, wxColor bg)
{
    SetMarginType(wxNUM_ID, wxSTC_MARGIN_NUMBER);
    SetMarginWidth(wxNUM_ID, 50);
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, fg);
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, bg);
}

void Editor::SetupLexer()
{
    this->SetCodePage(wxSTC_CP_UTF8);
    this->SetLexer(wxSTC_LEX_PYTHON);
    this->SetLexerLanguage("python");

    this->completionData = {
    "False",
    "await",
    "else",
    "import",
    "pass",
    "None",
    "break",
    "except",
    "in",
    "raise",
    "True",
    "class",
    "finally",
    "is",
    "return",
    "and",
    "continue",
    "for",
    "lambda",
    "try",
    "as",
    "def",
    "from",
    "nonlocal",
    "while",
    "assert",
    "del",
    "global",
    "not",
    "with",
    "async",
    "elif",
    "if",
    "or",
    "yield",
    };
    std::sort(this->completionData.begin(), this->completionData.end());

    std::string keywords = "";
    std::for_each(this->completionData.begin(), this->completionData.end(), [&](std::string s) { keywords += s + " "; });
    this->SetKeyWords(0, keywords);
}

BEGIN_EVENT_TABLE(Editor, wxStyledTextCtrl)
    EVT_STC_CHARADDED(wxID_ANY, Editor::OnCharAdded)
    EVT_STC_CHANGE(wxID_ANY, Editor::OnChange)
    EVT_STC_MARGINCLICK(wxID_ANY, Editor::OnMarginClick)
END_EVENT_TABLE()