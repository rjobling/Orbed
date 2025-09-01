////////////////////////////////////////////////////////////////////////////////
// chunkymem.cpp
////////////////////////////////////////////////////////////////////////////////

#include "intromem.h"
#include "hardware\blit.h"
#include "imgui\imgui.h"
#include "framework.h"
#include "orbed.h"
#include "rocket.h"
#include "winuaemem.h"

#pragma warning(push)
#pragma warning(disable : 4121)
#pragma pack(push, 2)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static constexpr int kPaletteCount = 128;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct CopList
{
	CopCommand topwait;

	CopCommand topcolor0;
	CopCommand topcolor1;
	CopCommand topcolor2;
	CopCommand topcolor3;

	CopCommand topbpl1mod;
	CopCommand topbpl2mod;

	CopCommand midwait;

	CopCommand midcolor0;
	CopCommand midcolor1;
	CopCommand midcolor2;
	CopCommand midcolor3;

	CopCommand midbpl1mod;
	CopCommand midbpl2mod;

	CopCommand end;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Intro_Data
{
	big_u32 label[2];
	uae_ptr self;

	bool pause;

	big_s32 frame;

	big_s16 scroll;
	big_s16 palIndex;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Intro_Chip
{
	big_u32 label[2];
	uae_ptr self;

	CopList copList;
};

#pragma pack(pop)
#pragma warning(pop)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Host_Data
{
	int scroll;
	int palIndex;
	int midHpos;
	int midVpos;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static WinUAEMem sDataMem = {};
static WinUAEMem sChipMem = {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static Host_Data sHostData = {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool IntroMem_Aquire()
{
	if (sDataMem.tag != nullptr)
	{
		return true;
	}

	if (!WinUAEMem_Aquire(sDataMem, "IntrData"))
	{
		return false;
	}

	if (!WinUAEMem_Aquire(sChipMem, "IntrChip"))
	{
		WinUAEMem_Release(sDataMem);

		return false;
	}

	Rocket_Register(&sHostData.scroll, "Intro:scroll", -255, 255);
	Rocket_Register(&sHostData.palIndex, "Intro:palIndex", 0, kPaletteCount - 1);
	Rocket_Register(&sHostData.midHpos, "Intro:midHpos", 0, 127);
	Rocket_Register(&sHostData.midVpos, "Intro:midVPos", 0, 255);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void IntroMem_Release()
{
	if (sDataMem.tag == nullptr)
	{
		return;
	}

	Rocket_Unregister(&sHostData.scroll);
	Rocket_Unregister(&sHostData.palIndex);
	Rocket_Unregister(&sHostData.midHpos);
	Rocket_Unregister(&sHostData.midVpos);

	WinUAEMem_Release(sDataMem);
	WinUAEMem_Release(sChipMem);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool IntroMem_Update()
{
	if (sDataMem.tag == nullptr)
	{
		return false;
	}

	Intro_Data& data = (Intro_Data&) *sDataMem.tag;
	Intro_Chip& chip = (Intro_Chip&) *sChipMem.tag;

	bool connected = WinUAEMem_Read(sDataMem, &data, sizeof(Intro_Data)) &&
					 WinUAEMem_Read(sChipMem, &chip, sizeof(Intro_Chip));

	ImGui::Text("Intro");

	if (ImGui::Checkbox("Pause", &data.pause))
	{
		WinUAEMem_Write(sDataMem, &data.pause, sizeof(data.pause));
	}

	if (!data.pause)
	{
		sHostData.scroll = (int) data.scroll;
		sHostData.palIndex = (int) data.palIndex;
		sHostData.midHpos = (((int) chip.copList.midwait.inst) & 0x00ff) >> 1;
		sHostData.midVpos = (((int) chip.copList.midwait.inst) & 0xff00) >> 8;
	}

	ImGui::SliderInt("Scroll", &sHostData.scroll, -128, 128);
	ImGui::SliderInt("PalIndex", &sHostData.palIndex, 0, kPaletteCount - 1);
	ImGui::SliderInt("MidHpos", &sHostData.midHpos, 0, 127);
	ImGui::SliderInt("MidVpos", &sHostData.midVpos, 0, 255);

	if (data.pause)
	{
		data.scroll = (big_s16) sHostData.scroll;
		data.palIndex = (big_s16) sHostData.palIndex;
		chip.copList.midwait.inst = (big_u16) ((sHostData.midVpos << 8) | (sHostData.midHpos << 1) | 1);

		WinUAEMem_Write(sDataMem, &data.scroll, sizeof(data.scroll));
		WinUAEMem_Write(sDataMem, &data.palIndex, sizeof(data.palIndex));
		WinUAEMem_Write(sChipMem, &chip.copList.midwait.inst, sizeof(chip.copList.midwait.inst));
	}

	return connected;
}
