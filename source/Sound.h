#ifndef SOUND_HEADER
#define SOUND_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "AY38910/AY38910.h"

extern AY38910 ay38910_0;
extern AY38910 ay38910_1;

void soundInit(void);
void soundSetFrequency(void);
void setMuteSoundGUI(void);
void vblSound2(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SOUND_HEADER
