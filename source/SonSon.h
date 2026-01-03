#ifndef YIEAR_HEADER
#define YIEAR_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define GAME_COUNT (2)

/// This runs all save state functions for each chip.
int packState(void *statePtr);

/// This runs all load state functions for each chip.
void unpackState(const void *statePtr);

/// Gets the total state size in bytes.
int getStateSize(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // YIEAR_HEADER
