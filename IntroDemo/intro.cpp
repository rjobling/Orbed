////////////////////////////////////////////////////////////////////////////////
// intro.cpp
////////////////////////////////////////////////////////////////////////////////

#include "intro.h"
#include <hardware/blit.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include "framework/core.h"
#include "framework/customhelpers.h"
#include "framework/lsp.h"
#include "framework/orbed.h"
#include "framework/system.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static constexpr int kViewWidth	 = 320;
static constexpr int kViewHeight = 256;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static constexpr int kImageWidth	 = 320;
static constexpr int kImageHeight	 = 320;
static constexpr int kImagePlanes	 = 2;
static constexpr int kImagePlaneSize = (kImageWidth / 8) * kImageHeight;
static constexpr int kImagePitch	 = kImageWidth / 8;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static constexpr int kPaletteSize  = 1 << kImagePlanes;
static constexpr int kPaletteCount = 128;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
INCBIN_CHIP(gImageBpls, "data/image_bpls.bin");
INCBIN(gPalettes, "data/palettes.bin");

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
INCBIN(gLSPMusic, "data/statetrue.lsmusic");
INCBIN_CHIP(gLSPBank, "data/statetrue.lsbank");

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct CopList
{
	CopCommand topwait = CopWait(0, 20);

	CopCommand topcolor0 = CopMove(color[0], ((const u16*) gPalettes)[0]);
	CopCommand topcolor1 = CopMove(color[1], ((const u16*) gPalettes)[1]);
	CopCommand topcolor2 = CopMove(color[2], ((const u16*) gPalettes)[2]);
	CopCommand topcolor3 = CopMove(color[3], ((const u16*) gPalettes)[3]);

	CopCommand topbpl1mod = CopMove(bpl1mod, 0);
	CopCommand topbpl2mod = CopMove(bpl2mod, 0);

	CopCommand midwait = CopWait(4, 44 + 192);

	CopCommand midcolor0 = CopMove(color[0], ~((const u16*) gPalettes)[0]);
	CopCommand midcolor1 = CopMove(color[1], ~((const u16*) gPalettes)[1]);
	CopCommand midcolor2 = CopMove(color[2], ~((const u16*) gPalettes)[2]);
	CopCommand midcolor3 = CopMove(color[3], ~((const u16*) gPalettes)[3]);

	CopCommand midbpl1mod = CopMove(bpl1mod, -kImagePitch * 3);
	CopCommand midbpl2mod = CopMove(bpl2mod, -kImagePitch * 3);

	CopCommand end = CopEnd();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Intro_Chip
{
	OrbedTag tag = {'Intr', 'Chip', this};

	CopList copList;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Intro_Data
{
	OrbedTag tag = {'Intr', 'Data', this};

	bool pause = false;

	s32 frame = 0;

	s16 scroll = 0;
	s16 palIndex = 0;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static Intro_Chip sChip __attribute__((section(".MEMF_CHIP")));
static Intro_Data sData;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool Intro_Init()
{
	custom.bplcon0 = PackBplcon0(kImagePlanes);
	custom.bplcon1 = PackBplcon1(0, 0);
	custom.bplcon2 = PackBplcon2(false, 4, 4);
	custom.diwstrt = PackDiwstrt(0, 0);
	custom.diwstop = PackDiwstop(kViewWidth, kViewHeight);
	custom.ddfstrt = PackDdfstrt(0);
	custom.ddfstop = PackDdfstop(kViewWidth);

	custom.bpl1mod = 0;
	custom.bpl2mod = 0;

	debug_register_bitmap(gImageBpls, "ImageBpls", kImageWidth, kImageHeight, kImagePlanes, 0);
	debug_register_palette(gPalettes, "Palette", kPaletteSize, 0);

	LSP_MusicDriver_CIA_Start(gLSPMusic, gLSPBank);
	System_SetAudioFilter(false);

	// Wait for the top of the frame and prepare to start.
	System_WaitVbl();

	// Prime the copper prior to starting.
	custom.cop1lc = (int) &sChip.copList;

	// Wait for the top of the frame before starting.
	System_WaitVbl();

	// Enable DMA which will start the copper.
	custom.dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER;

	// Enable interrupts when still nearly at the top of the frame.
	custom.intena = INTF_SETCLR | INTF_INTEN;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Intro_Deinit()
{
	System_WaitVbl();

	// Disable DMA which will stop the copper.
	custom.dmacon = DMAF_RASTER | DMAF_COPPER;

	LSP_MusicDriver_CIA_Stop();

	debug_unregister(gImageBpls);
	debug_unregister(gPalettes);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool Intro_Update()
{
	System_WaitVbl();

	if (!sData.pause)
	{
		sData.scroll = (16384 - cos(sData.frame << 9)) >> 9;
		sData.palIndex = (sData.frame >> 1) & (kPaletteCount - 1);

		sData.frame++;
	}

	const u8* bpls = ((const u8*) gImageBpls) + sData.scroll * kImagePitch;
	custom.bplpt[0] = (void*) (bpls + kImagePlaneSize * 0);
	custom.bplpt[1] = (void*) (bpls + kImagePlaneSize * 1);

	const u16* pal = &((const u16*) gPalettes)[sData.palIndex * kPaletteSize];
	sChip.copList.topcolor0.data = pal[0];
	sChip.copList.topcolor1.data = pal[1];
	sChip.copList.topcolor2.data = pal[2];
	sChip.copList.topcolor3.data = pal[3];
	sChip.copList.midcolor0.data = ~pal[0];
	sChip.copList.midcolor1.data = ~pal[1];
	sChip.copList.midcolor2.data = ~pal[2];
	sChip.copList.midcolor3.data = ~pal[3];

	return !System_TestLMB1();
}
