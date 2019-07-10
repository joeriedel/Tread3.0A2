/**********************************

	C++ ANSI "C" string class
	This is optimized for performance

**********************************/

#ifndef __BRSTRING_H__
#define __BRSTRING_H__

#ifndef __BURGER__
#include <Burger.h>
#endif

typedef struct BRString_t {
	char *DataPtr;		/* Pointer to the string */
	Short StrLength;	/* Length of the string */
	Short Flags;		/* Flags */
	char Raw[56];		/* Temp preallocated buffer for most strings */
} BRString_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void FASTCALL BRStringInit(BRString_t *Output);
extern void FASTCALL BRStringInitBRString(BRString_t *Output,const BRString_t *Input);
extern void FASTCALL BRStringInitBRStringRange(BRString_t *Output,const BRString_t *Input,Word Start,Word End);
extern void FASTCALL BRStringInitPChar(BRString_t *Output,const char *Input);
extern void FASTCALL BRStringInitPCharRange(BRString_t *Output,const char *Input,Word Start,Word End);
extern void FASTCALL BRStringInitChar(BRString_t *Output,char Input);
extern void FASTCALL BRStringInitChar2(BRString_t *Output,char Input,Word FillSize);
extern void FASTCALL BRStringInitPChar2(BRString_t *Output,const char *Input1,const char *Input2);
extern void FASTCALL BRStringInitPChar3(BRString_t *Output,const char *Input1,const char *Input2,const char *Input3);
extern void FASTCALL BRStringInitPChar4(BRString_t *Output,const char *Input1,const char *Input2,const char *Input3,const char *Input4);
extern void FASTCALL BRStringCopyBRString(BRString_t *Output,const BRString_t *Input);
extern void FASTCALL BRStringCopyPChar(BRString_t *Output,const char *Input);
extern void FASTCALL BRStringCopyChar(BRString_t *Output,char Input);
extern void FASTCALL BRStringAppendBRString(BRString_t *Output,const BRString_t *Input);
extern void FASTCALL BRStringAppendPChar(BRString_t *Output,const char *Input);
extern void FASTCALL BRStringAppendChar(BRString_t *Output,char Input);
extern int FASTCALL BRStringCompareBRString(const BRString_t *Input1,const BRString_t *Input2);
extern int FASTCALL BRStringComparePChar(const BRString_t *Input1,const char *Input2);
extern int FASTCALL BRStringCompareChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringEqBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringEqPChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringEqChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringNeBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringNePChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringNeChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringLtBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringLtPChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringLtChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringLeBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringLePChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringLeChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringGtBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringGtPChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringGtChar(const BRString_t *Input1,char Input2);
extern Word FASTCALL BRStringGeBString(const BRString_t *Input1,const BRString_t *Input2);
extern Word FASTCALL BRStringGePChar(const BRString_t *Input1,const char *Input2);
extern Word FASTCALL BRStringGeChar(const BRString_t *Input1,char Input2);
extern void FASTCALL BRStringDestroy(BRString_t *Input);
extern void FASTCALL BRStringDelete(BRString_t* Input);
#define BRStringLength(x) (x)->StrLength
extern char FASTCALL BRStringGet(const BRString_t *Input,Word Where);
extern void FASTCALL BRStringPut(BRString_t *Output,Word Where,char Input);
extern char * FASTCALL BRStringstrstr(const BRString_t *Input1,const char *Input2);
extern void FASTCALL BRStringToLowerCase(BRString_t *Input);
extern void FASTCALL BRStringToUpperCase(BRString_t *Input);
extern Word FASTCALL BRStringValid(const BRString_t *Input);
extern void FASTCALL BRStringClear(BRString_t *Input);
extern Word FASTCALL BRStringCopy(const BRString_t *Input,char *Output,Word MaxLen);
extern Word FASTCALL BRStringPCopy(const BRString_t *Input,Byte *Output,Word MaxLen);
extern Word FASTCALL BRStringRemove(BRString_t *Input,Word Start,Word Len);
extern Word FASTCALL BRStringInsert(BRString_t *Input,Word Start,const char *Text,Word MaxLen);
extern void FASTCALL BRStringLeft(BRString_t *Input,Word NewLen,char padch);
extern void FASTCALL BRStringRight(BRString_t *Input,Word NewLen,char padch);
extern Word FASTCALL BRStringRemoveChar(BRString_t *Input,char KillChar);
extern Word FASTCALL BRStringRemoveChars(BRString_t *Input,const char *CharList);
extern Word FASTCALL BRStringCaseSensitive(BRString_t *Input);
extern Word FASTCALL BRStringCaseInsensitive(BRString_t *Input);
extern Word FASTCALL BRStringSetCase(BRString_t *Input,Word State);
#ifdef __cplusplus
}
class BRString : private BRString_t {
private:
	static char Dummy;		/* Used for failsafe routines */
				/* Structure size is 64 bytes */
public:
	BRString(void) { DataPtr = Raw;StrLength=0;Flags=0;Raw[0]=0;}
	BRString(const BRString &Input) { BRStringInitBRString(this,&Input); }
	BRString(const BRString &Input,Word Start,Word End = (Word)-1) { BRStringInitBRStringRange(this,&Input,Start,End); }
	BRString(const char *Input) { BRStringInitPChar(this,Input); }
	BRString(const char *Input,Word Start,Word End) { BRStringInitPCharRange(this,Input,Start,End); }
	BRString(char Input) { BRStringInitChar(this,Input); }
	BRString(char Input,Word FillSize) { BRStringInitChar2(this,Input,FillSize); }
	BRString(const char *Input1,const char *Input2) { BRStringInitPChar2(this,Input1,Input2); }
	BRString(const char *Input1,const char *Input2,const char *Input3) { BRStringInitPChar3(this,Input1,Input2,Input3); }
	BRString(const char *Input1,const char *Input2,const char *Input3,const char *Input4) { BRStringInitPChar4(this,Input1,Input2,Input3,Input4); }
	~BRString() { if (DataPtr != Raw) DeallocAPointer(DataPtr); }

	operator char *() { return this->DataPtr; }
	operator const char *() const { return this->DataPtr; }
	const char *c_str() const { return this->DataPtr; }
	char *c_str() { return this->DataPtr; }
	operator char () const { return this->DataPtr[0]; }

	BRString & operator = (const BRString &Input) { BRStringCopyBRString(this,&Input); return *this; }
	BRString & operator = (const char *Input) { BRStringCopyPChar(this,Input); return *this; }
	BRString & operator = (char Input) { BRStringCopyChar(this,Input); return *this; }

	BRString & operator += (const BRString &Input) { BRStringAppendBRString(this,&Input); return *this; }
	BRString & operator += (const char *Input) { BRStringAppendPChar(this,Input); return *this; }
	BRString & operator += (char Input) { BRStringAppendChar(this,Input); return *this; }

	BRString operator () (Word Start,Word End) const {return BRString(*this,Start,End);}
	char & operator () (Word Input) { if (Input<StrLength) return DataPtr[Input]; Dummy = 0; return Dummy;}
	char const & operator ()(Word Input) const { if (Input<StrLength) return DataPtr[Input]; Dummy = 0; return Dummy;}
	char & operator [] (Word Input) { if (Input<StrLength) return DataPtr[Input]; Dummy = 0; return Dummy;}
	char const & operator [](Word Input) const { if (Input<StrLength) return DataPtr[Input]; Dummy = 0; return Dummy;}

	friend Word operator == (BRString const &Input1,BRString const &Input2) { return BRStringEqBString(&Input1,&Input2); }
	friend Word operator == (BRString const &Input1,const char *Input2) { return BRStringEqPChar(&Input1,Input2); }
	friend Word operator == (const char *Input1,BRString const &Input2) { return BRStringEqPChar(&Input2,Input1); }
	friend Word operator == (BRString const &Input1,char Input2) { return BRStringEqChar(&Input1,Input2); }
	friend Word operator == (char Input1,BRString const &Input2) { return BRStringEqChar(&Input2,Input1); }
	
	friend Word operator != (BRString const &Input1,BRString const &Input2) { return BRStringNeBString(&Input1,&Input2); }
	friend Word operator != (BRString const &Input1,const char *Input2) { return BRStringNePChar(&Input1,Input2); }
	friend Word operator != (const char *Input1,BRString const &Input2) { return BRStringNePChar(&Input2,Input1); }
	friend Word operator != (BRString const &Input1,char Input2) { return BRStringNeChar(&Input1,Input2); }
	friend Word operator != (char Input1,BRString const &Input2) { return BRStringNeChar(&Input2,Input1); }

	friend Word operator < (BRString const &Input1,BRString const &Input2) { return BRStringLtBString(&Input1,&Input2); }
	friend Word operator < (BRString const &Input1,const char *Input2) { return BRStringLtPChar(&Input1,Input2); }
	friend Word operator < (const char *Input1,BRString const &Input2) { return BRStringGtPChar(&Input2,Input1); }
	friend Word operator < (BRString const &Input1,char Input2) { return BRStringLtChar(&Input1,Input2); }
	friend Word operator < (char Input1,BRString const &Input2) { return BRStringGtChar(&Input2,Input1); }

	friend Word operator <= (BRString const &Input1,BRString const &Input2) { return BRStringLeBString(&Input1,&Input2); }
	friend Word operator <= (BRString const &Input1,const char *Input2) { return BRStringLePChar(&Input1,Input2); }
	friend Word operator <= (const char *Input1,BRString const &Input2) { return BRStringGePChar(&Input2,Input1); }
	friend Word operator <= (BRString const &Input1,char Input2) { return BRStringLeChar(&Input1,Input2); }
	friend Word operator <= (char Input1,BRString const &Input2) { return BRStringGeChar(&Input2,Input1); }

	friend Word operator > (BRString const &Input1,BRString const &Input2) { return BRStringGtBString(&Input1,&Input2); }
	friend Word operator > (BRString const &Input1,const char *Input2) { return BRStringGtPChar(&Input1,Input2); }
	friend Word operator > (const char *Input1,BRString const &Input2) { return BRStringLtPChar(&Input2,Input1); }
	friend Word operator > (BRString const &Input1,char Input2) { return BRStringGtChar(&Input1,Input2); }
	friend Word operator > (char Input1,BRString const &Input2) { return BRStringLtChar(&Input2,Input1); }

	friend Word operator >= (BRString const &Input1,BRString const &Input2) { return BRStringGeBString(&Input1,&Input2); }
	friend Word operator >= (BRString const &Input1,const char *Input2) { return BRStringGePChar(&Input1,Input2); }
	friend Word operator >= (const char *Input1,BRString const &Input2) { return BRStringLePChar(&Input2,Input1); }
	friend Word operator >= (BRString const &Input1,char Input2) { return BRStringGeChar(&Input1,Input2); }
	friend Word operator >= (char Input1,BRString const &Input2) { return BRStringLeChar(&Input2,Input1); }

	friend BRString FASTCALL operator + (const BRString &Input1,const BRString &Input2) { return BRString(Input1.DataPtr,Input2.DataPtr); }
	friend BRString FASTCALL operator + (const BRString &Input1,const char *Input2) { return BRString(Input1.DataPtr,Input2); }
	friend BRString FASTCALL operator + (const char *Input1,const BRString &Input2) { return BRString(Input1,Input2.DataPtr); }
	friend BRString FASTCALL operator + (const BRString &Input1,char *Input2) { return BRString(Input1.DataPtr,Input2); }
	friend BRString FASTCALL operator + (char *Input1,const BRString &Input2) { return BRString(Input1,Input2.DataPtr); }
//	friend BRString FASTCALL operator + (const BRString &Input1,char Input2);
//	friend BRString FASTCALL operator + (char Input1,const BRString &Input2);

	Word length(void) const { return this->StrLength; }
	char FASTCALL get(Word Where) const { return BRStringGet(this,Where); }
	void FASTCALL put(Word Where,char Input) { BRStringPut(this,Where,Input); }
	char * strstr(const char *Input) const { return BRStringstrstr(this,Input); }

	BRString & tolowercase(void) { BRStringToLowerCase(this); return *this; }
	BRString & touppercase(void) { BRStringToUpperCase(this); return *this; }
	BRString lowercase(void) const { return BRString(*this).tolowercase(); }
	BRString uppercase(void) const { return BRString(*this).touppercase(); }

	Word operator ! () const { return BRStringValid(this); }
	Word valid(void) const { return BRStringValid(this); }
	void clear(void) { BRStringClear(this); }
	Word copy(char *Output,Word MaxLen = (Word)-1) const { return BRStringCopy(this,Output,MaxLen); }
	Word pcopy(Byte *Output,Word MaxLen = 256) const { return BRStringPCopy(this,Output,MaxLen); }
	Word remove(Word Start,Word Len=(Word)-1) { return BRStringRemove(this,Start,Len); }
	Word insert(Word Start,const char *Text,Word MaxLen=(Word)-1) { return BRStringInsert(this,Start,Text,MaxLen); }
	BRString &left(Word Start,char PadChar=' ') { BRStringLeft(this,Start,PadChar); return *this; }
	BRString &right(Word Start,char PadChar=' ') { BRStringRight(this,Start,PadChar); return *this; }
	Word casesensitive(void) { return BRStringCaseSensitive(this); }
	Word caseinsensitive(void) { return BRStringCaseInsensitive(this); }
	Word setcaseflag(Word NewFlag) { return BRStringSetCase(this,NewFlag); }
	int compare(const BRString &Input) const { return BRStringCompareBRString(this,&Input); }
	int compare(const char *Input) const { return BRStringComparePChar(this,Input); }
	int compare(char Input) const { return BRStringCompareChar(this,Input); }
	Word removech(const char *Input) { return BRStringRemoveChars(this,Input); }
	Word removech(char Input) { return BRStringRemoveChar(this,Input); }
};

#endif

#endif
