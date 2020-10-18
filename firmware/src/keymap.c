#include "keymap.h"

// The key map array.
//const char KEYMAP[NUM_KEYS] PROGMEM = {
//	HID_MEDIACONTROLLER_SC_TOGGLE,		// Key 1
//	HID_MEDIACONTROLLER_SC_STOP,		// Key 2
//	HID_MEDIACONTROLLER_SC_PREVIOUS,	// Key 3
//	HID_MEDIACONTROLLER_SC_NEXT		// Key 4
//};

const char KEYMAP[] PROGMEM = {
	HID_MEDIACONTROLLER_SC_TOGGLE,		// Key 1
	HID_MEDIACONTROLLER_SC_STOP,		// Key 2
};
const char MACROMAP[][255] PROGMEM = {
	{"      _\n     ( )\n      H\n      H\n     _H_\n  .-'-.-'-.\n /         \\\n|           |\n|   .-------'._\n|  / /  '.' '. \\\n|  \\ \\ @   @ / /\n|   '---------'\n|    _______|\n|  .'-+-+-+|\n|  '.-+-+-+|\n|    \"\"\"\"\"\" |\n'-.__   __.-'\n     \"\"\"\n\n"},
	{"        _\n      /X \\ \n    _------_\n   /        \\\n  |          |\n  |          |\n  |     __  __)\n  |    /  \\/  \\\n /\\/\\ (o   )o  )\n /c    \\__/ --.\n \\_   _-------'\n  |  /         \\\n  | | '\\________)\n  |  \\_____)\n  |_____ |\n |_____/\\/\\\n /        \\\n\n"},
	{""}	// Final element should be an empty char array used when no macro key is pressed.
};