#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <ctime>
#include <string>
#include <sstream>

uintptr_t modulebaseaddr;
DWORD procId;
HWND csgowindow;
HANDLE hProcess;
bool toggle = true;


uintptr_t GetmodulebaseAddress(const char* modName) { 
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}


void polymorphic() 
{
	std::srand(std::time(0)); 

	int count = 0;
	for (count; count < 10; count++)
	{
		int index = rand() % (6 - 0 + 1) + 0; 

		switch (index)
		{
		case 0:


			__asm __volatile 
			{
				sub eax, 3
				add eax, 1
				add eax, 2
			}


		case 1:

			__asm __volatile
			{
				push eax
				pop eax
			}

		case 2:

			__asm __volatile
			{
				inc eax
				dec eax
			}


		case 3:
			__asm __volatile
			{
				dec eax
				add eax, 1
			}



		case 4:

			__asm __volatile
			{
				pop eax
				push eax
			}

		case 5:

			__asm __volatile
			{
				mov eax, eax
				sub eax, 1
				add eax, 1
				

			}

		case 6:


			__asm __volatile
			{
				xor eax, eax
				mov eax, eax
			}


		}

	}
}

struct ouroffsets
{

	uintptr_t localPlayeraddr = 0xD3DD14;
	uintptr_t GlowIndex = 0xA438;
	uintptr_t GlowObject = 0x529A1D0;
	uintptr_t PlayerList = 0x4D5239C;
	uintptr_t Isdormant = 0xED;
	uintptr_t GetTeam = 0xF4;
	uintptr_t health = 0x100;
	int MaxPlayers = 64;


}addr;

struct glowconfig {
	float red = 0, green = 0, blue = 0, alpha = 1;
	uint8_t padding[8];
	float unknown = 1;
	uint8_t padding2[4];
	BYTE renderOccluded = 1;
	BYTE renderUnoccluded = 0;
	BYTE fullBloom = 0;
}config; 




uintptr_t Read(uintptr_t addr) { 
	uintptr_t val;
	ReadProcessMemory(hProcess, (LPVOID)addr, &val, sizeof(val), NULL);

	return val;
}

auto TitleGen = [](int num) { 
	std::string nameoftitle;
	for (int i = 0; i < num; i++)
		nameoftitle += rand() % 255 + 1;
	return nameoftitle;
};


int main() {

	polymorphic();
	SetConsoleTitleA(TitleGen(rand() % 100 + 30).c_str());

	
	HWND hconsole = GetConsoleWindow();
	SetWindowPos(hconsole, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	std::cout << "Made by taco#1736" << std::endl;
	std::cout << "Its free do not pay anything." << std::endl;
	std::cout << "Press F1 to toggle on and off" << std::endl;
	Sleep(1000);
		
	csgowindow = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(csgowindow, &procId);
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, NULL, procId);
	modulebaseaddr = GetmodulebaseAddress("client.dll");



	while (true)
	{

		if (GetAsyncKeyState(VK_F1) & 1)
		{
			toggle = !toggle;
		}

		if (toggle)
		{

			uintptr_t LocalPlayer = Read(modulebaseaddr + addr.localPlayeraddr); 

			do
			{
				uintptr_t LocalPlayer = Read(modulebaseaddr + addr.localPlayeraddr);
			} while (LocalPlayer == NULL); 

			uintptr_t dwGlowManager = Read(modulebaseaddr + addr.GlowObject);
			int LocalTeam = Read(LocalPlayer + addr.GetTeam);

			polymorphic();
			
			for (int i = 1; i < addr.MaxPlayers; i++) {
				uintptr_t dwEntity = Read(modulebaseaddr + addr.PlayerList + i * 0x10);
				int GlowIndex = Read(dwEntity + addr.GlowIndex);
				int EntityHealth = Read(dwEntity + addr.health);
				int IsDormant = Read(dwEntity + addr.Isdormant);
				int EntityTeam = Read(dwEntity + addr.GetTeam);
				polymorphic();
				if (LocalTeam == EntityTeam && IsDormant == NULL && EntityHealth != 0)
				{
					config.red = 0;
					config.green = 0.5;
					config.blue = 0.5;


					WriteProcessMemory(hProcess, (LPVOID)(dwGlowManager + (GlowIndex * 0x38) + 0x4), &config, sizeof(config), NULL); 
					
				
				}
				else if (LocalTeam != EntityTeam && IsDormant == NULL && EntityHealth != 0)
				{
					config.red = 0.9;
					config.green = 0.2;
					config.blue = 0;
					WriteProcessMemory(hProcess, (LPVOID)(dwGlowManager + (GlowIndex * 0x38) + 0x4), &config, sizeof(config), NULL);
					
				}
				polymorphic();
			}
		}
		Sleep(1);
	}

	
}
