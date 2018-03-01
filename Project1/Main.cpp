// Author: pr0n1s
// Credits: Fleep

#include <iostream>
#include <windows.h>
#include <string>
#include <ctime>

LPCSTR lGameWindow = "AssaultCube";
std::string sGameName = "AssaultCube";
std::string sGameStatus;

bool bIsGameAvailable;
bool bUpdate;
bool bAmmoStatus;
bool bHealthStatus;
bool bArmorStatus;
bool bRapidFireStatus;
bool bNoRecoilStatus;
bool bElevateStatus;

DWORD dwPlayerBase = { 0x00509B74 };

//Todo
DWORD dwEntityBase;

BYTE bAmmoValue[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD dwAmmoOffSets = { 0x150 };

BYTE bHealthValue[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD dwHealthOffsets = { 0xF8 };

BYTE fCarbineTimerVal[] = { 0xFF, 0xFF, 0xD8, 0xF0 };
DWORD dwCarbineTimerOffset = { 0x178 };

BYTE bArmorValue = { 0x32 };
DWORD dwArmorOffset = { 0xFC };

BYTE bNoRecoilValue[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
DWORD dwNoRecoilAddress = { 0x463786 };

FLOAT fElevateValue = { 40.0 };
FLOAT fElevateOldValue = { 4.5 };
DWORD dwElevateOffset = { 0x5C };

// Can be converted to deal with multi-level pointers
DWORD dwGetAddressToWriteTo(int pointerLevel, HANDLE hProcHandle, DWORD offsets, DWORD baseAddress) {
	DWORD dwBase = baseAddress;
	DWORD dwTmp;
	DWORD dwAddressToWriteTo;
	
	for (int i = 0; i < pointerLevel; i++) {
		
		if (i == 0) {
			ReadProcessMemory(hProcHandle, (LPCVOID)dwBase, &dwTmp, sizeof(dwTmp), NULL);
		}
		dwAddressToWriteTo = dwTmp + offsets;
		ReadProcessMemory(hProcHandle, (LPCVOID)dwAddressToWriteTo, &dwTmp, sizeof(dwTmp), NULL);
	}
	return dwAddressToWriteTo;
}

// Needs to be refactored 
void vWriteToMemory(HANDLE hProcHandle) {
	DWORD dwAddressToWriteTo;
	
	if (bAmmoStatus) {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwAmmoOffSets, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &bAmmoValue, sizeof(bAmmoValue), NULL);
	}

	if (bHealthStatus) {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwHealthOffsets, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &bHealthValue, sizeof(bHealthValue), NULL);
	}

	if (bRapidFireStatus) {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwCarbineTimerOffset, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &fCarbineTimerVal, sizeof(fCarbineTimerVal), NULL);
	}

	if (bNoRecoilStatus) {
		WriteProcessMemory(hProcHandle, (LPVOID)(dwNoRecoilAddress), &bNoRecoilValue, sizeof(bNoRecoilValue), NULL);
	}

	if (bArmorStatus) {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwArmorOffset, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &bArmorValue, sizeof(bArmorValue), NULL);
	}

	if (bElevateStatus) {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwElevateOffset, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &fElevateValue, sizeof(fElevateValue), NULL);
	} else {
		dwAddressToWriteTo = dwGetAddressToWriteTo(1, hProcHandle, dwElevateOffset, dwPlayerBase);
		WriteProcessMemory(hProcHandle, (LPVOID)dwAddressToWriteTo, &fElevateOldValue, sizeof(fElevateOldValue), NULL);
	}
}

int main() {
	HWND hGameWin = NULL;
	DWORD dwProcId = NULL;
	HANDLE hProcHandle = NULL;

	int iLastUpdate = clock();
	int iGameTimer = clock();
	int iEnabledTimer = clock();

	std::string sAmmoStatus = "OFF";
	std::string sHealthStatus = "OFF";
	std::string sArmorStatus = "OFF";
	std::string sRapidFireStatus = "OFF";
	std::string sNoRecoilStatus = "OFF";
	std::string sElevateStatus = "OFF";

	bUpdate = true;

	while (!GetAsyncKeyState(VK_INSERT)) {

		if (clock() - iGameTimer > 100) {
			iGameTimer = clock();
			bIsGameAvailable = false;

			hGameWin = FindWindow(NULL, lGameWindow);
			if (!hGameWin) {
				sGameStatus = "AssualtCube not found";

			}
			else {
				GetWindowThreadProcessId(hGameWin, &dwProcId);

				if (!dwProcId) {
					sGameStatus = "Failed to get process id";

				}
				else {
					hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);

					if (!hProcHandle || hProcHandle == INVALID_HANDLE_VALUE) {
						sGameStatus = "Failed to obtain handle on process";

					}
					else {
						sGameStatus = "AssaultCube ready!";
						bIsGameAvailable = true;
					}
				}
			}

			// Find a better way to do this
			if (bUpdate || clock() - iLastUpdate > 5000) {
				system("cls");
				std::cout << "----------------------------------------" << std::endl;
				std::cout << "           AssaultCube Hack" << std::endl;
				std::cout << "----------------------------------------" << std::endl;
				std::cout << "Game status: \t\t\t" << sGameStatus << std::endl;
				std::cout << "[F1] Unlimted Ammo \t--> \t\t" << sAmmoStatus << std::endl;
				std::cout << "[F2] Unlimted Health \t--> \t\t" << sHealthStatus << std::endl;
				std::cout << "[F3] Unlimited Armor \t--> \t\t" << sArmorStatus << std::endl;
				std::cout << "[F4] RapidFire \t\t--> \t\t" << sRapidFireStatus << std::endl;
				std::cout << "[F5] No Recoil \t\t--> \t\t" << sNoRecoilStatus << std::endl;
				std::cout << "[F6] Elevate \t\t--> \t\t" << sElevateStatus << std::endl;
				std::cout << "[Insert] Exit " << std::endl;

				bUpdate = false;
				iLastUpdate = clock();
			}

			if (bIsGameAvailable) {
				vWriteToMemory(hProcHandle);
			}
		}

		if (clock() - iEnabledTimer > 400) {

			if (GetAsyncKeyState(VK_F1)) {
				iEnabledTimer = clock();
				bAmmoStatus = !bAmmoStatus;
				bUpdate = true;

				(bAmmoStatus) ? sAmmoStatus = "ON" : sAmmoStatus = "OFF";
			}

			else if (GetAsyncKeyState(VK_F2)) {
				iEnabledTimer = clock();
				bHealthStatus = !bHealthStatus;
				bUpdate = true;

				(bHealthStatus) ? sHealthStatus = "ON" : sHealthStatus = "OFF";
			}

			else if (GetAsyncKeyState(VK_F3)) {
				iEnabledTimer = clock();
				bArmorStatus = !bArmorStatus;
				bUpdate = true;

				(bArmorStatus) ? sArmorStatus = "ON" : sArmorStatus = "OFF";
			}

			else if (GetAsyncKeyState(VK_F4)) {
				iEnabledTimer = clock();
				bRapidFireStatus = !bRapidFireStatus;
				bUpdate = true;

				(bRapidFireStatus) ? sRapidFireStatus = "ON" : sRapidFireStatus = "OFF";
			}

			else if (GetAsyncKeyState(VK_F5)) {
				iEnabledTimer = clock();
				bNoRecoilStatus = !bNoRecoilStatus;
				bUpdate = true;

				(bNoRecoilStatus) ? sNoRecoilStatus = "ON" : sNoRecoilStatus = "OFF";
			}

			else if (GetAsyncKeyState(VK_F6)) {
				iEnabledTimer = clock();
				bElevateStatus = !bElevateStatus;
				bUpdate = true;

				(bElevateStatus) ? sElevateStatus = "ON" : sElevateStatus = "OFF";
			}
		}
	}
	return 0;
}