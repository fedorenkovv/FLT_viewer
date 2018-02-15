#pragma once
#include <wx\treectrl.h>

static const int ObjectsTree_Ctrl = 1000;

class ObjectData : public wxTreeItemData
{
public:
	std::string node;
};

class ObjectsTreeCtrl :	public wxTreeCtrl
{
public:
	ObjectsTreeCtrl();
	ObjectsTreeCtrl(wxWindow *parent, wxWindowID id) : wxTreeCtrl(parent, id){}
	
	~ObjectsTreeCtrl();

	std::string selectFolderToShow;

	void OnTreeSelChanged(wxTreeEvent& event);

	wxDECLARE_EVENT_TABLE();
};

