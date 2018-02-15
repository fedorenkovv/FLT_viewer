#include "ObjectsTreeCtrl.h"
#include <string>

wxBEGIN_EVENT_TABLE(ObjectsTreeCtrl, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(ObjectsTree_Ctrl, ObjectsTreeCtrl::OnTreeSelChanged)
wxEND_EVENT_TABLE()

ObjectsTreeCtrl::ObjectsTreeCtrl()
{
}


ObjectsTreeCtrl::~ObjectsTreeCtrl()
{
}


void ObjectsTreeCtrl::OnTreeSelChanged(wxTreeEvent& event)
{
	// show some info about this item
	

	wxTreeItemId itemId = event.GetItem();
	ObjectData *data = dynamic_cast<ObjectData*>(GetItemData(itemId));

	if (data != NULL)
	{
		selectFolderToShow = data->node;
	}
	else
	{
		selectFolderToShow = "";
	}

}