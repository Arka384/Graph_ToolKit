#include "VisualizationManager.hpp"
#include "ControlIDs.hpp"
#include <Windows.h>
#include <WinUser.h>

/*
	Add how to info properly.
	change the name of childClass window class
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
HWND ShowMatrixWindow, SM_textbox, B_SM_Close;
HWND ShortestPathWindow, SP_textBoxSource, SP_textBoxDest, B_SP_Apply;

//graph settings window handlers
HWND SettingWindow;	//child window
HWND childCheckbox1, childCheckbox2, childCheckbox3, childCheckbox4;
HWND BC_Save;

//help window handlers
HWND HelpWindow, HelpTextWindow, B_HelpClose;
LPCSTR temp;
int textPosX = 0, textPosY = 0;

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
bool SmWindowOpen = false;
bool SpWindowOpen = false;

const TCHAR *colors[] = { TEXT("Red"), TEXT("Dark Gold"), TEXT("Green"), TEXT("Teal"), TEXT("Purple"), TEXT("Lemon Chiffon"),
TEXT("Sandy Brown"), TEXT("Ivory"), TEXT("White"), TEXT("Black"), TEXT("Orange Red"), TEXT("Olive"), TEXT("Colcolate"), TEXT("Rosy Brown"),
 TEXT("Light Pink"), TEXT("Slate Gray")	};
const TCHAR *fontSizes[] = { TEXT("20"), TEXT("25"), TEXT("30"), TEXT("35"), TEXT("40") };
const TCHAR *vertexSizes[] = { TEXT("25"), TEXT("30"), TEXT("35"), TEXT("40"), TEXT("45"), TEXT("50") };
const TCHAR *edgeSizes[] = { TEXT("5"), TEXT("10"), TEXT("15") };
const TCHAR *speed[] = { TEXT("Fast"), TEXT("Medium"), TEXT("Slow") };
TCHAR tempText[256];
int selected = 0;
const int nColors = 16;
const int profileSize = 11;
int currentProfile[profileSize] = { 0 };

VertexManager V;
EdgeManager E;
File_IO FIO;
VisualizationManager VisualM;

void restoreProfile(void);

bool EdgeSynch = false;
int deletedVertex = 0;
float dt = 0;
float timer = 0;
std::string adjString;

////window class creating function
void setWindowClassParam(WNDCLASS &wndClass, HINSTANCE currInstance);
///////////
sf::Color selectColour(int index);
float selectSize(int index);
float selectVertexSize(int index);
float selectEdgeSize(int index);
int selectSpeed(int index);
void getTextHeight(HWND handle);

//thread handlers and functions
HANDLE ThreadBfs, ThreadDfs, ThreadBfsSP;
DWORD WINAPI BFS(LPVOID param) {
	VisualM.onProgress = true;
	while (VisualM.getQueueSize() != 0) {
		VisualM.bfs(FIO.adj_matrix, V, E, FIO.matrixBaseIndex, dt);
		sf::sleep(sf::milliseconds(VisualM.sleepTime));
	}
	VisualM.getTraversed();
	while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
	VisualM.showingTraverse = false;
	VisualM.reset(V);
	return 0;
}

DWORD WINAPI DFS(LPVOID param) {
	VisualM.onProgress = true;
	while (VisualM.getStackSize() != 0) {
		VisualM.dfs(FIO.adj_matrix, V, E, FIO.matrixBaseIndex, dt);
		sf::sleep(sf::milliseconds(VisualM.sleepTime));
	}
	VisualM.getTraversed();
	while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
	VisualM.showingTraverse = false;
	VisualM.reset(V);
	return 0;
}

DWORD WINAPI bfsShortestPath(LPVOID param) {
	//VisualM.onProgress = true;
	while (VisualM.getQueueSize() != 0) {
		VisualM.bfsPathFind(FIO.adj_matrix, V, E, FIO.matrixBaseIndex);
		sf::sleep(sf::milliseconds(VisualM.sleepTime - 500));
	}
	VisualM.getPath(VisualM.pred, V);
	while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
	VisualM.showingTraverse = false;
	VisualM.reset(V);
	return 0;
}



void addMenu(HWND hwnd) {
	HMENU FileSubMenu = CreateMenu();
	HMENU ToolsSubMenu = CreateMenu();
	HMENU VisualizationMenu = CreateMenu();
	HMENU HelpMenu = CreateMenu();
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
	AppendMenu(VisualizationMenu, MF_STRING, ID_SHOWMATRIX, "View Adjcent Matrix");
	AppendMenu(VisualizationMenu, MF_STRING, ID_SHORTESTPATH, "Find Shortest Path");
	AppendMenu(VisualizationMenu, MF_STRING, ID_VISUALSETTINGS, "Settings");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)VisualizationMenu, "Visualization");

	//help menu
	AppendMenu(HelpMenu, MF_STRING, ID_HOWTOUSE, "How to use");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)HelpMenu, "Help");

	SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK ProcessMessageMain(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	int id = 0;
	std::string fileLine;
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

		////File io control menu
		case ID_EXPORT:
			id = MessageBox(handle, TEXT("Do you wish to save the current graph ?"), TEXT("Warning"), MB_YESNO);
			if (id == 7)
				break;
			FIO.create_adj_matrix(V, E);
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
			FIO.create_adj_matrix(V, E);
			ThreadBfs = CreateThread(0, 0, BFS, NULL, 0, NULL);
			break;
		case ID_VISUALIZEDFS:
			FIO.create_adj_matrix(V, E);
			ThreadDfs = CreateThread(0, 0, DFS, NULL, 0, NULL);
			break;

		case ID_SHOWMATRIX:
			FIO.reset();
			FIO.create_adj_matrix(V, E);
			adjString = FIO.matrixToString(V.current_vertices, V.startFromZero, V.usingAlpha);
			SetWindowText(SM_textbox, adjString.c_str());
			getTextHeight(SM_textbox);	//resize that window and controls according to the text string 
			if (!SmWindowOpen) {
				ShowWindow(ShowMatrixWindow, SW_SHOWNORMAL);
				SmWindowOpen = true;
			}
			else {
				ShowWindow(ShowMatrixWindow, SW_HIDE);
				SmWindowOpen = false;
			}
			break;

		case ID_SHORTESTPATH:
			if (!SpWindowOpen) {
				ShowWindow(ShortestPathWindow, SW_SHOWNORMAL);
				SpWindowOpen = true;
			}
			else {
				ShowWindow(ShortestPathWindow, SW_HIDE);
				SpWindowOpen = false;
			}
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

		//Editor control menu
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
		
		//Help menu control
		case ID_HOWTOUSE:
			//create this window on demand unlike the others
			HelpWindow = CreateWindow(TEXT("Help"), TEXT("How-To"), WS_VISIBLE | WS_SYSMENU, 500, 200,
				640, 480, NULL, NULL, NULL, NULL);
			HelpTextWindow = CreateWindow(TEXT("STATIC"), temp, WS_VISIBLE | WS_CHILD, 0, 0, 800, 800,
				HelpWindow, NULL, NULL, NULL);

			//load info from file
			fileLine = FIO.loadHowToInfo();
			temp = fileLine.c_str();
			SetWindowText(HelpTextWindow, temp);
			//std::cout << temp;
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
		FIO.loadLastProfile(currentProfile, profileSize);
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
			FIO.saveCurrentProfile(currentProfile, profileSize);
			ShowWindow(EditorWindow, SW_HIDE);
			editorWindowOpen = false;
			break;
		case ID_SCATTERVERTICES:
			V.isScattered = IsDlgButtonChecked(handle, ID_SCATTERVERTICES);
			currentProfile[10] = (V.isScattered) ? 1 : 0;
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
			FIO.saveCurrentProfile(currentProfile, profileSize);
			ShowWindow(VisualSettingWindow, SW_HIDE);
			VsSettingsOpen = false;
		}
		break;
	default:
		break;
	}

	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT CALLBACK ShowMatProcessedMessage(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	
	switch (message)
	{
	case WM_COMMAND:
		if (wparam == ID_SM_CLOSE) {
			SmWindowOpen = false;
			ShowWindow(ShowMatrixWindow, SW_HIDE);
		}
		break;
	default:
		break;
	}

	return DefWindowProc(handle, message, wparam, lparam);	
}

LRESULT CALLBACK ShortestPathParamMessage(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message)
	{
	case WM_COMMAND:
		char src[3], dest[3];
		if (wparam == ID_SP_APPLY) {
			GetWindowText(SP_textBoxSource, src, 3);
			GetWindowText(SP_textBoxDest, dest, 3);
			VisualM.getInputSP(src, dest, V.usingAlpha, V.startFromZero);
			SetWindowText(SP_textBoxSource, "");
			SetWindowText(SP_textBoxDest, "");
			
			ShowWindow(ShortestPathWindow, SW_HIDE);
			SpWindowOpen = false;
			FIO.create_adj_matrix(V, E);
			ThreadBfsSP = CreateThread(NULL, 0, bfsShortestPath, NULL, 0, NULL);
		}
	default:
		break;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT CALLBACK helpProcessesMessage(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {

	switch (message)
	{
	case WM_CREATE:
		SCROLLINFO SbInfo;
		SbInfo.cbSize = sizeof(SbInfo);
		SbInfo.nPos = 0;
		SbInfo.nMax = 2800;	//change this according to text size
		SbInfo.nPage = 400;
		SbInfo.fMask = SIF_ALL;
		SbInfo.nMin = 0;
		SetScrollInfo(handle, SB_HORZ, &SbInfo, TRUE);
		SetScrollInfo(handle, SB_VERT, &SbInfo, TRUE);
		break;

	case WM_HSCROLL:
		SbInfo.cbSize = sizeof(SbInfo);
		GetScrollInfo(handle, SB_HORZ, &SbInfo);
		switch (LOWORD(wparam)) {
			case SB_LINELEFT:
				SbInfo.nPos -= 5;
				break;
			case SB_LINERIGHT:
				SbInfo.nPos += 5;
				break;
			case SB_THUMBTRACK:
				SbInfo.nPos = HIWORD(wparam);
				break;
		}
		SetScrollInfo(handle, SB_HORZ, &SbInfo, TRUE);
		textPosX = -SbInfo.nPos;
		MoveWindow(HelpTextWindow, textPosX, textPosY, 1800, 1800, TRUE);
		break;

	case WM_VSCROLL:
		SbInfo.cbSize = sizeof(SbInfo);
		GetScrollInfo(handle, SB_VERT, &SbInfo);
		switch (LOWORD(wparam)) {
		case SB_LINEUP:
			SbInfo.nPos -= 5;
			break;
		case SB_LINEDOWN:
			SbInfo.nPos += 5;
			break;
		case SB_THUMBTRACK:
			SbInfo.nPos = HIWORD(wparam);
			break;
		}
		SetScrollInfo(handle, SB_VERT, &SbInfo, TRUE);
		textPosY = -SbInfo.nPos;
		MoveWindow(HelpTextWindow, textPosX, textPosY, 1800, 1800, TRUE);
		break;
		
	default:
		break;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

void getTextHeight(HWND handle) {

	HDC hdc;
	TEXTMETRIC textMetrics;
	SCROLLINFO sInfo;
	int TextHeight;
	int TextWidth;
	int ParentWidth = 400;

	hdc = GetDC(handle);
	GetTextMetrics(hdc, &textMetrics);
	TextHeight = textMetrics.tmHeight * V.current_vertices;
	TextWidth = (textMetrics.tmWeight * V.current_vertices)/40;	//approximate
	ReleaseDC(handle, hdc);
	
	if (TextWidth > ParentWidth)
		ParentWidth = TextWidth;

	SetWindowPos(ShowMatrixWindow, NULL, 500, 200, ParentWidth, TextHeight + 100, NULL);
	SetWindowPos(SM_textbox, NULL, 10, 10, ParentWidth - 40, TextHeight +2, NULL);
	SetWindowPos(B_SM_Close, NULL, 10, TextHeight + 17, ParentWidth - 40, 30, NULL);

}

//window class creating function
void setWindowClassParam(WNDCLASS &wndClass, HINSTANCE currInstance) {
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndClass.hCursor = 0;
	wndClass.hIcon = NULL;
	wndClass.hInstance = currInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.style = 0;
}

int main()
{
	srand(time(NULL));
	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASS windowClass;
	setWindowClassParam(windowClass, instance);
	windowClass.lpfnWndProc = &ProcessMessageMain;
	windowClass.lpszClassName = TEXT("SFML_APP");
	RegisterClass(&windowClass);

	//main win32 app window
	HWND window = CreateWindow(TEXT("SFML_APP"), TEXT("Graph_Toolkit"), WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
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
		50, 25, window, NULL, instance, NULL);
	HWND gap1 = CreateWindow(TEXT("STATIC"), TEXT("and"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 125, 202, 40, 20,
		window, NULL, instance, NULL);
	Add_E_textBox2 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 200,
		50, 25, window, NULL, instance, NULL);
	//weigth
	HWND gap2 = CreateWindow(TEXT("STATIC"), TEXT("Weight"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 185, 236, 80, 20,
		window, NULL, instance, NULL);
	Add_EW_textBox1 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 235,
		50, 25, window, NULL, instance, NULL);
	B_AddEdge = CreateWindow(TEXT("BUTTON"), TEXT("ADD"), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 143, 266,
		70, 30, window, (HMENU)ID_ADDEDGE, instance, NULL);

	//Deleting edge
	HWND edgeMessage2 = CreateWindow(TEXT("STATIC"), TEXT("Delete_edge_between"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 200, 310,
		180, 40, window, NULL, instance, NULL);
	Del_E_textBox1 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 185, 335,
		50, 25, window, NULL, instance, NULL);
	HWND gap3 = CreateWindow(TEXT("STATIC"), TEXT("and"), WS_VISIBLE | WS_CHILD, win32WindowSize.x - 125, 338, 40, 20,
		window, NULL, instance, NULL);
	Del_E_textBox2 = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, win32WindowSize.x - 80, 335,
		50, 25, window, NULL, instance, NULL);
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
	setWindowClassParam(editorClass, instance);
	editorClass.lpfnWndProc = &EditorProcessMessages;
	editorClass.lpszClassName = TEXT("Editor_class");
	RegisterClass(&editorClass);

	EditorWindow = CreateWindow(TEXT("Editor_class"), TEXT("Editor"), WS_VISIBLE, 500, 200,
		800, 500, NULL, NULL, instance, NULL);
	ShowWindow(EditorWindow, SW_HIDE);
	B_EditorClose = CreateWindow(TEXT("BUTTON"), TEXT("Save and Close"), WS_VISIBLE | WS_CHILD | WS_BORDER, 620, 400, 150, 40, EditorWindow,
		(HMENU)ID_EDITORCLOSE, instance, NULL);
	CheckBox_V_scatter = CreateWindow(TEXT("BUTTON"), TEXT("Scattered mode"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 420, 160, 30,
		EditorWindow, (HMENU)ID_SCATTERVERTICES, instance, NULL);
	if(currentProfile[10] == 1)
		SendMessage(CheckBox_V_scatter, BM_SETCHECK, BST_CHECKED, 0);

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
	for (int i = 0; i < nColors; i++)
		SendMessage(COMBO_V_Color, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_V_Color, CB_SETCURSEL, (WPARAM)currentProfile[0], (LPARAM)(TCHAR)colors[currentProfile[0]]);

	HWND LB_VertexNameColor = CreateWindow(TEXT("STATIC"), TEXT("Vertex Name Color:"), WS_VISIBLE | WS_CHILD, 590, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_V_nameColor = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 590, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_VERTEXNUMCOLORCHANGE, instance, NULL);
	for (int i = 0; i < nColors; i++)
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
	for (int i = 0; i < nColors; i++)
		SendMessage(COMBO_E_Color, CB_ADDSTRING, 0, (LPARAM)colors[i]);
	SendMessage(COMBO_E_Color, CB_SETCURSEL, (WPARAM)currentProfile[4], (LPARAM)(TCHAR)colors[currentProfile[4]]);

	HWND LB_EdgeWeightColor = CreateWindow(TEXT("STATIC"), TEXT("Edge Weight Color:"), WS_VISIBLE | WS_CHILD, 590, LB_Ypos, 180, 20,
		EditorWindow, NULL, instance, NULL);
	COMBO_E_weightColor = CreateWindow(TEXT("COMBOBOX"), NULL, WS_VSCROLL | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 590, COMBO_Ypos, 180,
		COMBO_Height, EditorWindow, (HMENU)ID_EDGEWEIGHTCOLORCHANGE, instance, NULL);
	for (int i = 0; i < nColors; i++)
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
	//////graph setting window class
	WNDCLASS ChildClass;
	setWindowClassParam(ChildClass, instance);
	ChildClass.lpfnWndProc = &SettingsProcessMessage;
	ChildClass.lpszClassName = TEXT("Child_Class");
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
	setWindowClassParam(VS_settings, instance);
	VS_settings.lpfnWndProc = &VSProcessedMessage;
	VS_settings.lpszClassName = TEXT("VS_Settings_Class");
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
	//Show matirx window
	WNDCLASS VS_ShowMatrix;
	setWindowClassParam(VS_ShowMatrix, instance);
	VS_ShowMatrix.lpfnWndProc = &ShowMatProcessedMessage;
	VS_ShowMatrix.lpszClassName = TEXT("VS_Show_Matrix");
	RegisterClass(&VS_ShowMatrix);

	ShowMatrixWindow = CreateWindow(TEXT("VS_Show_Matrix"), TEXT("Adjacent Matrix"), WS_VISIBLE, 500, 200,
		400, 400, NULL, NULL, instance, NULL);
	ShowWindow(ShowMatrixWindow, SW_HIDE);

	SM_textbox = CreateWindow(TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, 
		10, 10, 360, 290, ShowMatrixWindow, NULL, instance, NULL);

	B_SM_Close = CreateWindow(TEXT("BUTTON"), TEXT("Close"), WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 315,
		363, 30, ShowMatrixWindow, (HMENU)ID_SM_CLOSE, instance, NULL);

	////////////////////////////////////////////////
	///////shortest path parameters collector window
	WNDCLASS VS_ShortestPath;
	setWindowClassParam(VS_ShortestPath, instance);
	VS_ShortestPath.lpfnWndProc = &ShortestPathParamMessage;
	VS_ShortestPath.lpszClassName = TEXT("VS_ShortestPath");
	RegisterClass(&VS_ShortestPath);

	ShortestPathWindow = CreateWindow(TEXT("VS_ShortestPath"), TEXT("Shortest path details"), WS_VISIBLE, 500, 200,
		300, 200, NULL, NULL, instance, NULL);
	ShowWindow(ShortestPathWindow, SW_HIDE);

	HWND SP_LableSource = CreateWindow(TEXT("STATIC"), TEXT("Source:"), WS_VISIBLE | WS_CHILD, 8, 20, 100, 20,
		ShortestPathWindow, NULL, instance, NULL);
	HWND SP_LableDest = CreateWindow(TEXT("STATIC"), TEXT("Destination:"), WS_VISIBLE | WS_CHILD, 8, 60, 100, 20,
		ShortestPathWindow, NULL, instance, NULL);

	SP_textBoxSource = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 18,
		100, 25, ShortestPathWindow, (HMENU)ID_SP_SOURCE, instance, NULL);
	SP_textBoxDest = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 58,
		100, 25, ShortestPathWindow, (HMENU)ID_SP_DEST, instance, NULL);

	B_SP_Apply = CreateWindow(TEXT("BUTTON"), TEXT("Apply"), WS_VISIBLE | WS_CHILD | WS_BORDER, 202, 110,
		70, 35, ShortestPathWindow, (HMENU)ID_SP_APPLY, instance, NULL);

	//////////////////////////////////////////////////
	///////Help window
	WNDCLASS helpWindowClass;
	setWindowClassParam(helpWindowClass, instance);
	helpWindowClass.lpfnWndProc = &helpProcessesMessage;
	helpWindowClass.lpszClassName = TEXT("Help");
	RegisterClass(&helpWindowClass);
	//creating the window is handled in callback of main parent window


	/////////////////////////////////////////////////
	//////////////////event loop/////////////////////
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
	V.isScattered = (currentProfile[10] == 1) ? true : false;
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
	case 10:
		c = sf::Color(255, 69, 0, 255);
		break;
	case 11:
		c = sf::Color(128, 128, 0, 255);
		break;
	case 12:
		c = sf::Color(210, 105, 30, 255);
		break;
	case 13:
		c = sf::Color(188, 143, 143, 255);
		break;
	case 14:
		c = sf::Color(255, 182, 193, 255);
		break;
	case 15:
		c = sf::Color(176, 196, 222, 255);
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