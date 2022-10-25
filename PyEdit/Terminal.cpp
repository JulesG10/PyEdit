#include "Terminal.h"

Terminal::Terminal(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(600,600))
{
	wxBoxSizer* vbox = new wxBoxSizer(wxHORIZONTAL);

	wxNotebook* noteBook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(500, 500));
	
	wxPanel* terminal = new wxPanel(noteBook, wxID_ANY);
	wxPanel* error = new wxPanel(noteBook, wxID_ANY);
	noteBook->AddPage(terminal,"Variables", true);
	noteBook->AddPage(error, "Erreurs", false);
	
	vbox->Add(noteBook, wxEXPAND | wxALL, 10);
	
	this->SetSizer(vbox);
	Centre();
}

Terminal::~Terminal()
{
}
