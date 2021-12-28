
#include "Utils.hpp"

/*
TO DO:
	1 -> Implement a way so that the profile is not loded for the first time in new device
		Once the new device saves a profile load it from the next times.

	2 -> Find more bugs and finishing touches
*/

sf::Vector2i win32WindowSize = sf::Vector2i(1285, 720);
sf::Vector2i sfmlWindowSize = sf::Vector2i(1080, 640);
//win32 buttons and window handlers
HWND B_AddVertex, B_DeleteVertex;	//vertex buttons
HWND B_AddEdge, B_DelEdge;	//edge buttons
HWND B_Settings, B_Clear;	//Settings

//editor settings window handlers
HWND EditorWindow, B_EditorClose;
HWND COMBO_V_Color, COMBO_V_nameColor, COMBO_V_Size, COMBO_V_FontSize;
HWND COMBO_E_Color, COMBO_E_weightColor, COMBO_E_Size, COMBO_E_FontSize;
HWND CheckBox_V_scatter;

//visualisation settings window handlers
HWND VisualSettingWindow, B_VS_Save;
HWND COMBO_VS_Colour, COMBO_VS_Speed;

//graph settings window handlers
HWND SettingWindow;	//child window
HWND childCheckbox1, childCheckbox2, childCheckbox3, childCheckbox4;
HWND BC_Save;
//handlers for win32 menu
HMENU hMenu;
//handlers for text boxes
HWND Add_E_textBox1, Add_E_textBox2, Add_EW_textBox1;
HWND Del_E_textBox1, Del_E_textBox2;
//window closed flags
bool ParentClosed = false;
bool ChildClosed = false;
bool ChildWindowOpen = false;
bool editorWindowOpen = false;
bool VsSettingsOpen = false;

const TCHAR *colors[] = { TEXT("Red"), TEXT("Dark Gold"), TEXT("Green"), TEXT("Teal"), TEXT("Purple"), TEXT("Lemon Chiffon"),
TEXT("Sandy Brown"), TEXT("Ivory"), TEXT("White"), TEXT("Black") };
const TCHAR *fontSizes[] = { TEXT("20"), TEXT("25"), TEXT("30"), TEXT("35"), TEXT("40") };
const TCHAR *vertexSizes[] = { TEXT("25"), TEXT("30"), TEXT("35"), TEXT("40"), TEXT("45"), TEXT("50") };
const TCHAR *edgeSizes[] = { TEXT("5"), TEXT("10"), TEXT("15") };
const TCHAR *speed[] = { TEXT("Fast"), TEXT("Medium"), TEXT("Slow") };
TCHAR tempText[256];
int selected = 0;
int currentProfile[10] = { 0 };

VertexManager V;
EdgeManager E;
File_IO FIO;
VisualizationManager VisualM;
void restoreProfile(void);

bool EdgeSynch = false;
int deletedVertex = 0;
float dt = 0;
float timer = 0;
sf::Color selectColour(int index);
float selectSize(int index);
float selectVertexSize(int index);
float selectEdgeSize(int index);
int selectSpeed(int index);

//thread handlers and functions
HANDLE ThreadBfs, ThreadDfs;
DWORD WINAPI BFS(LPVOID param) {
	VisualM.onProgress = true;
	while (VisualM.getQueueSize() != 0) {
		VisualM.bfs(FIO.adj_matrix, V, E, FIO.matrixBaseIndex, dt);
		sf::sleep(sf::milliseconds(VisualM.sleepTime));
	}
	VisualM.reset(V);
	return 0;
}

DWORD WINAPI DFS(LPVOID param) {
	VisualM.onProgress = true;
	while (VisualM.getStackSize() != 0) {
		VisualM.dfs(FIO.adj_matrix, V, E, FIO.matrixBaseIndex, dt);
		sf::sleep(sf::milliseconds(VisualM.sleepTime));
	}
	VisualM.reset(V);
	return 0;
}

void addMenu(HWND hwnd) {
	HMENU FileSubMenu = CreateMenu();
	HMENU ToolsSubMenu = CreateMenu();
	HMENU VisualizationMenu = CreateMenu();
	hMenu = CreateMenu();

	//creating file menu and its submenus
	AppendMenu(FileSubMenu, MF_STRING, ID_IMPORT, "Import");
	AppendMenu(FileSubMenu, MF_STRING, ID_EXPORT, "Export");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)FileSubMenu, " File ");

	//creating tools menu and its submeus
	AppendMenu(ToolsSubMenu, MF_STRING, ID_EDITOROPTIONS, "Editor Options");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)ToolsSubMenu, "Tools");

	//creating visualization menu
	AppendMenu(VisualizationMenu, MF_STRING, ID_VISUALIZEBFS, "Visualize BFS");
	AppendMenu(VisualizationMenu, MF_STRING, ID_VISUALIZEDFS, "Visualize DFS");
	AppendMenu(VisualizationMenu, MF_STRING, ID_VISUALSETTINGS, "Settings");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)VisualizationMenu, "Visualization");

	SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK ProcessMessageMain(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	int id = 0;
	switch (message)
	{
	case WM_CLOSE:
		ParentClosed = true;
		break;
	case WM_COMMAND:
		switch (wparam)
		{
		case ID_ADDEDGE:
			char s1[3], s2[3], s3[3];
			GetWindowText(Add_E_textBox1, s1, 3);
			GetWindowText(Add_E_textBox2, s2, 3);
			if (E.weightedGraph)
				GetWindowText(Add_EW_textBox1, s3, 3);
			if (strlen(s1) != 0 && strlen(s2) != 0) {
				E.createEdge(V.vertices, s1, s2, s3, V.usingAlpha);
				SetWindowText(Add_E_textBox1, "");
				SetWindowText(Add_E_textBox2, "");
				SetWindowText(Add_EW_textBox1, "");
			}
			else
				MessageBeep(MB_ICONERROR);
			break;
		case ID_DELEDGE:
			char s21[3], s22[3];
			GetWindowText(Del_E_textBox1, s21, 3);
			GetWindowText(Del_E_textBox2, s22, 3);
			E.deleteEdge(V.vertices, s21, s22, V.usingAlpha);
			SetWindowText(Del_E_textBox1, "");
			SetWindowText(Del_E_textBox2, "");
			break;
		case ID_ADDVERTEX:
			V.createVertex();
			break;
		case ID_DELVERTEX:
			deletedVertex = V.deleteVertex();
			E.removeRedundantEdge(deletedVertex);
			break;
		case ID_SETTINGS:
			if (!ChildWindowOpen) {
				ShowWindow(SettingWindow, SW_SHOWNORMAL);
				ChildWindowOpen = true;
			}
			else {
				ShowWindow(SettingWindow, SW_HIDE);
				ChildWindowOpen = false;
			}
			break;
		case ID_CLEAR:
			E.reset();
			V.reset();
			break;

			//////////////////
		case ID_EXPORT:
			id = MessageBox(handle, TEXT("Do you wish to save the current graph ?"), TEXT("Warning"), MB_YESNO);
			if (id == 7)
				break;
			FIO.create_adj_matrix(E.edges, E.directedGraph, V.startFromZero);
			FIO.writeInFile(V.current_vertices);
			break;
		case ID_IMPORT:
			id = MessageBox(handle, TEXT("Make sure to apply appropriate graph settings before importing graph from file. Do you wish to continue ?"),
				TEXT("Warning"), MB_YESNO);
			if (id == 7)
				break;
			E.reset();
			V.reset();
			FIO.readFromFile(V.startFromZero);
			FIO.constructGraph(V, E);
			break;

			//visualization tools and menu handles
		case ID_VISUALIZEBFS:
			FIO.create_adj_matrix(E.edges, E.directedGraph, V.startFromZero);
			ThreadBfs = CreateThread(0, 0, BFS, NULL, 0, NULL);
			break;
		case ID_VISUALIZEDFS:
			FIO.create_adj_matrix(E.edges, E.directedGraph, V.startFromZero);
			ThreadDfs = CreateThread(0, 0, DFS, NULL, 0, NULL);
			break;
		case ID_VISUALSETTINGS:
			if (!editorWindowOpen) {
				ShowWindow(VisualSettingWindow, SW_SHOWNORMAL);
				VsSettingsOpen = true;
			}
			else {
				ShowWindow(VisualSettingWindow, SW_HIDE);
				VsSettingsOpen = false;
			}
			break;
		case ID_EDITOROPTIONS:
			if (!editorWindowOpen) {
				ShowWindow(EditorWindow, SW_SHOWNORMAL);
				editorWindowOpen = true;
			}
			else {
				ShowWindow(EditorWindow, SW_HIDE);
				editorWindowOpen = false;
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT CALLBACK SettingsProcessMessage(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message)
	{
	case WM_CLOSE:
		ChildClosed = true;
		break;
	case WM_COMMAND:
		switch (wparam)
		{
		case ID_SAVE:
			E.weightedGraph = IsDlgButtonChecked(handle, ID_WEIGHTED);
			E.directedGraph = IsDlgButtonChecked(handle, ID_DIRECTED);
			V.startFromZero = (IsDlgButtonChecked(handle, ID_STARTWITHZERO) ? true : false);
			V.usingAlpha = (IsDlgButtonChecked(handle, ID_USEALPHA) ? true : false);
			if (V.usingAlpha) V.startFromZero = true;
			V.changeNumbering(EdgeSynch);
			V.changeNaming();

			if (EdgeSynch) {
				E.synchronizeEdgeIncidents(V.startFromZero);
				EdgeSynch = false;
			}

			ShowWindow(SettingWindow, SW_HIDE);
			ChildWindowOpen = false;
			break;
		default:
			break;
		}
	default:
		break;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT CALLBACK EditorProcessMessages(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {

	switch (message)
	{
	case WM_CREATE:
		FIO.loadLastProfile(currentProfile, 10);
		restoreProfile();
		//setting appropriate text for newly resotred profile is set in main for each combobox.
		break;
	case WM_COMMAND:
		//the HIWORD of wparam contains the optional command code. Need the CBN_SELCHANGE notification in this case
		if (HIWORD(wparam) == CBN_SELCHANGE) {
			switch (LOWORD(wparam))
			{//the low word of wparam contains the control commands. Here I have my IDs as controls 
			case ID_VERTEXCOLORCHANGE:
				selected = SendMessage(COMBO_V_Color, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_V_Color, CB_GETLBTEXT, selected, (LPARAM)tempText);
				V.changeVertexColor(selectColour(selected));
				currentProfile[0] = selected;
				break;
			case ID_VERTEXNUMCOLORCHANGE:
				selected = SendMessage(COMBO_V_nameColor, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_V_nameColor, CB_GETLBTEXT, selected, (LPARAM)tempText);
				V.changeNameTextColor(selectColour(selected));
				currentProfile[1] = selected;
				break;
			case ID_VERTEXSIZECHANGE:
				selected = SendMessage(COMBO_V_Size, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_V_Size, CB_GETLBTEXT, selected, (LPARAM)tempText);
				V.changeVertexSize(selectVertexSize(selected));
				currentProfile[2] = selected;
				break;
			case ID_VERTEXFONTSIZECHANGE:
				selected = SendMessage(COMBO_V_FontSize, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_V_FontSize, CB_GETLBTEXT, selected, (LPARAM)tempText);
				V.changeNameFontSize(selectSize(selected));
				currentProfile[3] = selected;
				break;
			case ID_EDGECOLORCHANGE:
				selected = SendMessage(COMBO_E_Color, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_E_Color, CB_GETLBTEXT, selected, (LPARAM)tempText);
				E.changeEdgeColour(selectColour(selected));
				currentProfile[4] = selected;
				break;
			case ID_EDGEWEIGHTCOLORCHANGE:
				selected = SendMessage(COMBO_E_weightColor, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_E_weightColor, CB_GETLBTEXT, selected, (LPARAM)tempText);
				E.changeWeightColour(selectColour(selected));
				currentProfile[5] = selected;
				break;
			case ID_EDGESIZECHANGE:
				selected = SendMessage(COMBO_E_Size, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_E_Size, CB_GETLBTEXT, selected, (LPARAM)tempText);
				E.changeEdgeSize(selectEdgeSize(selected));
				currentProfile[6] = selected;
				break;
			case ID_EDGEWEIGHTSIZECHANGE:
				selected = SendMessage(COMBO_E_FontSize, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_E_FontSize, CB_GETLBTEXT, selected, (LPARAM)tempText);
				E.changeWeightFontSize(selectSize(selected));
				currentProfile[7] = selected;
				break;
			default:
				break;
			}
		}

		switch (wparam)
		{
		case ID_EDITORCLOSE:
			//save the profile
			FIO.saveCurrentProfile(currentProfile, 10);
			ShowWindow(EditorWindow, SW_HIDE);
			editorWindowOpen = false;
			break;
		case ID_SCATTERVERTICES:
			V.isScattered = IsDlgButtonChecked(handle, ID_SCATTERVERTICES);
			break;
		default:
			break;
		}
	default:
		break;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT CALLBACK VSProcessedMessage(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {

	switch (message)
	{
	case WM_COMMAND:
		if (HIWORD(wparam) == CBN_SELCHANGE) {
			if (LOWORD(wparam) == ID_VS_COLOUR) {
				selected = SendMessage(COMBO_VS_Colour, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_VS_Colour, CB_GETLBTEXT, selected, (LPARAM)tempText);
				VisualM.changeHighlightColour(selectColour(selected));
				currentProfile[8] = selected;
			}
			if (LOWORD(wparam) == ID_VS_SPEED) {
				selected = SendMessage(COMBO_VS_Speed, CB_GETCURSEL, 0, 0);
				SendMessage(COMBO_VS_Speed, CB_GETLBTEXT, selected, (LPARAM)tempText);
				VisualM.changeSpeed(selectSpeed(selected));
				currentProfile[9] = selected;
			}
		}

		if (wparam == ID_VISUALCLOSE) {
			//save profile for these too...
			FIO.saveCurrentProfile(currentProfile, 10);
			ShowWindow(VisualSettingWindow, SW_HIDE);
			VsSettingsOpen = false;
		}
		break;
	default:
		break;
	}

	return DefWindowProc(handle, message, wparam, lparam);
}



int main()
{
	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASS windowClass;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.hCursor = 0;
	windowClass.hIcon = NULL;
	windowClass.hInstance = instance;
	windowClass.lpfnWndProc = &ProcessMessageMain;
	windowClass.lpszClassName = TEXT("SFML_APP");
	windowClass.lpszMenuName = NULL;
	windowClass.style = 0;
	RegisterClass(&windowClass);

	//main win32 app window
	HWND window = CreateWindow(TEXT("SFML_APP"), TEXT("Sfml Win32"), WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		win32WindowSize.x, win32WindowSize.y, NULL, NULL, instance, NULL);
	SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) | WS_MINIMIZEBOX);
	//sfml window
	HWND childWindow = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		5, 5, sfmlWindowSize.x, sfmlWindowSize.y, window, NULL, instance, NULL);
	sf::RenderWindow sfmlWindow(childWindow);
	//sfmlWindow.setVerticalSyncEnabled(true);
	sfmlWindow.setFramerateLimit(200);
	sf::Clock clk;
	sf::Time time;
	V.load();
	E.load();

	//Vertex controls
	HWND vertexControls = CreateWindow(TEXT("STATIC"), TEXT("Vertex_Controls"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 178, 10,
		200, 40, window, NULL, instance, NULL);
	HWND line1 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, win32WindowSize.x - 178,
		30, 130, 3, window, NULL, instance, NULL);
	B_AddVertex = CreateWindow(TEXT("BUTTON"), TEXT("Add a Vertex"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 184, 40,
		150, 36, window, (HMENU)ID_ADDVERTEX, instance, NULL);
	B_DeleteVertex = CreateWindow(TEXT("BUTTON"), TEXT("Delete a Vertex"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 184, 85,
		150, 36, window, (HMENU)ID_DELVERTEX, instance, NULL);

	HWND line2 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, win32WindowSize.x - 194,
		130, 170, 5, window, NULL, instance, NULL);

	//Edge controls
	HWND EdgeControls = CreateWindow(TEXT("STATIC"), TEXT("Edge_Controls"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 170, 145,
		200, 40, window, NULL, instance, NULL);
	HWND line3 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, win32WindowSize.x - 175,
		168, 130, 3, window, NULL, instance, NULL);
	//Adding edge
	HWND edgeMessage = CreateWindow(TEXT("STATIC"), TEXT("add_edge_between"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 188, 175,
		164, 40, window, NULL, instance, NULL);
	Add_E_textBox1 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 185, 200,
		50, 25, window, NULL, NULL, NULL);
	HWND gap1 = CreateWindow(TEXT("STATIC"), TEXT("and"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 125, 202, 40, 20,
		window, NULL, instance, NULL);
	Add_E_textBox2 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 200,
		50, 25, window, NULL, NULL, NULL);
	//weigth
	HWND gap2 = CreateWindow(TEXT("STATIC"), TEXT("Weight"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 185, 236, 80, 20,
		window, NULL, instance, NULL);
	Add_EW_textBox1 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 235,
		50, 25, window, NULL, NULL, NULL);
	B_AddEdge = CreateWindow(TEXT("BUTTON"), TEXT("ADD"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 143, 266,
		70, 30, window, (HMENU)ID_ADDEDGE, instance, NULL);

	//Deleting edge
	HWND edgeMessage2 = CreateWindow(TEXT("STATIC"), TEXT("Delete_edge_between"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 200, 310,
		180, 40, window, NULL, instance, NULL);
	Del_E_textBox1 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 185, 335,
		50, 25, window, NULL, NULL, NULL);
	HWND gap3 = CreateWindow(TEXT("STATIC"), TEXT("and"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 125, 338, 40, 20,
		window, NULL, instance, NULL);
	Del_E_textBox2 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 335,
		50, 25, window, NULL, NULL, NULL);
	B_DelEdge = CreateWindow(TEXT("BUTTON"), TEXT("DELETE"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 153, 368,
		90, 30, window, (HMENU)ID_DELEDGE, instance, NULL);
	HWND line4 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, win32WindowSize.x - 194,
		408, 170, 5, window, NULL, instance, NULL);

	//Settings
	B_Settings = CreateWindow(TEXT("BUTTON"), TEXT("Settings"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 155, win32WindowSize.y - 120,
		100, 40, window, (HMENU)ID_SETTINGS, instance, NULL);
	//Clear
	B_Clear = CreateWindow(TEXT("BUTTON"), TEXT("Clear Canvas"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 180, 430,
		140, 40, window, (HMENU)ID_CLEAR, instance, NULL);

	addMenu(window);//the menu with parent window

	//////////////////////////////////////////////////////////////////////
	///////////////editor window
	WNDCLASS editorClass;
	editorClass.cbClsExtra = 0;
	editorClass.cbWndExtra = 0;
	editorClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	editorClass.hCursor = 0;
	editorClass.hIcon = NULL;
	editorClass.hInstance = instance;
	editorClass.lpfnWndProc = &EditorProcessMessages;
	editorClass.lpszClassName = TEXT("Editor_class");
	editorClass.lpszMenuName = NULL;
	editorClass.style = 0;
	RegisterClass(&editorClass);

	EditorWindow = CreateWindow(TEXT("Editor_class"), TEXT("Editor"), WS_VISIBLE, 500, 200,
		800, 500, NULL, NULL, instance, NULL);
	ShowWindow(EditorWindow, SW_HIDE);
	B_EditorClose = CreateWindow(TEXT("BUTTON"), TEXT("Save and Close"), WS_VISIBLE | WS_CHILD | WS_BORDER, 620, 400, 150, 40, EditorWindow,
		(HMENU)ID_EDITORCLOSE, instance, NULL);
	CheckBox_V_scatter = CreateWindow(TEXT("BUTTON"), TEXT("Scattered mode"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 420, 160, 30,
		EditorWindow, (HMENU)ID_SCATTERVERTICES, instance, NULL);


	//vertex editor
	HWND LB_VertexOptions = CreateWindow(TEXT("STATIC"), TEXT("Vertex Editor"), WS_VISIBLE | WS_CHILD, 8, 2, 120, 20, EditorWindow,
		NULL, instance, NULL);
	HWND editorLine1 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, 4, 22, 120, 3,
		EditorWindow, NULL, instance, NULL);

	int LB_Ypos = 35, COMBO_Ypos = 60, COMBO_Height = 140;
	HWND LB_VertexColor = CreateWindow(TEXT("STATIC"), TEXT("Vertex Color:"), WS_VISIBLE | WS_CHILD, 8, LB_Ypos, 120, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_V_Color = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 8, COMBO_Ypos, 140,
		COMBO_Height, EditorWindow, (HMENU)ID_VERTEXCOLORCHANGE, instance, NULL);
	for (int i = 0; i < 10; i++)
		SendMessage(COMBO_V_Color, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_V_Color, CB_SETCURSEL, (WPARAM)currentProfile[0], (LPARAM)(TCHAR)colors[currentProfile[0]]);

	HWND LB_VertexNameColor = CreateWindow(TEXT("STATIC"), TEXT("Vertex Name Color:"), WS_VISIBLE | WS_CHILD, 590, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_V_nameColor = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 590, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_VERTEXNUMCOLORCHANGE, instance, NULL);
	for (int i = 0; i < 10; i++)
		SendMessage(COMBO_V_nameColor, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_V_nameColor, CB_SETCURSEL, (WPARAM)currentProfile[1], (LPARAM)(TCHAR)colors[currentProfile[1]]);

	HWND LB_VertexSize = CreateWindow(TEXT("STATIC"), TEXT("Vertex Size:"), WS_VISIBLE | WS_CHILD, 185, LB_Ypos, 120, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_V_Size = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 185, COMBO_Ypos, 140,
		COMBO_Height, EditorWindow, (HMENU)ID_VERTEXSIZECHANGE, instance, NULL);
	for (int i = 0; i < 6; i++)
		SendMessage(COMBO_V_Size, CB_ADDSTRING, 0, (LPARAM)vertexSizes[i]);
	SendMessage(COMBO_V_Size, CB_SETCURSEL, (WPARAM)currentProfile[2], (LPARAM)(TCHAR)vertexSizes[currentProfile[2]]);

	HWND LB_VertexFontSize = CreateWindow(TEXT("STATIC"), TEXT("Vertex Font Size:"), WS_VISIBLE | WS_CHILD, 370, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_V_FontSize = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 370, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_VERTEXFONTSIZECHANGE, instance, NULL);
	for (int i = 0; i < 5; i++)
		SendMessage(COMBO_V_FontSize, CB_ADDSTRING, 0, (LPARAM)fontSizes[i]);
	SendMessage(COMBO_V_FontSize, CB_SETCURSEL, (WPARAM)currentProfile[3], (LPARAM)(TCHAR)fontSizes[currentProfile[3]]);


	//Edge editor
	HWND LB_EdgeOptions = CreateWindow(TEXT("STATIC"), TEXT("Edge Editor"), WS_VISIBLE | WS_CHILD, 8, 200, 120, 20, EditorWindow,
		NULL, instance, NULL);
	HWND editorLine2 = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_GRAYRECT | WS_BORDER, 4, 220, 120, 3,
		EditorWindow, NULL, instance, NULL);

	LB_Ypos = 235;
	COMBO_Ypos = 260;
	HWND LB_EdgeColor = CreateWindow(TEXT("STATIC"), TEXT("Edge Color:"), WS_VISIBLE | WS_CHILD, 8, LB_Ypos, 120, 20, EditorWindow,
		NULL, instance, NULL);
	COMBO_E_Color = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 8, COMBO_Ypos, 140,
		COMBO_Height, EditorWindow, (HMENU)ID_EDGECOLORCHANGE, instance, NULL);
	for (int i = 0; i < 10; i++)
		SendMessage(COMBO_E_Color, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_E_Color, CB_SETCURSEL, (WPARAM)currentProfile[4], (LPARAM)(TCHAR)colors[currentProfile[4]]);

	HWND LB_EdgeWeightColor = CreateWindow(TEXT("STATIC"), TEXT("Edge Weight Color:"), WS_VISIBLE | WS_CHILD, 590, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_E_weightColor = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 590, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_EDGEWEIGHTCOLORCHANGE, instance, NULL);
	for (int i = 0; i < 10; i++)
		SendMessage(COMBO_E_weightColor, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_E_weightColor, CB_SETCURSEL, (WPARAM)currentProfile[5], (LPARAM)(TCHAR)colors[currentProfile[5]]);

	HWND LB_EdgeSize = CreateWindow(TEXT("STATIC"), TEXT("Edge Size:"), WS_VISIBLE | WS_CHILD, 185, LB_Ypos, 120, 20, EditorWindow,
		NULL, instance, NULL);
	COMBO_E_Size = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 185, COMBO_Ypos, 140,
		COMBO_Height, EditorWindow, (HMENU)ID_EDGESIZECHANGE, instance, NULL);
	for (int i = 0; i < 3; i++)
		SendMessage(COMBO_E_Size, CB_ADDSTRING, 0, (LPARAM)edgeSizes[i]);
	SendMessage(COMBO_E_Size, CB_SETCURSEL, (WPARAM)currentProfile[6], (LPARAM)(TCHAR)edgeSizes[currentProfile[6]]);

	HWND LB_EdgeFontSize = CreateWindow(TEXT("STATIC"), TEXT("Edge Weight Size:"), WS_VISIBLE | WS_CHILD, 370, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_E_FontSize = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 370, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_EDGEWEIGHTSIZECHANGE, instance, NULL);
	for (int i = 0; i < 5; i++)
		SendMessage(COMBO_E_FontSize, CB_ADDSTRING, 0, (LPARAM)fontSizes[i]);
	SendMessage(COMBO_E_FontSize, CB_SETCURSEL, (WPARAM)currentProfile[7], (LPARAM)(TCHAR)fontSizes[currentProfile[7]]);



	//////////////////////////////////////////////////////////////////////
	WNDCLASS ChildClass;
	ChildClass.cbClsExtra = 0;
	ChildClass.cbWndExtra = 0;
	ChildClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	ChildClass.hCursor = 0;
	ChildClass.hIcon = NULL;
	ChildClass.hInstance = instance;
	ChildClass.lpfnWndProc = &SettingsProcessMessage;
	ChildClass.lpszClassName = TEXT("Child_Class");
	ChildClass.lpszMenuName = NULL;
	ChildClass.style = 0;
	RegisterClass(&ChildClass);

	SettingWindow = CreateWindow(TEXT("Child_Class"), TEXT("Settings"), WS_VISIBLE, 500, 200,
		400, 200, NULL, NULL, instance, NULL);
	ShowWindow(SettingWindow, SW_HIDE);
	//check boxes
	childCheckbox1 = CreateWindow(TEXT("BUTTON"), TEXT("Weighted"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10, 100, 30,
		SettingWindow, (HMENU)ID_WEIGHTED, instance, NULL);
	childCheckbox2 = CreateWindow(TEXT("BUTTON"), TEXT("Directed"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 40, 100, 30,
		SettingWindow, (HMENU)ID_DIRECTED, instance, NULL);
	childCheckbox3 = CreateWindow(TEXT("BUTTON"), TEXT("Start from Zero"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 70, 150, 30,
		SettingWindow, (HMENU)ID_STARTWITHZERO, instance, NULL);
	childCheckbox4 = CreateWindow(TEXT("BUTTON"), TEXT("Use Alphabets"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 100, 150, 30,
		SettingWindow, (HMENU)ID_USEALPHA, instance, NULL);
	BC_Save = CreateWindow(TEXT("BUTTON"), TEXT("save"), WS_VISIBLE | WS_CHILD | WS_BORDER, 300, 100, 70, 40, SettingWindow,
		(HMENU)ID_SAVE, instance, NULL);


	///////////////////////////////
	//visualisation settings
	WNDCLASS VS_settings;
	VS_settings.cbClsExtra = 0;
	VS_settings.cbWndExtra = 0;
	VS_settings.hbrBackground = (HBRUSH)COLOR_WINDOW;
	VS_settings.hCursor = 0;
	VS_settings.hIcon = NULL;
	VS_settings.hInstance = instance;
	VS_settings.lpfnWndProc = &VSProcessedMessage;
	VS_settings.lpszClassName = TEXT("VS_Settings_Class");
	VS_settings.lpszMenuName = NULL;
	VS_settings.style = 0;
	RegisterClass(&VS_settings);

	VisualSettingWindow = CreateWindow(TEXT("VS_Settings_Class"), TEXT("Settings"), WS_VISIBLE, 500, 200,
		450, 300, NULL, NULL, instance, NULL);
	ShowWindow(VisualSettingWindow, SW_HIDE);

	HWND VS_colour = CreateWindow(TEXT("STATIC"), TEXT("Vertex Highlight Color:"), WS_VISIBLE | WS_CHILD, 8, 10, 180, 20,
		VisualSettingWindow, NULL, instance, NULL);
	COMBO_VS_Colour = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 8, 35, 180,
		140, VisualSettingWindow, (HMENU)ID_VS_COLOUR, instance, NULL);
	for (int i = 0; i < 10; i++)
		SendMessage(COMBO_VS_Colour, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_VS_Colour, CB_SETCURSEL, (WPARAM)currentProfile[8], (LPARAM)(TCHAR)colors[currentProfile[8]]);

	HWND VS_speed = CreateWindow(TEXT("STATIC"), TEXT("Visualisation speed:"), WS_VISIBLE | WS_CHILD, 8, 110, 180, 20,
		VisualSettingWindow, NULL, instance, NULL);
	COMBO_VS_Speed = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 8, 145, 180,
		140, VisualSettingWindow, (HMENU)ID_VS_SPEED, instance, NULL);
	for (int i = 0; i < 3; i++)
		SendMessage(COMBO_VS_Speed, CB_ADDSTRING, 0, (LPARAM)speed[i]);
	SendMessage(COMBO_VS_Speed, CB_SETCURSEL, (WPARAM)currentProfile[9], (LPARAM)(TCHAR)speed[currentProfile[9]]);

	B_EditorClose = CreateWindow(TEXT("BUTTON"), TEXT("Save"), WS_VISIBLE | WS_CHILD | WS_BORDER, 335, 200, 80, 40, VisualSettingWindow,
		(HMENU)ID_VISUALCLOSE, instance, NULL);


	/////////////////////////////////////////////////
	////////////////////////////////////////////////
	MSG message;
	message.message = static_cast<UINT>(~WM_QUIT);
	while (!ParentClosed) {
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			time = clk.restart();
			dt = time.asSeconds();

			V.updateVertices(sfmlWindow);
			E.updateEdge(V.vertices);


			sfmlWindow.clear();

			E.renderEdge(sfmlWindow);
			V.renderVertices(sfmlWindow);
			VisualM.displayStatus(sfmlWindow);

			sfmlWindow.display();
		}
	}

	return 0;
}

void restoreProfile(void) {
	V.changeVertexColor(selectColour(currentProfile[0]));
	V.changeNameTextColor(selectColour(currentProfile[1]));
	V.changeVertexSize(selectVertexSize(currentProfile[2]));
	V.changeNameFontSize(selectSize(currentProfile[3]));
	E.changeEdgeColour(selectColour(currentProfile[4]));
	E.changeWeightColour(selectColour(currentProfile[5]));
	E.changeEdgeSize(selectEdgeSize(currentProfile[6]));
	E.changeWeightFontSize(selectSize(currentProfile[7]));
	VisualM.changeHighlightColour(selectColour(currentProfile[8]));
	VisualM.changeSpeed(selectSpeed(currentProfile[9]));
}

sf::Color selectColour(int index)
{
	sf::Color c;
	switch (index)
	{
	case 0:
		c = sf::Color::Red;
		break;
	case 1:
		c = sf::Color(218, 165, 32, 255);
		break;
	case 2:
		c = sf::Color::Green;
		break;
	case 3:
		c = sf::Color(0, 128, 128, 255);
		break;
	case 4:
		c = sf::Color(128, 0, 128, 255);
		break;
	case 5:
		c = sf::Color(255, 250, 205, 255);
		break;
	case 6:
		c = sf::Color(244, 164, 96, 255);
		break;
	case 7:
		c = sf::Color(255, 255, 240, 255);
		break;
	case 8:
		c = sf::Color::White;
		break;
	case 9:
		c = sf::Color::Black;
		break;
	default:
		break;
	}

	return c;
}

float selectSize(int index)
{
	switch (index)
	{
	case 0: return 20.f;
	case 1: return 25.f;
	case 2: return 30.f;
	case 3: return 35.f;
	case 4: return 40.f;
	case 5: return 45.f;
	default:
		break;
	}
	return 30.f;
}

float selectVertexSize(int index)
{
	switch (index)
	{
	case 0: return 25.f;
	case 1: return 30.f;
	case 2: return 35.f;
	case 3: return 40.f;
	case 4: return 45.f;
	case 5: return 50.f;
	default:
		break;
	}
	return 40.f;
}

float selectEdgeSize(int index)
{
	switch (index) {
	case 0: return 5.f;
	case 1: return 10.f;
	case 2: return 15.f;
	default:
		break;
	}
	return 10.f;
}

int selectSpeed(int index) {
	switch (index)
	{
	case 0: return 500;
	case 1: return 1000;
	case 2: return 1500;
	default:
		break;
	}
	return 1000;
}