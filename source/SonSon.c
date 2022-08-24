#include <gba.h>
#include <string.h>

#include "SonSon.h"
#include "Cart.h"
#include "cpu.h"
#include "Gfx.h"
#include "Sound.h"
#include "ARM6809/ARM6809.h"
#include "SonSonVideo/SonSonVideo.h"

int packState(void *statePtr) {
	int size = 0;
	memcpy(statePtr+size, soundCpuRam, sizeof(soundCpuRam));
	size += sizeof(soundCpuRam);
	size += ay38910SaveState(statePtr+size, &ay38910_1);
	size += ay38910SaveState(statePtr+size, &ay38910_0);
	size += sonSaveState(statePtr+size, &sonVideo_0);
	size += m6809SaveState(statePtr+size, &m6809CPU2);
	size += m6809SaveState(statePtr+size, &m6809OpTable);
	return size;
}

void unpackState(const void *statePtr) {
	int size = 0;
	memcpy(soundCpuRam,statePtr+size, sizeof(soundCpuRam));
	size += sizeof(soundCpuRam);
	size += ay38910LoadState(&ay38910_1, statePtr+size);
	size += ay38910LoadState(&ay38910_0, statePtr+size);
	size += sonLoadState(&sonVideo_0, statePtr+size);
	size += m6809LoadState(&m6809CPU2, statePtr+size);
	m6809LoadState(&m6809OpTable, statePtr+size);
}

int getStateSize() {
	int size = 0;
	size += sizeof(soundCpuRam);
	size += ay38910GetStateSize();
	size += ay38910GetStateSize();
	size += sonGetStateSize();
	size += m6809GetStateSize();
	size += m6809GetStateSize();
	return size;
}
