#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <windows.h>
#include <tlhelp32.h>

std::wstring x_title;
std::string	path_output = "./out.txt";

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
				// std::cout << "Found window:\n";
				// std::cout << "Process ID: " << pid << '\n';
				// std::wcout << "Title: " << title << "\n\n";
				x_title = title;
				return FALSE;
			}
		}
	}

	return TRUE;
}

int main()
{
	while(true)
	{
		std::vector<DWORD> pids;

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		// Do use a proper RAII type for this so it's robust against exceptions and code changes.
		auto cleanupSnap = [snap]
		{ CloseHandle(snap); };

		PROCESSENTRY32W entry;
		entry.dwSize = sizeof entry;

		if (!Process32FirstW(snap, &entry))
		{
			cleanupSnap();
			return 0;
		}

		do
		{
			if (found(entry))
				pids.emplace_back(entry.th32ProcessID);
		} while (Process32NextW(snap, &entry));
		cleanupSnap();

		EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&pids));
		std::wcout << x_title << std::endl;

		std::wofstream	output(path_output);
		output << x_title << std::endl;
		output.close();
		Sleep(5000);
	}
}