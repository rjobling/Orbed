////////////////////////////////////////////////////////////////////////////////
// winuaemem.cpp
////////////////////////////////////////////////////////////////////////////////

#include "winuaemem.h"
#include <windows.h>
#include <tlhelp32.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static uint FindProcID()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	uint id = 0;

	for (BOOL success = Process32First(snapshot, &entry); success; success = Process32Next(snapshot, &entry))
	{
		if (strcmp(entry.szExeFile, "winuae-gdb.exe") == 0)
		{
			id = entry.th32ProcessID;

			break;
		}

		if (strcmp(entry.szExeFile, "winuae64.exe") == 0)
		{
			id = entry.th32ProcessID;
		}
	}

	CloseHandle(snapshot);

	return id;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void* FindLabel(const char* label, size_t length, char* p, size_t size)
{
	while (size > length)
	{
		if (strncmp(p, label, length) == 0)
		{
			return p;
		}

		p++;
		size--;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool WinUAEMem_Aquire(WinUAEMem& mem, const char* label)
{
	uint id = FindProcID();

	if (id == 0)
	{
		return false;
	}

	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);

	if (proc == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	size_t length = strlen(label);

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	u8* buffer = nullptr;
	size_t size = 0;

	for (u8* base = nullptr; base < sysInfo.lpMaximumApplicationAddress; base += size)
	{
		MEMORY_BASIC_INFORMATION memInfo;
		if (VirtualQueryEx(proc, base, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
		{
			break;
		}

		base = (u8*) memInfo.BaseAddress;
		size = memInfo.RegionSize;

		if ((memInfo.State != MEM_COMMIT) || (memInfo.Protect != PAGE_READWRITE))
		{
			continue;
		}

		buffer = (u8*) malloc(size);

		assert_pointer(buffer);

		if (ReadProcessMemory(proc, base, buffer, size, nullptr))
		{
			Mem_Tag* tag = (Mem_Tag*) FindLabel(label, length, (char*) buffer, size);

			if (tag != nullptr)
			{
				mem = {
					.proc = proc,
					.base = base,
					.size = size,
					.buffer = buffer,
					.tag = tag,
				};

				return true;
			}
		}

		free(buffer);
	}

	CloseHandle(proc);

	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void WinUAEMem_Release(WinUAEMem& mem)
{
	if (mem.proc != nullptr)
	{
		free(mem.buffer);

		CloseHandle(mem.proc);
	}

	mem = {};
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool WinUAEMem_Read(WinUAEMem& mem, void* p, size_t size)
{
	assert_pointer(mem.proc);
	assert((p >= mem.buffer) && (((u8*) p + size) <= (mem.buffer + mem.size)));

	size_t offset = (u8*) p - mem.buffer;

	return ReadProcessMemory(mem.proc, mem.base + offset, p, size, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool WinUAEMem_Write(WinUAEMem& mem, const void* p, size_t size)
{
	assert_pointer(mem.proc);
	assert((p >= mem.buffer) && (((u8*) p + size) <= (mem.buffer + mem.size)));

	size_t offset = (u8*) p - mem.buffer;

	return WriteProcessMemory(mem.proc, mem.base + offset, p, size, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void* WinUAEMem_MakeHostPointer(WinUAEMem& mem, uae_ptr tp)
{
	assert_pointer(mem.proc);

	int offset = (int) (tp - mem.tag->self);

	void* hp = ((u8*) mem.tag) + offset;

	assert((hp >= mem.buffer) && (hp < (mem.buffer + mem.size)));

	return hp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
uae_ptr WinUAEMem_MakeTargetPointer(WinUAEMem& mem, void* hp)
{
	assert_pointer(mem.proc);
	assert((hp >= mem.buffer) && (hp < (mem.buffer + mem.size)));

	int offset = (int) (((u8*) hp) - ((u8*) mem.tag));

	uae_ptr tp = (uae_ptr) (((int) mem.tag->self) + offset);

	return tp;
}
