#pragma once
#include "commdlg.h" // for open panel
#include "ShlObj.h"
#include "locale.h"
#include <string>

class SaveLoadClass
{
public:
	
	std::wstring* wstr_savePath;
	std::wstring* wstr_loadPath;

	SaveLoadClass();
	~SaveLoadClass();

	std::wstring* open_save_panel(HWND parentWin, LPCTSTR extension, LPCTSTR fileFilter);
	std::wstring* open_load_panel(HWND parentWin, LPCWSTR title, LPCTSTR fileFilter, int numFilter);
};

