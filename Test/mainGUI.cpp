// tstImgEnhancer.cpp : Defines the entry point for the console application.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>

#include "ObjectsTreeCtrl.h"

#include "boost/filesystem.hpp" 
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

#include "mgapiall.h"

using namespace std;

const int windowWIDTH = 480;
const int windowHEIGHT = 640;
const int BORDER = 10;
const int ButtonW = 80;
const int ButtonH = 40;
const int ImageSizeW = 16;
const int ImageSizeH = 16;

int count_files(std::string directory);


class MyApp : public wxApp
{
public:
	MyApp(){};
	virtual bool OnInit();
	virtual int OnExit();
};

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	void databaseButtonClick(wxCommandEvent& event);
	void readVertexes(mgrec* parent);
	void getChildTree(mgrec* item, wxTreeItemId parentNode);
	void getVertexes(string object);

	void OnTreeItemActivated(wxTreeEvent& event);
	
	void checkBoxClick(wxCommandEvent& event);

	static const long databaseButtonID;
	wxButton *databaseButton;

	wxStaticText* infoText;
	static const long infoTextID;
	wxStaticText* inputDataInfo;
	static const long inputDataInfoID;

	ObjectsTreeCtrl *m_objectsTree;
	wxTreeItemId m_rootId;
	static const long idObjectsTree;

	wxTextCtrl* textBoxICAO;
	static const long idtextBoxICAO;
	wxTextCtrl* textBoxHeight;
	static const long idtextBoxHeight;
	wxTextCtrl* textBoxAngle;
	static const long idtextBoxAngle;

	wxCheckBox* checkBoxIsP;
	static const long idCheckBoxIsP;
	string m_writeP;

	std::string databasePath;
	mgrec* db;

	wxPanel *panel;


	wxDECLARE_EVENT_TABLE();
};


const long MyFrame::databaseButtonID = ::wxNewId();
const long MyFrame::infoTextID = ::wxNewId();
const long MyFrame::inputDataInfoID = ::wxNewId();
const long MyFrame::idObjectsTree = ::wxNewId();
const long MyFrame::idtextBoxICAO = ::wxNewId();
const long MyFrame::idtextBoxHeight = ::wxNewId();
const long MyFrame::idtextBoxAngle = ::wxNewId();
const long MyFrame::idCheckBoxIsP = ::wxNewId();

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(databaseButtonID, MyFrame::databaseButtonClick)
EVT_TREE_ITEM_ACTIVATED(ObjectsTree_Ctrl, MyFrame::OnTreeItemActivated)
EVT_TREE_SEL_CHANGED(ObjectsTree_Ctrl, MyFrame::OnTreeItemActivated)
EVT_CHECKBOX(idCheckBoxIsP, MyFrame::checkBoxClick)
EVT_TREE_ITEM_RIGHT_CLICK(ObjectsTree_Ctrl, MyFrame::OnTreeItemActivated)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{

	if (!wxApp::OnInit())
		return false;

	// screen resolution
	RECT actualDesktop;
	GetWindowRect(GetDesktopWindow(), &actualDesktop);

	// window position
	int x = actualDesktop.right / 2 - windowWIDTH / 2;
	int y = actualDesktop.bottom / 2 - windowHEIGHT / 2;

	new MyFrame("exr2flo", wxPoint(x, y), wxSize(windowWIDTH, windowHEIGHT));
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{

	panel = new wxPanel(this, -1, wxPoint(0, 0),
		wxSize(windowWIDTH, windowHEIGHT));

	::wxInitAllImageHandlers();

	wxBoxSizer *tree_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *button_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *info_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	infoText = new wxStaticText(panel, infoTextID, "Выберите файл flt...",
		wxPoint(windowWIDTH, BORDER));

	inputDataInfo = new wxStaticText(panel, infoTextID, "Введите данные:",
		wxPoint(windowWIDTH, BORDER));

	// Source BUTTON
	databaseButton = new wxButton(panel, databaseButtonID, "База данных",
		wxPoint(windowWIDTH - ButtonW - BORDER, 3 * BORDER),
		wxSize(ButtonW, ButtonH), 0);

	textBoxICAO = new wxTextCtrl(panel, idtextBoxICAO, "ICAO",
		wxPoint(ButtonW, 5 * BORDER),
		wxSize(60, 20));


	textBoxHeight = new wxTextCtrl(panel, idtextBoxICAO, "Высота",
		wxPoint(ButtonW * 2, 5 * BORDER),
		wxSize(60, 20));
	textBoxAngle = new wxTextCtrl(panel, idtextBoxICAO, "Угол",
		wxPoint(ButtonW * 3, 5 * BORDER),
		wxSize(60, 20));

	checkBoxIsP = new wxCheckBox(panel, idCheckBoxIsP, "добавить P",
		wxPoint(ButtonW, 5 * BORDER),
		wxSize(100, 20));

	// дерево объектов
	m_objectsTree = new ObjectsTreeCtrl(panel, ObjectsTree_Ctrl);
	m_rootId = m_objectsTree->AddRoot(wxT("Узлы файла"));
	tree_sizer->Add(m_objectsTree, 1, wxEXPAND);

	button_sizer->Add(infoText);
	button_sizer->Add(databaseButton, 1, wxGROW, 0);
	button_sizer->Add(1, 10);

	button_sizer->Add(inputDataInfo);
	info_sizer->Add(textBoxICAO);
	info_sizer->Add(10,1);
	info_sizer->Add(textBoxHeight);
	info_sizer->Add(10, 1);
	info_sizer->Add(textBoxAngle);
	info_sizer->Add(10, 1);
	info_sizer->Add(checkBoxIsP);

	topSizer->Add(tree_sizer, 1, wxGROW);
	topSizer->Add(button_sizer);
	topSizer->Add(info_sizer);
	SetSizerAndFit(topSizer);

	m_writeP = "";

	// non-resizable window
	SetMaxClientSize(wxSize(windowWIDTH, windowHEIGHT));
	SetMinClientSize(wxSize(windowWIDTH, windowHEIGHT));
	SetClientSize(windowWIDTH, windowHEIGHT);

	Show();
}

int MyApp::OnExit()
{
	return wxApp::OnExit();
}



void MyFrame::getChildTree(mgrec* item, wxTreeItemId parentNode)
{
	string name = "";

	mgrec* child = NULL;
	child = mgGetChild(item);
	if (child == NULL)
		return;

	name = mgGetName(child);
	wxTreeItemId childTree = m_objectsTree->AppendItem(parentNode, name);
	if (mgCountChild(child))
		getChildTree(child, childTree);

	if (mgCountChild(item))
	while (child = mgGetNext(child))
	{
		string nameCh = mgGetName(child);
		wxTreeItemId childTree = m_objectsTree->AppendItem(parentNode, nameCh);
		if (mgCountChild(child))
			getChildTree(child, childTree);
	}
}

bool isFloat(string s)
{
	istringstream iss(s);
	float dummy;
	iss >> skipws >> dummy;
	return (iss && iss.eof());     // Result converted to bool 
}

void MyFrame::OnTreeItemActivated(wxTreeEvent& event)
{
	if (textBoxICAO->GetValue() == "ICAO" || !textBoxHeight->GetValue().IsNumber() || !textBoxAngle->GetValue().IsNumber())
	{
		wxMessageBox("Проверьте введенные данные!", "Ошибка");
		return;
	}
	wxTreeItemId childTree = m_objectsTree->GetSelection().m_pItem;
	string selectedItem = m_objectsTree->GetItemText(childTree);
	getVertexes(selectedItem);
}

void MyFrame::readVertexes(mgrec *parent)
{

}

void MyFrame::databaseButtonClick(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this);
	if (openFileDialog->ShowModal() == wxID_OK){
		databasePath = openFileDialog->GetPath();
	}
	if (databasePath.length() == 0)
		return;

	string ext = databasePath.substr(databasePath.find_last_of("."), databasePath.length());
	if (ext != ".flt")
		return;

	mgrec *parent = NULL, *child = NULL;
	char* filename;
	mgbool ok;
	mgInit(0, 0);

	std::string dbPath = databasePath;
	db = mgOpenDb(dbPath.c_str());
	if (db == MG_NULL) {
		char msgbuf[1024];
		mgGetLastError(msgbuf, 1024);
		printf("%s\n", msgbuf);
		exit(EXIT_FAILURE);
	}
	// удаляем все элементы кроме корневого
	m_objectsTree->DeleteChildren(m_rootId);
	m_objectsTree->SetName(mgGetName(db));


	// читаем первый элемент
	parent = mgGetChild(db);
	string parName = mgGetName(parent);
	wxTreeItemId parentTree = m_objectsTree->AppendItem(m_rootId, parName);
	getChildTree(parent, parentTree);
	while (parent = mgGetNext(parent))
	{
		string parName1 = mgGetName(parent);
		wxTreeItemId parentTree1 = m_objectsTree->AppendItem(m_rootId, parName1);

		getChildTree(parent, parentTree1);
	}
	//boost::thread *thr = new boost::thread(&MyFrame::readVertexes, this, parent);

		
	infoText->SetLabelText(" ");
	// мигание окошка на панели задач
	FLASHWINFO fi;
	fi.cbSize = sizeof(FLASHWINFO);
	fi.hwnd = GetHWND();
	fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fi.uCount = 0;
	fi.dwTimeout = 0;
	FlashWindowEx(&fi);
	databasePath = "";

};



int g_count = 0;
static string PrintPolygon(mgrec* rec)
{
	char* name = mgGetName(rec);
	mgrec* vertex = mgGetChild(rec);
	mgcoord3d pos;
	int count = 1;
	stringstream stream;

	//printf("\n");
	//printf("Polygon: %s\n", name ? name : "Unknown");

	while (vertex)
	{
		if (mgGetVtxCoord(vertex, &pos.x, &pos.y, &pos.z) == MG_TRUE)
		{
			//printf("   Vertex %d: (%f, %f, %f)\n", count, pos.x, pos.y, pos.z);
			stream << "v " << pos.x << " " << pos.y << " " << pos.z << endl;
			count++;
		}
		vertex = mgGetNext(vertex);
	}
	g_count = count - 1;
	/* mgGetName allocates name, user must free it */
	if (name) mgFree(name);

	return stream.str();
}

int countObjects(string str)
{
	int count = 0;
	int i = 0;
	while (i < str.length())
	{
		if ((str[i] == 'g' || str[i] == 'o' || str[i] == 'f') && str[i + 1] == ' ')
		{
			count++;
		}
		i++;
	}
	return count;
}

int countALL = 0;
bool fClick = true;
string vertices, faces;

void MyFrame::getVertexes(string object)
{
	if (object == "")
	{
		wxMessageBox("Wrong selected object", "Error");
		return;
	}
	mgrec* findedObject = mgGetRecByName(db, object.c_str());

	if (findedObject == MG_NULL)
		return;


	stringstream stream;
	int c = 1;
	int count = countALL + g_count;

	stream << "o " << m_writeP << mgGetName(findedObject) << endl;
	vertices += PrintPolygon(findedObject);
	if (/*vertices == "" && */mgCountChild(findedObject) > 0)
	{
		findedObject = mgGetChild(findedObject);
		vertices += PrintPolygon(findedObject);
	}

	if (g_count > 0)
	{
		std::string par = mgGetName(findedObject);
		stream << "g " << m_writeP << par << endl;
		stream << "f ";
		for (int i = count + 1; i <= count + g_count; i++)
			stream << i << " ";
		stream << "\n";
	}
	faces += stream.str();
	stream.str("");
	count += g_count;

	mgrec* nestedChilds = mgGetNestedChild(findedObject);
	if (nestedChilds != NULL)
	{
		std::string par1 = mgGetName(nestedChilds);
		while (nestedChilds = mgGetNext(nestedChilds))
		{
			std::string par = mgGetName(nestedChilds);

			vertices += PrintPolygon(nestedChilds);
			if (g_count > 0)
			{
				stream << "g " << m_writeP << par << endl;
				stream << "f ";
				for (int i = count + 1; i <= count + g_count; i++)
					stream << i << " ";
				stream << "\n";
			}
			faces += stream.str();
			stream.str("");
			count += g_count;
		}
	}

	while (findedObject = mgGetNext(findedObject))
	{
		std::string par = mgGetName(findedObject);

		vertices += PrintPolygon(findedObject);
		if (g_count > 0)
		{
			stream << "g " << m_writeP << par << endl;
			stream << "f ";
			for (int i = count + 1; i <= count + g_count; i++)
				stream << i << " ";
			stream << "\n";
		}
		faces += stream.str();
		stream.str("");
		count += g_count;

		mgrec* nestedChilds = mgGetNestedChild(findedObject);
		if (nestedChilds != NULL)
		{
			std::string par1 = mgGetName(nestedChilds);
			while (nestedChilds = mgGetNext(nestedChilds))
			{
				std::string par = mgGetName(nestedChilds);

				vertices += PrintPolygon(nestedChilds);
				if (g_count > 0)
				{
					stream << "g " << m_writeP << par << endl;
					stream << "f ";
					for (int i = count + 1; i <= count + g_count; i++)
						stream << i << " ";
					stream << "\n";
				}
				faces += stream.str();
				stream.str("");
				count += g_count;
			}
		}
	}
	//int countobj = countObjects(faces);
	countALL = count/* + countobj*/;
	
	g_count = 0;
	double lat, lon;
	mgGetAttList(db, fltOriginLat1, &lat);
	mgGetAttList(db, fltOriginLong1, &lon);

	string ICAO, height, rotateAngle;

	string airportInfo = "# " + textBoxICAO->GetValue() + '\n' +
		"# " + to_string(lat) + '\n' + "# " + to_string(lon) + '\n' +
		"# " + textBoxHeight->GetValue() + '\n' +
		"# " + textBoxAngle->GetValue() + '\n';

	string file = textBoxICAO->GetValue() + ".obj";

	ofstream test;
	test.open(file);
	if (test.is_open())
	{
		airportInfo = "";
		test << airportInfo << vertices << faces << endl;
	}
	test.close();
}

void MyFrame::checkBoxClick(wxCommandEvent& event)
{
	m_writeP = checkBoxIsP->GetValue() ? "P" : "";
}