#pragma once
// for GUI control
#include "commctrl.h"
#pragma comment( lib, "ComCtl32.lib" )

//
#define MAX_WINDOW_NUM 128

//*******************************
class mainController;
//*******************************

class WindowGUI_Manager_Class
{
public:

	// VARIABLES
	int num_of_window;
	HWND windowHandler[MAX_WINDOW_NUM];
	int current_x[MAX_WINDOW_NUM];
	int current_y[MAX_WINDOW_NUM];
	int current_width[MAX_WINDOW_NUM];
	int current_height[MAX_WINDOW_NUM];
	HWND window_level[MAX_WINDOW_NUM];



	WindowGUI_Manager_Class();
	~WindowGUI_Manager_Class();

	void setMainController(void* ptr);

	// method for window creation ******************************
	HWND createNewWindow(LPCWSTR className, LPCWSTR title, int mode);

	HWND create_nomenu_window(LPCWSTR className, LPCWSTR title);
	HWND create_borderless_window(LPCWSTR className);
	HWND create_panel_window(LPCWSTR className, LPCWSTR title);
	HWND create_white_panel_window(LPCWSTR className, LPCWSTR title);

	void set_window_size(HWND wnd, int width, int height);
	void set_window_position(HWND wnd, int x, int y);
	void set_window_level(HWND wnd, int level);

	void close_window(HWND wnd);
	void show_window(HWND wnd);
	void minimize_window(HWND wnd);
	void maximize_window(HWND wnd);
	void hide_window(HWND wnd);
	void initWindowParam(int idx);
	// method for window creation ******************************

	// method for GUI creation *********************************
	// text field
	HWND create_textfield(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID);
	void set_textfield_string(HWND targetWnd, LPCWSTR string, int size);

	// button
	HWND create_button(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID);
	void set_button_string(HWND targetWnd, LPCWSTR string, int size);
	
	// check box
	HWND create_checkbox(HWND parentWindow, LPCWSTR string, int x, int y, int width, int height, int ID);
	void set_checkbox_string(HWND targetWnd, LPCWSTR string, int size);
	void set_checkbox_state(HWND targetWnd, bool state);
	bool get_checkbox_state(HWND targetWnd);

	// slider
	HWND create_slider(HWND parentWindow, int range, int x, int y, int width, int heigh, int ID);
	HWND create_vertical_slider(HWND parentWindow, int range, int x, int y, int width, int height, int ID);
	int get_slider_position(HWND targetWnd);
	void set_slider_position(HWND targetWnd, int pos);
	void set_slider_range(HWND targetWnd, int min, int max);
	
	// combobox
	HWND create_combobox(HWND parentWindow, int x, int y, int width, int height, int ID);
	void add_combobox_item(HWND targetWnd, LPCWSTR item);
	void delete_combobox_item_at_index(HWND targetWnd, int index);
	void delete_combobox_all(HWND targetWnd);
	int get_combobox_itemcount(HWND targetWnd);
	int get_combobox_selected_index(HWND targetWnd);
	void set_combobox_selected_item_at_index(HWND targetWnd, int index);
	
	// list box
	HWND create_listbox(HWND parentWindow, int x, int y, int width, int height, int ID);
	void add_listbox_item(HWND targetWnd, LPCWSTR item);
	void delete_listbox_item_at_index(HWND targetWnd, int index);
	void delete_listbox_all(HWND targetWnd);
	int get_listbox_itemcount(HWND targetWnd);
	int get_listbox_selected_index(HWND targetWnd);
	void set_listbox_selected_item_at_index(HWND targetWnd, int index);

	// progress bar
	HWND create_progressbar(HWND parentWindow, int x, int y, int width, int height, int ID);
	void set_progressbar_position(HWND targetWnd, int pos);
	void increse_progressbar(HWND targetWnd);
	
	// tooltip controll (sub)


	// list view


	// tab control
		// common management
	void set_enabled(HWND targetWnd, bool yn);
	void set_frame(HWND targetWnd, int x, int y, int width, int height);
	void set_hidden(HWND targetWnd, bool yn);

private:
	HFONT h_smallFont;
	HFONT h_normalFont;
	HFONT h_largeFont;

	HINSTANCE hInst;
};

