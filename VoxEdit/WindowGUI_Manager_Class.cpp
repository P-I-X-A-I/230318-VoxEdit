#include "framework.h"
#include "WindowGUI_Manager_Class.h"

//****************************
#include "mainController.h"
//****************************
mainController* mctr_obj;

#pragma once
LRESULT CALLBACK WinGUICallBack(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WinGUICallBack(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst = GetModuleHandle(NULL); // application handler;

	// for window creation
	CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;

	// for window size restriction
	MINMAXINFO* minmaxStruct = (MINMAXINFO*)lParam;
	RECT winRect;


	// messega from children GUI
	UINT GUI_ID, EVENT_CODE;
	GUI_ID = LOWORD(wParam);
	EVENT_CODE = HIWORD(wParam);

	UINT LP_L, LP_H;
	LP_L = LOWORD(lParam);
	LP_H = HIWORD(lParam);

	//printf("default %x %d %d %d %d\n", message, GUI_ID, EVENT_CODE, LP_L, LP_H);

	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_COMMAND:
	{
		//printf("button %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);



		if (GUI_ID == 22)// button
		{
			mctr_obj->save_button();
		}
		else if (GUI_ID >= 300 && GUI_ID < 400)//combobox
		{
			if (EVENT_CODE == CBN_SELCHANGE)
			{
				//mc_obj->gui_combobox(GUI_ID);
			}
		}


		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
			//case IDM_ABOUT: // IDM_ABOUT, defined in Resource.h
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			//	break;
			//case IDM_EXIT:
			//	DestroyWindow(hWnd);
			//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_NOTIFY:
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: HDC を使用する描画コードをここに追加してください...
		//SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		//Rectangle(hdc, 30, 30, 60, 60);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	case WM_GETMINMAXINFO: // limit window size
		//GetWindowRect(hWnd, &winRect );
		//minmaxStruct->ptMaxPosition.x = winRect.left;
		//minmaxStruct->ptMaxPosition.y = winRect.top;
		//minmaxStruct->ptMinTrackSize.x = winRect.right - winRect.left;
		//minmaxStruct->ptMinTrackSize.y = winRect.bottom - winRect.top;
		//minmaxStruct->ptMaxTrackSize.x = winRect.right - winRect.left;
		//minmaxStruct->ptMaxTrackSize.y = winRect.bottom - winRect.top;
		break;
	case WM_HSCROLL:

		if (LOWORD(wParam) == SB_THUMBPOSITION) // when mouse released
		{
			/*
			mc_obj->gui_slider(
				GetDlgCtrlID((HWND)lParam), // slider ID
				SendMessage((HWND)lParam, TBM_GETPOS, 0, 0) // slider Pos
			);
			*/
		}
		//printf("slider %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);

		break;
	case WM_VSCROLL:
		break;
	case WM_KEYDOWN:
		// 37-L, 38-U, 39-R, 40-D, 32-space, 8-delete
		mctr_obj->arrowKey_hit(GUI_ID, (int)GetKeyState(VK_SHIFT));
		mctr_obj->spaceKey_hit(GUI_ID);
		mctr_obj->deleteKey_hit(GUI_ID);
		//printf("KEYDOWN %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		//printf("winthread ID %d\n", GetCurrentThreadId());

		break;
	case WM_KEYUP:
		//printf("KEYUP %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;

	case WM_MOUSEMOVE:
		//printf("MOUSE MOVE %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;

	case WM_LBUTTONDOWN:
		//printf("L BUTTON DOWN %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		mctr_obj->mouseFlag(true);
		break;
	case WM_RBUTTONDOWN:
		//printf("R BUTTON DOWN %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;

	case WM_LBUTTONUP:
		//printf("L BUTTON UP %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		mctr_obj->mouseFlag(false);
		break;
	case WM_RBUTTONUP:
		//printf("R BUTTON UP %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;

	case WM_LBUTTONDBLCLK:
		//printf("L DCLICK %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;
	case WM_RBUTTONDBLCLK:
		//printf("R DCLICK %d %d %d %d\n", GUI_ID, EVENT_CODE, LP_L, LP_H);
		break;
	case WM_SETFOCUS:
		printf("WINDOW FOCUSED\n");
		break;
	case WM_KILLFOCUS:
		printf("WINDOW LOST FOCUS\n");
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}

WindowGUI_Manager_Class::WindowGUI_Manager_Class()
{
	printf("windowGUI manager init\n");
	
	hInst = GetModuleHandle(NULL);

	// to use trackbar, init common controls
	InitCommonControls();

	// variable for window
	num_of_window = 0;

	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		windowHandler[i] = NULL;
		this->initWindowParam(i);
	}

	// small font
	h_smallFont = CreateFont(
		12, // size
		0, //width
		0, // escapement
		0, // orientation
		FW_NORMAL, //thickness
		FALSE, // itaric
		FALSE, // underline
		0, // strike out
		SHIFTJIS_CHARSET, // charset
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		NULL // font name
	);

	// normal
	h_normalFont = CreateFont(
		13, // size
		0, //width
		0, // escapement
		0, // orientation
		FW_NORMAL, //thickness
		FALSE, // itaric
		FALSE, // underline
		0, // strike out
		SHIFTJIS_CHARSET, // charset
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		NULL // font name
	);

	// large font
	h_largeFont = CreateFont(
		16, // size
		0, //width
		0, // escapement
		0, // orientation
		FW_NORMAL, //thickness
		FALSE, // itaric
		FALSE, // underline
		0, // strike out
		SHIFTJIS_CHARSET, // charset
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		NULL // font name
	);
}

WindowGUI_Manager_Class::~WindowGUI_Manager_Class()
{

}

void WindowGUI_Manager_Class::setMainController(void* ptr)
{
	mctr_obj = (mainController*)ptr;
}


//************************************************
// method for window creation
//************************************************
void WindowGUI_Manager_Class::initWindowParam(int idx)
{
	current_x[idx] = 100;
	current_y[idx] = 100;
	current_width[idx] = 300;
	current_height[idx] = 300;
	window_level[idx] = HWND_TOP;
}

HWND WindowGUI_Manager_Class::createNewWindow(LPCWSTR className, LPCWSTR title, int mode)
{
	// flag & blush
	long FLAG;
	HBRUSH bgBrush;

	switch (mode)
	{
	case 0: // no menu window
		FLAG = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
		bgBrush = CreateSolidBrush(RGB(255, 255, 255));
		break;
	case 1: // borderless window
		FLAG = WS_SYSMENU | WS_POPUP;
		bgBrush = CreateSolidBrush(RGB(255, 255, 255));
		break;
	case 2: // panel window
		FLAG = WS_OVERLAPPED;
		bgBrush = CreateSolidBrush(RGB(210, 210, 210));
		break;
	case 3: // same with WS_OVERLAPPEDWINDOW
		FLAG = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		bgBrush = CreateSolidBrush(RGB(255, 255, 255));
		break;
	default: // no resize, with close button
		FLAG = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		bgBrush = CreateSolidBrush(RGB(255, 255, 255));
		break;
	}

	// WS_CAPTION = with title bar(WS_BORDER | WS_DLGFRAME)
		// WS_SYSMENU = close button &....
		// WS_THICKFRAME = resizable
		// WS_MINIMIZEBOX = min button
		// WS_MAXIMIZEBOX = max button
		// 
	// register class
	WNDCLASSEXW wcex;
	HINSTANCE appInstance = GetModuleHandle(NULL); // application handler

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // redraw flag
	wcex.lpfnWndProc = WinGUICallBack;	//callback to catch event
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = appInstance; // application handler
	wcex.hIcon = LoadIcon(appInstance, MAKEINTRESOURCE(107)); // 107 means default, IDI_APPNAME defined in Resource.h
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hbrBackground = (HBRUSH)bgBrush;
	//wcex.lpszMenuName = MAKEINTRESOURCEW(109); // 109 means default, IDC_APPNAME defined in Resource.h
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108)); // 108 means small icon defined in Resource.h

	RegisterClassExW(&wcex);


	// create window
	HWND returnWindow = CreateWindowW(
		className,
		title,
		FLAG,
		current_x[num_of_window],
		current_y[num_of_window],
		current_width[num_of_window],
		current_height[num_of_window],
		nullptr, // parent window
		nullptr, // menu
		appInstance,
		nullptr // lParam
	);

	// set handler to variable
	windowHandler[num_of_window] = returnWindow;
	// add num of window
	num_of_window++;

	if (num_of_window >= MAX_WINDOW_NUM)
	{
		num_of_window = 0;
	}


	// show window
	ShowWindow(returnWindow, SW_SHOW);
	UpdateWindow(returnWindow);

	return returnWindow;

}

HWND WindowGUI_Manager_Class::create_nomenu_window(LPCWSTR className, LPCWSTR title)
{
	if (windowHandler[num_of_window] != NULL)
	{
		DestroyWindow(windowHandler[num_of_window]);
		this->initWindowParam(num_of_window);
	}

	return createNewWindow(className, title, 0);
}

HWND WindowGUI_Manager_Class::create_borderless_window(LPCWSTR className)
{
	if (windowHandler[num_of_window] != NULL)
	{
		DestroyWindow(windowHandler[num_of_window]);
		this->initWindowParam(num_of_window);
	}

	return createNewWindow(className, NULL, 1);
}

HWND WindowGUI_Manager_Class::create_panel_window(LPCWSTR className, LPCWSTR title)
{
	if (windowHandler[num_of_window] != NULL)
	{
		DestroyWindow(windowHandler[num_of_window]);
		this->initWindowParam(num_of_window);
	}

	return createNewWindow(className, title, 2);
}

HWND WindowGUI_Manager_Class::create_white_panel_window(LPCWSTR className, LPCWSTR title)
{
	if (windowHandler[num_of_window] != NULL)
	{
		DestroyWindow(windowHandler[num_of_window]);
		this->initWindowParam(num_of_window);
	}

	return createNewWindow(className, title, 3);
}

//***********************************************************************


void WindowGUI_Manager_Class::set_window_size(HWND wnd, int width, int height)
{
	int INDEX = 10000;

	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			INDEX = i;
			break;
		}
	}

	if (INDEX == 10000)
	{
		printf("the window is not registered in WindowManager. return\n");
		return;
	}

	HWND targetWnd = windowHandler[INDEX];
	current_width[INDEX] = width;
	current_height[INDEX] = height;

	SetWindowPos(
		targetWnd,
		window_level[INDEX],
		current_x[INDEX],
		current_y[INDEX],
		current_width[INDEX],
		current_height[INDEX],
		SWP_SHOWWINDOW
	);
}


void WindowGUI_Manager_Class::set_window_position(HWND wnd, int x, int y)
{
	int INDEX = 10000;

	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			INDEX = i;
			break;
		}
	}

	if (INDEX == 10000)
	{
		printf("the window is not registered in WindowManager. return\n");
		return;
	}

	HWND targetWnd = windowHandler[INDEX];
	current_x[INDEX] = x;
	current_y[INDEX] = y;

	SetWindowPos(
		targetWnd,
		window_level[INDEX],
		current_x[INDEX],
		current_y[INDEX],
		current_width[INDEX],
		current_height[INDEX],
		SWP_SHOWWINDOW
	);
}


void WindowGUI_Manager_Class::set_window_level(HWND wnd, int level)
{
	int INDEX = 10000;

	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			INDEX = i;
			break;
		}
	}

	if (INDEX == 10000)
	{
		printf("the window is not registered in WindowManager. return\n");
		return;
	}

	switch (level)
	{
	case 0:
		window_level[INDEX] = HWND_BOTTOM;
		break;
	case 1:
		window_level[INDEX] = HWND_TOP;
		break;
	case 2:
		window_level[INDEX] = HWND_TOPMOST;
		break;
	default:
		window_level[INDEX] = HWND_TOP;
		break;
	}

	HWND targetWnd = windowHandler[INDEX];

	SetWindowPos(
		targetWnd,
		window_level[INDEX],
		current_x[INDEX],
		current_y[INDEX],
		current_width[INDEX],
		current_height[INDEX],
		SWP_SHOWWINDOW
	);
}


//******************************************************************


void WindowGUI_Manager_Class::close_window(HWND wnd)
{

	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			DestroyWindow(windowHandler[i]);
			windowHandler[i] = NULL;
			this->initWindowParam(i);

			return;
		}
	}

	printf("the window is not registerd in WindowManager. return\n");
	return;
}


void WindowGUI_Manager_Class::show_window(HWND wnd)
{
	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			ShowWindow(windowHandler[i], SW_SHOWNORMAL);
			return;
		}
	}

	printf("the window is not registerd in WindowManager. return\n");
	return;
}


void WindowGUI_Manager_Class::minimize_window(HWND wnd)
{
	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			ShowWindow(windowHandler[i], SW_MINIMIZE);
			return;
		}
	}

	printf("the window is not registerd in WindowManager. return\n");
	return;
}


void WindowGUI_Manager_Class::maximize_window(HWND wnd)
{
	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			ShowWindow(windowHandler[i], SW_MAXIMIZE);
			return;
		}
	}

	printf("the window is not registerd in WindowManager. return\n");
	return;
}


void WindowGUI_Manager_Class::hide_window(HWND wnd)
{
	for (int i = 0; i < MAX_WINDOW_NUM; i++)
	{
		if (wnd == windowHandler[i])
		{
			ShowWindow(windowHandler[i], SW_HIDE);
			return;
		}
	}

	printf("the window is not registerd in WindowManager. return\n");
	return;
}



//***********************************************************
// method for GUI creation **********************************
//***********************************************************


HWND WindowGUI_Manager_Class::create_textfield(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID)
{
	HWND tempTexField = CreateWindow(
		TEXT("STATIC"),
		string,
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);


	int error = SetBkMode(GetDC(tempTexField), TRANSPARENT);
	SendMessage(tempTexField, WM_SETFONT, (WPARAM)h_normalFont, MAKELPARAM(FALSE, 0));

	return tempTexField;
}

void WindowGUI_Manager_Class::set_textfield_string(HWND targetWnd, LPCWSTR string, int size)
{
	HFONT fontsize = h_normalFont;
	switch (size)
	{
	case 0:
		fontsize = h_smallFont;
		break;
	case 1:
		fontsize = h_normalFont;
		break;
	case 2:
		fontsize = h_largeFont;
		break;
	default:
		fontsize = h_normalFont;
		break;
	}

	SendMessage(targetWnd, WM_SETFONT, (WPARAM)fontsize, MAKELPARAM(FALSE, 0));
	SendMessage(targetWnd, WM_SETTEXT, 0, (LPARAM)string);
}




//**************** button *************************

HWND WindowGUI_Manager_Class::create_button(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID)
{
	HWND tempButton = CreateWindowW(
		TEXT("BUTTON"),
		string,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempButton, WM_SETFONT, (WPARAM)h_normalFont, MAKELPARAM(FALSE, 0));

	return tempButton;
}


void WindowGUI_Manager_Class::set_button_string(HWND targetWnd, LPCWSTR string, int size)
{
	HFONT fontsize = h_normalFont;

	switch (size)
	{
	case 0:
		fontsize = h_smallFont;
		break;
	case 1:
		fontsize = h_normalFont;
		break;
	case 2:
		fontsize = h_largeFont;
		break;
	default:
		fontsize = h_normalFont;
		break;
	}

	SendMessage(targetWnd, WM_SETFONT, (WPARAM)fontsize, MAKELPARAM(FALSE, 0));
	SendMessage(targetWnd, WM_SETTEXT, 0, (LPARAM)string);
}


// check box *******************************
HWND WindowGUI_Manager_Class::create_checkbox(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID)
{
	HWND tempButton = CreateWindowW(
		TEXT("BUTTON"),
		string,
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempButton, WM_SETFONT, (WPARAM)h_normalFont, MAKELPARAM(FALSE, 0));

	return tempButton;
}
void WindowGUI_Manager_Class::set_checkbox_string(HWND targetWnd, LPCWSTR string, int size)
{
	HFONT fontsize = h_normalFont;

	switch (size)
	{
	case 0:
		fontsize = h_smallFont;
		break;
	case 1:
		fontsize = h_normalFont;
		break;
	case 2:
		fontsize = h_largeFont;
		break;
	default:
		fontsize = h_normalFont;
		break;
	}

	SendMessage(targetWnd, WM_SETFONT, (WPARAM)fontsize, MAKELPARAM(FALSE, 0));
	SendMessage(targetWnd, WM_SETTEXT, 0, (LPARAM)string);
}



void WindowGUI_Manager_Class::set_checkbox_state(HWND targetWnd, bool state)
{
	if (state)
	{
		SendMessage(targetWnd, BM_SETCHECK, BST_CHECKED, 0);
	}
	else
	{
		SendMessage(targetWnd, BM_SETCHECK, BST_UNCHECKED, 0);
	}
}




bool WindowGUI_Manager_Class::get_checkbox_state(HWND targetWnd)
{
	if (BST_CHECKED == SendMessage(targetWnd, BM_GETCHECK, 0, 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//******************* SLIDER ***************************

HWND WindowGUI_Manager_Class::create_slider(HWND parentWindow, int range, int x, int y, int width, int height, int ID)
{
	HWND tempSlider = CreateWindowEx(
		NULL, // window style
		TRACKBAR_CLASS, // class
		NULL, // window name
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempSlider, TBM_SETRANGE, true, MAKELPARAM(0, range));
	SendMessage(tempSlider, TBM_SETPOS, true, 0);
	SendMessage(tempSlider, TBM_SETPAGESIZE, 0, 1);
	return tempSlider;
}

HWND WindowGUI_Manager_Class::create_vertical_slider(HWND parentWindow, int range, int x, int y, int width, int height, int ID)
{
	HWND tempSlider = CreateWindowEx(
		NULL,
		TRACKBAR_CLASS,
		NULL,
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempSlider, TBM_SETRANGE, true, MAKELPARAM(0, range));
	SendMessage(tempSlider, TBM_SETPOS, true, 0);
	SendMessage(tempSlider, TBM_SETPAGESIZE, 0, 1);
	return tempSlider;
}

int WindowGUI_Manager_Class::get_slider_position(HWND targetWnd)
{
	int pos = SendMessage(targetWnd, TBM_GETPOS, 0, 0);

	return pos;
}

void WindowGUI_Manager_Class::set_slider_position(HWND targetWnd, int pos)
{
	SendMessage(targetWnd, TBM_SETPOS, true, pos);
}

void WindowGUI_Manager_Class::set_slider_range(HWND targetWnd, int min, int max)
{
	SendMessage(targetWnd, TBM_SETRANGE, true, MAKELPARAM(min, max));
}

//******************** COMBOBOX *********************

HWND WindowGUI_Manager_Class::create_combobox(HWND parentWindow, int x, int y, int width, int height, int ID)
{
	HWND tempCombo = CreateWindow(
		TEXT("COMBOBOX"),
		NULL,
		WS_CHILD | WS_VISIBLE | ES_RIGHT | CBS_DROPDOWNLIST,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("0"));
	SendMessage(tempCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
	SendMessage(tempCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("2"));

	SendMessage(tempCombo, WM_SETFONT, (WPARAM)h_largeFont, MAKELPARAM(FALSE, 0));

	SendMessage(tempCombo, CB_SETCURSEL, 0, 0);

	return tempCombo;
}


void WindowGUI_Manager_Class::add_combobox_item(HWND targetWnd, LPCWSTR item)
{
	SendMessage(targetWnd, CB_ADDSTRING, 0, (LPARAM)item);
}

void WindowGUI_Manager_Class::delete_combobox_item_at_index(HWND targetWnd, int index)
{
	SendMessage(targetWnd, CB_DELETESTRING, index, 0);
}

void WindowGUI_Manager_Class::delete_combobox_all(HWND targetWnd)
{
	while (SendMessage(targetWnd, CB_GETCOUNT, 0, 0) != 0)
	{
		SendMessage(targetWnd, CB_DELETESTRING, 0, 0);
	}
}

int WindowGUI_Manager_Class::get_combobox_itemcount(HWND targetWnd)
{
	int count = SendMessage(targetWnd, CB_GETCOUNT, 0, 0);

	return count;
}

int WindowGUI_Manager_Class::get_combobox_selected_index(HWND targetWnd)
{
	int selIndex = SendMessage(targetWnd, CB_GETCURSEL, 0, 0);

	return selIndex;
}

void WindowGUI_Manager_Class::set_combobox_selected_item_at_index(HWND targetWnd, int index)
{
	SendMessage(targetWnd, CB_SETCURSEL, index, 0);
}


// **************** LIST BOX *************************

HWND WindowGUI_Manager_Class::create_listbox(HWND parentWindow, int x, int y, int width, int height, int ID)
{
	HWND tempList = CreateWindow(
		TEXT("LISTBOX"),
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | LBS_NOINTEGRALHEIGHT | WS_VSCROLL,
		x, y,
		width, height,
		parentWindow,
		(HMENU)8000,
		hInst,
		NULL
	);

	SendMessage(tempList, WM_SETFONT, (WPARAM)h_largeFont, MAKELPARAM(FALSE, 0));
	SendMessage(tempList, LB_SETHORIZONTALEXTENT, 300, 0); // has h scroll bar
	return tempList;
}


void WindowGUI_Manager_Class::add_listbox_item(HWND targetWnd, LPCWSTR item)
{
	SendMessage(targetWnd, LB_ADDSTRING, 0, (LPARAM)item);
}
void WindowGUI_Manager_Class::delete_listbox_item_at_index(HWND targetWnd, int index)
{
	SendMessage(targetWnd, LB_DELETESTRING, index, 0);
}
void WindowGUI_Manager_Class::delete_listbox_all(HWND targetWnd)
{
	while (SendMessage(targetWnd, LB_GETCOUNT, 0, 0) != 0)
	{
		SendMessage(targetWnd, LB_DELETESTRING, 0, 0);
	}
}
int WindowGUI_Manager_Class::get_listbox_itemcount(HWND targetWnd)
{
	int count = SendMessage(targetWnd, LB_GETCOUNT, 0, 0);

	return count;
}
int WindowGUI_Manager_Class::get_listbox_selected_index(HWND targetWnd)
{
	int selIndex = SendMessage(targetWnd, LB_GETCURSEL, 0, 0);

	return selIndex;
}
void WindowGUI_Manager_Class::set_listbox_selected_item_at_index(HWND targetWnd, int index)
{
	SendMessage(targetWnd, LB_SETCURSEL, index, 0);
}


// ***************** progress bar ********************

HWND WindowGUI_Manager_Class::create_progressbar(HWND parentWindow, int x, int y, int width, int height, int ID)
{
	HWND tempProgress = CreateWindowEx(
		0,
		PROGRESS_CLASS,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | PBS_SMOOTH,
		x, y,
		width, height,
		parentWindow,
		(HMENU)ID,
		hInst,
		NULL
	);

	SendMessage(tempProgress, PBM_SETRANGE32, 0, 100);
	SendMessage(tempProgress, PBM_SETSTEP, 1, 0); // for SendMessage( hWnd, PBM_STEPIT, 0, 0 );
	SendMessage(tempProgress, PBM_SETPOS, 0, 0);

	return tempProgress;
}

void WindowGUI_Manager_Class::set_progressbar_position(HWND targetWnd, int pos)
{
	SendMessage(targetWnd, PBM_SETPOS, (WPARAM)pos, 0);
}


void WindowGUI_Manager_Class::increse_progressbar(HWND targetWnd)
{
	SendMessage(targetWnd, PBM_STEPIT, 0, 0);
}


//*************** common management ***********************

void WindowGUI_Manager_Class::set_enabled(HWND targetWnd, bool yn)
{
	EnableWindow(targetWnd, yn);
}

void WindowGUI_Manager_Class::set_frame(HWND targetWnd, int x, int y, int width, int height)
{
	SetWindowPos(
		targetWnd,
		NULL,
		x, y,
		width, height,
		SWP_NOZORDER
	);
}

void WindowGUI_Manager_Class::set_hidden(HWND targetWnd, bool yn)
{
	if (yn)
	{
		ShowWindow(targetWnd, SW_HIDE);
	}
	else
	{
		ShowWindow(targetWnd, SW_SHOW);
	}
}

