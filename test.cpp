#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <codecvt>

#include <windows.h>
#include <tlhelp32.h>
#include <locale.h>

// std::setlocale(LC_ALL, "");

std::wstring current;
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
				std::wcout << "Title: " << title << "\n\n";
				x_title = title;
				x_title.pop_back();
				return FALSE;
			}
		}
	}

	return TRUE;
}

int main()
{
	setlocale(LC_ALL, "");
	// std::wstring	chinese(L"三");
	// std::wcout << chinese << std::endl;
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
		if (current != x_title && x_title != L"Spotify Premium")
		{
			// https://stackoverflow.com/a/48826838/14512379
			// const uint16_t bom = 0xFEFF;
			std::wofstream	output(path_output);
			// output.write(reinterpret_cast<const char*>(x_title.data()), x_title.size() * sizeof(wchar_t));
			const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
			output.imbue(utf8_locale);
			output << x_title;
			output.close();
			std::cout << "here" << std::endl;
			current = x_title;
		}
		else if (x_title == L"Spotify Premium")
		{
			std::cout << "Nothing currently playing, or window closed" << std::endl;
			std::wofstream	output(path_output);
			output << "";
			output.close();
			current = x_title;
		}
		Sleep(5000);
	}
}