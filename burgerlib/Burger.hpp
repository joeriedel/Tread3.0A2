/**********************************

	Burger's Universal library WIN95 version
	This is for Watcom 10.5 and higher...
	Also support for MSVC 4.0

**********************************/

#ifndef __BURGERHPP__
#define __BURGERHPP__

#ifndef __BURGER__
#include <Burger.h>
#endif

/* Here are the Burgerlib structs represented as classes */

/* LWPoint_c class and calls */

class LWPoint_c:public LBPoint {
public:
	LWPoint_c *New(void) {return new LWPoint_c;}
	void Delete(void) {delete this;}
	void Init(void) {}
	void Destroy(void) {}
	void FromSYSPOINT(const SYSPOINT *Input) {LBPointFromSYSPOINT(this,Input);}
	void FromSYSPOINT(const SYSPOINT &Input) {LBPointFromSYSPOINT(this,&Input);}
	void ToSYSPOINT(SYSPOINT *Output) {LBPointToSYSPOINT(Output,this);}
};

/* LWRect_c class and calls */

class LWRect_c:public LBRect {
public:
	LWRect_c *New(void) {return new LWRect_c;}
	void Delete(void) {delete this;}
	void Init(void) {}
	void Destroy(void) {}
	void Set(int x1,int y1,int x2,int y2) {LBRectSetRect(this,x1,y1,x2,y2);}
	int Width(void) {return right-left;}
	int Height(void) {return bottom-top;}
	void Empty() {LBRectSetRectEmpty(this);}
	Word PtInRect(int x,int y) {return LBRectPtInRect(this,x,y);}
	void Offset(int h,int v) {LBRectOffsetRect(this,h,v);}
	void Inset(int x,int y) {LBRectInsetRect(this,x,y);}
	Word IsEqual(const LBRect *Input2) {return LBRectIsEqual(this,Input2);}
	Word IsEqual(const LBRect &Input2) {return LBRectIsEqual(this,&Input2);}
	Word Intersect(const LBRect *rect2) {return LBRectIntersectRect(0,this,rect2);}
	Word Intersect(const LBRect &rect2) {return LBRectIntersectRect(0,this,&rect2);}
	Word Intersect(LBRect *Output,const LBRect *rect2) {return LBRectIntersectRect(Output,this,rect2);}
	Word Intersect(LBRect *Output,const LBRect &rect2) {return LBRectIntersectRect(Output,this,&rect2);}
	void Union(const LBRect *Input2) {LBRectUnionRect(this,this,Input2);}
	void Union(const LBRect &Input2) {LBRectUnionRect(this,this,&Input2);}
	void Union(LBRect *Output,const LBRect *Input2) {LBRectUnionRect(Output,this,Input2);}
	void Union(LBRect *Output,const LBRect &Input2) {LBRectUnionRect(Output,this,&Input2);}
	void AddPoint(int x,int y) {LBRectAddPointToRect(this,x,y);}
	Word IsEmpty() {return LBRectIsRectEmpty(this);}
	void FromSYSRECT(const SYSRECT *Input) {LBRectFromSYSRECT(this,Input);}
	void FromSYSRECT(const SYSRECT &Input) {LBRectFromSYSRECT(this,&Input);}
	void ToSYSRECT(SYSRECT *Output) {LBRectToSYSRECT(Output,this);}
};

/* LWRectList_c class and calls */

class LWRectList_c:public LBRectList {
public:
	LWRectList_c(void) {LBRectListInit(this);}
	~LWRectList_c(void) {LBRectListDestroy(this);}
	LWRectList_c *New(void) {return (LWRectList_c *)LBRectListNew();}
	void Delete(void) {LBRectListDelete(this);}
	void Init(void) {LBRectListInit(this);}
	void Destroy(void) {LBRectListDestroy(this);}
	Word RectClip(const LBRect* b,const LBRect* t) {return LBRectListRectClip(this,b,t);}
	void ClipOut(const LBRect *bound) {LBRectListClipOutRect(this,bound);}
	void ClipOut(const LBRectList *list) {LBRectListClipOutRectList(this,list);}
	void Append(const LBRect *rect) {LBRectListAppendRect(this,rect);}
	void Append(const LBRectList *list) {LBRectListAppendRectList(this,list);}
	void Copy(const LBRectList *list) {LBRectListCopy(this,list);}
};

/* Font manager class and calls */

class FontRef_c:public FontRef_t {
public:
	FontRef_c(Word FontNum=0,const Byte *PalPtr=CurrentPalette) {FontInit(this,FontNum,PalPtr);}
	~FontRef_c() {ReleaseAFont2(this);}
	FontRef_c *New(Word FontNum=0,const Byte *PalPtr=CurrentPalette) {return (FontRef_c*)FontNew(FontNum,PalPtr);}
	void Delete(void) {FontDelete(this);}
	void Init(Word FontNum=0,const Byte *PalPtr=CurrentPalette) {FontInit(this,FontNum,PalPtr);}
	void Destroy(void) {ReleaseAFont2(this);}
	void SaveState(FontState_t *StatePtr) {FontSaveState2(this,StatePtr);}
	void RestoreState(const FontState_t *StatePtr) {FontRestoreState2(this,StatePtr);}
	void Draw(Word Letter) {DrawAChar2(this,Letter);}
	void Draw(const void *TextPtr) {DrawAString2(this,TextPtr);}
	Word Width(Word Letter) {return WidthAChar2(this,Letter);}
	Word Width(const void *TextPtr) {return WidthAString2(this,TextPtr);}
	void SetXY(int x,int y) {FontSetXY2(this,x,y);}
	void SetXY(const LBPoint *Pt) {FontSetXY2(this,Pt->x,Pt->y);}
	void SetColor(Word ColorNum,Word Color) {FontSetColor2(this,ColorNum,Color);}
	void UseZero(void) {FontUseZero2(this);}
	void UseMask(void) {FontUseMask2(this);}
	void Install(Word FontNum,const Byte *PalPtr=CurrentPalette) {InstallAFontToPalette2(this,FontNum,PalPtr);}
	void Release(void) {ReleaseAFont2(this);}
	void SetRGBList(const void *RGBList,const Byte *PalPtr=CurrentPalette) {FontSetColorRGBListToPalette2(this,RGBList,PalPtr);}
};

/* Random number manager and calls */

class Random_c:private Random_t {
public:
	Random_c(void) {RndRandomize(this);}
	Random_c(Word NewSeed) {RndRandomize(this); RndSetRandomSeed(this,NewSeed);}
	~Random_c(void) {}
	Random_c *New(Word NewSeed=0) {return (Random_c *)RndNew(NewSeed);}
	void Delete(void) {RndDelete(this);}
	void Init(void) {RndRandomize(this);}
	void Init(Word NewSeed) {RndRandomize(this); RndSetRandomSeed(this,NewSeed);}
	void Destroy(void) {}
	Word Random(Word Range) {return RndGetRandom(this,Range);}
	int Signed(Word Range) {return RndGetRandomSigned(this,Range);}
	void HardInit(void) {RndHardwareRandomize(this);}
};

#endif
