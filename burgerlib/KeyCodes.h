/**********************************

	Use the SC_XX indexes for indexing into
	the KeyArray table only

	The ASCII_XX codes are for KeyboardKbhit and KeyboardGetch

**********************************/

#ifndef __KEYCODES__
#define __KEYCODES__

#if !defined(__BURGER__) && !defined(__BRTYPES_H__)
#include <Burger.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**********************************

	Keycodes for Mac OS computers

**********************************/

#if defined(__MAC__)

/* Used exclusively as offsets into KeyArray */

#define SC_A			0x00
#define SC_S			0x01
#define SC_D			0x02
#define SC_F			0x03
#define SC_H			0x04
#define SC_G			0x05
#define SC_Z			0x06
#define SC_X			0x07
#define SC_C			0x08
#define SC_V			0x09
#define SC_FUSS			0x0A	/* German s */
#define SC_B			0x0B
#define SC_Q			0x0C
#define SC_W			0x0D
#define SC_E			0x0E
#define SC_R			0x0F
#define SC_Y			0x10
#define SC_T			0x11
#define SC_1			0x12
#define SC_2			0x13
#define SC_3			0x14
#define SC_4			0x15
#define SC_6			0x16
#define SC_5			0x17
#define SC_PLUS			0x18
#define SC_EQUALS		0x18
#define SC_9			0x19
#define SC_7			0x1A
#define SC_UNDERSCORE	0x1B
#define SC_MINUS		0x1B
#define SC_8			0x1C
#define SC_0			0x1D
#define SC_RIGHTBRACE	0x1E
#define SC_RIGHTBRACKET	0x1E
#define SC_O			0x1F
#define SC_U			0x20
#define SC_LEFTBRACE	0x21
#define SC_LEFTBRACKET	0x21
#define SC_I			0x22
#define SC_P			0x23
#define SC_RETURN		0x24
#define SC_L			0x25
#define SC_J			0x26
#define SC_QUOTE		0x27
#define SC_K			0x28
#define SC_COLON		0x29
#define SC_SEMICOLON	0x29
#define SC_VERTBAR		0x2A
#define SC_BACKSLASH	0x2A
#define SC_COMMA		0x2B
#define SC_SLASH		0x2C
#define SC_N			0x2D
#define SC_M			0x2E
#define SC_PERIOD		0x2F
#define SC_TAB			0x30
#define SC_SPACE		0x31
#define SC_TILDE		0x32
#define SC_BACKSPACE	0x33
#define SC_POWERBOOKENTER 0x34
#define SC_ESCAPE		0x35
#define SC_LEFTOPTION 0x36
#define SC_LEFTALT		0x37
#define SC_LEFTOPENAPPLE 0x37
#define SC_LEFTSHIFT	0x38
#define SC_CAPSLOCK		0x39
#define SC_RIGHTOPTION	0x3A
#define SC_LEFTCONTROL	0x3B
#define SC_RIGHTSHIFT	0x3C
#define SC_RIGHTALT		0x3D
#define SC_RIGHTOPENAPPLE 0x3D
#define SC_RIGHTCONTROL	0x3E
/* 0x3F Not used */
/* 0x40 Not used */
#define SC_KEYPADPERIOD 0x41
/* 0x42 Not used */
#define SC_KEYPADASTERISK 0x43
/* 0x44 Not used */
#define SC_KEYPADPLUS	0x45
/* 0x46 Not used */
#define SC_NUMLOCK		0x47
/* 0x48 Not used */
/* 0x49 Not used */
/* 0x4A Not used */
#define SC_KEYPADSLASH	0x4B
#define SC_KEYPADENTER	0x4C
/* 0x4D Not used */
#define SC_KEYPADMINUS	0x4E
/* 0x4F Not used */
/* 0x50 Not used */
#define SC_KEYPADEQUALS	0x51
#define SC_KEYPAD0		0x52
#define SC_KEYPAD1		0x53
#define SC_KEYPAD2		0x54
#define SC_KEYPAD3		0x55
#define SC_KEYPAD4		0x56
#define SC_KEYPAD5		0x57
#define SC_KEYPAD6		0x58
#define SC_KEYPAD7		0x59
/* 0x5A Not used */
#define SC_KEYPAD8		0x5B
#define SC_KEYPAD9		0x5C
/* 0x5D Not used */
/* 0x5E Not used */
/* 0x5F Not used */
#define SC_F5			0x60
#define SC_F6			0x61
#define SC_F7			0x62
#define SC_F3			0x63
#define SC_F8			0x64
#define SC_F9			0x65
#define SC_F11			0x67
/* 0x68 Not used */
#define SC_PRINTSCREEN	0x69
/* 0x6A Not used */
#define SC_SCROLLLOCK	0x6B
/* 0x6C Not used */
#define SC_F10			0x6D
/* 0x6E Not used */
#define SC_F12			0x6F
/* 0x70 Not used */
#define SC_PAUSE		0x71
#define SC_INSERT		0x72
#define SC_HOME			0x73
#define SC_PAGEUP		0x74
#define SC_DELETE		0x75
#define SC_F4			0x76
#define SC_END			0x77
#define SC_F2			0x78
#define SC_PAGEDOWN		0x79
#define SC_F1			0x7A
#define SC_LEFTARROW	0x7B
#define SC_RIGHTARROW	0x7C
#define SC_DOWNARROW	0x7D
#define SC_UPARROW		0x7E
#define SC_NONE			0x7F

/* Used exclusively as return values from KeyboardGetch and KeyboardKbhit */

#define ASCII_HOME		0x91
#define ASCII_UPARROW	0x0B
#define ASCII_PAGEUP	0x92
#define	ASCII_LEFTARROW	0x96
#define ASCII_RIGHTARROW 0x15
#define ASCII_END		0x93
#define ASCII_DOWNARROW	0x0A
#define ASCII_PAGEDOWN	0x94
#define ASCII_INSERT	0x95
#define ASCII_DELETE	0x97
#define ASCII_BACKSPACE 0x08
#define ASCII_ENTER 0x0D
#define ASCII_RETURN 0x0D
#define ASCII_ESCAPE 0x1B
#define ASCII_ESC 0x1B
#define ASCII_TAB 0x09

#define ASCII_PRINTSCREEN 0x8F
#define ASCII_PAUSE		0x80
#define ASCII_F1		0x81
#define ASCII_F2		0x82
#define ASCII_F3		0x83
#define ASCII_F4		0x84
#define ASCII_F5		0x85
#define ASCII_F6		0x86
#define ASCII_F7		0x87
#define ASCII_F8		0x88
#define ASCII_F9		0x89
#define ASCII_F10		0x8A
#define ASCII_F11		0x8B
#define ASCII_F12		0x8C
#define ASCII_SCROLLLOCK 0x8D

#else

/**********************************

	Keycodes for DOS and Win95 computers

**********************************/

/* Used exclusively as offsets into KeyArray */

#define SC_NONE			0x00
#define SC_ESCAPE		0x01
#define SC_1			0x02
#define SC_2			0x03
#define SC_3			0x04
#define SC_4			0x05
#define SC_5			0x06
#define SC_6			0x07
#define SC_7			0x08
#define SC_8			0x09
#define SC_9			0x0A
#define SC_0			0x0B
#define SC_UNDERSCORE	0x0C
#define SC_MINUS		0x0C
#define SC_PLUS			0x0D
#define SC_EQUALS		0x0D
#define SC_BACKSPACE	0x0E
#define SC_TAB			0x0F
#define SC_Q			0x10
#define SC_W			0x11
#define SC_E			0x12
#define SC_R			0x13
#define SC_T			0x14
#define SC_Y			0x15
#define SC_U			0x16
#define SC_I			0x17
#define SC_O			0x18
#define SC_P			0x19
#define SC_RIGHTBRACE	0x1A
#define SC_RIGHTBRACKET	0x1A
#define SC_LEFTBRACE	0x1B
#define SC_LEFTBRACKET	0x1B
#define SC_RETURN		0x1C
#define SC_LEFTCONTROL	0x1D
#define SC_A			0x1E
#define SC_S			0x1F
#define SC_D			0x20
#define SC_F			0x21
#define SC_G			0x22
#define SC_H			0x23
#define SC_J			0x24
#define SC_K			0x25
#define SC_L			0x26
#define SC_COLON		0x27
#define SC_SEMICOLON	0x27
#define SC_QUOTE		0x28
#define SC_TILDE		0x29
#define SC_LEFTSHIFT	0x2A
#define SC_VERTBAR		0x2B
#define SC_BACKSLASH	0x2B
#define SC_Z			0x2C
#define SC_X			0x2D
#define SC_C			0x2E
#define SC_V			0x2F
#define SC_B			0x30
#define SC_N			0x31
#define SC_M			0x32
#define SC_COMMA		0x33
#define SC_PERIOD		0x34
#define SC_SLASH		0x35
#define SC_RIGHTSHIFT	0x36
#define SC_KEYPADASTERISK	0x37
#define SC_LEFTALT		0x38
#define SC_LEFTOPENAPPLE	0x38
#define SC_SPACE		0x39
#define SC_CAPSLOCK		0x3A
#define SC_F1			0x3B
#define SC_F2			0x3C
#define SC_F3			0x3D
#define SC_F4			0x3E
#define SC_F5			0x3F
#define SC_F6			0x40
#define SC_F7			0x41
#define SC_F8			0x42
#define SC_F9			0x43
#define SC_F10			0x44
#define SC_NUMLOCK		0x45
#define SC_CLEAR		0x45	/* Same as numlock */
#define SC_SCROLLLOCK	0x46
#define SC_KEYPAD7		0x47
#define SC_KEYPAD8		0x48
#define SC_KEYPAD9		0x49
#define SC_KEYPADMINUS	0x4A
#define SC_KEYPAD4		0x4B
#define SC_KEYPAD5		0x4C
#define SC_KEYPAD6		0x4D
#define	SC_KEYPADPLUS	0x4E
#define SC_KEYPAD1		0x4F
#define SC_KEYPAD2		0x50
#define SC_KEYPAD3		0x51
#define SC_KEYPAD0		0x52
#define SC_KEYPADPERIOD	0x53
/* 0x54 */
/* 0x55 */
/* 0x56 */
#define SC_F11			0x57
#define SC_F12			0x58

#define SC_HOME			0x60	/* 0xE0,0x47 */
#define SC_UPARROW		0x61	/* 0xE0,0x48 */
#define SC_PAGEUP		0x62	/* 0xE0,0x49 */
#define SC_LEFTARROW	0x63	/* 0xE0,0x4B */
#define SC_RIGHTARROW	0x64	/* 0xE0,0x4D */
#define SC_END			0x65	/* 0xE0,0x4F */
#define SC_DOWNARROW	0x66	/* 0xE0,0x50 */
#define SC_PAGEDOWN		0x67	/* 0xE0,0x51 */
#define SC_INSERT		0x68	/* 0xE0,0x52 */
#define SC_DELETE		0x69	/* 0xE0,0x53 */

#define SC_KEYPADENTER	0x6A	/* 0xE0,0x1C */
#define SC_RIGHTCONTROL	0x6B	/* 0xE0,0x1D */
#define SC_KEYPADSLASH	0x6C	/* 0xE0,0x35 */
#define SC_PRINTSCREEN	0x6D	/* 0xE0,0x2A,0x37 */
#define SC_PAUSE		0x6E	/* 0xE1,0x1D,0x45 */
#define SC_RIGHTALT		0x6F	/* 0xE0,0x38 */
#define SC_RIGHTOPENAPPLE 0x6F
#define SC_RIGHTOPTION	0x70
#define SC_LEFTOPTION 0x71
#define SC_KEYPADEQUALS 0x72	/* 0xE0,0x0D */

/* Used exclusively as return values from KeyboardGetch and KeyboardKbhit */

#define ASCII_HOME		0x91	/* 0xE0,0x47 */
#define ASCII_UPARROW	0x0B	/* 0xE0,0x48 */
#define ASCII_PAGEUP	0x92	/* 0xE0,0x49 */
#define ASCII_LEFTARROW	0x96	/* 0xE0,0x4B */
#define ASCII_RIGHTARROW 0x15	/* 0xE0,0x4D */
#define ASCII_END		0x93	/* 0xE0,0x4F */
#define ASCII_DOWNARROW	0x0A	/* 0xE0,0x50 */
#define ASCII_PAGEDOWN	0x94	/* 0xE0,0x51 */
#define ASCII_INSERT	0x95	/* 0xE0,0x52 */
#define ASCII_DELETE	0x97	/* 0xE0,0x53 */
#define ASCII_BACKSPACE	0x08	/* 0x0E */
#define ASCII_ENTER 0x0D
#define ASCII_RETURN 0x0D
#define ASCII_ESCAPE 0x1B
#define ASCII_ESC 0x1B
#define ASCII_TAB 0x09

#define ASCII_PRINTSCREEN	0x8F	/* 0xE0,0x2A,0x37 */
#define ASCII_PAUSE		0x80	/* 0xE1,0x1D,0x45 */
#define ASCII_F1 0x81		/* 0x3B */
#define ASCII_F2 0x82		/* 0x3C */
#define ASCII_F3 0x83		/* 0x3D */
#define ASCII_F4 0x84		/* 0x3E */
#define ASCII_F5 0x85		/* 0x3F */
#define ASCII_F6 0x86		/* 0x40 */
#define ASCII_F7 0x87		/* 0x41 */
#define ASCII_F8 0x88		/* 0x42 */
#define ASCII_F9 0x89		/* 0x43 */
#define ASCII_F10 0x8A		/* 0x44 */
#define ASCII_F11 0x8B		/* 0x57 */
#define ASCII_F12 0x8C		/* 0x58 */
#define ASCII_SCROLLLOCK 0x8D	/* 0x1C */

#endif

#ifdef __cplusplus
}
#endif
#endif
