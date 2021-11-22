#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <cstdio>
#include <string>
#include "utils.h"
#include "writefile.h"
#include "readfile.h"
#include "checkpass.h"

void createarchive(const fs::path& fn, const std::string& extension = ".eua")
{
	print("Välj lösenord: ");
	std::string password;
	int c;
	while (c = std::getchar(), c != '\n' && c != EOF)
		password += (char)c;

	print("Ange lösenord igen: ");
	std::string password2;
	while (c = std::getchar(), c != '\n' && c != EOF)
		password2 += (char)c;
	
	if (password != password2)
	{
		std::puts("Lösenord matchar inte.");
		pressenter();
	}
	else
	{
		packfolder(fn.string() + extension, password);
	}
}

void createarchivekey(const fs::path& fn, const std::string& password, const std::string& extension = ".eua")
{
	packfolder(fn.string() + extension, password);
}

void unlockarchive(const fs::path& fn)
{
	print("Ange lösenord: ");
	std::string password;
	int c;
	while (c = std::getchar(), c != '\n' && c != EOF)
		password += (char)c;
	
	if (checkIfPasswordIsValidForFile(fn.string(), password))
	{
		unpackarchive(fn, password);
	}
	else
	{
		print("Lösenord är inkorrekt.\n");
		pressenter();
	}
}

void unlockarchivekey(const fs::path& fn, const std::string& password)
{
	if (checkIfPasswordIsValidForFile(fn.string(), password))
	{
		unpackarchive(fn, password);
	}
	else
	{
		print("Lösenord är inkorrekt.\n");
		pressenter();
	}
}

void printHelp()
{
	print(
		"Användning:\n"
		"    eua (flaggor) [arkiv] (flaggor)                Lås upp arkiv\n"
		"    eua (flaggor) [redan upplåst arkiv] (flaggor)  Lås arkiv igen\n"
		"    eua (flaggor) [mapp] (flaggor)                 Lås mapp i arkiv\n"
		"Flaggor:\n"
		"    -p [lösenord]  Fråga inte efter lösenord utan använd detta.\n"
		"    -q             Tyst läge.\n"
		"    -h             Visa hjälp.\n"
	);
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	fs::path fn;
	std::string key;
	for (int i = 1; i < argc; i++)
	{
		auto nextArg = [&]() -> void {
			if (++i >= argc)
			{
				printCrashMessage("Förväntade ett till argument.");
			}
		};

		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'h':
					printHelp();
				return 0;
				case 'p':
				{
					nextArg();
					key = argv[i];
				}
				break;
				case 'q':
					quiet = true;
				break;
				default:
					printCrashMessage(("Okänt argument \"" + std::string(argv[i]) + "\"").c_str());
			}
		}
		else
		{
			if (!fn.empty())
			{
				printCrashMessage("Endast ett fil- eller mappnamn får anges.");
			}
			fn = argv[i];
		}
	}

	if (fn.empty())
	{
		printHelp();
		return 1;
	}

	try
	{
		if (!fs::exists(fn))
		{
			printCrashMessage("Fil finns inte.");
		}

		if (fs::is_directory(fn))
		{
			if (key.empty())
				createarchive(fn);
			else
				createarchivekey(fn, key);
		}
		else
		{
			if (!fn.has_extension())
			{
				printCrashMessage("Arkiv måste ha en filändelse.");
			}

			if (fs::file_size(fn))
				if (key.empty())
					unlockarchive(fn);
				else
					unlockarchivekey(fn, key);
			else
				if (key.empty())
					createarchive(fn.parent_path() / fn.stem(), fn.extension().string());
				else
					createarchivekey(fn.parent_path() / fn.stem(), key, fn.extension().string());
		}
	}
	catch (const std::exception& e)
	{
		printCrashMessage(e.what());
	}
	return 0;
}