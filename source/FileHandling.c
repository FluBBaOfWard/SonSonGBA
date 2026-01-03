#include <gba.h>

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/dir.h>

#include "FileHandling.h"
#include "Emubase.h"
#include "Main.h"
#include "Shared/EmuMenu.h"
#include "Gui.h"
#include "SonSon.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"

static const char *const folderName = "acds";
static const char *const settingName = "settings.cfg";

ConfigData cfg;

#define GAMECOUNT (2)

//---------------------------------------------------------------------------------
int loadSettings() {
//	FILE *file;
/*
	if (findFolder(folderName)) {
		return 1;
	}
	if ( (file = fopen(settingName, "r")) ) {
		fread(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		if (!strstr(cfg.magic,"cfg")) {
			infoOutput("Error in settings file.");
			return 1;
		}
	} else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
		return 1;
	}
*/
	gDipSwitch0 = cfg.dipSwitch0;
	gDipSwitch1 = cfg.dipSwitch1;
	gDipSwitch2 = cfg.dipSwitch2;
	gScaling    = cfg.scaling&1;
	gFlicker    = cfg.flicker&1;
	gGammaValue = cfg.gammaValue;
	emuSettings  = cfg.emuSettings &~ 0xC0;			// Clear speed setting.
	sleepTime    = cfg.sleepTime;
	joyCfg       = (joyCfg&~0x400)|((cfg.controller&1)<<10);
//	strlcpy(currentDir, cfg.currentPath, sizeof(currentDir));

	infoOutput("Settings loaded.");
	return 0;
}
void saveSettings() {
//	FILE *file;

	strcpy(cfg.magic,"cfg");
	cfg.dipSwitch0  = gDipSwitch0;
	cfg.dipSwitch1  = gDipSwitch1;
	cfg.dipSwitch2  = gDipSwitch2;
	cfg.scaling     = gScaling&1;
	cfg.flicker     = gFlicker&1;
	cfg.gammaValue  = gGammaValue;
	cfg.emuSettings = emuSettings &~ 0xC0;			// Clear speed setting.
	cfg.sleepTime   = sleepTime;
	cfg.controller  = (joyCfg>>10)&1;
//	strlcpy(cfg.currentPath, currentDir, sizeof(currentDir));
/*
	if (findFolder(folderName)) {
		return;
	}
	if ( (file = fopen(settingName, "w")) ) {
		fwrite(&cfg, 1, sizeof(ConfigData), file);
		fclose(file);
		infoOutput("Settings saved.");
	} else {
		infoOutput("Couldn't open file:");
		infoOutput(settingName);
	}*/
	infoOutput("Settings saved.");
}

int loadNVRAM() {
	return 0;
}

void saveNVRAM() {
}

void loadState(void) {
	unpackState(testState);
	infoOutput("Loaded state.");
}
void saveState(void) {
	packState(testState);
	infoOutput("Saved state.");
}
/*
void loadState(void) {
	u32 *statePtr;
//	FILE *file;
	char stateName[32];

	if (findFolder(folderName)) {
		return;
	}
	strlcpy(stateName, gameNames[selectedGame], sizeof(stateName));
	strlcat(stateName, ".sta", sizeof(stateName));
	int stateSize = getStateSize();
	if ( (file = fopen(stateName, "r")) ) {
		if ( (statePtr = malloc(stateSize)) ) {
			fread(statePtr, 1, stateSize, file);
			unpackState(statePtr);
			free(statePtr);
			infoOutput("Loaded state.");
		} else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
}

void saveState(void) {
	u32 *statePtr;
//	FILE *file;
	char stateName[32];

	if (findFolder(folderName)) {
		return;
	}
	strlcpy(stateName, gameNames[selectedGame], sizeof(stateName));
	strlcat(stateName, ".sta", sizeof(stateName));
	int stateSize = getStateSize();
	if ( (file = fopen(stateName, "w")) ) {
		if ( (statePtr = malloc(stateSize)) ) {
			packState(statePtr);
			fwrite(statePtr, 1, stateSize, file);
			free(statePtr);
			infoOutput("Saved state.");
		} else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
}
*/
//---------------------------------------------------------------------------------
bool loadGame() {
	if (loadRoms(selected, false)) {
		return true;
	}
	selectedGame = selected;
	loadRoms(selectedGame, true);
	setEmuSpeed(0);
	loadCart(selectedGame,0);
	if (emuSettings & 4) {
		loadState();
	}
	return false;
}

bool loadRoms(int game, bool doLoad) {
//	int i, j, count;
//	bool found;
//	u8 *romArea = ROM_Space;
//	FILE *file;

//	count = fileCount[game];
/*
	chdir("/");			// Stupid workaround.
	if (chdir(currentDir) == -1) {
		return true;
	}

	for (i=0; i<count; i++) {
		found = false;
		if ( (file = fopen(romFilenames[game][i], "r")) ) {
			if (doLoad) {
				fread(romArea, 1, romFilesizes[game][i], file);
				romArea += romFilesizes[game][i];
			}
			fclose(file);
			found = true;
		} else {
			for (j=0; j<GAMECOUNT; j++) {
				if ( !(findFileInZip(gameZipNames[j], romFilenames[game][i])) ) {
					if (doLoad) {
						loadFileInZip(romArea, gameZipNames[j], romFilenames[game][i], romFilesizes[game][i]);
						romArea += romFilesizes[game][i];
					}
					found = true;
					break;
				}
			}
		}
		if (!found) {
			infoOutput("Couldn't open file:");
			infoOutput(romFilenames[game][i]);
			return true;
		}
	}
*/
	return false;
}
