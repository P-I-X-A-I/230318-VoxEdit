#include "framework.h"
#include "SaveLoadClass.h"


SaveLoadClass::SaveLoadClass()
{
	wstr_savePath = NULL;
	wstr_loadPath = NULL;
}


SaveLoadClass::~SaveLoadClass()
{

}



std::wstring* SaveLoadClass::open_save_panel(HWND parentWin, LPCTSTR extension, LPCTSTR fileFilter)
{
	// open save panel
	OPENFILENAME ofn;
	WCHAR saveFilePath[MAX_PATH] = {'\0'};
	WCHAR defaultDirectory[MAX_PATH] = {'\0'};

	// default directory is desktop
	LPITEMIDLIST pidl;
	HRESULT res = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl);
	SHGetPathFromIDList(pidl, defaultDirectory);
	CoTaskMemFree(pidl);

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parentWin;
	ofn.lpstrInitialDir = defaultDirectory;
	ofn.lpstrFile = saveFilePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = extension;
	ofn.lpstrFilter = fileFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;


	// if save canceled ********************
	if (!GetSaveFileName(&ofn))
	{
		return NULL;
	}
	// *************************************

	// get save file path
	wchar_t save_w_Path[MAX_PATH];
	for (int i = 0; i < MAX_PATH; i++)
	{
		save_w_Path[i] = saveFilePath[i];
	}


	// create wstring path
	if (wstr_savePath != NULL)
	{
		delete wstr_savePath;
	}

	wstr_savePath = new std::wstring(save_w_Path);


	return wstr_savePath;
}



std::wstring* SaveLoadClass::open_load_panel(HWND parentWin, LPCWSTR title, LPCTSTR fileFilter, int numFilter)
{
	setlocale(LC_CTYPE, "ja_JP.UTF-8");

	OPENFILENAME ofn;
	WCHAR WC_filePath[1000] = {0};
	WCHAR WC_fileTitle[1000] = { 0 };
	WCHAR WC_defaultDirectory[MAX_PATH];

	// set filePath[0] to null
	WC_filePath[0] = '\0';

	// get path of current process directory
	GetCurrentDirectory(MAX_PATH, WC_defaultDirectory);

	// get desktop path
	LPITEMIDLIST pidl;
	TCHAR desktop_path[MAX_PATH];
	HRESULT res = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl);
	SHGetPathFromIDList(pidl, desktop_path);
	CoTaskMemFree(pidl);


	// init ofn
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parentWin;
	ofn.lpstrFile = WC_filePath;
	ofn.nMaxFile = 1000;
	ofn.lpstrFileTitle = WC_fileTitle;
	ofn.nMaxFileTitle = 1000;
	ofn.lpstrInitialDir = desktop_path;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	if (title != NULL)
	{
		ofn.lpstrTitle = title;
	}
	else
	{
		ofn.lpstrTitle = L"open file";
	}

	if (fileFilter != NULL)
	{
		ofn.lpstrFilter = fileFilter;
		ofn.nFilterIndex = numFilter;
	}


	// if canceled ******************
	if (!GetOpenFileName(&ofn))
	{
		return NULL;
	}
	// ******************************


	BOOL isUnicode = IsTextUnicode(WC_filePath, 1000 * sizeof(WCHAR), NULL);

	wprintf(L"filepath : %ls\n", WC_filePath);
	wprintf(L"filetitle : %ls\n", WC_fileTitle);
}