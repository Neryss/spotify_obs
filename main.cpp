/*
// Don't gatekeep, share it!
// @neryss002
*/

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


// Do not change that
std::wstring current = L"unknown";
std::wstring x_title = L"unknown";

/*
// If you want to change output paths you can do so here
// <path_ouput> is the "buffer" file to read the complete title
// <display_path> is what you should read from OBS to get the animation
*/
std::string	output_path = "./out.txt";
std::string	display_path = "./display.txt";

// You can change the char limit before the animation starts looping
int			char_limit = 40;

// number of seconds between each animation step
int			animation_speed = 1;

// number of seconds between each Windows snapshot
int			title_poll_rate = 5;

// TODO: fix ugly thingy
int	parseConfig()
{
	std::ifstream	conf("config_file.cf");
	if (!conf)
	{
		std::cout << "Error reading config file" << std::endl;
		exit(1);
	}
	std::string					tmp;
	while (std::getline(conf, tmp))
	{
		unsigned int	name = tmp.find("=");
		std::string		s_name = tmp.substr(0, name);
		unsigned int	first = tmp.find("\"");
		unsigned int	last = tmp.find_last_of("\"");
		if (s_name == "output")
		{
			output_path = tmp.substr(first + 1, last - first - 1 );
			std::cout << "output: " << output_path << std::endl;
		}
		else if (s_name == "display")
		{
			display_path = tmp.substr(first + 1, last - first - 1 );
			std::cout << "display: " << display_path << std::endl;
		}
		else if (s_name == "limit")
		{
			std::string	part = tmp.substr(name + 1, tmp.length() - name);
			if (!std::any_of(part.begin(), part.end(), ::isdigit))
			{
				std::cout << part << " isn't a number" << std::endl;
				exit(1);
			}
			char_limit = std::stoi(part);
			std::cout << "limit: " << char_limit << std::endl;
		}
		else if (s_name == "animation_speed")
		{
			std::string	part = tmp.substr(name + 1, tmp.length() - name);
			if (!std::any_of(part.begin(), part.end(), ::isdigit))
			{
				std::cout << part << " isn't a number" << std::endl;
				exit(1);
			}
			animation_speed = std::stoi(part);
			std::cout << "anim_speed: " << animation_speed << std::endl;
		}
		else if (s_name == "poll_rate")
		{
			std::string	part = tmp.substr(name + 1, tmp.length() - name);
			if (!std::any_of(part.begin(), part.end(), ::isdigit))
			{
				std::cout << part << " isn't a number" << std::endl;
				exit(1);
			}
			title_poll_rate = std::stoi(part);
			std::cout << "poll_rate: " << title_poll_rate << std::endl;
		}
		else
		{
			std::cout << "Wrong field name: " << tmp << std::endl;
			std::cout << "refer to the readme.MD" << std::endl;
		}
	}
	return (0);
}

bool found(const PROCESSENTRY32W &entry)
{
	return std::wstring(entry.szExeFile) == L"Spotify.exe";
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
{
	const auto	&pids = *reinterpret_cast<std::vector<DWORD> *>(lParam);

	DWORD		winId;
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
	std::wofstream		output(output_path);
	const std::locale	utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
	output.imbue(utf8_locale);
	output << x_title;
	std::wcout << "\x1B[2J\x1B[H";
	std::wcout << "currently playing: \n"  << x_title;
	output.close();
	current = x_title;
}

void	noSong()
{
	std::wcout << "\x1B[2J\x1B[H";
	std::wcout << "Nothing currently playing, or window closed" << std::endl;
	std::wofstream	output(output_path);
	output << "";
	output.close();
	current = x_title;
}

// Used for animate ft
std::wstring	actual = L"None";
bool			first = true;
bool			has_changed = false;

void	animate()
{
	std::wifstream		ifs;
	std::wstring		line;
	std::wstring		str;
	const std::locale	utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
	while (true)
	{
		ifs.open(output_path);
		ifs.imbue(utf8_locale);
		std::wofstream display(display_path, std::ios::trunc);
		display.imbue(utf8_locale);
		std::getline(ifs, line);
		if (line.length() == 0)
		{
			display << L"";
			display.close();
		}
		if (actual != line)
		{
			str = line;
			actual = str;
			has_changed = true;
		}
		if (!first && !has_changed && str.length() > char_limit)
		{
			wchar_t tmp = str[0];
			str = str.substr(1, str.length()) + tmp;
		}
		else if ((first || has_changed) && str.length() > char_limit)
		{
			has_changed = false;
			str.append(L" | ");
		}
		if (str.length() > char_limit)
		{
			std::wstring	tmp = L"♫ ";
			tmp.append(str.substr(0, char_limit));
			display << tmp;
		}
		else
		{
			std::wstring	tmp = L"♫ ";
			tmp.append(str);
			display << tmp;
		}
		first = false;
		display.close();
		ifs.close();
		Sleep(animation_speed * 1000);
	}
}

int main(int argc, char **argv)
{
	bool	debug = false;
	if (!argc > 1)
		if (!strcmp(argv[1], "-debug"))
			debug = true;
	parseConfig();
	if (debug)
	{
		std::cout << "return because of debug mode" << std::endl;
		exit(1);
	}
	_setmode(_fileno(stdout),_O_U16TEXT);
	std::thread	t1(animate);
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
			writeToFile();
		else if (x_title == L"Spotify Premium")
			noSong();
		Sleep(title_poll_rate * 1000);
	}
}
