#include "Editor.h"

//https://wiki.wxwidgets.org/WxStyledTextCtrl
//https://proton-ce.sourceforge.net/rc/scintilla/pyframe/www.pyframe.com/stc/index-2.html
Editor::Editor(wxWindow* window) : wxStyledTextCtrl(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
{
	this->SetupLexer();
    this->LoadTheme(wxString(""));
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

bool Editor::LoadTheme(wxString themePath)
{
    this->SetupFold(wxColor(255, 255, 255), wxColor(150, 150, 150));
    this->SetupLineNum(wxColour(75, 75, 75), wxColour(220, 220, 220));


    StyleSetBackground(wxSTC_STYLE_INDENTGUIDE, wxColor(200, 200, 200));
    SetIndentationGuides(wxSTC_IV_REAL);
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);
    SetIndent(4);

    SetUseTabs(true);
    SetTabWidth(4);
    
    SetWrapMode(wxSTC_WRAP_WORD);

    SetCaretLineVisible(true);
    SetCaretLineBackground(wxColor(250, 250, 255));
    
    
    wxFont monospace(11, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    StyleSetFont(wxSTC_STYLE_DEFAULT, monospace);
    StyleSetFont(wxSTC_C_WORD, monospace);


    StyleClearAll();


    // python styles
    /*
 wxSTC_P_DEFAULT 0
 wxSTC_P_COMMENTLINE 1
 wxSTC_P_NUMBER 2
 wxSTC_P_STRING 3
 wxSTC_P_CHARACTER 4
 wxSTC_P_WORD 5
 wxSTC_P_TRIPLE 6
 wxSTC_P_TRIPLEDOUBLE 7
 wxSTC_P_CLASSNAME 8
 wxSTC_P_DEFNAME 9
 wxSTC_P_OPERATOR 10
 wxSTC_P_IDENTIFIER 11
 wxSTC_P_COMMENTBLOCK 12
 wxSTC_P_STRINGEOL 13
 wxSTC_P_WORD2 14
 wxSTC_P_DECORATOR 15
    */

    /*
    
    [monokai]
normal-foreground= #F8F8F2
normal-background= #272822

keyword-foreground= #F92672
keyword-background= #272822

builtin-foreground= #66D9EF
builtin-background= #272822

comment-foreground= #75715E
comment-background= #272822

string-foreground= #FD971F
string-background= #272822

definition-foreground= #A6E22E
definition-background= #272822

hilite-foreground= #F8F8F2
hilite-background= gray

break-foreground= black
break-background= #ffff55

hit-foreground= #F8F8F2
hit-background= #171812

error-foreground= #ff3338
error-background= #272822

cursor-foreground= #F8F8F2

stdout-foreground= #DDDDDD
stdout-background= #272822

stderr-foreground= #ff3338
stderr-background= #272822

console-foreground= #75715E
console-background= #272822
    */

     /*
    StyleSetForeground(wxSTC_C_GLOBALCLASS, wxColor(_("#ef0000")));
    StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);
    StyleSetBold(wxSTC_C_COMMENTDOCKEYWORDERROR, true);
    StyleSetItalic(wxSTC_C_WORD2, true);
    StyleSetUnderline(wxSTC_C_COMMENTDOCKEYWORDERROR, true);
    */

    StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColor(230, 230, 230));
    StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColor(0, 100, 100));
    StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColor(200, 0, 0));

    return false;
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

	const wxString pyKeywords = "False await else import pass None break except in raise True class finally is return and continue for lambda try as def from nonlocal while assert del global not with async elif if or yield";
	this->SetKeyWords(0, pyKeywords);
}




BEGIN_EVENT_TABLE(Editor, wxStyledTextCtrl)
    EVT_STC_CHARADDED(wxID_ANY, Editor::OnCharAdded)
    EVT_STC_CHANGE(wxID_ANY, Editor::OnChange)
    EVT_STC_MARGINCLICK(wxID_ANY, Editor::OnMarginClick)
    
    //EVT_STC_AUTOCOMP_COMPLETED(wxID_ANY, Editor::OnAutoComplete)
END_EVENT_TABLE()