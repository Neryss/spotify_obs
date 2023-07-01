#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <codecvt>
#include <thread>

#include <windows.h>
#include <tlhelp32.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>

std::wstring current = L"unknown";
std::wstring x_title = L"unknown";
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
				std::wstring tmp(title);
				tmp.pop_back();
				printf("%s\n", tmp);
				if (tmp != L"Default IME" && tmp != L"GDI+ Window (Spotify.exe)" && tmp != L"MSCTFIME UI")
				{
					x_title = title;
					x_title.pop_back();
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void	writeToFile()
{
	std::wofstream	output(path_output);
	const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
	output.imbue(utf8_locale);
	output << x_title;
	// std::wcout << "\x1B[2J\x1B[H";
	std::wcout << "currently playing: \n"  << x_title;
	output.close();
	current = x_title;
}

void	noSong()
{
	// std::wcout << "\x1B[2J\x1B[H";
	std::wcout << "Nothing currently playing, or window closed" << std::endl;
	std::wofstream	output(path_output);
	output << "";
	output.close();
	current = x_title;
}

void	test()
{
	std::wifstream ifs;
	ifs.open("out.txt");
	std::wofstream display("test.txt");
	if (!ifs.is_open())
	{
		std::wcout << "Error before" << std::endl;
		return ;
	}
	std::wstring line;
	const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
	display.imbue(utf8_locale);
	ifs.imbue(utf8_locale);
	while (true)
	{
		std::getline(ifs, line);
		if (!ifs.is_open())
		{
			std::wcout << "Error" << std::endl;
			break;
		}
		if (!display.is_open())
		{
			std::wcout << "Error display" << std::endl;
			break;
		}
		display.write(line.c_str(), line.size());
		display.flush();
		std::wcout << "File content: " << line << std::endl;
		std::wcout << "Sexe" << std::endl;
		Sleep(1000);
	}
	display.close();
}

void	test2()
{
	
}

//	TODO: add subprocess to handle text display
int main()
{
	_setmode(_fileno(stdout),_O_U16TEXT);
	std::thread t1(test);
	// t1.join();
	// while(true)
	// {
	// 	std::wcout << "Main" << std::endl;
	// 	Sleep(4000);
	// }
	while(true)
	{
		std::vector<DWORD> pids;

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		// Do use a proper RAII type for this so it's robust against exceptions and code changes.
		auto cleanupSnap = [snap]
		{ CloseHandle(snap); };

		PROCESSENTRY32W entry;
		entry.dwSize = sizeof entry;
		std::wcout << "Fack" << std::endl;

		if (!Process32FirstW(snap, &entry))
		{
			std::wcout << "WTF" << std::endl;
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
			writeToFile();
		else if (x_title == L"Spotify Premium")
			noSong();
		Sleep(5000);
	}
}