#ifndef GUI_HEADER
#define GUI_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u8 gGammaValue;

void setupGUI(void);
void enterGUI(void);
void exitGUI(void);
void quickSelectGame(void);
void nullUINormal(int key);
void nullUIDebug(int key);
void resetGame(void);

void uiNullNormal(void);
void uiMainMenu(void);
void uiFile(void);
void uiSettings(void);
void uiController(void);
void uiDisplay(void);
void uiDipswitches(void);
void uiAbout(void);
void uiLoadGame(void);

void controllerSet(void);
void swapABSet(void);

void scalingSet(void);
void gammaSet(void);
void bgrLayerSet(void);
void sprLayerSet(void);

void coinageSet(void);
void coinAffectSet(void);
void demoSet(void);
void difficultSet(void);
void livesSet(void);
void bonusSet(void);
void secondPlayerSet(void);
void serviceSet(void);
void flipSet(void);
void freezeSet(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GUI_HEADER
