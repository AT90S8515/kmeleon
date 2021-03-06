
// look for "filename" first in the settingsDir,
// then skinsDir\CurrentSkin,
// then skinsDir\default
// copy the full path into "szSkinFile"
// if it's not anywhere, return settingsDir

#include <windows.h>
#include <tchar.h>
#include "kmeleon_plugin.h"
#include "utf.h"

extern kmeleonPlugin kPlugin;

BOOL _FindSkinFile(TCHAR* szSkinFile, TCHAR* szSkinDir, const TCHAR* filename)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFile;
	TCHAR szTmpSkinName[MAX_PATH];
	TCHAR szTmpSkinFile[MAX_PATH];

	kPlugin.kFuncs->GetPreference(PREF_TSTRING, "kmeleon.general.skinsCurrent", szTmpSkinName, (TCHAR*)_T(""));

	if (*szSkinDir) {
		while (*szTmpSkinName) {
			size_t len = _tcslen(szSkinDir);
			if (szSkinDir[len-1] != _T('\\'))
				_tcscat(szSkinDir, _T("\\"));

			len = _tcslen(szTmpSkinName);
			if (szTmpSkinName[len-1] != '\\')
				_tcscat(szTmpSkinName, _T("\\"));

			_tcscpy(szTmpSkinFile, szSkinDir);
			_tcscat(szTmpSkinFile, szTmpSkinName);
			_tcscat(szTmpSkinFile, filename);

			hFile = FindFirstFile(szTmpSkinFile, &FindData);
			if(hFile != INVALID_HANDLE_VALUE) {   
				FindClose(hFile);
				_tcscpy( szSkinFile, szTmpSkinFile );
				return true;
			}

			len = _tcslen(szTmpSkinName);
			if (len > 1)
				szTmpSkinName[len-2] = 0;
		}

		// it wasn't in the current skin directory, check the default

		_tcscpy(szTmpSkinFile, szSkinDir);
		_tcscat(szTmpSkinFile, _T("default\\"));
		_tcscat(szTmpSkinFile, filename);

		hFile = FindFirstFile(szTmpSkinFile, &FindData);
		if(hFile != INVALID_HANDLE_VALUE) {   
			FindClose(hFile);
			_tcscpy( szSkinFile, szTmpSkinFile );
			return true;
		}
	}
	return false;
}

//To really do
void FindSkinFile(TCHAR *szSkinFile, const TCHAR *filename) 
{
	WIN32_FIND_DATA FindData;
	HANDLE hFile;

	TCHAR szTmpSkinDir[MAX_PATH] = _T("");
	TCHAR szTmpSkinFile[MAX_PATH] = _T("");

	if (!szSkinFile || !filename || !*filename)
		return;

	// check for the file in the settingsDir

	char folder[MAX_PATH];
	kPlugin.kFuncs->GetFolder(UserSettingsFolder, folder, MAX_PATH);
	strcat(folder, "\\");
#if _UNICODE
	utf8_to_utf16(folder, szTmpSkinDir, MAX_PATH);
#else
	utf8_to_ansi(folder, szTmpSkinDir, MAX_PATH);
#endif

	if (*szTmpSkinDir) {
		_tcscpy(szTmpSkinFile, szTmpSkinDir);
		_tcscat(szTmpSkinFile, filename);

		hFile = FindFirstFile(szTmpSkinFile, &FindData);
		if(hFile != INVALID_HANDLE_VALUE) {   
			FindClose(hFile);
			_tcscpy( szSkinFile, szTmpSkinFile );
			return;
		}	  
	}


	// it wasn't in settingsDir, check the current skin   

	_tcscat(szTmpSkinDir, _T("Skins\\"));
	if (_FindSkinFile(szSkinFile, szTmpSkinDir, filename))
		return;

	kPlugin.kFuncs->GetFolder(SkinsFolder, folder, MAX_PATH);
	strcat(folder, "\\");
#if _UNICODE
	utf8_to_utf16(folder, szTmpSkinDir, MAX_PATH);
#else
	utf8_to_ansi(folder, szTmpSkinDir, MAX_PATH);
#endif
	if (_FindSkinFile(szSkinFile, szTmpSkinDir, filename)) 
		return;

	// it wasn't anywhere, return the path to the settingsDir, in case the file is being created
	kPlugin.kFuncs->GetFolder(UserSettingsFolder, folder, MAX_PATH);
	strcat(folder, "\\");
#if _UNICODE
	utf8_to_utf16(folder, szTmpSkinDir, MAX_PATH);
#else
	utf8_to_ansi(folder, szTmpSkinDir, MAX_PATH);
#endif
	_tcscpy(szSkinFile, szTmpSkinDir);
	if (! *folder)      // no settingsDir, bad
		_tcscpy(szSkinFile, filename);
	else
		_tcscat(szSkinFile, filename);
}
