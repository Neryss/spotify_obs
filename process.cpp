#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <tlhelp32.h>

bool found(const PROCESSENTRY32W &entry)
{
	return std::wstring(entry.szExeFile) == L"Spotify.exe";
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
{
	const auto &pids = *reinterpret_cast<std::vector<DWORD> *>(lParam);

	DWORD winId;
	GetWindowThreadProcessId(hwnd, &winId);

	for (DWORD pid : pids)
	{
		if (winId == pid)
		{
			std::wstring title(GetWindowTextLength(hwnd) + 1, L'\0');
			GetWindowTextW(hwnd, &title[0], title.size()); // note: C++11 only

			if (title.length() > 1)
			{
				std::cout << "Found window:\n";
				std::cout << "Process ID: " << pid << '\n';
				std::wcout << "Title: " << title << "\n\n";
			}
		}
	}

	return TRUE;
}

void	printProcess(DWORD processID)
{
	TCHAR	process_name[MAX_PATH] = TEXT("<unknown>");
	HANDLE	h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (h_process)
	{
		HMODULE	h_mod = nullptr;
		DWORD	cb_needed = 0;
		if (EnumProcessModules(h_process, &h_mod, sizeof(h_mod), &cb_needed))
			GetModuleBaseName(h_process, h_mod, process_name, sizeof(process_name)/sizeof(TCHAR));
	}
	if (!strcmp((char *)process_name, "Spotify.exe"))
		std::cout << process_name << " (PID: " << processID << ")" << std::endl;
	CloseHandle(h_process);
}

int	main()
{
	DWORD	a_process[1024];
	DWORD	cb_needed = 0;
	DWORD	c_process = 0;

	if (!EnumProcesses(a_process, sizeof(a_process), &cb_needed))
		return (1);
	
	c_process = cb_needed / sizeof(DWORD);

	for (unsigned int i = 0; i < c_process; i++)
	{
		if (a_process[i])
			printProcess(a_process[i]);
	}
	return (0);
}