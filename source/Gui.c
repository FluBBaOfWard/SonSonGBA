#include <gba.h>

#include "Gui.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Main.h"
#include "FileHandling.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"
#include "cpu.h"
#include "SonSon.h"
#include "ARM6809/Version.h"
#include "AY38910/Version.h"
#include "SonSonVideo/Version.h"

#define EMUVERSION "V0.2.0 2026-01-04"

static void scalingSet(void);
static const char *getScalingText(void);
static void controllerSet(void);
static const char *getControllerText(void);
static void swapABSet(void);
static const char *getSwapABText(void);
static void gammaChange(void);
static void bgrLayerSet(void);
static const char *getBgrLayerText(void);
static void sprLayerSet(void);
static const char *getSprLayerText(void);
static void coinageSet(void);
static const char *getCoinageText(void);
static void coinAffectSet(void);
static const char *getCoinAffectText(void);
static void demoSet(void);
static const char *getDemoText(void);
static void livesSet(void);
static const char *getLivesText(void);
static void difficultySet(void);
static const char *getDifficultyText(void);
static void bonusSet(void);
static const char *getBonusText(void);
static void secondPlayerSet(void);
static const char *getSecondPlayerText(void);
static void flipSet(void);
static const char *getFlipText(void);
static void serviceSet(void);
static const char *getServiceText(void);
static void freezeSet(void);
static const char *getFreezeText(void);

const MItem dummyItems[] = {
	{"", uiDummy},
};
const MItem mainItems[] = {
	{"File->", ui2},
	{"Controller->", ui3},
	{"Display->", ui4},
	{"DipSwitches->", ui5},
	{"Settings->", ui6},
	{"Debug->", ui7},
	{"About->", ui8},
	{"Sleep", gbaSleep},
	{"Restart", resetGame},
	{"Quit Emulator", ui10},
};
const MItem fileItems[] = {
	{"Load Game->", ui9},
	{"Load State", loadState},
	{"Save State", saveState},
	{"Save Settings", saveSettings},
	{"Reset Game", resetGame},
};
const MItem ctrlItems[] = {
	{"B Autofire: ", autoBSet, getAutoBText},
	{"A Autofire: ", autoASet, getAutoAText},
	{"Controller: ", controllerSet, getControllerText},
	{"Swap A-B:   ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Display: ", scalingSet, getScalingText},
	{"Scaling: ", flickSet, getFlickText},
	{"Gamma: ", gammaChange, getGammaText},
};
const MItem dipswitchItems[] = {
	{"Coinage: ", coinageSet, getCoinageText},
	{"Coinage affects: ", coinAffectSet, getCoinAffectText},
	{"Demo Sound: ", demoSet, getDemoText},
	{"Lives: ", livesSet, getLivesText},
	{"Difficulty: ", difficultySet, getDifficultyText},
	{"Bonus: ", bonusSet, getBonusText},
	{"2 Players: ", secondPlayerSet, getSecondPlayerText},
	{"Flip Screen: ", flipSet, getFlipText},
	{"Service: ", serviceSet, getServiceText},
	{"Freeze: ", freezeSet, getFreezeText},
};
const MItem setItems[] = {
	{"Speed: ", speedSet, getSpeedText},
	{"Autoload State: ", autoStateSet, getAutoStateText},
	{"Autosave Settings: ", autoSettingsSet, getAutoSettingsText},
	{"Autopause Game: ", autoPauseGameSet, getAutoPauseGameText},
	{"EWRAM Overclock: ", ewramSet, getEWRAMText},
	{"Autosleep: ", sleepSet, getSleepText},
};
const MItem debugItems[] = {
	{"Debug Output: ", debugTextSet, getDebugText},
	{"Disable Background: ", bgrLayerSet, getBgrLayerText},
	{"Disable Sprites: ", sprLayerSet, getSprLayerText},
	{"Step Frame", stepFrame},
};
const MItem fnList9[GAME_COUNT] = {
	{"Son Son", quickSelectGame},
	{"Son Son (Japan)", quickSelectGame},
};
const MItem quitItems[] = {
	{"Yes", exitEmulator},
	{"No", backOutOfMenu},
};

const Menu menu0 = MENU_M("", uiNullNormal, dummyItems);
Menu menu1 = MENU_M("Main Menu", uiAuto, mainItems);
const Menu menu2 = MENU_M("File Handling", uiAuto, fileItems);
const Menu menu3 = MENU_M("Controller Settings", uiAuto, ctrlItems);
const Menu menu4 = MENU_M("Display Settings", uiAuto, displayItems);
const Menu menu5 = MENU_M("Dipswitch Settings", uiAuto, dipswitchItems);
const Menu menu6 = MENU_M("Other Settings", uiAuto, setItems);
const Menu menu7 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu8 = MENU_M("About", uiAbout, dummyItems);
const Menu menu9 = MENU_M("Load game", uiAuto, fnList9);
const Menu menu10 = MENU_M("Quit Emulator?", uiAuto, quitItems);

const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6, &menu7, &menu8, &menu9, &menu10 };

char *const ctrlTxt[]   = {"1P","2P"};
char *const dispTxt[]   = {"Unscaled","Scaled"};

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
	emuSettings = AUTOPAUSE_EMULATION;
//	keysSetRepeat(25, 4);	// Delay, repeat.
	menu1.itemCount = ARRSIZE(mainItems) - (enableExit?0:1);
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

void uiAbout() {
	setupSubMenuText();
	drawText("Select: Insert coin", 3);
	drawText("Start:  Start button", 4);
	drawText("DPad:   Move character", 5);
	drawText("B:      Shoot", 6);

	drawText("SonSonGBA   " EMUVERSION, 17);
	drawText("ARM6809     " ARM6809VERSION, 18);
	drawText("ARMAY38910  " ARMAY38910VERSION, 19);
}

void uiDipswitches() {
	char s[10];
	uiAuto();

	setMenuItemRow(15);
	int2Str(coinCounter0, s);
	drawSubItem("CoinCounter1:       ", s);
	int2Str(coinCounter1, s);
	drawSubItem("CoinCounter2:       ", s);
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
const char *getControllerText() {
	return ctrlTxt[(joyCfg>>29)&1];
}

/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}
const char *getSwapABText() {
	return autoTxt[(joyCfg>>10)&1];
}

/// Turn on/off scaling
void scalingSet(){
	gScaling ^= 0x01;
	refreshGfx();
}
const char *getScalingText() {
	return dispTxt[gScaling];
}

/// Change gamma (brightness)
void gammaChange() {
	gammaSet();
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
}

/// Turn on/off rendering of background
void bgrLayerSet() {
	gGfxMask ^= 0x03;
}
const char *getBgrLayerText() {
	return autoTxt[(gGfxMask>>1)&1];
}
/// Turn on/off rendering of sprites
void sprLayerSet() {
	gGfxMask ^= 0x10;
}
const char *getSprLayerText() {
	return autoTxt[(gGfxMask>>4)&1];
}

/// Number of coins for credits
void coinageSet() {
	int i = (gDipSwitch1+1) & 0xf;
	gDipSwitch1 = (gDipSwitch1 & ~0xf) | i;
}
const char *getCoinageText() {
	return coinTxt[gDipSwitch1 & 0xf];
}
/// Which coin slot is affected
void coinAffectSet() {
	gDipSwitch1 ^= 0x10;
}
const char *getCoinAffectText() {
	return coinAffectTxt[(gDipSwitch1>>4) & 0x1];
}
/// Demo sound on/off
void demoSet() {
	gDipSwitch1 ^= 0x20;
}
const char *getDemoText() {
	return autoTxt[(gDipSwitch1>>5) & 1];
}
/// Number of lifes to start with
void livesSet() {
	int i = (gDipSwitch2+0x01) & 0x03;
	gDipSwitch2 = (gDipSwitch2 & ~0x03) | i;
}
const char *getLivesText() {
	return livesTxt[gDipSwitch2 & 3];
}
/// Game difficulty
void difficultySet() {
	int i = (gDipSwitch2+0x20) & 0x60;
	gDipSwitch2 = (gDipSwitch2 & ~0x60) | i;
}
const char *getDifficultyText() {
	return diffTxt[(gDipSwitch2>>5) & 3];
}
/// At which score you get bonus lifes
void bonusSet() {
	int i = (gDipSwitch2+0x08) & 0x18;
	gDipSwitch2 = (gDipSwitch2 & ~0x18) | i;
}
const char *getBonusText() {
	return bonusTxt[(gDipSwitch2>>3) & 3];
}
/// Does second player need own credit?
void secondPlayerSet() {
	gDipSwitch2 ^= 0x04;
}
const char *getSecondPlayerText() {
	return credit2Txt[(gDipSwitch2>>2) & 1];
}
/// Test/Service mode
void serviceSet() {
	gDipSwitch1 ^= 0x40;
}
const char *getServiceText() {
	return autoTxt[(gDipSwitch1>>6) & 1];
}
/// Flip screen
void flipSet() {
	gDipSwitch1 ^= 0x80;
}
const char *getFlipText() {
	return autoTxt[(gDipSwitch1>>7) & 1];
}
/// Pause game
void freezeSet() {
	gDipSwitch2 ^= 0x80;
}
const char *getFreezeText() {
	return autoTxt[(gDipSwitch2>>7) & 1];
}
