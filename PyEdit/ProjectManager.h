#pragma once
#include "stdafx.h"

class ProjectManager : public wxTreeCtrl
{
public:
	ProjectManager(wxWindow* parent, std::function<void(std::string)> );
	~ProjectManager();

	bool LoadProject(std::string file);
	bool CreateProject(std::string name);

	std::string name;
	std::string path;
	std::string pyedit_path;
	std::string main_path;
	bool active = false;
private:
	void CreateFileTree(std::string path, wxTreeItemId root = NULL); 
	void OnItemClick(wxTreeEvent& event);

	std::function<void(std::string)> OpenFile;

	std::map<wxTreeItemId, std::string> files = {};
	DECLARE_EVENT_TABLE();
};

