#include <iostream>
#include <windows.h>
#include <string>
#include <ctime>

LPCSTR lGameWindow = "AssaultCube";
std::string gameName = "AssaultCube";
std::string gameStatus;

bool isGameAvail;
bool update;

// Ammo Vars
bool ammoStatus;
BYTE ammoVal[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD ammoBaseAddy = { 0x00509B74 };
DWORD ammoOffsets[] = { 0x384, 0x14, 0x0 };

// Health Vars
bool healthStatus;
BYTE healthVal[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD healthBaseAddy = { 0x00509B74 };
DWORD healthOffsets[] = { 0xF8 };

DWORD findDmaAddy(int pointer, HANDLE hProcHandle, DWORD Offsets[], DWORD baseAddress);
void writeToMemory(HANDLE hProcHandle);

int main()
{
	HWND hGameWindow = NULL;
	DWORD procId = NULL;
	HANDLE hProcHandle = NULL;

	int lastUpdate = clock();
	int gameAvailTimer = clock();
	int onePressTimer = clock();

	update = true;

	std::string sAmmoStatus = "OFF";
	std::string sHealthStatus = "OFF";

	while (!GetAsyncKeyState(VK_INSERT))
	{
		if (clock() - gameAvailTimer > 100)
		{
			gameAvailTimer = clock();
			isGameAvail = false;

			hGameWindow = FindWindow(NULL, lGameWindow);
			if (hGameWindow)
			{
				GetWindowThreadProcessId(hGameWindow, &procId);
				if (procId)
				{
					hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
					if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL)
					{
						gameStatus = "Failed to obtain handle on process";
					}
					else
					{
						gameStatus = "AssaultCube Ready";
						isGameAvail = true;
					}
				}
				else
				{
					gameStatus = "Failed to get process id";
				}
			}
			else
			{
				gameStatus = "AssaultCube not found";
			}
			if (update || clock() - lastUpdate > 5000)
			{
				system("cls");
				std::cout << "----------------------------------------" << std::endl;
				std::cout << "           AssaultCube Hack" << std::endl;
				std::cout << "----------------------------------------" << std::endl << std::endl;
				std::cout << "Game status: " << gameStatus << std::endl << std::endl;
				std::cout << "[F1] Unlimted Ammo --> " << sAmmoStatus << " <-- " << std::endl << std::endl;
				std::cout << "[F2] Unlimted Health --> " << sHealthStatus << " <-- " << std::endl << std::endl;
				std::cout << "[Insert] Exit " << std::endl;
				update = false;
				lastUpdate = clock();
			}
			if (isGameAvail)
			{
				writeToMemory(hProcHandle);
			}
		}
		if (clock() - onePressTimer > 400)
		{
			if (isGameAvail)
			{
				if (GetAsyncKeyState(VK_F1))
				{
					onePressTimer = clock();
					ammoStatus = !ammoStatus;
					update = true;

					if (ammoStatus)
					{
						sAmmoStatus = "ON";
					}
					else
					{
						sAmmoStatus = "OFF";
					}
				}
				else if (GetAsyncKeyState(VK_F2))
				{
					onePressTimer = clock();
					healthStatus = !healthStatus;
					update = true;

					if (healthStatus)
					{
						sHealthStatus = "ON";
					}
					else
					{
						sHealthStatus = "OFF";
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

DWORD findDmaAddy(int pointerLevel, HANDLE hProcHandle, DWORD offsets[], DWORD baseAddress)
{
	DWORD ptr = baseAddress;
	DWORD pTemp;
	DWORD ptrAddy;
	for (int i = 0; i < pointerLevel; i++)
	{
		if (i == 0)
		{
			ReadProcessMemory(hProcHandle, (LPCVOID)ptr, &pTemp, sizeof(pTemp), NULL);
		}
		ptrAddy = pTemp + offsets[i];
		ReadProcessMemory(hProcHandle, (LPCVOID)ptrAddy, &pTemp, sizeof(pTemp), NULL);
	}
	return ptrAddy;
}

void writeToMemory(HANDLE hProcHandle)
{
	DWORD addyToWrite;
	if (ammoStatus)
	{
		addyToWrite = findDmaAddy(3, hProcHandle, ammoOffsets, ammoBaseAddy);
		WriteProcessMemory(hProcHandle, (BYTE*)addyToWrite, &ammoVal, sizeof(ammoVal), NULL);
	}

	if (healthStatus)
	{
		addyToWrite = findDmaAddy(1, hProcHandle, healthOffsets, healthBaseAddy);
		WriteProcessMemory(hProcHandle, (BYTE*)addyToWrite, &healthVal, sizeof(healthVal), NULL);
	}
}