#include <gba.h>

#include "GUI.h"
#include "Shared/EmuMenu.h"
#include "Shared/AsmExtra.h"
#include "Main.h"
#include "FileHandling.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"
#include "ARM6809/Version.h"
#include "AY38910/Version.h"
#include "SonSonVideo/Version.h"

#define EMUVERSION "V0.1.5 2022-08-23"

const fptr fnMain[] = {nullUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI, subUI};

const fptr fnList0[] = {uiDummy};
const fptr fnList1[] = {ui2, ui3, ui4, ui5, ui6, ui7, gbaSleep, resetGame};
const fptr fnList2[] = {ui8, loadState, saveState, saveSettings, resetGame};
const fptr fnList3[] = {autoBSet, autoASet, controllerSet, swapABSet};
const fptr fnList4[] = {scalingSet, flickSet, gammaSet, bgrLayerSet, sprLayerSet};
const fptr fnList5[] = {speedSet, autoStateSet, autoSettingsSet, autoPauseGameSet, debugTextSet, sleepSet};
const fptr fnList6[] = {coinageSet, coinAffectSet, demoSet, livesSet, difficultSet, bonusSet, secondPlayerSet, flipSet, serviceSet, freezeSet};
const fptr fnList7[] = {uiDummy};
const fptr fnList8[] = {quickSelectGame, quickSelectGame};
const fptr *const fnListX[] = {fnList0, fnList1, fnList2, fnList3, fnList4, fnList5, fnList6, fnList7, fnList8};
const u8 menuXItems[] = {ARRSIZE(fnList0), ARRSIZE(fnList1), ARRSIZE(fnList2), ARRSIZE(fnList3), ARRSIZE(fnList4), ARRSIZE(fnList5), ARRSIZE(fnList6), ARRSIZE(fnList7), ARRSIZE(fnList8)};
const fptr drawUIX[] = {uiNullNormal, uiMainMenu, uiFile, uiController, uiDisplay, uiSettings, uiDipswitches, uiAbout, uiLoadGame};
const u8 menuXBack[] = {0,0,1,1,1,1,1,1,2};

u8 gGammaValue = 0;

char *const autoTxt[]   = {"Off","On","With R"};
char *const speedTxt[]  = {"Normal","200%","Max","50%"};
char *const sleepTxt[]  = {"5min","10min","30min","Off"};
char *const brighTxt[]  = {"I","II","III","IIII","IIIII"};
char *const ctrlTxt[]   = {"1P","2P"};
char *const dispTxt[]   = {"Unscaled","Scaled"};
char *const flickTxt[]  = {"No Flicker","Flicker"};

char *const coinTxt[] = {
	"1 Coin - 1 Credit",  "1 Coin - 2 Credits", "1 Coin - 3 Credits", "1 Coin - 4 Credits",
	"1 Coin - 5 Credits", "1 Coin - 6 Credits", "1 Coin - 7 Credits", "2 Coins - 1 Credit",
	"2 Coins - 3 Credits","2 Coins - 5 Credits","3 Coins - 1 Credit", "3 Coins - 2 Credits",
	"3 Coins - 4 Credits","4 Coins - 1 Credit", "4 Coins - 3 Credits","Free Play"
};
char *const diffTxt[] = {"Easy","Normal","Difficult","Very Difficult"};
char *const credit2Txt[] = {"1 Credit","2 Credits"};
char *const coinAffectTxt[] = {"Coin A","Coin B"};
char *const livesTxt[] = {"3","4","5","7"};
char *const bonusTxt[] = {"20K","30K","20K 80K 100K+","30K 90K 120K+"};


/// This is called at the start of the emulator
void setupGUI() {
//	keysSetRepeat(25, 4);	// Delay, repeat.
	closeMenu();
}

/// This is called when going from emu to ui.
void enterGUI() {
}

/// This is called going from ui to emu.
void exitGUI() {
}

void quickSelectGame(void) {
	while (loadGame()) {
		redrawUI();
		return;
	}
	closeMenu();
}

void uiNullNormal() {
	uiNullDefault();
}

void uiFile() {
	setupSubMenu("File Handling");
	drawMenuItem("Load Game->");
	drawMenuItem("Load State");
	drawMenuItem("Save State");
	drawMenuItem("Save Settings");
	drawMenuItem("Reset Game");
}

void uiMainMenu() {
	setupSubMenu("Main Menu");
	drawMenuItem("File->");
	drawMenuItem("Controller->");
	drawMenuItem("Display->");
	drawMenuItem("Settings->");
	drawMenuItem("DipSwitches->");
	drawMenuItem("Help->");
	drawMenuItem("Sleep");
	drawMenuItem("Restart");
	if (enableExit) {
		drawMenuItem("Exit");
	}
}

void uiAbout() {
	setupSubMenu("Help");
	drawText(" Select: Insert coin", 3);
	drawText(" Start:  Start button", 4);
	drawText(" DPad:   Move character", 5);
	drawText(" B:      Shoot", 6);

	drawText(" SonSonGBA   " EMUVERSION, 17);
	drawText(" ARM6809     " ARM6809VERSION, 18);
	drawText(" ARMAY38910  " ARMAY38910VERSION, 19);
}

void uiController() {
	setupSubMenu("Controller Settings");
	drawSubItem("B Autofire: ", autoTxt[autoB]);
	drawSubItem("A Autofire: ", autoTxt[autoA]);
	drawSubItem("Controller: ", ctrlTxt[(joyCfg>>29)&1]);
	drawSubItem("Swap A-B:   ", autoTxt[(joyCfg>>10)&1]);
}

void uiDisplay() {
	setupSubMenu("Display Settings");
	drawSubItem("Display: ", dispTxt[gScaling]);
	drawSubItem("Scaling: ", flickTxt[gFlicker]);
	drawSubItem("Gamma: ", brighTxt[gGammaValue]);
	drawSubItem("Disable Background: ", autoTxt[gGfxMask&1]);
	drawSubItem("Disable Sprites: ", autoTxt[(gGfxMask>>4)&1]);
}

void uiSettings() {
	setupSubMenu("Other Settings");
	drawSubItem("Speed: ", speedTxt[(emuSettings>>6)&3]);
	drawSubItem("Autoload state: ", autoTxt[(emuSettings>>2)&1]);
	drawSubItem("Autosave Settings: ", autoTxt[(emuSettings>>9)&1]);
	drawSubItem("Autopause game: ", autoTxt[emuSettings&1]);
	drawSubItem("Debug Output: ", autoTxt[gDebugSet&1]);
	drawSubItem("Autosleep: ", sleepTxt[(emuSettings>>4)&3]);
}

void uiDipswitches() {
	char s[10];
	setupSubMenu("Dipswitch Settings");
	drawSubItem("Coinage: ", coinTxt[g_dipSwitch1 & 0xf]);
	drawSubItem("Coinage affects: ", coinAffectTxt[(g_dipSwitch1>>4) & 0x1]);
	drawSubItem("Demo Sound: ", autoTxt[(g_dipSwitch1>>5) & 1]);
	drawSubItem("Lives: ", livesTxt[g_dipSwitch2 & 3]);
	drawSubItem("Difficulty: ", diffTxt[(g_dipSwitch2>>5) & 3]);
	drawSubItem("Bonus: ", bonusTxt[(g_dipSwitch2>>3) & 3]);
	drawSubItem("2 Players: ", credit2Txt[(g_dipSwitch2>>2) & 1]);
	drawSubItem("Flip Screen: ", autoTxt[(g_dipSwitch1>>7) & 1]);
	drawSubItem("Service: ", autoTxt[(g_dipSwitch1>>6) & 1]);
	drawSubItem("Freeze: ", autoTxt[(g_dipSwitch2>>7) & 1]);

	setMenuItemRow(15);
	int2Str(coinCounter0, s);
	drawSubItem("CoinCounter1:       ", s);
	int2Str(coinCounter1, s);
	drawSubItem("CoinCounter2:       ", s);
}

void uiLoadGame() {
	setupSubMenu("Load game");
	drawMenuItem("Son Son");
	drawMenuItem("Son Son (Japan)");
}

void nullUINormal(int key) {
}

void nullUIDebug(int key) {
}

void resetGame() {
	loadCart(0,0);
}


//---------------------------------------------------------------------------------
/// Switch between Player 1 & Player 2 controls
void controllerSet() {				// See io.s: refreshEMUjoypads
	joyCfg ^= 0x20000000;
}

/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}

/// Turn on/off scaling
void scalingSet(){
	gScaling ^= 0x01;
	refreshGfx();
}

/// Change gamma (brightness)
void gammaSet() {
	gGammaValue++;
	if (gGammaValue > 4) gGammaValue=0;
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
}

/// Turn on/off rendering of background
void bgrLayerSet() {
	gGfxMask ^= 0x03;
}
/// Turn on/off rendering of sprites
void sprLayerSet() {
	gGfxMask ^= 0x10;
}


/// Number of coins for credits
void coinageSet() {
	int i = (g_dipSwitch1+1) & 0xf;
	g_dipSwitch1 = (g_dipSwitch1 & ~0xf) | i;
}
/// Which coin slot is affected
void coinAffectSet() {
	g_dipSwitch1 ^= 0x10;
}
/// Demo sound on/off
void demoSet() {
	g_dipSwitch1 ^= 0x20;
}
/// Number of lifes to start with
void livesSet() {
	int i = (g_dipSwitch2+0x01) & 0x03;
	g_dipSwitch2 = (g_dipSwitch2 & ~0x03) | i;
}
/// Game difficulty
void difficultSet() {
	int i = (g_dipSwitch2+0x20) & 0x60;
	g_dipSwitch2 = (g_dipSwitch2 & ~0x60) | i;
}
/// At which score you get bonus lifes
void bonusSet() {
	int i = (g_dipSwitch2+0x08) & 0x18;
	g_dipSwitch2 = (g_dipSwitch2 & ~0x18) | i;
}
/// Does second player need own credit?
void secondPlayerSet() {
	g_dipSwitch2 ^= 0x04;
}
/// Test/Service mode
void serviceSet() {
	g_dipSwitch1 ^= 0x40;
}
/// Flip screen
void flipSet() {
	g_dipSwitch1 ^= 0x80;
}
/// Pause game
void freezeSet() {
	g_dipSwitch2 ^= 0x80;
}
