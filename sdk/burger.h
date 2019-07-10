/*******************************

	The Burger Bill Universal library
	Main typedefs header (This is usually included
	by everything!!!)

	Copyright 2008 Rebecca Ann Heineman, All Rights Reserved.
	This header and associated libraries are distributed and may only be used for Tread.
	To obtain a copy of this SDK for any other purpose please visit: 
	http://www.burgerbecky.com/burgerlib.htm

*******************************/

#ifndef __BURGER__
#define __BURGER__

/* Common defines */

#define __MOUSE__
#define __JOYSTICK__
#define __KEYBOARD__

#ifndef FASTCALL		/* prevent "macro redefinition" error when source also (bloatedly) includes Burger.h */
#define FASTCALL
#endif
#ifndef ANSICALL
#define ANSICALL
#endif

/* Now determine the compiler and create the machine specific types */

#if defined(__WATCOMC__)		/* DOS? */
#define __IBM__
#define __DOS__
#define __INTEL__

#elif defined(__BEOS__)			/* GNUC for BeOS */

#elif defined(__MWERKS__) || defined(__MRC__)		/* Codewarrior? Mac/BeOS/Win95 */
#if macintosh			/* MacOS? */
#define __MAC__
#define __MACOS__
#if !defined(__POWERPC__)
#define __68K__
#endif

#elif defined(__MACH__)	/* MacOSX? */
#define __MACOSX__

#elif !defined(__be_os) || (__be_os != __dest_os) /* Metrowerks for Win95 */
#define __WIN95__
#define __INTEL__

#else	/* Metrowerks for BeOS */
#define __BEOS__
#endif

#elif defined(__MACOSX__)		/* MacOSX */
#define __POWERPC__

#else				/* I assume Microsquish C++ 5.0 */
#pragma warning(disable:4103)
#pragma warning(disable:4291)
#undef ANSICALL
#undef FASTCALL
#define ANSICALL __cdecl
#define FASTCALL __fastcall		/* Use fastcall parms for MSVC++ */
#define __WIN95__
#define __INTEL__
#endif

#ifndef INLINECALL
#if defined(__cplusplus) || defined(__MWERKS__)
#define INLINECALL inline		/* Use the C++ standard keyword */
#elif defined(__WIN95__) && !defined(__WATCOMC__)
#define INLINECALL __inline		/* Keyword for Visual C 6.0 */
#else
#define INLINECALL				/* Not supported */
#endif
#endif

#if defined(__INTEL__)		/* Little endian machine? */
#define __LITTLEENDIAN__
#else
#define __BIGENDIAN__
#endif

#if !defined(TRUE)
#define TRUE 1
#define FALSE 0
#endif

#undef NDEBUG
#if !defined(_DEBUG) && (defined(__MWERKS__) || defined(__MRC__))
#if __option(sym)	/* Ask code warrior about it */
#define _DEBUG 1
#endif
#endif
#ifndef _DEBUG
#define _DEBUG 0
#endif
#if !_DEBUG
#define NDEBUG 1	/* Do not allow assert.h to make code */
#endif

typedef unsigned int Word;
typedef unsigned long LongWord;
#if !defined(__MAC__) || !defined(__MACTYPES__)
typedef unsigned char Byte;
typedef unsigned char Boolean;
typedef long Fixed;
#endif
typedef unsigned short Short;
typedef long Frac;

#if defined(__LITTLEENDIAN__)
#define LoadIntelLong(x) x
#define LoadIntelShort(x) x
#define LoadIntelUShort(x) x
#define LoadIntelDouble(x) (x)[0]
#define LoadIntelLongPtr(x) (x)[0]
#define LoadIntelShortPtr(x) (x)[0]
#define LoadIntelUShortPtr(x) (x)[0]
#define LoadMotoLong(x) SwapULong(x)
#define LoadMotoShort(x) SwapShort(x)
#define LoadMotoUShort(x) SwapUShort(x)
#define LoadMotoDouble(x) SwapDouble(x)
#define LoadMotoLongPtr(x) SwapULong((x)[0])
#define LoadMotoShortPtr(x) SwapShort((x)[0])
#define LoadMotoUShortPtr(x) SwapUShort((x)[0])
#else
#define LoadIntelLong(x) SwapULong(x)
#define LoadIntelShort(x) SwapShort(x)
#define LoadIntelUShort(x) SwapUShort(x)
#define LoadIntelDouble(x) SwapDouble(x)
#if defined(__POWERPC__)
#define LoadIntelLongPtr(x) __lwbrx((void *)(x),0)
#define LoadIntelShortPtr(x) ((short)__lhbrx((void *)(x),0))
#define LoadIntelUShortPtr(x) ((Short)__lhbrx((void *)(x),0))
#else
#define LoadIntelLongPtr(x) SwapULong((x)[0])
#define LoadIntelShortPtr(x) SwapShort((x)[0])
#define LoadIntelUShortPtr(x) SwapUShort((x)[0])
#endif
#define LoadMotoLong(x) x
#define LoadMotoShort(x) x
#define LoadMotoUShort(x) x
#define LoadMotoDouble(x) (x)[0]
#define LoadMotoLongPtr(x) (x)[0]
#define LoadMotoShortPtr(x) (x)[0]
#define LoadMotoUShortPtr(x) (x)[0]
#endif

#if _DEBUG
#define BRASSERT(f) if (!(f)) { Fatal("Assertion failed at %s, %i", __FILE__, __LINE__);}
#else
#define BRASSERT(f)
#endif

#if defined(__MAC__)
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif
#endif

#include <stdio.h>
#include <string.h>

#if defined(__INTEL__) && !defined(__BEOS__)
#include <pshpack1.h>		/* Byte align structures */
#elif defined(__MWERKS__)
#pragma options align=mac68k
#endif

#if defined(__MWERKS__)		/* Allow nameless structs */
#pragma ANSI_strict off
#pragma enumsalwaysint on
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LBPoint {	/* Point coord in 2D space */
	int	x;		/* X coord of point */
	int y;		/* Y coord of point */
} LBPoint;

typedef struct LBRect {	/* Rect coord in 2D space */
	int left;	/* Topleft x of rect */
	int top;	/* Topleft y of rect */
	int right;	/* Bottomright x of rect */
	int bottom;	/* Bottomright y of rect */
} LBRect;

typedef struct LBRectList {	/* Array of rects */
	Word NumRects;	/* Current number of rects in list */
	Word MaxRects;	/* Size of the array */
	LBRect **RectList;	/* Handle to array of rects */
} LBRectList;

#if defined(__MAC__)
typedef struct Point SYSPOINT;		/* Mac OS has a precompiler header! */
typedef struct Rect SYSRECT;
#elif defined(__BEOS__)
typedef struct {			/* BeOS! */
	float x;
	float y;
} SYSPOINT;
typedef struct {
	float left;
	float top;
	float right;
	float bottom;
} SYSRECT;
#else
typedef struct {			/* Dos and Windows! */
	long x;
	long y;
} SYSPOINT;
typedef struct {
	long left;
	long top;
	long right;
	long bottom;
} SYSRECT;
#endif

/* IBM specific data structures */

#define HANDLELOCK 0x80		/* Lock flag */
#define HANDLEFIXED 0x40	/* Fixed memory flag */
#define HANDLEMALLOC 0x20	/* Memory was Malloc'd */
#define HANDLEINUSE 0x10	/* True if handle is used */
#define HANDLEPURGEBITS 0x01	/* Allow purge flag */

#define REZ_MEM_ID 0xFFF1	/* Resource manager memory ID */
#define DEBUG_MEM_ID 0xFFF2	/* Memory debugger memory ID */
#define DEAD_MEM_ID 0xFFFE	/* Dead memory space ID */

typedef struct MyHandle {
	void *MemPtr;			/* Pointer to true memory (Must be first!) */
	LongWord Length;		/* Length of memory */
	LongWord Flags;			/* Memory flags or parent used handle */
	struct MyHandle *NextHandle;	/* Next handle in the chain */
	struct MyHandle *PrevHandle;
} MyHandle;

/* 64 bit value handler */

#define LONGWORD64NATIVE
#if defined(__MWERKS__) || defined(__MRC__)
typedef long long LongWord64_t;
#elif defined(__WATCOMC__) || defined(__WIN95__)
typedef __int64 LongWord64_t;
#else
#undef LONGWORD64NATIVE
typedef struct LongWord64_t {
#if defined(__BIGENDIAN__)
	long hi;		/* Upper 32 bits */
	LongWord lo;	/* Lower 32 bits */
#else
	LongWord lo;	/* Lower 32 bits */
	long hi;		/* Upper 32 bits */
#endif
} LongWord64_t;
#endif

/* 64 bit value handler */

#ifdef LONGWORD64NATIVE
#define LongWord64Add(Output,Input) (Output)[0]+=(Input)[0]
#define LongWord64Add3(Output,First,Second) (Output)[0] = (First)[0]+(Second)[0]
#define LongWord64Sub(Output,Input) (Output)[0]-=(Input)[0]
#define LongWord64Sub3(Output,First,Second) (Output)[0] = (First)[0]-(Second)[0]
#define LongWord64Mul(Output,Input) (Output)[0]*=(Input)[0]
#define LongWord64Mul3(Output,First,Second) (Output)[0] = (First)[0]*(Second)[0]
#define LongWord64ToDouble(Input) (double)((Input)[0])
#define LongWord64FromDouble(Output,Input) (Output)[0] = (LongWord64_t)Input
#define LongWord64MulLongTo64(Output,First,Second) (Output)[0] = XLongWord64MulLongTo64(First,Second)
#define LongWord64Negate(Input) (Input)[0] = -(Input)[0]
#define LongWord64FromLong(Output,Input) (Output)[0] = (LongWord64_t)Input
#define LongWord64FromLong2(x,y,z) ((x)[0] = ((LongWord64_t)y)+((LongWord64_t)z<<32))
#define LongWord64ToLong(Input) (long)((Input)[0])
#define LongWord64ToHiLong(Input) (long)((Input)[0]>>32)
#define LongWord64Compare(First,Second) XLongWord64Compare((First)[0],(Second)[0])
#define LongWord64DivideByLong(Numerator,Denominator) (Numerator)[0] = XLongWord64DivideByLong((Numerator)[0],Denominator)
extern LongWord64_t FASTCALL XLongWord64MulLongTo64(long First,long Second);
extern int FASTCALL XLongWord64Compare(LongWord64_t First,LongWord64_t Second);
extern LongWord64_t FASTCALL XLongWord64DivideByLong(LongWord64_t Numerator,long Denominator);
#else
extern void FASTCALL LongWord64Add(LongWord64_t *Output,const LongWord64_t *Input);
extern void FASTCALL LongWord64Add3(LongWord64_t *Output,const LongWord64_t *First,const LongWord64_t *Second);
extern void FASTCALL LongWord64Sub(LongWord64_t *Output,const LongWord64_t *Input);
extern void FASTCALL LongWord64Sub3(LongWord64_t *Output,const LongWord64_t *First,const LongWord64_t *Second);
extern void FASTCALL LongWord64Mul(LongWord64_t *Output,const LongWord64_t *Input);
extern void FASTCALL LongWord64Mul3(LongWord64_t *Output,const LongWord64_t *First,const LongWord64_t *Second);
extern double FASTCALL LongWord64ToDouble(const LongWord64_t *Input);
extern void FASTCALL LongWord64FromDouble(LongWord64_t *Output,double Input);
extern void FASTCALL LongWord64MulLongTo64(LongWord64_t *Output,long First,long Second);
extern void FASTCALL LongWord64Negate(LongWord64_t *Input);
extern void FASTCALL LongWord64FromLong(LongWord64_t *Output,long Input);
#define LongWord64FromLong2(x,y,z) ((x).lo=y),((x.hi)=z)
#define LongWord64ToLong(Input) (long)((Input).lo)
#define LongWord64ToHiLong(Input) (long)((Input).hi)
extern int FASTCALL LongWord64Compare(LongWord64_t *First,LongWord64_t *Second);
#endif

#if defined(__WATCOMC__)
#pragma aux XLongWord64MulLongTo64 = "imul edx" parm [eax] [edx] value [eax edx]
#pragma aux XLongWord64DivideByLong = "idiv ebx" parm [eax] [edx] [ebx] modify exact [eax edx] value [eax edx]
#endif

/* Math functions */

typedef struct Vector2D_t {
	float x,y;
} Vector2D_t;

typedef struct Vector3D_t {
	float x,y,z;
} Vector3D_t;

typedef struct Matrix3D_t {
	Vector3D_t x,y,z;
} Matrix3D_t;

typedef struct Quat_t {
	float x,y,z,w;
} Quat_t;

typedef struct Matrix4D_t {
	Quat_t x,y,z,w;
} Matrix4D_t;

typedef struct FixedEuler_t {
	Fixed x,y,z;
} FixedEuler_t;

typedef struct Euler_t {
	int x,y,z;
} Euler_t;

#define	Pi 3.141592653589793238		/* Pretty accurate eh? */
#define ANGLERANGE 2048		   		/* Number of angles in angle table */
#define ANGLEMASK (ANGLERANGE-1)	/* Rounding mask for angles */
#define TANTABLESIZE 2048			/* Size of the Tangent arrays */
#define FLOATRECIPTABLESIZE 1024	/* Size of the float recip table */

/* Math functions */

extern float FAngleArray[TANTABLESIZE+1];
extern Fixed FixedArcCosineTable[ANGLERANGE+1];
extern Fixed FixedArcSineTable[ANGLERANGE+1];
extern float FSineTable[ANGLERANGE+(ANGLERANGE/4)];
extern float FloatRecipTable[FLOATRECIPTABLESIZE];
extern float FASTCALL SqrtFast(float Input);
extern float FASTCALL CeilingFast(float Input);
extern int FASTCALL CeilingFastInt(float Input);
extern float FASTCALL FloorFast(float Input);
extern int FASTCALL FloorFastInt(float Input);
extern int FASTCALL FloatToInt(float Input);
extern float FASTCALL FloatSine(float Angle);
extern float FASTCALL FloatCosine(float Angle);
extern float FASTCALL FloatSineInt(int Angle);
extern float FASTCALL FloatCosineInt(int Angle);
extern float FASTCALL FloatATan2(float x,float y);
extern Fixed FASTCALL FixedArcCosine(Fixed c);
extern Fixed FASTCALL FloatArcCosine(float c);
extern Fixed FASTCALL FixedArcSine(Fixed s);
extern Fixed FASTCALL FloatArcSine(float s);
extern float FASTCALL FloatAbs(float Input);

extern void FASTCALL Vector2DInit(Vector2D_t *Input,float x,float y);
#define Vector2DDestroy(x)
#define Vector2DSet(a,x,y) Vector2DInit(a,x,y)
extern void FASTCALL Vector2DZero(Vector2D_t *Output);
extern void FASTCALL Vector2DNegate(Vector2D_t *Input);
extern void FASTCALL Vector2DNegate2(Vector2D_t *Output,const Vector2D_t *Input);
extern float FASTCALL Vector2DDot(const Vector2D_t *Input1,const Vector2D_t *Input2);
extern float FASTCALL Vector2DCross(const Vector2D_t *Input1,const Vector2D_t *Input2);
extern float FASTCALL Vector2DGetRadiusSqr(const Vector2D_t *Input);
extern float FASTCALL Vector2DGetRadius(const Vector2D_t *Input);
extern float FASTCALL Vector2DGetRadiusFast(const Vector2D_t *Input);
extern float FASTCALL Vector2DGetDistanceSqr(const Vector2D_t *Input1,const Vector2D_t *Input2);
extern float FASTCALL Vector2DGetDistance(const Vector2D_t *Input1,const Vector2D_t *Input2);
extern float FASTCALL Vector2DGetDistanceFast(const Vector2D_t *Input1,const Vector2D_t *Input2);
extern void FASTCALL Vector2DNormalize(Vector2D_t *Input);
extern void FASTCALL Vector2DNormalize2(Vector2D_t *Output,const Vector2D_t *Input);
extern void FASTCALL Vector2DNormalize3(Vector2D_t *Output,float x,float y);
extern void FASTCALL Vector2DNormalizeFast(Vector2D_t *Input);
extern void FASTCALL Vector2DNormalizeFast2(Vector2D_t *Output,const Vector2D_t *Input);
extern void FASTCALL Vector2DNormalizeFast3(Vector2D_t *Output,float x,float y);

extern void FASTCALL Vector3DInit(Vector3D_t *Input,float x,float y,float z);
#define Vector3DSet(a,x,y,z) Vector3DInit(a,x,y,z)
extern void FASTCALL Vector3DZero(Vector3D_t *Output);
extern void FASTCALL Vector3DFromIntVector3D(Vector3D_t *Output,const struct FixedVector3D_t *Input);
extern void FASTCALL Vector3DFromFixedVector3D(Vector3D_t *Output,const struct FixedVector3D_t *Input);
extern void FASTCALL Vector3DNegate(Vector3D_t *Input);
extern void FASTCALL Vector3DNegate2(Vector3D_t *Output,const Vector3D_t *Input);
extern void FASTCALL Vector3DAdd(Vector3D_t *Output,const Vector3D_t *Input);
extern void FASTCALL Vector3DAdd3(Vector3D_t *Output,const Vector3D_t *Input1,const Vector3D_t *Input2);
extern void FASTCALL Vector3DSub(Vector3D_t *Output,const Vector3D_t *Input);
extern void FASTCALL Vector3DSub3(Vector3D_t *Output,const Vector3D_t *Input1,const Vector3D_t *Input2);
extern void FASTCALL Vector3DMul(Vector3D_t *Output,float Val);
extern void FASTCALL Vector3DMul3(Vector3D_t *Output,const Vector3D_t *Input,float Val);
extern Word FASTCALL Vector3DEqual(const Vector3D_t *Input1,const Vector3D_t *Input2);
extern Word FASTCALL Vector3DEqualWithinRange(const Vector3D_t *Input1,const Vector3D_t *Input2,float Range);
extern float FASTCALL Vector3DGetAxis(const Vector3D_t *Input,Word Axis);
extern void FASTCALL Vector3DSetAxis(Vector3D_t *Output,Word Axis,float Val);
extern float FASTCALL Vector3DDot(const Vector3D_t *Input1,const Vector3D_t *Input2);
extern void FASTCALL Vector3DCross(Vector3D_t *Output,const Vector3D_t *Input1,const Vector3D_t *Input2);
extern float FASTCALL Vector3DGetRadiusSqr(const Vector3D_t *Input);
extern float FASTCALL Vector3DGetRadius(const Vector3D_t *Input);
extern float FASTCALL Vector3DGetRadiusFast(const Vector3D_t *Input);
extern float FASTCALL Vector3DGetDistanceSqr(const Vector3D_t *Input1,const Vector3D_t *Input2);
extern float FASTCALL Vector3DGetDistance(const Vector3D_t *Input1,const Vector3D_t *Input2);
extern float FASTCALL Vector3DGetDistanceFast(const Vector3D_t *Input1,const Vector3D_t *Input2);
extern void FASTCALL Vector3DSetRadius(Vector3D_t *Input,float Len);
extern void FASTCALL Vector3DNormalize(Vector3D_t *Input);
extern void FASTCALL Vector3DNormalize2(Vector3D_t *Output,const Vector3D_t *Input);
extern void FASTCALL Vector3DNormalize3(Vector3D_t *Output,float x,float y,float z);
extern void FASTCALL Vector3DNormalizeFast(Vector3D_t *Input);
extern void FASTCALL Vector3DNormalizeFast2(Vector3D_t *Output,const Vector3D_t *Input);
extern void FASTCALL Vector3DNormalizeFast3(Vector3D_t *Output,float x,float y,float z);
extern void FASTCALL Vector3DNormalizeToLen(Vector3D_t *Input,float Len);
extern void FASTCALL Vector3DNormalizeToLen2(Vector3D_t *Output,const Vector3D_t *Input,float Len);

#define Matrix3DInit(x) Matrix3DZero(x)
extern void FASTCALL Matrix3DZero(Matrix3D_t *Input);
extern void FASTCALL Matrix3DIdentity(Matrix3D_t *Input);
extern void FASTCALL Matrix3DFromFixedMatrix3D(Matrix3D_t *Output,const struct FixedMatrix3D_t *Input);
extern void FASTCALL Matrix3DSet(Matrix3D_t *Output,float yaw,float pitch,float roll);
extern void FASTCALL Matrix3DSetYaw(Matrix3D_t *Output,float yaw);
extern void FASTCALL Matrix3DSetPitch(Matrix3D_t *Output,float pitch);
extern void FASTCALL Matrix3DSetRoll(Matrix3D_t *Output,float roll);
extern void FASTCALL Matrix3DSetInt(Matrix3D_t *Output,Word yaw,Word pitch,Word roll);
extern void FASTCALL Matrix3DSetYawInt(Matrix3D_t *Output,Word yaw);
extern void FASTCALL Matrix3DSetPitchInt(Matrix3D_t *Output,Word pitch);
extern void FASTCALL Matrix3DSetRollInt(Matrix3D_t *Output,Word roll);
extern void FASTCALL Matrix3DSetFixed(Matrix3D_t *Output,Fixed yaw,Fixed pitch,Fixed roll);
extern void FASTCALL Matrix3DSetYawFixed(Matrix3D_t *Output,Fixed yaw);
extern void FASTCALL Matrix3DSetPitchFixed(Matrix3D_t *Output,Fixed pitch);
extern void FASTCALL Matrix3DSetRollFixed(Matrix3D_t *Output,Fixed roll);
extern void FASTCALL Matrix3DSetFromEuler(Matrix3D_t *Output,const Euler_t *Input);
extern void FASTCALL Matrix3DSetFromFixedEuler(Matrix3D_t *Output,const FixedEuler_t *Input);
extern void FASTCALL Matrix3DSetFromQuat(Matrix3D_t *Output,const Quat_t *Input);
extern void FASTCALL Matrix3DSetTranslate2D(Matrix3D_t *Input,float xVal,float yVal);
extern void FASTCALL Matrix3DTranspose(Matrix3D_t *Input);
extern void FASTCALL Matrix3DTranspose2(Matrix3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DMul(Matrix3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DMul2(Matrix3D_t *Output,const Matrix3D_t *Input1,const Matrix3D_t *Input2);
extern void FASTCALL Matrix3DGetXVector(Vector3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DGetYVector(Vector3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DGetZVector(Vector3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DTransformVector3D(Vector3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DTransformVector3D2(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Input2);
extern void FASTCALL Matrix3DTransformVector3DAdd(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Add);
extern void FASTCALL Matrix3DTransformVector3DAdd2(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Add,const Vector3D_t *InputV);
extern void FASTCALL Matrix3DITransformVector3D(Vector3D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DITransformVector3D2(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Input2);
extern void FASTCALL Matrix3DITransformVector3DAdd(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Add);
extern void FASTCALL Matrix3DITransformVector3DAdd2(Vector3D_t *Output,const Matrix3D_t *Input,const Vector3D_t *Add,const Vector3D_t *InputV);
extern void FASTCALL Matrix3DTransform2D(Vector2D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DTransform2D2(Vector2D_t *Output,const Matrix3D_t *Input,const Vector2D_t *InputV);
extern void FASTCALL Matrix3DITransform2D(Vector2D_t *Output,const Matrix3D_t *Input);
extern void FASTCALL Matrix3DITransform2D2(Vector2D_t *Output,const Matrix3D_t *Input,const Vector2D_t *InputV);

extern void FASTCALL EulerFromMatrix3D(Euler_t *Output,const Matrix3D_t *Input);
extern void FASTCALL FixedEulerFromMatrix3D(FixedEuler_t *Output,const Matrix3D_t *Input);

extern void FASTCALL QuatIdentity(Quat_t *Input);
extern void FASTCALL QuatNormalize(Quat_t *Input);
extern float FASTCALL QuatDot(const Quat_t *Input1,const Quat_t *Input2);
extern void FASTCALL QuatMul2(Quat_t *Output,const Quat_t *Input1,const Quat_t *Input2);

extern void FASTCALL Matrix4DZero(Matrix4D_t *Input);
extern void FASTCALL Matrix4DIdentity(Matrix4D_t *Input);
extern void FASTCALL Matrix4DSetTranslate3D(Matrix4D_t *Output,float xVal,float yVal,float zVal);
extern void FASTCALL Matrix4DSetScale(Matrix4D_t *Output,float xVal,float yVal,float zVal);
extern void FASTCALL Matrix4DTransformVector3D(Vector3D_t *Output,const Matrix4D_t *Input);
extern void FASTCALL Matrix4DTransformVector3D2(Vector3D_t *Output,const Matrix4D_t *Input,const Vector3D_t *Input2);
extern void FASTCALL Matrix4DITransformVector3D(Vector3D_t *Output,const Matrix4D_t *Input);
extern void FASTCALL Matrix4DITransformVector3D2(Vector3D_t *Output,const Matrix4D_t *Input,const Vector3D_t *Input2);

/* Inlines... */

#if defined(__INTEL__) || defined(__POWERPC__)
#define Vector3DZero(a) {(a)->x=0;(a)->y=0;(a)->z=0;}
#define Vector3DNegate(a) {(a)->x=-(a)->x;(a)->y=-(a)->y;(a)->z=-(a)->z;}
#define Vector3DNegate2(a,b) {(a)->x=-(b)->x;(a)->y=-(b)->y;(a)->z=-(b)->z;}
#define Vector3DAdd(a,b) {(a)->x=(a)->x+(b)->x;(a)->y=(a)->y+(b)->y;(a)->z=(a)->z+(b)->z;}
#define Vector3DAdd3(a,b,c) {(a)->x=(b)->x+(c)->x;(a)->y=(b)->y+(c)->y;(a)->z=(b)->z+(c)->z;}
#define Vector3DSub(a,b) {(a)->x=(a)->x-(b)->x;(a)->y=(a)->y-(b)->y;(a)->z=(a)->z-(b)->z;}
#define Vector3DSub3(a,b,c) {(a)->x=(b)->x-(c)->x;(a)->y=(b)->y-(c)->y;(a)->z=(b)->z-(c)->z;}
#define Vector3DMul(a,b) {(a)->x=(a)->x*(b);(a)->y=(a)->y*(b);(a)->z=(a)->z*(b);}
#define Vector3DMul3(a,b,c) {(a)->x=(b)->x*(c);(a)->y=(b)->y*(c);(a)->z=(b)->z*(c);}
#define Vector3DDot(a,b) (((a)->x*(b)->x) + ((a)->y*(b)->y) + ((a)->z * (b)->z))
#endif

#if defined(__INTEL__)
#define FloatAbs(x) (float)fabs(x)
#elif defined(__POWERPC__)
#define FloatAbs(x) (float)__fabs(x)
#endif

/* Fixed point math */

typedef struct FixedVector2D_t {
	Fixed x,y;
} FixedVector2D_t;

typedef struct FixedVector3D_t {
	Fixed x,y,z;
} FixedVector3D_t;

typedef struct FixedMatrix3D_t {
	FixedVector3D_t x,y,z;
} FixedMatrix3D_t;

typedef struct FixedQuat_t {
	Fixed x,y,z,w;
} FixedQuat_t;

typedef struct FixedMatrix4D_t {
	FixedQuat_t x,y,z,w;
} FixedMatrix4D_t;

#define FLOATTOFIXED(x) (Fixed)((x)*65536.0f)
#define FIXEDTOFLOAT(x) (float)((x)*(1.0f/65536.0f))
#define INTTOFIXED(x) (Fixed)((x)<<16)
#define FIXEDTOINT(x) FixedToInt(x)

extern Word FASTCALL IMLoWord(LongWord Input);
extern Word FASTCALL IMHiWord(LongWord Input);
extern LongWord FASTCALL IMHexIt(Word Value);
extern LongWord FASTCALL IMMultiply(Word InputA,Word InputB);
extern Word FASTCALL IMIntSqrt(LongWord Input);
extern LongWord FASTCALL IMFixSqrt(LongWord x);
extern long FASTCALL IMIntMulRatioFast(long Mul1,long Mul2,long Div);
extern long FASTCALL IntDblMulAdd(long Mul1,long Mul2,long Mul3,long Mul4);
extern long FASTCALL IntMulHigh32(long Mul1,long Mul2);

extern Fixed FASTCALL IMFixMul(Fixed InputA,Fixed InputB);
extern Fixed FASTCALL IMFixMulFast(Fixed InputA,Fixed InputB);
extern Fixed FASTCALL IMFixDiv(Fixed Numerator,Fixed Denominator);
extern Fixed FASTCALL IMFixDivFast(Fixed Numerator,Fixed Denominator);
extern int FASTCALL IMFixRound(Fixed Input);
extern Fixed FASTCALL IMLong2Fix(long Input);
extern Fixed FASTCALL IMFixReciprocal(Fixed Input);
extern Fixed FASTCALL IMFixATan2(long Input1,long Input2);

extern Frac FASTCALL IMFracCos(Fixed Input);
extern Frac FASTCALL IMFracSin(Fixed Input);
extern Frac FASTCALL IMFracMul(Frac InputA,Frac InputB);
extern Frac FASTCALL IMFracDiv(Frac Numerator,Frac Denominator);

extern Fixed FASTCALL FloatToFixed(float Input);
extern float FASTCALL FixedToFloat(Fixed Input);
extern int FASTCALL FixedToInt(Fixed Input);

extern void FASTCALL FixedVector3DInit(FixedVector3D_t *Input,Fixed x,Fixed y,Fixed z);
#define FixedVector3DDestroy(x)
#define FixedVector3DSet(a,x,y,z) FixedVector3DInit(a,x,y,z)
extern void FASTCALL FixedVector3DZero(FixedVector3D_t *Output);
extern void FASTCALL FixedVector3DFromVector3D(FixedVector3D_t *Output,const struct Vector3D_t *Input);
extern void FASTCALL IntVector3DFromVector3D(FixedVector3D_t *Output,const struct Vector3D_t *Input);
extern void FASTCALL FixedVector3DNegate(FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DNegate2(FixedVector3D_t *Output,const FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DAdd(FixedVector3D_t *Output,const FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DAdd3(FixedVector3D_t *Output,const FixedVector3D_t *Input1,const FixedVector3D_t *Input2);
extern void FASTCALL FixedVector3DSub(FixedVector3D_t *Output,const FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DSub3(FixedVector3D_t *Output,const FixedVector3D_t *Input1,const FixedVector3D_t *Input2);
extern void FASTCALL FixedVector3DMul(FixedVector3D_t *Output,Fixed Val);
extern void FASTCALL FixedVector3DMul3(FixedVector3D_t *Output,const FixedVector3D_t *Input,Fixed Val);
extern Word FASTCALL FixedVector3DEqual(const FixedVector3D_t *Input1,const FixedVector3D_t *Input2);
extern Word FASTCALL FixedVector3DEqualWithinRange(const FixedVector3D_t *Input1,const FixedVector3D_t *Input2,Fixed Range);
extern Fixed FASTCALL FixedVector3DGetAxis(const FixedVector3D_t *Input,Word Axis);
extern void FASTCALL FixedVector3DSetAxis(FixedVector3D_t *Output,Word Axis,Fixed Val);
extern Fixed FASTCALL FixedVector3DDot(const FixedVector3D_t *Input1,const FixedVector3D_t *Input2);
extern void FASTCALL FixedVector3DCross(FixedVector3D_t *Output,const FixedVector3D_t *Input1,const FixedVector3D_t *Input2);
extern Fixed FASTCALL FixedVector3DGetRadiusSqr(const FixedVector3D_t *Input);
extern Fixed FASTCALL FixedVector3DGetRadius(const FixedVector3D_t *Input);
extern Fixed FASTCALL FixedVector3DGetRadiusFast(const FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DSetRadius(FixedVector3D_t *Input,Fixed Len);
extern void FASTCALL FixedVector3DNormalize(FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DNormalizeFast(FixedVector3D_t *Input);
extern void FASTCALL FixedVector3DNormalizeToLen(FixedVector3D_t *Input,Fixed Len);

#define FixedMatrix3DInit(x) FixedMatrix3DZero(x)
#define FixedMatrix3DDestroy(x)
extern void FASTCALL FixedMatrix3DZero(FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DIdentity(FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DFromMatrix3D(FixedMatrix3D_t *Output,const struct Matrix3D_t *Input);
extern void FASTCALL FixedMatrix3DSet(FixedMatrix3D_t *Output,Fixed yaw,Fixed pitch,Fixed roll);
extern void FASTCALL FixedMatrix3DSetYaw(FixedMatrix3D_t *Output,Fixed yaw);
extern void FASTCALL FixedMatrix3DSetPitch(FixedMatrix3D_t *Output,Fixed pitch);
extern void FASTCALL FixedMatrix3DSetRoll(FixedMatrix3D_t *Output,Fixed roll);
extern void FASTCALL FixedMatrix3DTranspose(FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DTranspose2(FixedMatrix3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DMul(FixedMatrix3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DMul2(FixedMatrix3D_t *Output,const FixedMatrix3D_t *Input1,const FixedMatrix3D_t *Input2);
extern void FASTCALL FixedMatrix3DGetXVector(FixedVector3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DGetYVector(FixedVector3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DGetZVector(FixedVector3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DMulVector(FixedVector3D_t *Output,const FixedMatrix3D_t *Input);
extern void FASTCALL FixedMatrix3DMulVector2(FixedVector3D_t *Output,const FixedMatrix3D_t *Input,const FixedVector3D_t *Input2);
extern void FASTCALL FixedMatrix3DMulVectorAddVector(FixedVector3D_t *Output,const FixedMatrix3D_t *Input,const FixedVector3D_t *Add);
extern void FASTCALL FixedMatrix3DMulVectorAddVector2(FixedVector3D_t *Output,const FixedMatrix3D_t *Input,const FixedVector3D_t *Add,const FixedVector3D_t *InputV);

extern void FASTCALL FixedQuatIdentity(FixedQuat_t *Input);

#if defined(__WATCOMC__)
#pragma aux IMFixMulFast = "imul edx" "shrd eax,edx,16" parm [eax] [edx] value [eax] modify exact [eax edx]
#pragma aux IMFixDivFast = "mov edx,eax" "shl eax,16" "sar edx,16" "idiv ebx" parm [eax] [ebx] value [eax] modify exact [eax edx]
#pragma aux IMIntMulRatioFast = "imul edx" "idiv ebx" parm [eax] [edx] [ebx] value [eax] modify exact [eax edx]
#elif defined(__68K__)
#pragma parameter __D0 IMIntMulRatioFast(__D0,__D1,__D2)
long IMIntMulRatioFast(long a,long b,long c) = {0x4c01,0xc01,0x4c42,0xc01};	/* muls.l d1,d1:d0 divs.l ds,d1:d0 */
#endif

/* Time and events */

typedef struct TimeDate_t {	/* Used by FileModTime routines */
	LongWord Year;		/* Year 2002 */
	Short Milliseconds;	/* 0-999 */
	Byte Month;		/* 1-12 */
	Byte Day;		/* 1-31 */
	Byte DayOfWeek;	/* 0-6 */
	Byte Hour;		/* 0-23 */
	Byte Minute;	/* 0-59 */
	Byte Second;	/* 0-59 */
} TimeDate_t;

#define TICKSPERSEC 60
extern LongWord LastTick;
extern LongWord FASTCALL ReadTick(void);
#define ResetLastTick() LastTick=ReadTick()
#define WaitEvent() WaitTicksEvent(0)
#define WaitOneTick() ResetLastTick(),WaitTick()
#define WaitTick() WaitTicks(1)
extern void FASTCALL WaitTicks(Word TickCount);
extern Word FASTCALL WaitTicksEvent(Word TickCount);
extern LongWord FASTCALL ReadTickMicroseconds(void);
extern LongWord FASTCALL ReadTickMilliseconds(void);
extern void FASTCALL TimeDateGetCurrentTime(TimeDate_t *Input);
extern Word FASTCALL TimeDateFromANSITime(TimeDate_t *Output,LongWord Input);
extern void FASTCALL TimeDateTimeString(char *Output,const TimeDate_t *Input);
extern void FASTCALL TimeDateTimeStringPM(char *Output,const TimeDate_t *Input);
extern void FASTCALL TimeDateDateString(char *Output,const TimeDate_t *Input);
extern void FASTCALL TimeDateDateStringVerbose(char *Output,const TimeDate_t *Input);
extern void FASTCALL TimeDateStreamHandleRead(TimeDate_t *Output,struct StreamHandle_t *Input);
extern void FASTCALL TimeDateStreamHandleWrite(const TimeDate_t *Input,struct StreamHandle_t *Output);

/* Thread manager */

typedef LongWord Mutex_t;
typedef struct TimerTask_t TimerTask_t;
typedef Word (FASTCALL *TimerTaskProc)(void *DataPtr);

extern Word FASTCALL MutexLock(Mutex_t *Input);
extern void FASTCALL MutexUnlock(Mutex_t *Input);
extern TimerTask_t * FASTCALL TimerTaskNew(Word Period,TimerTaskProc Proc,void *ProcData,Word Active);
extern void FASTCALL TimerTaskDelete(TimerTask_t *Input);
extern Word FASTCALL TimerTaskGetActive(const TimerTask_t *Input);
extern Word FASTCALL TimerTaskSetActive(TimerTask_t *Input,Word Flag);
extern Word FASTCALL TimerTaskGetPeriod(const TimerTask_t *Input);
extern Word FASTCALL TimerTaskSetPeriod(TimerTask_t *Input,Word Period);

/* In Palette */

typedef struct HSL_t {
	float Hue;			/* Color hue 0 to 1 */
	float Saturation;	/* Color saturation 0 to 1 */
	float Luminance;	/* Color luminance 0 to 1 */
} HSL_t;

typedef struct RGB_t {
	float Red;			/* Red intensity 0 to 1 */
	float Green;		/* Green intensity 0 to 1 */
	float Blue;			/* Blue intensity 0 to 1 */
} RGB_t;

typedef void (FASTCALL *PaletteChangedProcPtr)(void);
typedef void (FASTCALL *PaletteFadeCallBackProcPtr)(Word Pass);

extern Byte CurrentPalette[256*3];		/* Current palette in the hardware (Read ONLY) */
extern const Word ByteSquareTable[255+256];	/* Table of -255 to 255 squared */
extern const Word RGB5ToRGB8Table[32];	/* Table to convert 5 bit color to 8 bit color */
extern const Word RGB6ToRGB8Table[64];	/* Table to convert 6 bit color to 8 bit color */
extern Word FadeSpeed;			/* Delay in Ticks for a palette change */
extern Word PaletteVSync;		/* Set to TRUE if the palette MUST be changed only during VSync */
extern PaletteFadeCallBackProcPtr PaletteFadeCallBack;
extern PaletteChangedProcPtr PaletteChangedProc;	/* Called whenever the palette was changed */

extern void FASTCALL PaletteConvertRGB15ToRGB24(Byte *RGBOut,Word RGBIn);
extern void FASTCALL PaletteConvertRGB16ToRGB24(Byte *RGBOut,Word RGBIn);
extern Word FASTCALL PaletteConvertRGB24ToRGB15(const Byte *RGBIn);
extern Word FASTCALL PaletteConvertRGB24ToRGB16(const Byte *RGBIn);
extern Word FASTCALL PaletteConvertRGB24ToDepth(const Byte *RGBIn,Word Depth);
extern Word FASTCALL PaletteConvertRGBToDepth(Word Red,Word Green,Word Blue,Word Depth);
extern Word FASTCALL PaletteConvertPackedRGBToDepth(LongWord Color,Word Depth);
extern void FASTCALL PaletteMake16BitLookup(Word *Output,const Byte *Input,Word Depth);
extern void FASTCALL PaletteMake16BitLookupRez(Word *Output,struct RezHeader_t *Input,Word RezNum,Word Depth);
extern void FASTCALL PaletteMakeRemapLookup(Byte *Output,const Byte *DestPal,const Byte *SourcePal);
extern void FASTCALL PaletteMakeRemapLookupMasked(Byte *Output,const Byte *DestPal,const Byte *SourcePal);
extern void FASTCALL PaletteMakeColorMasks(Byte *Output,Word MaskColor);
extern void FASTCALL PaletteMakeFadeLookup(Byte *Output,const Byte *Input,Word r,Word g,Word b);
extern Word FASTCALL PaletteFindColorIndex(const Byte *PalPtr,Word Red,Word Green,Word Blue,Word Count);
extern void FASTCALL PaletteBlack(void);
extern void FASTCALL PaletteWhite(void);
extern void FASTCALL PaletteFadeToBlack(void);
extern void FASTCALL PaletteFadeToWhite(void);
extern void FASTCALL PaletteFadeTo(struct RezHeader_t *Input,Word ResID);
extern void FASTCALL PaletteFadeToPtr(const Byte *PalettePtr);
extern void FASTCALL PaletteFadeToHandle(void **PaletteHandle);
extern void FASTCALL PaletteSet(struct RezHeader_t *Input,Word PalNum);
extern void FASTCALL PaletteSetHandle(Word Start,Word Count,void **PaletteHandle);
extern void FASTCALL PaletteSetPtr(Word Start,Word Count,const Byte *PalettePtr);
extern Word FASTCALL PaletteGetBorderColor(void);
extern void FASTCALL PaletteSetBorderColor(Word Color);
extern void FASTCALL PaletteRGB2HSL(HSL_t *Output,const RGB_t *Input);
extern void FASTCALL PaletteHSL2RGB(RGB_t *Output,const HSL_t *Input);
extern void FASTCALL PaletteHSLTween(HSL_t *Output,const HSL_t *HSLPtr1,const HSL_t *HSLPtr2,float Factor,Word Dir);
extern void FASTCALL PaletteRGBTween(RGB_t *Output,const RGB_t *RGBPtr1,const RGB_t *RGBPtr2,float Factor,Word Dir);

/* Input handlers */

#define PadLeft 0x1UL
#define PadRight 0x2UL
#define PadUp 0x4UL
#define PadDown 0x8UL
#define PadHatLeft 0x10UL
#define PadHatRight 0x20UL
#define PadHatUp 0x40UL
#define PadHatDown 0x80UL
#define PadThrottleUp 0x100UL
#define PadThrottleDown 0x200UL
#define PadTwistLeft 0x400UL
#define PadTwistRight 0x800UL
#define PadButton1 0x1000UL
#define PadButton2 0x2000UL
#define PadButton3 0x4000UL
#define PadButton4 0x8000UL
#define PadButton5 0x10000UL
#define PadButton6 0x20000UL
#define PadButton7 0x40000UL
#define PadButton8 0x80000UL
#define PadButton9 0x100000UL
#define PadButton10 0x200000UL
#define PadButton11 0x400000UL
#define PadButton12 0x800000UL
#define PadButton13 0x1000000UL
#define PadButton14 0x2000000UL
#define PadButton15 0x4000000UL
#define PadButton16 0x8000000UL
#define PadButton17 0x10000000UL
#define PadButton18 0x20000000UL
#define PadButton19 0x40000000UL
#define PadButton20 0x80000000UL

typedef struct JoyAutoRepeat_t {	/* Used by JoyAutoRepeater */
	LongWord JoyBits;	/* Bit field to test for */
	Word InitialTick;	/* Delay for initial joydown */
	Word RepeatTick;	/* Delay for repeater */
	LongWord TimeMark;	/* Internal time mark */
	Word HeldDown;		/* Zero this to init the struct */
} JoyAutoRepeat_t;

typedef void (FASTCALL *KeyboardCallBack)(void *);
typedef Word (FASTCALL *KeyboardGetchCallBackPtr)(Word Key);
typedef struct ForceFeedback_t ForceFeedback_t;
typedef struct ForceFeedbackData_t ForceFeedbackData_t;
typedef struct ForceFeedbackEffect_t ForceFeedbackEffect_t;

extern Word FASTCALL InputSetState(Word ActiveFlag);
extern Word FASTCALL InputGetState(void);

extern volatile Byte KeyArray[128];		/* Scan codes of keys pressed */
extern void FASTCALL KeyboardInit(void);
extern void FASTCALL KeyboardDestroy(void);
extern KeyboardGetchCallBackPtr KeyboardGetchCallBack;	/* Key stealers */
extern Word FASTCALL KeyboardGetch(void);
extern Word FASTCALL KeyboardKbhit(void);
extern void FASTCALL KeyboardAddRoutine(KeyboardCallBack Proc,void *Data);
extern void FASTCALL KeyboardRemoveRoutine(KeyboardCallBack Proc,void *Data);
extern void FASTCALL KeyboardFlush(void);
extern Word FASTCALL KeyboardGet(void);
extern Word FASTCALL KeyboardGet2(void);
extern void FASTCALL KeyboardCallPollingProcs(void);
extern Word FASTCALL KeyboardGetKeyLC(void);
extern Word FASTCALL KeyboardGetKeyUC(void);
extern Word FASTCALL KeyboardAnyPressed(void);
extern Word FASTCALL KeyboardIsPressed(Word ScanCode);
extern Word FASTCALL KeyboardHasBeenPressed(Word ScanCode);
extern void FASTCALL KeyboardClearKey(Word ScanCode);
extern Word FASTCALL KeyboardHasBeenPressedClear(Word ScanCode);
extern Word FASTCALL KeyboardStringToScanCode(const char *StringPtr);
extern void FASTCALL KeyboardScanCodeToString(char *StringPtr,Word StringSize,Word ScanCode);
extern Word FASTCALL KeyboardWait(void);

extern Word MousePresent;
extern Word MouseClicked;
extern Word FASTCALL MouseInit(void);
extern void FASTCALL MouseDestroy(void);
extern Word FASTCALL MouseReadButtons(void);
extern void FASTCALL MouseReadAbs(Word *x,Word *y);
extern void FASTCALL MouseReadDelta(int *x,int *y);
extern int FASTCALL MouseReadWheel(void);
extern void FASTCALL MouseSetRange(Word x,Word y);
extern void FASTCALL MouseSetPosition(Word x,Word y);

#define AXISCOUNT 6
#define MAXJOYNUM 4
enum {AXISMIN,AXISMAX,AXISCENTER,AXISLESS,AXISMORE,AXISENTRIES};
extern Word JoystickPercent[MAXJOYNUM][AXISCOUNT];		/* Cache for percentages */
extern Word JoystickPresent;
extern LongWord JoystickLastButtons[MAXJOYNUM];
extern LongWord JoystickLastButtonsDown[MAXJOYNUM];
extern Word JoystickBoundaries[MAXJOYNUM][AXISENTRIES*AXISCOUNT];
extern Word FASTCALL JoystickInit(void);
extern void FASTCALL JoystickDestroy(void);
extern LongWord FASTCALL JoystickReadButtons(Word Which);
extern void FASTCALL JoystickReadNow(Word Which);
extern Word FASTCALL JoystickReadAbs(Word Axis,Word Which);
extern int FASTCALL JoystickReadDelta(Word Axis,Word Which);
extern Word FASTCALL JoystickGetAxisCount(Word Which);
extern void FASTCALL JoystickSetCenter(Word Axis,Word Which);
extern void FASTCALL JoystickSetMin(Word Axis,Word Which);
extern void FASTCALL JoystickSetMax(Word Axis,Word Which);
extern void FASTCALL JoystickSetDigital(Word Axis,Word Percent,Word Which);
extern void FASTCALL JoystickBoundariesChanged(void);
extern Word FASTCALL JoyAutoRepeater(JoyAutoRepeat_t *Input,LongWord JoyBits);

extern ForceFeedback_t * FASTCALL ForceFeedbackNew(void);
extern void FASTCALL ForceFeedbackDelete(ForceFeedback_t *RefPtr);
extern void FASTCALL ForceFeedbackReacquire(ForceFeedback_t *RefPtr);
extern ForceFeedbackData_t * FASTCALL ForceFeedbackDataNew(ForceFeedback_t *RefPtr,const char *FilenamePtr);
extern ForceFeedbackData_t * FASTCALL ForceFeedbackDataNewRez(ForceFeedback_t *RefPtr,struct RezHeader_t *RezRef,Word RezNum);
extern void FASTCALL ForceFeedbackDataDelete(ForceFeedbackData_t *FilePtr);
extern ForceFeedbackEffect_t * FASTCALL ForceFeedbackEffectNew(ForceFeedbackData_t *FilePtr,const char *EffectNamePtr);
extern void FASTCALL ForceFeedbackEffectDelete(ForceFeedbackEffect_t *effect);
extern Word FASTCALL ForceFeedbackEffectPlay(ForceFeedbackEffect_t *Input);
extern void FASTCALL ForceFeedbackEffectStop(ForceFeedbackEffect_t *Input);
extern Word FASTCALL ForceFeedbackEffectIsPlaying(ForceFeedbackEffect_t *Input);
extern void FASTCALL ForceFeedbackEffectSetGain(ForceFeedbackEffect_t *Input,long NewGain);
extern void FASTCALL ForceFeedbackEffectSetDuration(ForceFeedbackEffect_t *Input,LongWord NewDuration);

#if defined(__MAC__)
extern Word KeyModifiers;			/* If a key is read, pass back the keyboard modifiers */
extern Word ScanCode;				/* Scan code of key last read */
extern Word FixMacKey(struct EventRecord *Event);
extern Boolean MacSystemTaskFlag;
extern Word (*MacEventIntercept)(struct EventRecord *MyEventPtr);
extern Word DoMacEvent(Word Mask,struct EventRecord *Event);
extern Word FASTCALL MacInputLockInputSprocket(void);
extern void FASTCALL MacInputUnlockInputSprocket(void);
#endif

#if defined(__BEOS__)
extern int FASTCALL BeOSSpawnMain(int (*MainCode)(int,char **),int argc, char **argv);
#endif

/* In Font */

typedef void (FASTCALL *FontDrawProc)(struct FontRef_t *,const char *,Word);
typedef Word (FASTCALL *FontWidthProc)(struct FontRef_t *,const char *,Word);

typedef struct FontRef_t {		/* This is the generic class for fonts */
	int FontX;			/* X coord to draw the font */
	int FontY;			/* Y coord to draw the font */
	Word FontHeight;	/* Height of the font in pixels */
	Word FontFirst;		/* First allowable font to draw */
	Word FontLast;		/* Last char I can draw */
	FontDrawProc Draw;	/* Draw text */
	FontWidthProc GetWidth;	/* Get the width of the text */
} FontRef_t;

typedef struct BurgerFontState_t {
	Byte FontOrMask[32];	/* Or mask (Could be shorts) */
	struct RezHeader_t *FontRezFile;	/* Resource file */
	int FontX;			/* X coord */
	int FontY;			/* Y coord */
	Word FontLoaded;	/* Resource */
	Word FontColorZero;	/* And mask */
} BurgerFontState_t;

typedef struct BurgerFont_t {	/* State of a font */
	FontRef_t Root;		/* Root of the font */
	Byte FontOrMask[32];	/* Color of font (Could be shorts) */
	struct RezHeader_t *FontRezFile;	/* Resource file */
	void **FontHandle;	/* Handle to the active font */
	Word FontOffset;	/* Offset to the pixel array */
	Word FontLoaded;	/* Resource ID of the last font loaded */
	Word FontColorZero;	/* Mask for font */
} BurgerFont_t;

typedef struct FontWidthEntry_t {	/* Describe each line */
	LongWord Offset;				/* Offset to the text string */
	Word Length;					/* Length of the string entry */
} FontWidthEntry_t;

typedef struct FontWidthLists_t {	/* Hash for formatted text */
	FontRef_t *FontPtr;				/* Font to use */
	Word Count;						/* Number of valid entries */
	FontWidthEntry_t Entries[1];	/* Array of entries */
} FontWidthLists_t;

extern void FASTCALL BurgerFontInit(BurgerFont_t *Input,struct RezHeader_t *RezFile,Word RezNum,const Byte *PalPtr);
extern BurgerFont_t * FASTCALL BurgerFontNew(struct RezHeader_t *RezFile,Word RezNum,const Byte *PalPtr);
#define BurgerFontDestroy(x) BurgerFontRelease(x)
extern void FASTCALL BurgerFontDelete(BurgerFont_t *Input);
extern void FASTCALL BurgerFontSaveState(BurgerFont_t *RefPtr,BurgerFontState_t *StatePtr);
extern void FASTCALL BurgerFontRestoreState(BurgerFont_t *RefPtr,const BurgerFontState_t *StatePtr);
extern void FASTCALL BurgerFontDrawChar(BurgerFont_t *RefPtr,Word Letter);
extern void FASTCALL BurgerFontDrawText(BurgerFont_t *RefPtr,const char *TextPtr,Word TextLen);
extern Word FASTCALL BurgerFontWidthText(BurgerFont_t *RefPtr,const char *TextPtr,Word TextLen);
extern void FASTCALL BurgerFontSetColor(BurgerFont_t *RefPtr,Word ColorNum,Word Color);
extern void FASTCALL BurgerFontUseZero(BurgerFont_t *RefPtr);
extern void FASTCALL BurgerFontUseMask(BurgerFont_t *RefPtr);
extern void FASTCALL BurgerFontInstallToPalette(BurgerFont_t *RefPtr,struct RezHeader_t *RezFile,Word FontNum,const Byte *PalPtr);
extern void FASTCALL BurgerFontRelease(BurgerFont_t *RefPtr);
extern void FASTCALL BurgerFontSetColorRGBListToPalette(BurgerFont_t *RefPtr,const void *RGBList,const Byte *PalPtr);
extern void FASTCALL BurgerFontSetToPalette(BurgerFont_t *RefPtr,const Byte *PalPtr);

extern void FASTCALL FontSetXY(FontRef_t *RefPtr,int x,int y);
extern Word FASTCALL FontWidthChar(FontRef_t *RefPtr,Word Letter);
extern Word FASTCALL FontWidthLong(FontRef_t *Input,long Val);
extern Word FASTCALL FontWidthLongWord(FontRef_t *RefPtr,LongWord Val);
extern Word FASTCALL FontWidthString(FontRef_t *RefPtr,const char *TextPtr);
extern Word FASTCALL FontWidthListWidest(FontRef_t *FontPtr,struct LinkedList_t *ListPtr);
extern void FASTCALL FontDrawChar(FontRef_t *RefPtr,Word Letter);
extern void FASTCALL FontDrawLong(FontRef_t *Input,long Val);
extern void FASTCALL FontDrawLongWord(FontRef_t *RefPtr,LongWord Val);
extern void FASTCALL FontDrawString(FontRef_t *RefPtr,const char *TextPtr);
extern void FASTCALL FontDrawStringCenterX(FontRef_t *RefPtr,int x,int y,const char *TextPtr);
extern void FASTCALL FontDrawStringAtXY(FontRef_t *RefPtr,int x,int y,const char *TextPtr);
extern Word FASTCALL FontCharsForPixelWidth(FontRef_t *RefPtr,const char *TextPtr,Word Width);

extern FontWidthLists_t * FASTCALL FontWidthListNew(FontRef_t *FontRef,const char *TextPtr,Word PixelWidth);
#define FontWidthListDelete(x) DeallocAPointer(x)
extern void FASTCALL FontWidthListDraw(FontWidthLists_t *Input,const struct LBRect *BoundsPtr,Word YTop,const char *TextPtr);

/* In Graphics */

struct LBRect;		/* Unix compilers want this, sigh */
#if defined(__MAC__)
#define BLACK 255
#define WHITE 0
#else
#define BLACK 0
#define WHITE 255
#endif

#define SETDISPLAYWINDOW 0x0				/* Best for debugging */
#define SETDISPLAYFULLSCREEN 0x1			/* True if full screen */
#define SETDISPLAYOFFSCREEN 0x2				/* True if I force system memory video page */
#define SETDISPLAYDOUBLEOK 0x4				/* True if I can allow pixel doubling for 320x200 */
#define SETDISPLAYOPENGL 0x8				/* True if 3D hardware is requested (Check Video3DPreference for API override) */
#define SETDISPLAYD3D 0x10					/* True if 3D hardware is requested (Check Video3DPreference for API override) */
#define SETDISPLAYNOWINDOWTWIDDLE 0x20		/* Don't change any of my window settings for me */

#define VIDEOMODEOPENGL 0x01		/* OpenGL is supported in this mode */
#define VIDEOMODEHARDWARE 0x02		/* DirectX or other native support */
#define VIDEOMODEFAKE 0x04			/* This is a pixel double mode */

#define VIDEOAPISOFTWARE 0
#define VIDEOAPIOPENGL 1
#define VIDEOAPIDIRECT3D 2
#define VIDEOAPIDIRECTDRAW 3

#define VIDEOMODEGETNOFAKE 0x0100	/* Don't allow pixel double modes */

/* Texture formats supported */
#define VIDEOTEXTURETYPE555 0x0001
#define VIDEOTEXTURETYPE565 0x0002
#define VIDEOTEXTURETYPE4444 0x0004
#define VIDEOTEXTURETYPE1555 0x0008
#define VIDEOTEXTURETYPE8PAL 0x0010
#define VIDEOTEXTURETYPE888 0x0020
#define VIDEOTEXTURETYPE8888 0x0040

#define VIDEOTEXTURESQUARE 0x0001		/* 3D Textures MUST be square */
#define VIDEOTEXTUREPOW2 0x0002			/* 3D Textures MUST be a power of 2 in size */
#define VIDEOTEXTURESYSTEMMEMORY 0x0004	/* Textures can exist in system memory (AGP) */

typedef struct LWShape_t {
	Short Width;		/* Width of the shape */
	Short Height;		/* Height of the shape */
	Byte Data[1];		/* Raw shape data */
} LWShape_t;

typedef struct LWXShape_t {
	short XOffset;		/* Signed offset for x */
	short YOffset;		/* Signed offset for y */
	LWShape_t Shape;	/* Shape data */
} LWXShape_t;

typedef struct GfxShape_t {
	Byte Palette[768];	/* Palette for the shape */
	LWXShape_t XShape;	/* XShape for the actual data */
} GfxShape_t;

typedef struct VideoMode_t {
	Word Width;		/* Width of video mode */
	Word Height;	/* Height of video mode */
	Word Depth;		/* Depth of video mode */
	Word Hertz;		/* Video scan rate (0 if not supported) */
	Word Flags;		/* Hardware/OpenGL */
} VideoMode_t;

typedef struct VideoModeArray_t {
	Word Count;				/* Number of modes in the array */
	Word DevNumber;			/* Device number */
	char DeviceName[64];	/* Name of the device */
	VideoMode_t Array[1];	/* Array of modes */
} VideoModeArray_t;

typedef struct VideoDeviceArray_t {
	Word Count;						/* Number of video cards present */
	VideoModeArray_t **Array[1];	/* Modes present (Handles) */
} VideoDeviceArray_t;

typedef struct VideoSaveState_t {
	Word Width;		/* Width of the current screen */
	Word Height;	/* Height of the current screen */
	Word Depth;		/* BIt depth of the current screen */
	Word Flags;		/* Special flags for the current mode */
} VideoSaveState_t;

typedef struct GammaTable_t {
	float Red[256];		/* Red gamma 0.0 = min, 1.0 = max */
	float Green[256];	/* Green gamma */
	float Blue[256];	/* Blue gamma */
} GammaTable_t;

typedef void (FASTCALL *DrawARectProc)(int x,int y,Word Width,Word Height,Word Color);
typedef void (FASTCALL *DrawALineProc)(int x1,int y1,int x2,int y2,Word Color);
typedef void (FASTCALL *DrawAPixelProc)(int x,int y,Word Color);
typedef void (FASTCALL *DrawARectRemapProc)(int x,int y,Word Width,Word Height,const void *RemapPtr);
typedef Word (FASTCALL *VideoModePurgeProc)(VideoMode_t *);

extern Word Table8To16[256];		/* 8 bit to 16 bit conversion table */
extern Word *Table8To16Ptr;			/* Pointer to the draw table to use */
extern Byte *VideoPointer;			/* Pointer to current video buffer */
extern Word VideoWidth;				/* Bytes per scan line */
extern Word ScreenClipLeft;			/* Clip left */
extern Word ScreenClipTop;			/* Clip top */
extern Word ScreenClipRight;		/* Clip right */
extern Word ScreenClipBottom;		/* Clip bottom */
extern Word ScreenWidth;			/* Clip width in pixels */
extern Word ScreenHeight;			/* Clip height in pixels */
extern Word ScreenFlags;			/* Current flags used for this mode */
extern Word VideoRedShift;			/* Bits to shift for Red */
extern Word VideoGreenShift;		/* Bits to shift for Green */
extern Word VideoBlueShift;			/* Bits to shift for Blue */
extern Word VideoRedMask;			/* Bitmask for Red */
extern Word VideoGreenMask;			/* Bitmask for Green */
extern Word VideoBlueMask;			/* Bitmask for Blue */
extern Word VideoRedBits;			/* Number of bits of Red */
extern Word VideoGreenBits;			/* Number of bits of Green */
extern Word VideoBlueBits;			/* Number of bits of Blue */
extern Word VideoColorDepth;		/* TRUE if palette doesn't exist */
extern Word VideoTrueScreenWidth;	/* Width in PIXELS of the video display */
extern Word VideoTrueScreenHeight;	/* Width in PIXELS of the video display */
extern Word VideoTrueScreenDepth;	/* Depth in BITS of the video display */
extern Word VideoPixelDoubleMode;	/* Set to the mode requested 0-3 */
extern Word VideoFullScreen; 		/* TRUE if full screen mode is active */
extern Word VideoPageLocked;		/* True if the video memory is locked */
extern Byte *VideoOffscreen;		/* Pointer to offscreen buffer if used */
extern Word VideoHardWidth;			/* Width in BYTES of a video scan line */
extern Word VideoAPIInUse;			/* True if OpenGL is present and active */
extern Word VideoTextureTypesAllowed;	/* Texture formats I can support */
extern Word VideoTextureRules;			/* Special rules I have to follow */
extern Word VideoTextureMinWidth;		/* Minimum texture size */
extern Word VideoTextureMaxWidth;		/* Maximum texture size */
extern Word VideoTextureMinHeight;		/* Minimum texture height */
extern Word VideoTextureMaxHeight;		/* Maximum texture height */
extern Word VideoVertexMaxCount;		/* Maximum number of vertexs */
extern Word VideoVertexCount;			/* Number of vertex's processed */
extern Word VideoUseColorZero;			/* TRUE if Color #0 is used in textures */
extern DrawARectProc ScreenRect;	/* Draw a rect intercept vector */
extern DrawALineProc ScreenLine;	/* Draw a line intercept vector */
extern DrawAPixelProc ScreenPixel;	/* Draw a pixel intercept vector */
extern DrawARectRemapProc ScreenRectRemap;	/* Remap a rect of pixels */

extern Word BurgerMaxVideoPage;		/* Maximum number of video pages */
extern Word BurgerVideoPage;		/* Currently using this page */
extern Word BurgerVesaVersion;		/* 0 = No VESA, 1 = 1.2, 2 = 2.0 */
extern Boolean Burger8BitPalette;	/* TRUE if 8 bit palettes are supported */
extern LongWord BurgerScreenSize;	/* Size in BYTES of the offscreen bitmap */
extern Byte *BurgerBaseVideoPointer;	/* Pointer to base video memory range */
extern Byte *BurgerVideoPointers[3];	/* Pointers to each video page */
extern Byte *BurgerVideoCallbackBuffer;	/* Pointer to VESA callback code */
extern Boolean BurgerLinearFrameBuffer;	/* TRUE if hardware is linear */

#define SetNewVideoWidth(x) VideoWidth=x
extern void FASTCALL ReleaseVideo(void);
extern Word FASTCALL SetDisplayToSize(Word Width,Word Height,Word Depth,Word Flags);
extern void FASTCALL UpdateAndPageFlip(void);
extern void FASTCALL UpdateAndNoPageFlip(void);
extern void FASTCALL LockVideoPage(void);
extern void FASTCALL LockFrontVideoPage(void);
extern void FASTCALL VideoSetWindowString(const char *Title);
extern VideoModeArray_t **FASTCALL VideoModeArrayNew(Word Flags);
#define VideoModeArrayDelete(x) DeallocAHandle((void **)x)
extern VideoDeviceArray_t **FASTCALL VideoDeviceArrayNew(Word Flags);
extern void FASTCALL VideoDeviceArrayDelete(VideoDeviceArray_t **Input);
extern VideoModeArray_t ** FASTCALL VideoModeArrayPurge(VideoModeArray_t **Input,VideoModePurgeProc Proc);
extern VideoDeviceArray_t ** FASTCALL VideoDeviceArrayPurge(VideoDeviceArray_t **Input,VideoModePurgeProc Proc);
extern void FASTCALL VideoGetCurrentMode(VideoSaveState_t *Input);
extern Word FASTCALL VideoSetMode(const VideoSaveState_t *Input);
extern void FASTCALL UnlockVideoPage(void);
extern void FASTCALL Video555To565(void);

extern void FASTCALL VideoOSGammaInit(void);
extern void FASTCALL VideoOSGammaDestroy(void);
extern void FASTCALL VideoOSGammaAdjust(Fixed Intensity);
extern void FASTCALL VideoOSGammaSet(const GammaTable_t *TablePtr);
extern void FASTCALL VideoOSGammaGet(GammaTable_t *TablePtr);

#define GetShapeWidth(ShapePtr) (((LWShape_t *)ShapePtr)->Width)
#define GetShapeHeight(ShapePtr) (((LWShape_t *)ShapePtr)->Height)
#define GetXShapeXOffset(ShapePtr) (((LWXShape_t *)ShapePtr)->XOffset)
#define GetXShapeYOffset(ShapePtr) (((LWXShape_t *)ShapePtr)->YOffset)
#define GetXShapeWidth(ShapePtr) (((LWXShape_t *)ShapePtr)->Shape.Width)
#define GetXShapeHeight(ShapePtr) (((LWXShape_t *)ShapePtr)->Shape.Height)
#define GetShapeIndexPtr(ShapeArrayPtr,Index) (&((Byte *)ShapeArrayPtr)[((LongWord *)ShapeArrayPtr)[Index]])

extern void FASTCALL SetTheClipBounds(Word Left,Word Top,Word Right,Word Bottom);
extern void FASTCALL SetTheClipRect(const struct LBRect *RectPtr);
extern void FASTCALL GetTheClipRect(struct LBRect *RectPtr);
extern Word FASTCALL GetAPixel(Word x,Word y);
extern Word FASTCALL GetAPixel16(Word x,Word y);
extern void FASTCALL SetAPixel(Word x,Word y,Word Color);
extern void FASTCALL SetAPixel16(Word x,Word y,Word Color);
extern void FASTCALL SetAPixelTo16(Word x,Word y,Word Color);
extern void FASTCALL DrawARect(int x,int y,Word Width,Word Height,Word Color);
extern void FASTCALL DrawARect16(int x,int y,Word Width,Word Height,Word Color);
extern void FASTCALL DrawARectTo16(int x,int y,Word Width,Word Height,Word Color);
extern void FASTCALL DrawALine(int x1,int y1,int x2,int y2,Word Color);
extern void FASTCALL DrawALine16(int x1,int y1,int x2,int y2,Word Color);
extern void FASTCALL DrawALineTo16(int x1,int y1,int x2,int y2,Word Color);
extern void FASTCALL DrawARectRemap(int x,int y,Word Width,Word Height,const Byte *RemapPtr);
extern void FASTCALL DrawARectRemap16(int x,int y,Word Width,Word Height,const Short *RemapPtr);

extern void FASTCALL ScreenClear(Word Color);
extern void FASTCALL ScreenBox(int x,int y,Word Width,Word Height,Word Color);
extern void FASTCALL ScreenBox2(int x,int y,Word Width,Word Height,Word Color1,Word Color2);
extern void FASTCALL ScreenThickBox(int x,int y,Word Width,Word Height,Word Color);
extern void FASTCALL ScreenThickBox2(int x,int y,Word Width,Word Height,Word Color1,Word Color2);
extern void FASTCALL ScreenBoxRemap(int x,int y,Word Width,Word Height,const void *RemapPtr);
extern void FASTCALL ScreenBoxDropShadow(int x,int y,Word Width,Word Height,Word Color1,Word Color2);
extern void FASTCALL ScreenRectDropShadow(int x,int y,Word Width,Word Height,Word Color1,Word Color2,Word Color3);

extern void FASTCALL DrawShapeLowLevel(Word x,Word y,Word Width,Word Height,Word Skip,void *ShapePtr);
extern void FASTCALL DrawShapeLowLevelClipped(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawShape(x,y,p) DrawShapeLowLevel(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawShapeClipped(x,y,p) DrawShapeLowLevelClipped(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawShape(x,y,w,h,p) DrawShapeLowLevel(x,y,w,h,0,p)
#define DrawRawShapeClipped(x,y,w,h,p) DrawShapeLowLevelClipped(x,y,w,h,0,p)
#define DrawXShape(x,y,p) DrawShapeLowLevel(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXShapeClipped(x,y,p) DrawShapeLowLevelClipped(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
extern void FASTCALL DrawRezShape(Word x,Word y,struct RezHeader_t *Input,Word RezNum);
extern void FASTCALL DrawRezCenterShape(struct RezHeader_t *Input,Word RezNum);

extern void FASTCALL DrawMShapeLowLevel(Word x,Word y,Word Width,Word Height,Word Skip,void *ShapePtr);
extern void FASTCALL DrawMShapeLowLevelClipped(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawMShape(x,y,p) DrawMShapeLowLevel(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawMShapeClipped(x,y,p) DrawMShapeLowLevelClipped(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawMShape(x,y,w,h,p) DrawMShapeLowLevel(x,y,w,h,0,p)
#define DrawRawMShapeClipped(x,y,w,h,p) DrawMShapeLowLevelClipped(x,y,w,h,0,p)
#define DrawXMShape(x,y,p) DrawMShapeLowLevel(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXMShapeClipped(x,y,p) DrawMShapeLowLevelClipped(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
extern void FASTCALL DrawRezMShape(Word x,Word y,struct RezHeader_t *Input,Word RezNum);
extern void FASTCALL DrawRezCenterMShape(struct RezHeader_t *Input,Word RezNum);

#define DrawShapeLowLevel16(x,y,w,h,s,p) DrawShapeLowLevel(x*2,y,w*2,h,s,p)
extern void FASTCALL DrawShapeLowLevelClipped16(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawShape16(x,y,p) DrawShapeLowLevel16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawShapeClipped16(x,y,p) DrawShapeLowLevelClipped16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawShape16(x,y,w,h,p) DrawShapeLowLevel16(x,y,w,h,0,p)
#define DrawRawShapeClipped16(x,y,w,h,p) DrawShapeLowLevelClipped16(x,y,w,h,0,p)
#define DrawXShape16(x,y,p) DrawShapeLowLevel16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXShapeClipped16(x,y,p) DrawShapeLowLevelClipped16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
extern void FASTCALL DrawRezShape16(Word x,Word y,struct RezHeader_t *Input,Word RezNum);
extern void FASTCALL DrawRezCenterShape16(struct RezHeader_t *Input,Word RezNum);

extern void FASTCALL DrawMShapeLowLevel16(Word x,Word y,Word Width,Word Height,Word Skip,void *ShapePtr);
extern void FASTCALL DrawMShapeLowLevelClipped16(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawMShape16(x,y,p) DrawMShapeLowLevel16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawMShapeClipped16(x,y,p) DrawMShapeLowLevelClipped16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawMShape16(x,y,w,h,p) DrawMShapeLowLevel16(x,y,w,h,0,p)
#define DrawRawMShapeClipped16(x,y,w,h,p) DrawMShapeLowLevelClipped16(x,y,w,h,0,p)
#define DrawXMShape16(x,y,p) DrawMShapeLowLevel(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXMShapeClipped16(x,y,p) DrawMShapeLowLevelClipped16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
extern void FASTCALL DrawRezMShape16(Word x,Word y,struct RezHeader_t *Input,Word RezNum);
extern void FASTCALL DrawRezCenterMShape16(struct RezHeader_t *Input,Word RezNum);

extern void FASTCALL DrawShapeLowLevelTo16(Word x,Word y,Word Width,Word Height,Word Skip,void *ShapePtr);
extern void FASTCALL DrawShapeLowLevelClippedTo16(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawShapeTo16(x,y,p) DrawShapeLowLevelTo16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawShapeClippedTo16(x,y,p) DrawShapeLowLevelClippedTo16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawShapeTo16(x,y,w,h,p) DrawShapeLowLevelTo16(x,y,w,h,0,p)
#define DrawRawShapeClippedTo16(x,y,w,h,p) DrawShapeLowLevelClippedTo16(x,y,w,h,0,p)
#define DrawXShapeTo16(x,y,p) DrawShapeLowLevelTo16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXShapeClippedTo16(x,y,p) DrawShapeLowLevelClippedTo16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)

extern void FASTCALL DrawMShapeLowLevelTo16(Word x,Word y,Word Width,Word Height,Word Skip,void *ShapePtr);
extern void FASTCALL DrawMShapeLowLevelClippedTo16(int x,int y,Word Width,Word Height,Word Skip,void *ShapePtr);
#define DrawMShapeTo16(x,y,p) DrawMShapeLowLevelTo16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawMShapeClippedTo16(x,y,p) DrawMShapeLowLevelClippedTo16(x,y,((LWShape_t *)p)->Width,((LWShape_t *)p)->Height,0,&((LWShape_t *)p)->Data)
#define DrawRawMShapeTo16(x,y,w,h,p) DrawMShapeLowLevelTo16(x,y,w,h,0,p)
#define DrawRawMShapeClippedTo16(x,y,w,h,p) DrawMShapeLowLevelClippedTo16(x,y,w,h,0,p)
#define DrawXMShapeTo16(x,y,p) DrawMShapeLowLevelTo16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)
#define DrawXMShapeClippedTo16(x,y,p) DrawMShapeLowLevelClippedTo16(((LWXShape_t*)p)->XOffset+x,((LWXShape_t*)p)->YOffset+y, \
	((LWXShape_t *)p)->Shape.Width,((LWXShape_t *)p)->Shape.Height,0,&((LWXShape_t*)p)->Shape.Data)

extern void FASTCALL EraseShape(Word x,Word y,void *ShapePtr);
extern void FASTCALL EraseMBShape(Word x,Word y,void *ShapePtr,void *BackPtr);
extern Word FASTCALL TestMShape(Word x,Word y,void *ShapePtr);
extern Word FASTCALL TestMBShape(Word x,Word y,void *ShapePtr,void *BackPtr);
extern void FASTCALL VideoPixelDouble16(const Byte *SourcePtr,Byte *DestPtr,Word SourceRowBytes,Word DestRowBytes,Word Width,Word Height);
extern void FASTCALL VideoPixelDoubleChecker16(const Byte *SourcePtr,Byte *DestPtr,Word SourceRowBytes,Word DestRowBytes,Word Width,Word Height);
extern void FASTCALL VideoPixelDouble(const Byte *SourcePtr,Byte *DestPtr,Word SourceRowBytes,Word DestRowBytes,Word Width,Word Height);
extern void FASTCALL VideoPixelDoubleChecker(const Byte *SourcePtr,Byte *DestPtr,Word SourceRowBytes,Word DestRowBytes,Word Width,Word Height);

extern Word FASTCALL OpenGLSetDisplayToSize(Word Width,Word Height,Word Depth,Word Flags);
extern void FASTCALL OpenGLMakeCurrent(void);
extern void FASTCALL OpenGLRelease(void);
extern void FASTCALL OpenGLSwapBuffers(void);
extern void FASTCALL OpenGLInit(void);
extern void FASTCALL OpenGLSolidRect(int x,int y,Word Width,Word Height,LongWord Color);
extern void FASTCALL OpenGLLine(int x,int y,int x2,int y2,LongWord Color);
extern void FASTCALL OpenGLTextureDraw2D(Word TexNum,int x,int y,Word Width,Word Height);
extern void FASTCALL OpenGLTextureDraw2DSubRect(Word TexNum,int x,int y,Word Width,Word Height,const float *UVPtr);
extern Word FASTCALL OpenGLLoadTexture(Word *TextureNum,const struct Image_t *ImagePtr);
extern void FASTCALL OpenGLSetCurrent(void);
extern void FASTCALL OpenGLATISetTruform(Word Setting);
extern void FASTCALL OpenGLATISetFSAA(Word Setting);

#if defined(__MAC__)
extern struct __AGLContextRec * FASTCALL OpenGLSetContext(struct __AGLContextRec *Context);
extern struct __AGLContextRec * FASTCALL OpenGLGetContext(void);
#endif

#if defined(__WIN95__)

extern Word FASTCALL WinSetDisplayToSize(Word Width,Word Height,Word Depth);
extern void FASTCALL DrawARectDirectX(int x,int y,Word Width,Word Height,Word Color);
#define DrawARectDirectX16 DrawARectDirectX
extern void FASTCALL DrawARectDirectXTo16(int x,int y,Word Width,Word Height,Word Color);

/* These macros help store data into the output stream for D3D operations */
/* I assume little endian in the operations since this belongs to a PC exclusively */

#define STORE_OP_CONST(a,b,c,p) ((LongWord *)p)[0] = (((LongWord)a) + (((LongWord)b)<<8) + (((LongWord)c)<<16)); p =p+4;
#define STORE_OP(a,b,c,p) ((D3DINSTRUCTION*)p)->bOpcode = (Byte)a; ((D3DINSTRUCTION*)p)->bSize = (Byte)b; ((D3DINSTRUCTION*)p)->wCount = (Short)c; p = p+4;

#define STORE_OP_RENDERSTATE_CONST(a,p) ((LongWord *)p)[0] = ((LongWord)D3DOP_STATERENDER + (((LongWord)sizeof(D3DSTATE))<<8) + (((LongWord)a)<<16)); p = p+4;
#define STORE_OP_RENDERSTATE(a,p) ((Short *)p)[0] = (((Short)D3DOP_STATERENDER) + ((Short)sizeof(D3DSTATE)<<8)); ((Short *)p)[1] = (Short)a; p = p+4;
#define STORE_OP_PROCESSVERTICES_CONST(a,p) ((LongWord *)p)[0] = ((LongWord)D3DOP_PROCESSVERTICES + (((LongWord)sizeof(D3DPROCESSVERTICES))<<8) + (((LongWord)a)<<16)); p = p+4;
#define STORE_OP_PROCESSVERTICES(a,p) ((Short *)p)[0] = (((Short)D3DOP_PROCESSVERTICES) + ((Short)sizeof(D3DPROCESSVERTICES)<<8)); ((Short *)p)[1] = (Short)a; p = p+4;
#define STORE_OP_TRIANGLE_CONST(a,p) ((LongWord *)p)[0] = ((LongWord)D3DOP_TRIANGLE + (((LongWord)sizeof(D3DTRIANGLE))<<8) + (((LongWord)a)<<16)); p = p+4;
#define STORE_OP_TRIANGLE(a,p) ((Short *)p)[0] = (((Short)D3DOP_TRIANGLE) + ((Short)sizeof(D3DTRIANGLE)<<8)); ((Short *)p)[1] = (Short)a; p = p+4;
#define STORE_OP_EXIT(p) ((LongWord *)p)[0] = (LongWord)D3DOP_EXIT; p = (void *)((Byte *)p+4);

#define STORE_DATA_STATE(a,b,p) ((D3DSTATE *)p)->drstRenderStateType = (D3DRENDERSTATETYPE)a; ((D3DSTATE *)p)->dwArg[0] = b; p = p+8;
#define STORE_DATA_PROCESSVERTICES(flgs,strt,cnt,p) ((D3DPROCESSVERTICES *)p)->dwFlags = flgs; ((D3DPROCESSVERTICES *)p)->wStart = (Short)strt; ((D3DPROCESSVERTICES *)p)->wDest = (Short)strt; ((D3DPROCESSVERTICES *)p)->dwCount = cnt; ((D3DPROCESSVERTICES *)p)->dwReserved = 0; p = p+16;
#define STORE_DATA_TRIANGLE(a,b,c,p) ((D3DTRIANGLE *)p)->v1 = (Short)(a); ((D3DTRIANGLE *)p)->v2 = (Short)(b); ((D3DTRIANGLE *)p)->v3 = (Short)(c); ((D3DTRIANGLE *)p)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE; p = p + 8;

#define MAXD3DINSTRUCTIONS 1024		/* Maximum number of D3D instructions to queue */

extern struct IDirect3D *Direct3DPtr;				/* Reference to the direct 3D object */
extern struct IDirect3DDevice *Direct3DDevicePtr;	/* Reference to the direct 3D rendering device */
extern struct IDirect3DViewport *Direct3DViewPortPtr;	/* Reference to the direct 3d Viewport */
extern struct IDirect3DExecuteBuffer *Direct3DExecBufPtr;	/* Reference to the direct 3d execute buffer */
extern Byte *Direct3DExecDataBuf;			/* Pointer to the execute data buffer */
extern Byte *Direct3DExecInstBuf;			/* Pointer to the execute instruction buffer */
extern Byte *Direct3DExecDataPtr;			/* Current free data pointer */
extern Byte *Direct3DExecInstPtr;			/* Current free instruction pointer */
extern Byte *Direct3DExecInstStartPtr;		/* Pointer to the beginning of the last instruction chunk */
extern LongWord Direct3DExecBuffSize;		/* Size of the execute buffer */

extern void FASTCALL D3DGetTextureInfo(void);
extern void FASTCALL D3DSetStandardViewport(void);
extern void FASTCALL D3DCreateZBuffer(void);
extern void FASTCALL D3DInitExecuteBuffer(void);
extern void FASTCALL D3DInit(const struct _GUID *Input);
extern void FASTCALL Direct3DDestroy(void);
extern void FASTCALL Direct3DLockExecuteBuffer(void);
extern void FASTCALL Direct3DUnlockExecuteBuffer(void);
extern void FASTCALL Direct3DCheckExecBuffer(Word InstCount,Word VertexCount);
extern void FASTCALL Direct3DBeginScene(void);
extern void FASTCALL Direct3DEndScene(void);
extern void FASTCALL Direct3DSolidRect(int x,int y,Word Width,Word Height,LongWord Color);
extern void FASTCALL Direct3DLine(int x,int y,int x2,int y2,LongWord Color);

#endif

/* Screen Shape Manager */

enum {
	SCREENSHAPEMODESOFTWARE,
	SCREENSHAPEMODEOPENGL,
	SCREENSHAPEMODEDIRECT3D,
	SCREENSHAPEMODEDIRECTDRAW
};

#define SCREENSHAPEFLAGMODEMASK 0x0F
#define SCREENSHAPEFLAGMASK 0x010
#define SCREENSHAPEFLAGCOMPRESSED 0x020
#define SCREENSHAPEFLAGPURGEABLE 0x040
#define SCREENSHAPEFLAGMASKDISABLE 0x8000

typedef enum {
	TRANSLUCENCYMODE_OFF,
	TRANSLUCENCYMODE_NORMAL,
	TRANSLUCENCYMODE_INVCOLOR,
	TRANSLUCENCYMODE_COLOR,
	TRANSLUCENCYMODE_GLOWING,
	TRANSLUCENCYMODE_DARKENINGCOLOR,
	TRANSLUCENCYMODE_JUSTSETZ,
	TRANSLUCENCYMODE_BAD=0x70000000
} TranslucencyMode_e;

typedef enum {
	FILTERINGMODE_OFF,
	FILTERINGMODE_BILINEAR,
	FILTERINGMODE_BAD=0x70000000
} FilteringMode_e;

typedef enum {
	SHADINGMODE_FLAT,
	SHADINGMODE_GOURAUD,
	SHADINGMODE_PHONG,
	SHADINGMODE_BAD=0x70000000
} ShadingMode_e;

typedef enum {
	DEPTHTESTMODE_NEVER,
	DEPTHTESTMODE_LESS,
	DEPTHTESTMODE_EQUAL,
	DEPTHTESTMODE_LESSEQUAL,
	DEPTHTESTMODE_GREATER,
	DEPTHTESTMODE_NOTEQUAL,
	DEPTHTESTMODE_GREATEREQUAL,
	DEPTHTESTMODE_ALWAYS,
	DEPTHTESTMODE_BAD=0x70000000
} DepthTestMode_e;

struct ScreenShape_t;
typedef Word (FASTCALL *ScreenShapeActionProc)(struct ScreenShape_t *);
typedef Word (FASTCALL *ScreenShapeDrawProc)(struct ScreenShape_t* screen_shape, const struct LBRect* dest_rect);

typedef struct ScreenShape_t {	/* Root data class for shapes */
	void *Data1;			/* Reference to data (Usually a handle) */
	void *Data2;			/* Reference to data (Usually a texture ref) */
	void *Data3;			/* Extra reference */
	ScreenShapeActionProc ActionProc;	/* How to load this data? */
	ScreenShapeDrawProc DrawProc;	/* This can be NULL, in which case, blib automatically handles it based on screen mode */
	Word Flags;				/* Is color zero present? */
	Word Width,Height;		/* Size of the shape in pixels */
	int XOffset,YOffset;	/* X/Y adjust for drawing shape */
} ScreenShape_t;

typedef struct ScreenShapeRez_t {	/* Class for loading a shape from a rez file */
	ScreenShape_t MyShape;			/* Root class */
	struct RezHeader_t *RezFile;	/* Master resource reference */
	Word RezNum;					/* Resource number for the shape group to get info from */
	Word RezPal;					/* Palette for resource */
} ScreenShapeRez_t;

typedef struct ScreenShapeGfx_t {	/* Class for loading a Gfx shape from a rez file */
	ScreenShape_t MyShape;			/* Root class */
	struct RezHeader_t *RezFile;	/* Master resource reference */
	Word RezNum;					/* Resource number for the shape */
} ScreenShapeGfx_t;

typedef struct ScreenShapeGfxFile_t {	/* Class for loading a Gfx shape from a file */
	ScreenShape_t MyShape;			/* Root class */
	char **FileName;				/* Handle to filename */
} ScreenShapeGfxFile_t;

typedef struct ScreenShapePtr_t {	/* Class for loading a shape from a pointer */
	ScreenShape_t MyShape;			/* Root class */
	struct Image_t *ImagePtr;		/* Pointer to the shape */
} ScreenShapePtr_t;

typedef struct ScreenShapeRezGroup_t {	/* Class for loading from a group */
	ScreenShape_t MyShape;			/* Root class */
	struct RezHeader_t *RezFile;	/* Master resource reference */
	Word RezNum;					/* Resource number for the shape group to get info from */
	Word RezPal;					/* Palette for resource */
	Word Which;						/* Which shape in the group */
} ScreenShapeRezGroup_t;

typedef struct ScreenShapeGifFile_t {	/* Class for loading from a gif file */
	ScreenShape_t MyShape;			/* Root class */
	char **FileName;				/* Handle to filename */
} ScreenShapeGifFile_t;

typedef struct ScreenShapeGif_t {	/* Class for loading from a gif file */
	ScreenShape_t MyShape;			/* Root class */
	struct RezHeader_t *RezFile;	/* Master resource reference */
	Word RezNum;					/* Resource number for the shape */
} ScreenShapeGif_t;

typedef struct ScreenShapeBmpFile_t {	/* Class for loading from a bmp file */
	ScreenShape_t MyShape;			/* Root class */
	char **FileName;				/* Handle to filename */
} ScreenShapeBmpFile_t;

typedef struct ScreenShapeBmp_t {	/* Class for loading from a group */
	ScreenShape_t MyShape;			/* Root class */
	struct RezHeader_t *RezFile;	/* Master resource reference */
	Word RezNum;					/* Resource number for the shape */
} ScreenShapeBmp_t;

typedef void (FASTCALL *ScreenShapeInitProcPtr)(void);
typedef void (FASTCALL *ScreenShapeSolidRectProcPtr)(int,int,Word,Word,LongWord);

extern Word ScreenAPI;			/* Which API am I using (Direct3D, OpenGL?) */
extern TranslucencyMode_e ScreenTranslucencyMode;		/* Current 3D translucency mode */
extern FilteringMode_e ScreenFilteringMode;				/* Current texture filtering mode */
extern ShadingMode_e ScreenShadingMode;					/* Current shading mode */
extern DepthTestMode_e ScreenDepthTestMode;				/* Type of ZBuffer test */
extern Word ScreenDepthWriteMode;						/* Write to the ZBuffer? */
extern Word ScreenPerspectiveMode;						/* Perspective correct mode active? */
extern Word ScreenWireFrameMode;						/* Are polygons wireframed? */
extern Word ScreenBlendMode;							/* Last alpha mode */
extern Word ScreenUsing2DCoords;						/* Whether 2D drawing is currently on */
extern LongWord ScreenCurrentTexture;					/* Last texture found */
extern ScreenShapeInitProcPtr ScreenInit;									/* Init the 3D context */
extern ScreenShapeSolidRectProcPtr ScreenSolidRect;		/* Draw a solid rect */

extern void FASTCALL ScreenInitAPI(Word APIType);
extern Word FASTCALL ScreenSetDisplayToSize(Word Width,Word Height,Word Depth,Word Flags);
extern void FASTCALL ScreenBeginScene(void);
extern void FASTCALL ScreenEndScene(void);
#define ScreenSetTranslucencyMode(x) if (ScreenTranslucencyMode!=(x)) { ScreenForceTranslucencyMode(x); }
extern void FASTCALL ScreenForceTranslucencyMode(TranslucencyMode_e NewMode);
#define ScreenSetFilteringMode(x) if (ScreenFilteringMode!=(x)) { ScreenForceFilteringMode(x); }
extern void FASTCALL ScreenForceFilteringMode(FilteringMode_e NewMode);
#define ScreenSetWireFrameMode(x) if (ScreenWireFrameMode!=(x)) { ScreenForceWireFrameMode(x); }
extern void FASTCALL ScreenForceWireFrameMode(Word Flag);
#define ScreenSetTexture(x) if (ScreenCurrentTexture!=(x)) { ScreenForceTexture(x); }
extern void FASTCALL ScreenForceTexture(LongWord TexNum);
#define ScreenSetPerspective(x) if (ScreenPerspectiveMode!=(x)) { ScreenForcePerspective(x); }
extern void FASTCALL ScreenForcePerspective(Word Flag);
#define ScreenSetShadingMode(x) if (ScreenShadingMode!=(x)) { ScreenForceShadingMode(x); }
extern void FASTCALL ScreenForceShadingMode(ShadingMode_e NewMode);
#define ScreenSetDepthWriteMode(x) if (ScreenDepthWriteMode!=(x)) { ScreenForceDepthWriteMode(x); }
extern void FASTCALL ScreenForceDepthWriteMode(Word Flag);
#define ScreenSetDepthTestMode(x) if (ScreenDepthTestMode!=(x)) { ScreenForceDepthTestMode(x); }
extern void FASTCALL ScreenForceDepthTestMode(DepthTestMode_e NewMode);
#define ScreenUse2DCoords(x) if (ScreenUsing2DCoords!=(x)) { ScreenForceUse2DCoords(x); }
extern void FASTCALL ScreenForceUse2DCoords( Word use2d );

extern void FASTCALL ScreenShapeInit(ScreenShape_t *Input,ScreenShapeActionProc ActionProc);
extern ScreenShape_t *FASTCALL ScreenShapeNew(ScreenShapeActionProc ActionProc);
extern void FASTCALL ScreenShapeDestroy(ScreenShape_t *Input);
extern void FASTCALL ScreenShapeDelete(ScreenShape_t *Input);
extern void FASTCALL ScreenShapePurge(ScreenShape_t *Input);
extern void FASTCALL ScreenShapeDraw(ScreenShape_t *Input,int x,int y);
extern void FASTCALL ScreenShapeDrawScaled(ScreenShape_t *Input,const struct LBRect *DestRect);
extern void FASTCALL ScreenShapeDrawScaledSubRect(ScreenShape_t *Input,const struct LBRect *DestRect,const struct LBRect *SrcRect);
extern void FASTCALL ScreenShapeLock(ScreenShape_t *Input,struct Image_t *Output);
extern void FASTCALL ScreenShapeUnlock(ScreenShape_t *Input);
extern Word FASTCALL ScreenShapeLoad(ScreenShape_t *Input);
extern void FASTCALL ScreenShapeDisallowPurge(ScreenShape_t *Input);
extern void FASTCALL ScreenShapeAllowPurge(ScreenShape_t *Input);
extern void FASTCALL ScreenShapeGetBounds(ScreenShape_t *Input,struct LBRect *Bounds);
extern void FASTCALL ScreenShapeGetSize(ScreenShape_t *Input,struct LBPoint *Size);
extern Word FASTCALL ScreenShapeGetPixel(ScreenShape_t *Input,int x,int y);
extern int FASTCALL ScreenShapeVPatternBar(ScreenShape_t **ArrayPtr,int x,int TopY,int BottomY);
extern int FASTCALL ScreenShapeHPatternBar(ScreenShape_t **ArrayPtr,int y,int LeftX,int RightX);
extern Word FASTCALL ScreenShapeConvertFromImage(ScreenShape_t *Input,const struct Image_t *ImagePtr);

extern void FASTCALL ScreenShapeGfxInit(ScreenShapeGfx_t *Input,struct RezHeader_t *RezFile,Word RezNum);
extern ScreenShapeGfx_t *FASTCALL ScreenShapeGfxNew(struct RezHeader_t *RezFile,Word RezNum);
extern void FASTCALL ScreenShapeGfxReinit(ScreenShapeGfx_t *Input,struct RezHeader_t *RezFile,Word RezNum);

extern void FASTCALL ScreenShapeGfxFileInit(ScreenShapeGfxFile_t *Input,const char *FileName);
extern ScreenShapeGfxFile_t *FASTCALL ScreenShapeGfxFileNew(const char *FileName);

extern void FASTCALL ScreenShapePtrInit(ScreenShapePtr_t *Input,struct Image_t *ImagePtr);
extern ScreenShapePtr_t *FASTCALL ScreenShapePtrNew(struct Image_t *ImagePtr);

extern void FASTCALL ScreenShapeRezGroupInit(ScreenShapeRezGroup_t *Input,struct RezHeader_t *RezFile,Word RezNum,Word RezPal,Word Which);
extern ScreenShapeRezGroup_t *FASTCALL ScreenShapeRezGroupNew(struct RezHeader_t *RezFile,Word RezNum,Word RezPal,Word Which);

extern void FASTCALL ScreenShapeGifFileInit(ScreenShapeGifFile_t *Input,const char *FileName);
extern ScreenShapeGifFile_t *FASTCALL ScreenShapeGifFileNew(const char *FileName);

extern void FASTCALL ScreenShapeGifInit(ScreenShapeGif_t *Input,struct RezHeader_t *RezFile,Word RezNum);
extern ScreenShapeGif_t *FASTCALL ScreenShapeGifNew(struct RezHeader_t *RezFile,Word RezNum);
extern void FASTCALL ScreenShapeGifReinit(ScreenShapeGif_t *Input,struct RezHeader_t *RezFile,Word RezNum);

extern void FASTCALL ScreenShapeBmpFileInit(ScreenShapeBmpFile_t *Input,const char *FileName);
extern ScreenShapeBmpFile_t *FASTCALL ScreenShapeBmpFileNew(const char *FileName);

extern void FASTCALL ScreenShapeBmpInit(ScreenShapeBmp_t *Input,struct RezHeader_t *RezFile,Word RezNum);
extern ScreenShapeBmp_t *FASTCALL ScreenShapeBmpNew(struct RezHeader_t *RezFile,Word RezNum);
extern void FASTCALL ScreenShapeBmpReinit(ScreenShapeBmp_t *Input,struct RezHeader_t *RezFile,Word RezNum);

/* OS Cursor handler */

extern void FASTCALL OSCursorSet(Word Curnum);
extern void FASTCALL OSCursorShow(void);
extern void FASTCALL OSCursorHide(void);
extern void FASTCALL OSCursorReset(void);
extern Word FASTCALL OSCursorPresent(void);
extern Word FASTCALL OSCursorIsVisible(void);
extern Word FASTCALL OSCursorNumber(void);

/* Rect handlers */

extern void FASTCALL LBPointFromSYSPOINT(LBPoint *Output,const SYSPOINT *Input);
extern void FASTCALL LBPointToSYSPOINT(SYSPOINT *Output,const LBPoint *Input);
extern Word FASTCALL SYSRECTPtInRect(const SYSRECT *InputRect,int x,int y);
extern Word FASTCALL LBPointRead(LBPoint *Output,FILE *fp);
extern Word FASTCALL LBPointWrite(const LBPoint *Input,FILE *fp);

extern void FASTCALL LBRectSetRect(LBRect *Input,int x1,int y1,int x2,int y2);
#define LBRectWidth(Input) ((Input)->right-(Input)->left)
#define LBRectHeight(Input) ((Input)->bottom-(Input)->top)
extern void FASTCALL LBRectSetRectEmpty(LBRect *Input);
extern void FASTCALL LBRectSetWidth(LBRect *Input,int Width);
extern void FASTCALL LBRectSetHeight(LBRect *Input,int Height);
extern Word FASTCALL LBRectPtInRect(const LBRect *Input,int x,int y);
extern Word FASTCALL LBRectPointInRect(const LBRect *Input1,const LBPoint *Input2);
extern void FASTCALL LBRectOffsetRect(LBRect *Input,int h,int v);
extern void FASTCALL LBRectInsetRect(LBRect *Input,int x,int y);
extern Word FASTCALL LBRectIsEqual(const LBRect *Input1,const LBRect *Input2);
extern Word FASTCALL LBRectIntersectRect(LBRect *Input,const LBRect *rect1,const LBRect *rect2);
extern void FASTCALL LBRectUnionRect(LBRect *Output,const LBRect *Input1,const LBRect *Input2);
extern void FASTCALL LBRectAddPointToRect(LBRect *Output,const LBPoint *Input);
extern void FASTCALL LBRectAddXYToRect(LBRect *Input,int x,int y);
extern Word FASTCALL LBRectIsRectEmpty(const LBRect *Input);
extern Word FASTCALL LBRectIsInRect(const LBRect *Input1,const LBRect *Input2);
extern void FASTCALL LBRectClipWithinRect(LBRect *Input,const LBRect *Bounds);
extern void FASTCALL LBRectMove(LBRect *Input,int x,int y);
extern void FASTCALL LBRectMoveX(LBRect *Input,int x);
extern void FASTCALL LBRectMoveY(LBRect *Input,int y);
extern void FASTCALL LBRectMoveToPoint(LBRect *Input,const LBPoint *Input2);
extern void FASTCALL LBRectMoveWithinRect(LBRect *Input,const LBRect *Bounds);
extern void FASTCALL LBRectFix(LBRect *Input);
extern void FASTCALL LBRectGetCenter(int *x,int *y,const LBRect *Input);
extern int FASTCALL LBRectGetCenterX(const LBRect *Input);
extern int FASTCALL LBRectGetCenterY(const LBRect *Input);
extern void FASTCALL LBRectGetCenterPoint(LBPoint *Output,const LBRect *Input);
extern void FASTCALL LBRectCenterAroundPoint(LBRect *Output,const LBPoint *Input);
extern void FASTCALL LBRectCenterAroundXY(LBRect *Output,int x,int y);
extern void FASTCALL LBRectCenterAroundX(LBRect *Output,int x);
extern void FASTCALL LBRectCenterAroundY(LBRect *Output,int y);
extern void FASTCALL LBRectCenterAroundRectCenter(LBRect *Output,const LBRect *Input);
extern void FASTCALL LBRectCenterAroundRectCenterX(LBRect *Output,const LBRect *Input);
extern void FASTCALL LBRectCenterAroundRectCenterY(LBRect *Output,const LBRect *Input);
extern void FASTCALL LBRectMapPoint(LBPoint *Output,const LBRect *SrcBoundsRect,const LBRect *DestBoundsRect,const LBPoint *Input);
extern void FASTCALL LBRectMapRect(LBRect *Output,const LBRect *SrcBoundsRect,const LBRect *DestBoundsRect,const LBRect *Input);
extern void FASTCALL LBRectFromSYSRECT(LBRect *Output,const SYSRECT *Input);
extern void FASTCALL LBRectToSYSRECT(SYSRECT *Output,const LBRect *Input);
extern Word FASTCALL LBRectRead(LBRect *Output,FILE *fp);
extern Word FASTCALL LBRectWrite(const LBRect *Input,FILE *fp);
extern void FASTCALL LBRectReadStream(LBRect *Output,struct StreamHandle_t *fp);
extern void FASTCALL LBRectWriteStream(const LBRect *Input,struct StreamHandle_t *fp);

extern LBRectList * FASTCALL LBRectListNew(void);
extern void FASTCALL LBRectListDelete(LBRectList *Input);
extern void FASTCALL LBRectListInit(LBRectList *Input);
extern void FASTCALL LBRectListDestroy(LBRectList *Input);
extern Word FASTCALL LBRectListRectClip(LBRectList *Input,const LBRect* b,const LBRect* t);
extern void FASTCALL LBRectListClipOutRect(LBRectList *Input,const LBRect *bound);
extern void FASTCALL LBRectListClipOutRectList(LBRectList *Input,const LBRectList *list);
extern void FASTCALL LBRectListAppendRect(LBRectList *Input,const LBRect *rect);
extern void FASTCALL LBRectListAppendRectList(LBRectList *Input,const LBRectList *list);
extern void FASTCALL LBRectListCopy(LBRectList *Input,const LBRectList *list);
extern void FASTCALL LBRectListRead(LBRectList *Output,FILE *fp);
extern void FASTCALL LBRectListWrite(const LBRectList *Input,FILE *fp);

/* Memory handlers */

#define MMStageCompact 0
#define MMStagePurge 1

typedef void (FASTCALL *MemPurgeProcPtr)(Word Stage);

extern LongWord MaxMemSize;		/* Maximum memory the program will take (4M) */
extern LongWord MinReserveSize;	/* Minimum memory for OS (64K) */
extern Word MinHandles;			/* Number of handles to create (500) */
extern MemPurgeProcPtr MemPurgeCallBack;	/* Callback before memory purging */
extern void FASTCALL InitMemory(void);	/* Call this FIRST! */
#define AllocAHandle(MemSize) AllocAHandle2(MemSize,0)
extern void ** FASTCALL AllocAHandle2(LongWord MemSize,Word Flag);
extern void ** FASTCALL AllocAHandleClear(LongWord MemSize);
extern void FASTCALL DeallocAHandle(void **MemHandle);
extern void * FASTCALL AllocAPointer(LongWord MemSize);
extern void * FASTCALL AllocAPointerClear(LongWord MemSize);
extern void FASTCALL DeallocAPointer(void *MemPtr);
extern void ** FASTCALL ReallocAHandle(void **MemHandle);
extern void ** FASTCALL FindAHandle(void *MemPtr);
extern void * FASTCALL LockAHandle(void **MemHandle);
extern void FASTCALL UnlockAHandle(void **MemHandle);
extern void FASTCALL CompactHandles(void);
extern void FASTCALL PurgeAHandle(void **MemHandle);
extern Word FASTCALL PurgeHandles(LongWord MemSize);
extern LongWord FASTCALL GetAHandleSize(void **MemHandle);
extern LongWord FASTCALL GetAPointerSize(void *MemPtr);
extern Word FASTCALL GetAHandleLockedState(void **MemHandle);
extern void FASTCALL SetAHandleLockedState(void **MemHandle,Word State);
extern LongWord FASTCALL GetTotalFreeMem(void);
extern LongWord FASTCALL GetTotalAllocatedMem(void);
extern void FASTCALL SetHandlePurgeFlag(void **MemHandle,Word Flag);
extern void FASTCALL DumpHandles(void);
extern void * FASTCALL ResizeAPointer(void *Mem,LongWord Size);
extern void ** FASTCALL ResizeAHandle(void **Mem,LongWord Size);
extern void * FASTCALL MemoryNewPointerCopy(const void *Mem,LongWord Size);
extern void ** FASTCALL MemoryNewHandleCopy(void **Mem);
extern void FASTCALL DebugAddSourceLine(void **MemHandle,const char *Source,LongWord Line,Word IsPointer);
extern Word FASTCALL DebugRemoveSourceLine(void **MemHandle,const char *Source,Word LineNum);
extern void FASTCALL DebugGetSourceLineInfo(void **MemHanel,char **Source,LongWord *Line);
extern void ** FASTCALL DebugAllocAHandle2(LongWord MemSize,Word Flag,const char *Source,Word LineNum);
extern void ** FASTCALL DebugAllocAHandleClear(LongWord MemSize,const char *Source,Word LineNum);
extern void FASTCALL DebugDeallocAHandle(void **MemHandle,const char *Source,Word LineNum);
extern void * FASTCALL DebugAllocAPointer(LongWord MemSize,const char *Source,Word LineNum);
extern void * FASTCALL DebugAllocAPointerClear(LongWord MemSize,const char *Source,Word LineNum);
extern void FASTCALL DebugDeallocAPointer(void *MemPtr,const char *Source,Word LineNum);
extern void ** FASTCALL DebugReallocAHandle(void **MemHandle,const char *Source,Word LineNum);
extern void * FASTCALL DebugResizeAPointer(void *Mem,LongWord Size,const char *Source,Word LineNum);
extern void ** FASTCALL DebugResizeAHandle(void **Mem,LongWord Size,const char *Source,Word LineNum);
extern void * FASTCALL DebugMemoryNewPointerCopy(const void *Mem,LongWord Size,const char *Source,Word LineNum);
extern void ** FASTCALL DebugMemoryNewHandleCopy(void **Mem,const char *Source,Word LineNum);
extern Word FASTCALL DebugMemoryIsPointerValid(void *MemPtr);
extern Word FASTCALL DebugMemoryIsHandleValid(void **MemHandle);

#if _DEBUG
#undef AllocAHandle
#define AllocAHandle(x) DebugAllocAHandle2(x,0,__FILE__,__LINE__)
#define AllocAHandle2(x,y) DebugAllocAHandle2(x,y,__FILE__,__LINE__)
#define AllocAHandleClear(x) DebugAllocAHandleClear(x,__FILE__,__LINE__)
#define DeallocAHandle(x) DebugDeallocAHandle(x,__FILE__,__LINE__)
#define AllocAPointer(x) DebugAllocAPointer(x,__FILE__,__LINE__)
#define AllocAPointerClear(x) DebugAllocAPointerClear(x,__FILE__,__LINE__)
#define DeallocAPointer(x) DebugDeallocAPointer(x,__FILE__,__LINE__)
#define ReallocAHandle(x) DebugReallocAHandle(x,__FILE__,__LINE__)
#define ResizeAPointer(x,y) DebugResizeAPointer(x,y,__FILE__,__LINE__)
#define ResizeAHandle(x,y) DebugResizeAHandle(x,y,__FILE__,__LINE__)
#define MemoryNewPointerCopy(x,y) DebugMemoryNewPointerCopy(x,y,__FILE__,__LINE__)
#define MemoryNewHandleCopy(x) DebugMemoryNewHandleCopy(x,__FILE__,__LINE__)
#define MemoryIsPointerValid(x) DebugMemoryIsPointerValid(x)
#define MemoryIsHandleValid(x) DebugMemoryIsHandleValid(x)
#else
#define MemoryIsPointerValid(x)
#define MemoryIsHandleValid(x)
#endif

#if !defined(__MAC__)
extern MyHandle UsedHand1;		/* First used memory handle */
extern MyHandle UsedHand2;		/* Last used memory handle */
extern MyHandle PurgeHands;		/* Purged handle list */
extern MyHandle FreeMemHands;	/* Free handle list */
extern MyHandle PurgeHandleFiFo;	/* Purged handle linked list */
extern void FASTCALL ReleaseMemRange(void *MemPtr,LongWord Length,MyHandle *Parent);
extern void FASTCALL GrabMemRange(void *MemPtr,LongWord Length,MyHandle *Parent,MyHandle *Scan);
extern MyHandle * FASTCALL GetFromFreeHandleList(void);
extern void FASTCALL AddToFreeHandleList(MyHandle *Input);
#endif

#if defined(__IBM__)
extern Byte *_x32_zero_base_ptr;
extern Short _x32_zero_base_selector;
#define ZeroBase _x32_zero_base_ptr
extern LongWord FASTCALL AllocRealMemory(LongWord Size);
extern void FASTCALL DeallocRealMemory(LongWord RealPtr);
extern void * FASTCALL RealToProtectedPtr(LongWord RealPtr);
extern LongWord FASTCALL GetRealBufferPtr(void);
extern void * FASTCALL GetRealBufferProtectedPtr(void);
#endif

typedef enum { MEMPOOL_STATIC,MEMPOOL_DYNAMIC,MEMPOOL_LOOP} MemPoolBehaviour_e;

typedef struct MemPool_t {
	void ***ArrayHandle;		/* Handle to free memory list */
	Byte *RootMem;				/* Pointer to the allocated memory */
	Word ArraySize;				/* Size of the free memory list */
	Word Count;					/* Number of entries used */
	Word ChunkSize;				/* Size of each entry in bytes */
	MemPoolBehaviour_e Behaviour;	/* Type of MemPool_t */
} MemPool_t;

extern Word FASTCALL MemPoolInit(MemPool_t *Input,Word InitialSize,Word ChunkSize,MemPoolBehaviour_e Behaviour);
extern MemPool_t * FASTCALL MemPoolNew(Word InitialSize,Word ChunkSize,MemPoolBehaviour_e Behaviour);
extern void FASTCALL MemPoolDestroy(MemPool_t *Input);
extern void FASTCALL MemPoolDelete(MemPool_t *Input);
extern void * FASTCALL MemPoolAllocate(MemPool_t *Input);
extern void FASTCALL MemPoolDeallocate(MemPool_t *Input,void *MemPtr);

/* Resource Handlers */

typedef struct RezHeader_t {	/* Master entry to the resource manager */
	void (FASTCALL *DecompPtrs[3])(Byte *,Byte *,LongWord,LongWord);	/* Decompressors */
	LongWord Count;				/* Number of resource groups */
	LongWord RezNameCount;		/* Number of resource names */
	void *fp;					/* Open file reference */
	struct RezGroup_t **GroupHandle;	/* First entry */
	struct RezName_t **RezNames;		/* Handle to resource names if present */
	Word Flags;					/* Flags on how to handle resources */
} RezHeader_t;

typedef struct RezNameReturn_t {
	char *RezName;		/* Resource name */
	Word RezNum;		/* Resource number */
} RezNameReturn_t;

typedef void (FASTCALL *ResourceDecompressorProcPtr)(Byte *,Byte *,LongWord Length,LongWord PackLength);

extern Boolean ResourceJustLoaded;	/* TRUE if ResourceLoadHandle() freshly loaded a handle */
extern RezHeader_t MasterRezHeader;	/* Default resource file */

extern RezHeader_t *ResourceNew(const char *FileName,LongWord StartOffset);
extern Word FASTCALL ResourceInit(RezHeader_t *Input,const char *FileName,LongWord StartOffset);
extern void FASTCALL ResourceDestroy(RezHeader_t *Input);
extern void FASTCALL ResourceDelete(RezHeader_t *Input);
extern Word FASTCALL ResourceInitMasterRezHeader(const char *FileName);
extern void FASTCALL ResourcePurgeCache(RezHeader_t *Input);
extern Word FASTCALL ResourceExternalFlag(RezHeader_t *Input,Word Flag);
extern Word FASTCALL ResourceDontCacheFlag(RezHeader_t *Input,Word Flag);
extern Word FASTCALL ResourceAddName(RezHeader_t *Input,const char *RezName);
extern void FASTCALL ResourceRemove(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourceRemoveName(RezHeader_t *Input,const char *RezName);
extern Word FASTCALL ResourceRead(RezHeader_t *Input,Word RezNum,void *DestPtr,LongWord BufSize);
extern void * FASTCALL ResourceLoad(RezHeader_t *Input,Word RezNum);
extern void * FASTCALL ResourceLoadByName(RezHeader_t *Input,const char *RezName);
extern void ** FASTCALL ResourceLoadHandle(RezHeader_t *Input,Word RezNum);
extern void ** FASTCALL ResourceLoadHandleByName(RezHeader_t *Input,const char *RezName);
extern void FASTCALL ResourceRelease(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourceReleaseByName(RezHeader_t *Input,const char *RezName);
extern void FASTCALL ResourceKill(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourceKillByName(RezHeader_t *Input,const char *RezName);
extern void FASTCALL ResourceDetach(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourceDetachByName(RezHeader_t *Input,const char *RezName);
extern void FASTCALL ResourcePreload(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadByName(RezHeader_t *Input,const char *RezName);
extern Word FASTCALL ResourceGetRezNum(RezHeader_t *Input,const char *RezName);
extern Word FASTCALL ResourceGetName(RezHeader_t *Input,Word RezNum,char *Buffer,Word BufferSize);
extern Word FASTCALL ResourceGetIDFromHandle(RezHeader_t *Input,const void **RezHand,Word *IDFound,char *NameBuffer,Word NameBufferSize);
extern Word FASTCALL ResourceGetIDFromPointer(RezHeader_t *Input,const void *RezPtr,Word *IDFound,char *NameBuffer,Word NameBufferSize);
extern RezNameReturn_t *FASTCALL ResourceGetNameArray(RezHeader_t *Input,Word *EntryCountPtr);
extern void FASTCALL ResourceLogDecompressor(RezHeader_t *Input,Word CompressID,ResourceDecompressorProcPtr Proc);
extern struct LWShape_t * FASTCALL ResourceLoadShape(RezHeader_t *Input,Word RezNum);
extern struct LWXShape_t * FASTCALL ResourceLoadXShape(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadShape(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadXShape(RezHeader_t *Input,Word RezNum);
extern void * FASTCALL ResourceLoadShapeArray(RezHeader_t *Input,Word RezNum);
extern void * FASTCALL ResourceLoadXShapeArray(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadShapeArray(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadXShapeArray(RezHeader_t *Input,Word RezNum);
extern struct LWShape_t ** FASTCALL ResourceLoadShapeHandle(RezHeader_t *Input,Word RezNum);
extern struct LWXShape_t ** FASTCALL ResourceLoadXShapeHandle(RezHeader_t *Input,Word RezNum);
extern void ** FASTCALL ResourceLoadShapeArrayHandle(RezHeader_t *Input,Word RezNum);
extern void ** FASTCALL ResourceLoadXShapeArrayHandle(RezHeader_t *Input,Word RezNum);
extern struct GfxShape_t *FASTCALL ResourceLoadGfxShape(RezHeader_t *Input,Word RezNum);
extern void FASTCALL ResourcePreloadGfxShape(RezHeader_t *Input,Word RezNum);
extern void ** FASTCALL ResourceLoadGfxShapeHandle(RezHeader_t *Input,Word RezNum);


/* ASync data reading */

typedef struct ReadFileStream_t ReadFileStream_t;

extern struct ReadFileStream_t *FASTCALL ReadFileStreamNew(const char *FileName,Word Count,Word ChunkSize);
extern void FASTCALL ReadFileStreamDelete(struct ReadFileStream_t *Input);
extern Word FASTCALL ReadFileStreamActive(struct ReadFileStream_t *Input);
extern Word FASTCALL ReadFileStreamPending(struct ReadFileStream_t *Input);
extern void FASTCALL ReadFileStreamStop(struct ReadFileStream_t *Input);
extern void FASTCALL ReadFileStreamStart(struct ReadFileStream_t *Input,LongWord Offset);
extern LongWord FASTCALL ReadFileStreamAvailBytes(struct ReadFileStream_t *Input);
extern Byte * FASTCALL ReadFileStreamGetData(struct ReadFileStream_t *Input,LongWord *ReadSizeOut,LongWord ReadSize);
extern void FASTCALL ReadFileStreamAcceptData(struct ReadFileStream_t *Input);

/* Compression routines */

typedef struct PackState_t {	/* State of a decompression buffer */
    Byte *PackPtr;				/* Packed data pointer */
    LongWord PackLen;			/* Number of packed bytes remaining */
    Byte *OutPtr;				/* Output data pointer */
    LongWord OutLen;			/* Number of bytes in the output buffer */
    void *Internal;				/* Pointer to algorithm specific code */
} PackState_t;

typedef struct MACEState_t {	/* State of MACE compression/decompression */
	long Sample1;		/* Last running samples */
	long Sample2;		/* Second temp sample */
	long PrevVal;		/* Mask with 0x8000 for + or - direction */
	long TableIndex;	/* Which slope table */
	long LastAmplitude;	/* Slope * PrevVal */
	long LastSlope;		/* Last Slope value */
} MACEState_t;

typedef struct ADPCMUnpackState_t {	/* State of ADPCM decompression */
	Byte *SrcPtr;			/* Pointer to the source data */
	LongWord SrcLength;		/* Pointer to the size of the source buffer */
	Word Channels;			/* Number of channels to decode (1 or 2) */
	Word BlockSize;			/* Size of each compressed block */
	Word SamplesPerBlock;	/* Number of samples to decompress per block */
	short *OutputPtr;		/* Output buffer */
} ADPCMUnpackState_t;

extern void FASTCALL DLZSS(Byte *DestPtr,Byte *SrcPtr,LongWord Length,LongWord PackedLen);
extern void FASTCALL DLZSSFast(Byte *DestPtr,Byte *SrcPtr,LongWord Length);
extern void ** FASTCALL EncodeLZSS(Byte *InputBuffer,LongWord Length);
extern void FASTCALL DHuffman(Byte *DestPtr,Byte *SrcPtr,LongWord Length,LongWord PackedLen);
extern void FASTCALL DHuffmanFast(Byte *DestPtr,Byte *SrcPtr,LongWord Length);
extern void ** FASTCALL EncodeHuffman(Byte *InputBuffer,LongWord Length);
extern void FASTCALL DLZH(Byte *DestPtr,Byte *SrcPtr,LongWord Length,LongWord PackedLen);
extern void FASTCALL DLZHFast(Byte *DestPtr,Byte *SrcPtr,LongWord Length);
extern void ** FASTCALL EncodeLZH(Byte *InputBuffer,LongWord Length);
extern void FASTCALL DRLE(Byte *DestPtr,Byte *SrcPtr,LongWord Length,LongWord PackedLen);
extern void FASTCALL DRLEFast(Byte *DestPtr,Byte *SrcPtr,LongWord Length);
extern void ** FASTCALL EncodeRLE(Byte *InputBuffer,LongWord Length);
extern Word FASTCALL DInflateInit(PackState_t *Input);
extern Word FASTCALL DInflateMore(PackState_t *Input);
extern void FASTCALL DInflateDestroy(PackState_t *Input);
extern void FASTCALL DInflate(Byte *DestPtr,Byte *SrcPtr,LongWord Length,LongWord PackedLen);
extern void FASTCALL DInflateFast(Byte *DestPtr,Byte *SrcPtr,LongWord Length);
extern void ** FASTCALL EncodeInflate(Byte *InputBuffer,LongWord Length);
extern void FASTCALL MACEExp1to3(const Byte *InBufPtr,Byte *OutBufPtr,LongWord Count,MACEState_t *InStatePtr,MACEState_t *OutStatePtr,Word NumChannels,Word WhichChannel);
extern void FASTCALL MACEExp1to6(const Byte *InBufPtr,Byte *OutBufPtr,LongWord Count,MACEState_t *InStatePtr,MACEState_t *OutStatePtr,Word NumChannels,Word WhichChannel);
extern Word FASTCALL ADPCMDecodeBlock(ADPCMUnpackState_t *StatePtr);

/* Ogg libraries */

typedef struct oggpack_buffer {
	Byte *ptr;			/* Pointer to the current byte scanned */
	Byte *buffer;		/* Pointer to the work buffer */
	LongWord endbyte;	/* Bytes accepted */
	LongWord storage;	/* Size of the buffer in bytes */
	Word endbit;		/* Work bit (0-7) */
} oggpack_buffer;

typedef struct ogg_page {
	Byte *header;			/* Pointer to the data header */
	Byte *body;				/* Pointer to the data */
	LongWord header_len;	/* Size of the header */
	LongWord body_len;		/* Size of the data */
} ogg_page;

typedef struct ogg_stream_state {
	Byte *body_data;		/* bytes from packet bodies */
	long body_storage;		/* storage elements allocated */
	long body_fill;			/* elements stored; fill mark */
	long body_returned;		/* elements of fill returned */

	int *lacing_vals;      /* The values that will go to the segment table */
	LongWord64_t *granule_vals;	/* granulepos values for headers. Not compact */
								/* this way, but it is simple coupled to the lacing fifo */
	long lacing_storage;
	long lacing_fill;
	long lacing_packet;
	long lacing_returned;

	Byte header[282];		/* working space for header encode */
	Byte Padding[2];		/* Long align */
	int header_fill;

	int e_o_s;				/* set when we have buffered the last packet in the logical bitstream */
	int b_o_s;				/* set after we've written the initial page of a logical bitstream */
	long serialno;
	long pageno;
	LongWord64_t packetno;	/* sequence number for decode; the framing
								knows where there's a hole in the data,
								but we need coupling so that the codec
								(which is in a seperate abstraction
								layer) also knows about the gap */
	LongWord64_t granulepos;
} ogg_stream_state;

typedef struct ogg_packet {
	Byte *packet;
	long bytes;
	long b_o_s;
	long e_o_s;
	LongWord64_t granulepos;
	LongWord64_t packetno;		/* sequence number for decode; the framing
								knows where there's a hole in the data,
								but we need coupling so that the codec
								(which is in a seperate abstraction
								layer) also knows about the gap */
} ogg_packet;

typedef struct ogg_sync_state {
	Byte *data;
	int storage;
	int fill;
	int returned;
	int unsynced;
	int headerbytes;
	int bodybytes;
} ogg_sync_state;

extern void FASTCALL oggpack_writeinit(oggpack_buffer *b);
extern void FASTCALL oggpack_reset(oggpack_buffer *b);
extern void FASTCALL oggpack_writeclear(oggpack_buffer *b);
extern void FASTCALL oggpack_readinit(oggpack_buffer *b,Byte *buf,LongWord bytes);
extern void FASTCALL oggpack_write(oggpack_buffer *b,LongWord value,Word bits);
extern LongWord FASTCALL oggpack_look(oggpack_buffer *b,Word bits);
extern Word FASTCALL oggpack_look1(oggpack_buffer *b);
extern LongWord FASTCALL oggpack_look_huff(oggpack_buffer *b,Word bits);
extern void FASTCALL oggpack_adv(oggpack_buffer *b,Word bits);
extern void FASTCALL oggpack_adv1(oggpack_buffer *b);
extern int FASTCALL oggpack_adv_huff(oggpack_buffer *b,Word bits);
extern LongWord FASTCALL oggpack_read(oggpack_buffer *b,Word bits);
extern Word FASTCALL oggpack_read1(oggpack_buffer *b);
extern LongWord FASTCALL oggpack_bytes(oggpack_buffer *b);
extern LongWord FASTCALL oggpack_bits(oggpack_buffer *b);
extern Byte *FASTCALL oggpack_get_buffer(oggpack_buffer *b);
extern Word FASTCALL ogg_page_version(ogg_page *og);
extern Word FASTCALL ogg_page_continued(ogg_page *og);
extern Word FASTCALL ogg_page_bos(ogg_page *og);
extern Word FASTCALL ogg_page_eos(ogg_page *og);
extern LongWord64_t FASTCALL ogg_page_granulepos(ogg_page *og);
extern LongWord FASTCALL ogg_page_serialno(ogg_page *og);
extern LongWord FASTCALL ogg_page_pageno(ogg_page *og);
extern Word FASTCALL ogg_page_packets(ogg_page *og);
extern int FASTCALL ogg_stream_init(ogg_stream_state *os,int serialno);
extern void FASTCALL ogg_stream_clear(ogg_stream_state *os);
extern void FASTCALL ogg_stream_destroy(ogg_stream_state *os);
extern void FASTCALL ogg_page_checksum_set(ogg_page *og);
extern int FASTCALL ogg_stream_packetin(ogg_stream_state *os, ogg_packet *op);
extern int FASTCALL ogg_stream_flush(ogg_stream_state *os, ogg_page *og);
extern int FASTCALL ogg_stream_pageout(ogg_stream_state *os, ogg_page *og);
extern int FASTCALL ogg_stream_eos(ogg_stream_state *os);
extern int FASTCALL ogg_sync_init(ogg_sync_state *oy);
extern int FASTCALL ogg_sync_clear(ogg_sync_state *oy);
extern int FASTCALL ogg_sync_destroy(ogg_sync_state *oy);
extern char *FASTCALL ogg_sync_buffer(ogg_sync_state *oy, long size);
extern int FASTCALL ogg_sync_wrote(ogg_sync_state *oy, long bytes);
extern long FASTCALL ogg_sync_pageseek(ogg_sync_state *oy,ogg_page *og);
extern int FASTCALL ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og);
extern int FASTCALL ogg_stream_pagein(ogg_stream_state *os, ogg_page *og);
extern int FASTCALL ogg_sync_reset(ogg_sync_state *oy);
extern int FASTCALL ogg_stream_reset(ogg_stream_state *os);
extern int FASTCALL ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op);
extern int FASTCALL ogg_stream_packetpeek(ogg_stream_state *os,ogg_packet *op);
extern void FASTCALL ogg_packet_clear(ogg_packet *op);

/* Vorbis libraries */

#define OV_FALSE      -1  
#define OV_EOF        -2
#define OV_HOLE       -3
#define OV_EREAD      -128
#define OV_EFAULT     -129
#define OV_EIMPL      -130
#define OV_EINVAL     -131
#define OV_ENOTVORBIS -132
#define OV_EBADHEADER -133
#define OV_EVERSION   -134
#define OV_ENOTAUDIO  -135
#define OV_EBADPACKET -136
#define OV_EBADLINK   -137
#define OV_ENOSEEK    -138

typedef struct vorbis_info {
	int version;
	int channels;
	long rate;
	long bitrate_upper;
	long bitrate_nominal;
	long bitrate_lower;
	long bitrate_window;
	void *codec_setup;
} vorbis_info;

typedef struct vorbis_dsp_state {
	int analysisp;
	vorbis_info *vi;
	float **pcm;
	float **pcmret;
	int pcm_storage;
	int pcm_current;
	int pcm_returned;
	int preextrapolate;
	int eofflag;
	long lW;
	long W;
	long nW;
	long centerW;
	LongWord64_t granulepos;
	LongWord64_t sequence;
	LongWord64_t glue_bits;
	LongWord64_t time_bits;
	LongWord64_t floor_bits;
	LongWord64_t res_bits;
	void *backend_state;
} vorbis_dsp_state;

typedef struct vorbis_block {
	/* necessary stream state for linking to the framing abstraction */
	float  **pcm;       /* this is a pointer into local storage */ 
	oggpack_buffer opb;
	long lW;
	long W;
	long nW;
	int pcmend;
	int mode;
	int eofflag;
	LongWord64_t granulepos;
	LongWord64_t sequence;
	vorbis_dsp_state *vd; /* For read-only access of configuration */

	/* local storage to avoid remallocing; it's up to the mapping to
	structure it */
	void *localstore;
	long localtop;
	long localalloc;
	long totaluse;
	struct vorbis_alloc_chain *reap;

	/* bitmetrics for the frame */
	long glue_bits;
	long time_bits;
	long floor_bits;
	long res_bits;
	void *internal;
} vorbis_block;

typedef struct vorbis_alloc_chain {
	void *ptr;
	struct vorbis_alloc_chain *next;
} vorbis_alloc_chain;

typedef struct vorbis_comment{
	char **user_comments;
	int *comment_lengths;
	int comments;
	char *vendor;
} vorbis_comment;

extern int analysis_noisy;
extern int FASTCALL vorbis_analysis(vorbis_block *vb,ogg_packet *op);
extern void FASTCALL _analysis_output(char *base,int i,float *v,int n,int bark,int dB);
extern void FASTCALL vorbis_comment_init(vorbis_comment *vc);
extern void FASTCALL vorbis_comment_add(vorbis_comment *vc, char *comment); 
extern void FASTCALL vorbis_comment_add_tag(vorbis_comment *vc, char *tag, char *contents);
extern char *FASTCALL vorbis_comment_query(vorbis_comment *vc, char *tag, int count);
extern int FASTCALL vorbis_comment_query_count(vorbis_comment *vc, char *tag);
extern void FASTCALL vorbis_comment_clear(vorbis_comment *vc);
extern int FASTCALL vorbis_info_blocksize(vorbis_info *vi,int zo);
extern void FASTCALL vorbis_info_init(vorbis_info *vi);
extern void FASTCALL vorbis_info_clear(vorbis_info *vi);
extern int FASTCALL vorbis_synthesis_headerin(vorbis_info *vi,vorbis_comment *vc,ogg_packet *op);
extern int FASTCALL vorbis_commentheader_out(vorbis_comment *vc, ogg_packet *op);
extern int FASTCALL vorbis_analysis_headerout(vorbis_dsp_state *v,vorbis_comment *vc,
	ogg_packet *op,ogg_packet *op_comm,ogg_packet *op_code);
extern int FASTCALL vorbis_synthesis(vorbis_block *vb,ogg_packet *op);
extern long FASTCALL vorbis_packet_blocksize(vorbis_info *vi,ogg_packet *op);
extern int FASTCALL vorbis_block_init(vorbis_dsp_state *v, vorbis_block *vb);
extern void *FASTCALL _vorbis_block_alloc(vorbis_block *vb,long bytes);
extern void FASTCALL _vorbis_block_ripcord(vorbis_block *vb);
extern int FASTCALL vorbis_block_clear(vorbis_block *vb);
extern int FASTCALL vorbis_analysis_init(vorbis_dsp_state *v,vorbis_info *vi);
extern void FASTCALL vorbis_dsp_clear(vorbis_dsp_state *v);
extern float **FASTCALL vorbis_analysis_buffer(vorbis_dsp_state *v,int vals);
extern int FASTCALL vorbis_analysis_wrote(vorbis_dsp_state *v,int vals);
extern int FASTCALL vorbis_analysis_blockout(vorbis_dsp_state *v,vorbis_block *vb);
extern int FASTCALL vorbis_synthesis_init(vorbis_dsp_state *v,vorbis_info *vi);
extern int FASTCALL vorbis_synthesis_blockin(vorbis_dsp_state *v,vorbis_block *vb);
extern int FASTCALL vorbis_synthesis_pcmout(vorbis_dsp_state *v,float ***pcm);
extern int FASTCALL vorbis_synthesis_read(vorbis_dsp_state *v,int samples);

/* Image routines */

typedef enum {IMAGE332=4,IMAGE8ALPHA=5,IMAGE8_PAL_ALPHA_PAL=6,IMAGE8_PAL_ALPHA=7,IMAGE8_PAL=8,
	IMAGE4444=13,IMAGE1555=14,IMAGE555=15,IMAGE565=16,IMAGE888=24,IMAGE8888=32} ImageTypes_e;

typedef struct Image_t {	/* Used by image routines */
	Byte *ImagePtr;		/* Pointer to pixel array (AllocAPointer()) */
	Byte *PalettePtr;	/* Pointer to RGB tripletts, only for 8 BPP images */
	Byte *AlphaPtr;		/* Alpha channel if any */
	Word Width;			/* Width of image in pixels */
	Word Height;		/* Height of image in pixels */
	Word RowBytes;		/* Number of bytes per scan line */
	ImageTypes_e DataType;		/* 8,16,24 Bits per pixel */
} Image_t;

typedef enum {PSD_FLAG_RGB,PSD_FLAG_ALPHA,PSD_FLAG_MASK,PSD_FLAG_RGBA} PSDFlag_e;

typedef struct PSDImageLayer_t {
	char* Name;						/* Name of the image (If any) */
	int Top, Left, Bottom, Right;	/* Image bounds rect */
	int MaskTop, MaskLeft, MaskBottom, MaskRight;	/* Mask rect */
	int Width, Height;			/* Size of the image */
	int MaskWidth, MaskHeight;	/* Size of the mask */
	Byte* RGB;					/* RGB data (24 bit) */
	Byte* Alpha;				/* Alpha data */
	Byte* Mask;					/* Mask data */
} PSDImageLayer_t;

typedef struct PSDImage_t {
	Word Width;					/* Width in pixels */
	Word Height;				/* Height in pixels */
	Word NumLayers;				/* Number of layers present */
	PSDImageLayer_t* Layers;	/* Array of layers */
} PSDImage_t;

extern Word FASTCALL ImageInit(Image_t *Output,Word Width,Word Height,ImageTypes_e Depth);
extern Image_t * FASTCALL ImageNew(Word Width,Word Height,ImageTypes_e Depth);
extern Word FASTCALL ImageInitCopy(Image_t *Output,const Image_t *Input);
extern Image_t * FASTCALL ImageNewCopy(const Image_t *Input);
extern void FASTCALL ImageDelete(Image_t *ImagePtr);
extern void FASTCALL ImageDestroy(Image_t *ImagePtr);
extern Word FASTCALL ImageParseAny(Image_t *Output,const Byte *InputPtr,LongWord InputLength);
extern Word FASTCALL ImageParseBMP(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParseCicn(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParseGIF(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParseIIGS(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParseJPG(Image_t *Output,const Byte *InputPtr,LongWord InputLength);
extern Word FASTCALL ImageParseLBM(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParsePCX(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParsePPat(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParsePict(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParsePSD(Image_t *Output,const Byte *InputPtr,Word Layer);
extern Word FASTCALL ImageParseTGA(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageParseTIFF(Image_t *Output,const Byte *InputPtr);
extern Word FASTCALL ImageExtractFromPSDImage(Image_t *Output,PSDImageLayer_t* Layer,PSDFlag_e flags);
extern Word FASTCALL Image2BMPFile(Image_t *ImagePtr,const char *FileName,Word NoCompress);
extern Word FASTCALL Image2GIFFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2IIGSFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2JPGFile(Image_t *ImagePtr,const char *FileName,Word CompressionFactor);
extern Word FASTCALL Image2LBMFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2PBMFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2PCXFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2PSDFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL Image2TGAFile(Image_t *ImagePtr,const char *FileName,Word NoCompress);
extern Word FASTCALL Image2TIFFFile(Image_t *ImagePtr,const char *FileName);
extern Word FASTCALL ImageStore(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore4444(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore555(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore565(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore1555(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore888(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore8888(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore332(Image_t *Output,const Image_t *Input);
extern Word FASTCALL ImageStore8Pal(Image_t *Output,const Image_t *Input);
extern void FASTCALL ImageColorKey8888(Image_t* SrcImagePtr,Word r,Word g,Word b,Word a);
extern Word FASTCALL ImageSubImage(Image_t* Output,Word x,Word y,const Image_t* Input);
extern void FASTCALL ImageVerticalFlip(Image_t *ImagePtr);
extern void FASTCALL ImageHorizontalFlip(Image_t *ImagePtr);
extern void FASTCALL ImageRemove0And255(Image_t *ImagePtr);
extern void FASTCALL ImageRepaletteIndexed(Image_t *ImagePtr,const Byte *PalettePtr);
extern void FASTCALL ImageRemapIndexed(Image_t *ImagePtr,const Byte *RemapPtr);
extern void FASTCALL ImageSwapBGRToRGB(Image_t *ImagePtr);
extern Word FASTCALL ImageValidateToSave(Image_t *ImagePtr);
extern void FASTCALL ImageEncodeLBM(FILE *fp,const Byte *SrcPtr,Word Length);
extern void FASTCALL ImageEncodePCX(FILE *fp,const Byte *SrcPtr,Word Length);
extern Word FASTCALL PSDImageParse(PSDImage_t* Image, const Byte* InStream);
extern void FASTCALL PSDImageDestroy(PSDImage_t* Input);

/* Random number generator */

typedef struct Random_t {
	Word Seed;		/* Random number seed */
	Word Index1;	/* First lookup index */
	Word Index2;	/* Second lookup index */
	Word RndArray[17];	/* Array of seed values (Polynomial) */
} Random_t;

extern Random_t RndMain;		/* Main random number instance */
extern Random_t RndAux;			/* Aux random number instance */
extern Random_t * FASTCALL RndNew(Word NewSeed);
#define RndDelete(Input) DeallocAPointer(Input)
extern void FASTCALL RndRandomize(Random_t *Input);
extern void FASTCALL RndHardwareRandomize(Random_t *Input);
extern Word FASTCALL RndGetRandom(Random_t *Input,Word Range);
extern void FASTCALL RndSetRandomSeed(Random_t *Input,Word Seed);
extern int FASTCALL RndGetRandomSigned(Random_t *Input,Word Range);
#define Randomize() RndRandomize(&RndMain)
#define HardwareRandomize() RndHardwareRandomize(&RndMain)
#define GetRandom(Range) RndGetRandom(&RndMain,Range)
#define SetRandomSeed(Seed) RndSetRandomSeed(&RndMain,Seed)
#define GetRandomSigned(Range) RndGetRandomSigned(&RndMain,Range)
#define AuxRandomize() RndRandomize(&RndAux)
#define AuxHardwareRandomize() RndHardwareRandomize(&RndAux)
#define AuxGetRandom(Range) RndGetRandom(&RndAux,Range)
#define AuxSetRandomSeed(Seed) RndSetRandomSeed(&RndAux,Seed)
#define AuxGetRandomSigned(Range) RndGetRandomSigned(&RndAux,Range)

/* File manager */

#define FULLPATHSIZE 2048	/* Maximum pathname size for Burgerlib */
#define PREFIXMAX 35		/* Maximum prefixs */
#define PREFIXBOOT 32		/* * prefix */
#define PREFIXPREFS 33		/* @ prefix */
#define PREFIXSYSTEM 34		/* $ prefix */

typedef struct DirectorySearch_t {
	LongWord FileSize;	/* Size of the file */
	TimeDate_t Create;	/* Creation time */
	TimeDate_t Modify;	/* Modification time */
	Boolean Dir;		/* True if this is a directory */
	Boolean System;		/* True if this is a system file */
	Boolean Hidden;		/* True if this file is hidden */
	Boolean Locked;		/* True if this file is read only */
	char Name[256];		/* Filename */
#if defined(__MAC__)
	Word Index;			/* Directory index */
	short VRefNum;		/* Volume ID */
	short Padding1;
	long DirID;			/* Directory to scan */
#elif defined(__MACOSX__)
	void *Enumerator;	/* Filename enumerator */
	char *PathPrefix;	/* Pathname buffer */
	char *PathEnd;		/* Pathname ending */
#elif defined(__IBM__)
	Word HandleOk;		/* Handle is valid */
	short FileHandle;	/* Handle to the open directory */
	char MyFindT[44];	/* Dos FindT structure */
#elif defined(__WIN95__)
	void *FindHandle;	/* Win95 file handle */
	Word HandleOk;		/* Handle is valid */
	char MyFindT[320];	/* Win95 FindT structure */
#endif
} DirectorySearch_t;

extern void ** PrefixHandles[PREFIXMAX];
extern Word FASTCALL GetFileModTime(const char *FileName,struct TimeDate_t *Output);
extern Word FASTCALL GetFileModTimeNative(const char *FileName,struct TimeDate_t *Output);
extern Word FASTCALL GetFileCreateTime(const char *FileName,struct TimeDate_t *Output);
extern Word FASTCALL GetFileCreateTimeNative(const char *FileName,struct TimeDate_t *Output);
extern Word FASTCALL DoesFileExist(const char *FileName);
extern Word FASTCALL DoesFileExistNative(const char *FileName);
extern int FASTCALL CompareTimeDates(const struct TimeDate_t *First,const struct TimeDate_t *Second);
extern Word FASTCALL CreateDirectoryPath(const char *FileName);
extern Word FASTCALL CreateDirectoryPath2(const char *FileName);
extern Word FASTCALL CreateDirectoryPathNative(const char *FileName);
extern Word FASTCALL OpenADirectory(DirectorySearch_t *Input,const char *Name);
extern Word FASTCALL GetADirectoryEntry(DirectorySearch_t *Input);
extern Word FASTCALL GetADirectoryEntryExtension(DirectorySearch_t *Input,const char *ExtPtr);
extern void FASTCALL CloseADirectory(DirectorySearch_t *Input);
extern FILE * FASTCALL OpenAFile(const char *FileName,const char *type);
extern Word FASTCALL CopyAFile(const char *DestFile,const char *SrcFile);
extern Word FASTCALL CopyAFileNative(const char *DestFile,const char *SrcFile);
extern Word FASTCALL CopyAFileFP(FILE *Destfp,FILE *Srcfp);
extern Word FASTCALL DeleteAFile(const char *FileName);
extern Word FASTCALL DeleteAFileNative(const char *FileName);
extern Word FASTCALL RenameAFile(const char *NewName,const char *OldName);
extern Word FASTCALL RenameAFileNative(const char *NewName,const char *OldName);
extern char * FASTCALL ExpandAPath(const char *FileName);
extern char * FASTCALL ExpandAPathNative(const char *FileName);
extern void FASTCALL ExpandAPathToBuffer(char *BufferPtr,const char *FileName);
extern void FASTCALL ExpandAPathToBufferNative(char *BufferPtr,const char *FileName);
extern char * FASTCALL ConvertNativePathToPath(const char *FileName);
extern char * FASTCALL GetAPrefix(Word PrefixNum);
extern Word FASTCALL SetAPrefix(Word PrefixNum,const char *PrefixName);
extern void FASTCALL SetDefaultPrefixs(void);
extern void FASTCALL PopAPrefix(Word PrefixNum);
extern char * FASTCALL GetAVolumeName(Word DriveNum);
extern Word FASTCALL FindAVolumeByName(const char *VolumeName);
extern Word FASTCALL ChangeADirectory(const char *DirName);
extern LongWord FASTCALL fgetfilesize(FILE *fp);
extern void FASTCALL fwritelong(LongWord Val,FILE *fp);
extern void FASTCALL fwritelongrev(LongWord Val,FILE *fp);
extern void FASTCALL fwriteshort(Short Val,FILE *fp);
extern void FASTCALL fwriteshortrev(Short Val,FILE *fp);
extern void FASTCALL fwritestr(const char *ValPtr,FILE *fp);
extern LongWord FASTCALL fgetlong(FILE *fp);
extern LongWord FASTCALL fgetlongrev(FILE *fp);
extern short FASTCALL fgetshort(FILE *fp);
extern short FASTCALL fgetshortrev(FILE *fp);
extern Word FASTCALL fgetstr(char *Input,Word Length,FILE *fp);
extern Word FASTCALL SaveAFile(const char *FileName,const void *DataPtr,LongWord Length);
extern Word FASTCALL SaveAFileNative(const char *FileName,const void *DataPtr,LongWord Length);
extern Word FASTCALL SaveAFileFP(FILE *Filefp,const void *DataPtr,LongWord Length);
extern Word FASTCALL SaveATextFile(const char *FileName,const void *DataPtr,LongWord Length);
extern Word FASTCALL SaveATextFileNative(const char *FileName,const void *DataPtr,LongWord Length);
extern void * FASTCALL LoadAFile(const char *FileName,LongWord *Length);
extern void * FASTCALL LoadAFileNative(const char *FileName,LongWord *Length);
extern void * FASTCALL LoadAFileFP(FILE *Filefp,LongWord *Length);
extern void ** FASTCALL LoadAFileHandle(const char *FileName);
extern void ** FASTCALL LoadAFileHandleNative(const char *FileName);
extern void ** FASTCALL LoadAFileHandleFP(FILE *Filefp);
extern LongWord FASTCALL GetAnAuxType(const char *FileName);
extern LongWord FASTCALL GetAnAuxTypeNative(const char *FileName);
extern LongWord FASTCALL GetAFileType(const char *FileName);
extern LongWord FASTCALL GetAFileTypeNative(const char *FileName);
extern void FASTCALL SetAnAuxType(const char *FileName,LongWord AuxType);
extern void FASTCALL SetAnAuxTypeNative(const char *FileName,LongWord AuxType);
extern void FASTCALL SetAFileType(const char *FileName,LongWord FileType);
extern void FASTCALL SetAFileTypeNative(const char *FileName,LongWord FileType);
extern void FASTCALL FileSetFileAndAuxType(const char *FileName,LongWord FileType,LongWord AuxType);
extern void FASTCALL FileSetFileAndAuxTypeNative(const char *FileName,LongWord FileType,LongWord AuxType);
extern Word FASTCALL AreLongFilenamesAllowed(void);

#if defined(__LITTLEENDIAN__)
#define fwritelongb(x,y) fwritelongrev(x,y)
#define fwritelongl(x,y) fwritelong(x,y)
#define fwriteshortb(x,y) fwriteshortrev(x,y)
#define fwriteshortl(x,y) fwriteshort(x,y)
#define fgetlongb(x) fgetlongrev(x)
#define fgetlongl(x) fgetlong(x)
#define fgetshortb(x) fgetshortrev(x)
#define fgetshortl(x) fgetshort(x)
#else
#define fwritelongb(x,y) fwritelong(x,y)
#define fwritelongl(x,y) fwritelongrev(x,y)
#define fwriteshortb(x,y) fwriteshort(x,y)
#define fwriteshortl(x,y) fwriteshortrev(x,y)
#define fgetlongb(x) fgetlong(x)
#define fgetlongl(x) fgetlongrev(x)
#define fgetshortb(x) fgetshort(x)
#define fgetshortl(x) fgetshortrev(x)
#endif

/* Prefs manager */

typedef enum {PREFREAD,PREFWRITE,PREFDEFAULT} PrefState_e;

typedef char * (FASTCALL *PrefsRecordProc_t)(char *,struct PrefsRecord_t *,PrefState_e);	/* Callback proc for readers */

typedef struct IniIndex_t {	/* Win95 style prefs file image header struct */
	const char *Header;		/* Header to scan for image data */
	const char *ImagePtr;	/* Pointer to the image in memory */
	LongWord ImageLength;	/* Length of the image in memory */
} IniIndex_t;

typedef struct PrefsRecord_t {	/* A single search record */
	char *EntryName;	/* Ascii of entry */
	PrefsRecordProc_t Proc;	/* Callback to process the data */
	void *DataPtr;		/* Pointer to data to store */
	void *Default;		/* Default data pointer (Or possible value) */
	Word Count;			/* Size in elements of buffer */
} PrefsRecord_t;

typedef struct PrefsTemplate_t {	/* Prefs file description record */
	char *Header;		/* Ascii for the header */
	Word Count;			/* Number of entries to scan */
	PrefsRecord_t *ArrayPtr;	/* Array of "Count" entries */
} PrefsTemplate_t;

extern char NibbleToAscii[16];		/* 0-F in upper case */
extern char NibbleToAsciiLC[16];	/* 0-f in lower case */
#define LongWordToAscii(Input,AsciiPtr) LongWordToAscii2(Input,AsciiPtr,0)
extern char * FASTCALL LongWordToAscii2(LongWord Input,char *AsciiPtr,Word Printing);
#define longToAscii(Input,AsciiPtr) longToAscii2(Input,AsciiPtr,0)
extern char * FASTCALL longToAscii2(long Input,char *AsciiPtr,Word Printing);
#define AsciiToLongWord(AsciiPtr) AsciiToLongWord2(AsciiPtr,0)
extern LongWord FASTCALL AsciiToLongWord2(const char *AsciiPtr,char **DestPtr);
#define LongWordToAsciiHex(Input,AsciiPtr) LongWordToAsciiHex2(Input,AsciiPtr,0)
extern char * FASTCALL LongWordToAsciiHex2(LongWord Input,char *AsciiPtr,Word Printing);
extern char * FASTCALL ParseBeyondEOL(const char *Input);
extern char * FASTCALL ParseBeyondWhiteSpace(const char *Input);
extern char * FASTCALL StoreAString(char *WorkPtr,const char *Input);
extern char * FASTCALL StoreALongWordAscii(char *WorkPtr,LongWord Input);
extern char * FASTCALL StoreALongWordAsciiHex(char *WorkPtr,LongWord Input);
extern char * FASTCALL StoreAlongAscii(char *WorkPtr,long Input);
extern char * FASTCALL StoreAParsedString(char *WorkPtr,const char *Input);
extern char * FASTCALL GetAParsedString(const char *WorkPtr,char *DestPtr,Word Size);
#define PrefsWordProc PrefsLongWordProc
#define PrefsWordArrayProc PrefsLongWordArrayProc
extern char * FASTCALL PrefsLongWordProc(char *WorkPtr,PrefsRecord_t *RecordPtr,PrefState_e Pass);
extern char * FASTCALL PrefsShortProc(char *WorkPtr,PrefsRecord_t *RecordPtr,PrefState_e Pass);
extern char * FASTCALL PrefsLongWordArrayProc(char *WorkPtr,PrefsRecord_t *RecordPtr,PrefState_e Pass);
extern char * FASTCALL PrefsShortArrayProc(char *WorkPtr,PrefsRecord_t *RecordPtr,PrefState_e Pass);
extern char * FASTCALL PrefsStringProc(char *WorkPtr,PrefsRecord_t *RecordPtr,PrefState_e Pass);
extern Word FASTCALL ScanIniImage(IniIndex_t *IndexPtr,PrefsRecord_t *Record);
extern LongWord FASTCALL LongWordFromIniImage(const char *Header,const char *EntryName,const char *Input,LongWord InputLength);
extern void * FASTCALL PrefsCreateFileImage(PrefsTemplate_t *MyTemplate,LongWord *LengthPtr);
extern Word FASTCALL PrefsWriteFile(PrefsTemplate_t *MyTemplate,const char *FileName,Word AppendFlag);
extern void FASTCALL PrefsParseFile(PrefsTemplate_t *MyTemplate,const char *FilePtr,LongWord Length);
extern Word FASTCALL PrefsReadFile(PrefsTemplate_t *MyTemplate,const char *FileName);

/* Pref File Manager */

typedef struct PrefFile_t PrefFile_t;
typedef struct PrefFileSection_t PrefFileSection_t;
typedef struct PrefFileEntry_t PrefFileEntry_t;

extern PrefFile_t * FASTCALL PrefFileNew(void);
extern PrefFile_t * FASTCALL PrefFileNewFromMemory(const char *Data,LongWord Length);
extern PrefFile_t * FASTCALL PrefFileNewFromFile(const char *FileName);
extern PrefFile_t * FASTCALL PrefFileNewFromFileAlways(const char *FileName);
extern void FASTCALL PrefFileDelete(PrefFile_t *Input);
extern Word FASTCALL PrefFileSaveFile(PrefFile_t *Input,const char *FileName);
extern PrefFileSection_t * FASTCALL PrefFileFindSection(PrefFile_t *Input,const char *SectionName);
extern PrefFileSection_t * FASTCALL PrefFileFindSectionAlways(PrefFile_t *Input,const char *SectionName);
extern char *FASTCALL PrefFileGetList(PrefFile_t *Input);
extern PrefFileEntry_t * FASTCALL PrefFileSectionFindEntry(PrefFileSection_t *Input,const char *EntryName);
extern void FASTCALL PrefFileDeleteSection(PrefFile_t *Input,const char *SectionName);
extern void FASTCALL PrefFileDeletePrefFileSection(PrefFile_t *Input,PrefFileSection_t *SectionPtr);
extern PrefFileSection_t * FASTCALL PrefFileAddSection(PrefFile_t *Input,const char *SectionName);
extern Word FASTCALL PrefFileIsEntryPresent(PrefFile_t *Input,const char *SectionName,const char *EntryName);
extern char *FASTCALL PrefFileSectionGetList(PrefFileSection_t *Input);
extern char *FASTCALL PrefFileSectionGetRaw(PrefFileSection_t *Input,const char *EntryName);
extern Word FASTCALL PrefFileSectionGetBoolean(PrefFileSection_t *Input,const char *EntryName,Word Default);
extern Word FASTCALL PrefFileSectionGetWord(PrefFileSection_t *Input,const char *EntryName,Word Default,Word Min,Word Max);
extern int FASTCALL PrefFileSectionGetInt(PrefFileSection_t *Input,const char *EntryName,int Default,int Min,int Max);
extern float FASTCALL PrefFileSectionGetFloat(PrefFileSection_t *Input,const char *EntryName,float Default,float Min,float Max);
extern double FASTCALL PrefFileSectionGetDouble(PrefFileSection_t *Input,const char *EntryName,double Default,double Min,double Max);
extern void FASTCALL PrefFileSectionGetString(PrefFileSection_t *Input,const char *EntryName,const char *Default,char *Buffer,Word BufferSize);
extern void FASTCALL PrefFileSectionGetDualString(PrefFileSection_t *Input,const char *EntryName,const char *Default,char *Buffer,Word BufferSize,const char *Default2,char *Buffer2,Word BufferSize2);
extern void FASTCALL PrefFileSectionGetMem(PrefFileSection_t *Input,const char *EntryName,const Byte *Default,Byte *Buffer,Word BufferSize);
extern void FASTCALL PrefFileSectionGetWordArray(PrefFileSection_t *Input,const char *EntryName,const Word *Default,Word *Buffer,Word Count);
extern void FASTCALL PrefFileSectionAddEntry(PrefFileSection_t *Input,const char *EntryName,const char *Default);
extern void FASTCALL PrefFileSectionPutRaw(PrefFileSection_t *Input,const char *EntryName,const char *RawString);
extern void FASTCALL PrefFileSectionPutBoolean(PrefFileSection_t *Input,const char *EntryName,Word Data);
extern void FASTCALL PrefFileSectionPutWord(PrefFileSection_t *Input,const char *EntryName,Word Data);
extern void FASTCALL PrefFileSectionPutWordHex(PrefFileSection_t *Input,const char *EntryName,Word Data);
extern void FASTCALL PrefFileSectionPutInt(PrefFileSection_t *Input,const char *EntryName,int Data);
extern void FASTCALL PrefFileSectionPutFloat(PrefFileSection_t *Input,const char *EntryName,float Data);
extern void FASTCALL PrefFileSectionPutDouble(PrefFileSection_t *Input,const char *EntryName,double Data);
extern void FASTCALL PrefFileSectionPutString(PrefFileSection_t *Input,const char *EntryName,const char *Data);
extern void FASTCALL PrefFileSectionPutDualString(PrefFileSection_t *Input,const char *EntryName,const char *Data,const char *Data2);
extern void FASTCALL PrefFileSectionPutMem(PrefFileSection_t *Input,const char *EntryName,const Byte *Data,Word Length);
extern void FASTCALL PrefFileSectionPutWordArray(PrefFileSection_t *Input,const char *EntryName,const Word *DataPtr,Word Count);

/* Memory streaming routines */

typedef struct StreamHandle_t {
	Byte **DataHandle;	/* Handle of the buffer for streaming */
	LongWord Mark;		/* Current file mark */
	LongWord BufferSize;	/* Maximum memory in buffer */
	LongWord EOFMark;	/* Size of the valid data */
	Word ErrorFlag;		/* Set to TRUE if an error occured */
} StreamHandle_t;

extern StreamHandle_t * FASTCALL StreamHandleNewPut(void);
extern StreamHandle_t * FASTCALL StreamHandleNewGet(void **GetHandle);
extern StreamHandle_t * FASTCALL StreamHandleNewGetPtr(void *GetPtr,LongWord Size);
extern StreamHandle_t * FASTCALL StreamHandleNewGetFile(const char *FileName);
extern void FASTCALL StreamHandleInitPut(StreamHandle_t *Input);
extern void FASTCALL StreamHandleInitGet(StreamHandle_t *Input,void **GetHandle);
extern void FASTCALL StreamHandleInitGetPtr(StreamHandle_t *Input,void *GetPtr,LongWord Size);
extern Word FASTCALL StreamHandleInitGetFile(StreamHandle_t *Input,const char *FileName);
extern void FASTCALL StreamHandleDestroy(StreamHandle_t *Input);
extern void FASTCALL StreamHandleDelete(StreamHandle_t *Input);
extern void ** FASTCALL StreamHandleDetachHandle(StreamHandle_t *Input);
extern void FASTCALL StreamHandleEndSave(StreamHandle_t *Input);
extern Word FASTCALL StreamHandleSaveFile(StreamHandle_t *Input,const char *FileName);
extern Word FASTCALL StreamHandleSaveTextFile(StreamHandle_t *Input,const char *FileName);
extern Word FASTCALL StreamHandleGetByte(StreamHandle_t *Input);
extern Word FASTCALL StreamHandleGetShort(StreamHandle_t *Input);
extern LongWord FASTCALL StreamHandleGetLong(StreamHandle_t *Input);
extern float FASTCALL StreamHandleGetFloat(StreamHandle_t *Input);
extern double FASTCALL StreamHandleGetDouble(StreamHandle_t *Input);
extern void FASTCALL StreamHandleGetVector2D(StreamHandle_t *Input,struct Vector2D_t *Output);
extern void FASTCALL StreamHandleGetVector3D(StreamHandle_t *Input,struct Vector3D_t *Output);
extern void FASTCALL StreamHandleGetMatrix3D(StreamHandle_t *Input,struct Matrix3D_t *Output);
extern void FASTCALL StreamHandleGetEuler(StreamHandle_t *Input,struct Euler_t *Output);
extern void FASTCALL StreamHandleGetLWRect(StreamHandle_t *Input,struct LWRect_t *Output);
extern void FASTCALL StreamHandleGetLWPoint(StreamHandle_t *Input,struct LWPoint_t *Output);
extern Word FASTCALL StreamHandleGetShortMoto(StreamHandle_t *Input);
extern LongWord FASTCALL StreamHandleGetLongMoto(StreamHandle_t *Input);
extern float FASTCALL StreamHandleGetFloatMoto(StreamHandle_t *Input);
extern double FASTCALL StreamHandleGetDoubleMoto(StreamHandle_t *Input);
extern void FASTCALL StreamHandleGetVector2DMoto(StreamHandle_t *Input,struct Vector2D_t *Output);
extern void FASTCALL StreamHandleGetVector3DMoto(StreamHandle_t *Input,struct Vector3D_t *Output);
extern void FASTCALL StreamHandleGetMatrix3DMoto(StreamHandle_t *Input,struct Matrix3D_t *Output);
extern void FASTCALL StreamHandleGetEulerMoto(StreamHandle_t *Input,struct Euler_t *Output);
extern void FASTCALL StreamHandleGetMem(StreamHandle_t *Input,void *DestPtr,LongWord Length);
extern void * FASTCALL StreamHandleGetString(StreamHandle_t *Input,Word Flags);
extern void FASTCALL StreamHandleGetString2(StreamHandle_t *Input,Word Flags,char *DestBuffer,LongWord MaxLength);
extern void FASTCALL StreamHandlePutByte(StreamHandle_t *Input,Word Val);
extern void FASTCALL StreamHandlePutShort(StreamHandle_t *Input,Word Val);
extern void FASTCALL StreamHandlePutLong(StreamHandle_t *Input,LongWord Val);
extern void FASTCALL StreamHandlePutFloat(StreamHandle_t *Input,float Val);
extern void FASTCALL StreamHandlePutDouble(StreamHandle_t *Input,double Val);
extern void FASTCALL StreamHandlePutVector2D(StreamHandle_t *Input,const struct Vector2D_t *Val);
extern void FASTCALL StreamHandlePutVector3D(StreamHandle_t *Input,const struct Vector3D_t *Val);
extern void FASTCALL StreamHandlePutMatrix3D(StreamHandle_t *Input,const struct Matrix3D_t *Val);
extern void FASTCALL StreamHandlePutEuler(StreamHandle_t *Input,const struct Euler_t *Val);
extern void FASTCALL StreamHandlePutShortMoto(StreamHandle_t *Input,Word Val);
extern void FASTCALL StreamHandlePutLongMoto(StreamHandle_t *Input,LongWord Val);
extern void FASTCALL StreamHandlePutFloatMoto(StreamHandle_t *Input,float Val);
extern void FASTCALL StreamHandlePutDoubleMoto(StreamHandle_t *Input,double Val);
extern void FASTCALL StreamHandlePutVector2DMoto(StreamHandle_t *Input,const struct Vector2D_t *Val);
extern void FASTCALL StreamHandlePutVector3DMoto(StreamHandle_t *Input,const struct Vector3D_t *Val);
extern void FASTCALL StreamHandlePutMatrix3DMoto(StreamHandle_t *Input,const struct Matrix3D_t *Val);
extern void FASTCALL StreamHandlePutEulerMoto(StreamHandle_t *Input,const struct Euler_t *Val);
extern void FASTCALL StreamHandlePutMem(StreamHandle_t *Input,const void *SrcPtr,LongWord Length);
extern void FASTCALL StreamHandlePutString(StreamHandle_t *Input,const void *SrcPtr);
extern void FASTCALL StreamHandlePutStringNoZero(StreamHandle_t *Input,const void *SrcPtr);
#define StreamHandleGetMark(x) (x)->Mark
extern void FASTCALL StreamHandleSetMark(StreamHandle_t *Input,LongWord NewMark);
#define StreamHandleGetSize(x) (x)->EOFMark
extern void FASTCALL StreamHandleSetSize(StreamHandle_t *Input,LongWord Size);
#define StreamHandleGetErrorFlag(x) (x)->ErrorFlag
#define StreamHandleClearErrorFlag(x) (x)->ErrorFlag=FALSE
#define StreamHandleSetErrorFlag(x) (x)->ErrorFlag=TRUE
extern void FASTCALL StreamHandleSkip(StreamHandle_t *Input,long Offset);
extern void FASTCALL StreamHandleSkipString(StreamHandle_t *Input);
extern void * FASTCALL StreamHandleLock(StreamHandle_t *Input);
extern void * FASTCALL StreamHandleLockExpand(StreamHandle_t *Input,LongWord Size);
extern void FASTCALL StreamHandleUnlock(StreamHandle_t *Input,const void *EndPtr);
extern void FASTCALL StreamHandleExpand(StreamHandle_t *Input,LongWord Size);

/* "C" convenience routines */

#define SWITCH_NORMAL 0x0
#define SWITCH_CALLBACK 0x1
#define SWITCH_WORD 0x2

#define ASCIILEADINGZEROS 0x8000U
#define ASCIINONULL 0x4000U

#define EXTRACTSTRDELIMITLF 0x01
#define EXTRACTSTRNOTRANSLATE 0x02
#define EXTRACTSTRHANDLE 0x04

typedef void (FASTCALL *SwitchCallBackProc)(char *Input);

typedef struct Switch_t {	/* Used by Switches */
	char *StrPtr;	/* Ascii to check */
	LongWord Value;	/* Preset value to use (Or parm count) */
	void *ValuePtr;	/* Pointer to LongWord or function callback */
	Word Flags;		/* Flags for handler */
} Switch_t;

typedef struct MD2_t {	/* MD2 context */
	Byte state[16];		/* state */
	Byte checksum[16];	/* checksum */
	Byte buffer[16];	/* input buffer */
	Word count;			/* number of bytes, modulo 16 */
} MD2_t;

typedef struct MD4_t {	/* MD4 context. */
	LongWord state[4];	/* Current 128 bit value */
	LongWord countlo;	/* Number of bytes processed (64 bit value) */
	LongWord counthi;
	Byte buffer[64];	/* input buffer for processing */
} MD4_t;

typedef struct MD5_t {	/* MD5 context. */
	LongWord state[4];	/* Current 128 bit value */
	LongWord countlo;	/* Number of bytes processed (64 bit value) */
	LongWord counthi;
	Byte buffer[64];	/* input buffer for processing */
} MD5_t;

extern Byte ReverseBits[256];		/* Table to reverse bits in a byte */
extern Word FASTCALL Switches(Word argc,char *argv[],const Switch_t *SwitchList);
extern char * FASTCALL midstr(char *Dest,const char *Source,Word Start,Word Length);
extern char * FASTCALL stristr(const char *Input, const char *SubStr);
extern void FASTCALL CStr2PStr(char *DestPtr,const char *SrcPtr);
extern void FASTCALL PStr2CStr(char *DestPtr,const char *SrcPtr);
extern LongWord FASTCALL BCDToNum(LongWord Input);
extern LongWord FASTCALL NumToBCD(LongWord Input);
extern LongWord FASTCALL PowerOf2(LongWord Input);
extern LongWord FASTCALL CalcMoreCRC32(const Byte *buffPtr,LongWord buffSize,LongWord crc);
#define CalcCRC32(buffPtr,buffSize) CalcMoreCRC32(buffPtr,buffSize,0)
extern LongWord FASTCALL CalcMoreAdler(const Byte *Buffer,LongWord Length,LongWord crc);
#define CalcAdler(buffPtr,buffSize) CalcMoreAdler(buffPtr,buffSize,1)
extern Word FASTCALL CalcMoreAdler16(const Byte *Buffer,LongWord Length,Word CheckSum);
#define CalcAdler16(buffPtr,buffSize) CalcMoreAdler16(buffPtr,buffSize,1)
extern void FASTCALL MD2Init(MD2_t *Input);
extern void FASTCALL MD2Update(MD2_t *Input,const Byte *BufferPtr,LongWord Length);
extern void FASTCALL MD2Final(Byte *Output,MD2_t *Input);
extern void FASTCALL MD2Quick(Byte *Output,const Byte *BufferPtr,LongWord Length);
extern void FASTCALL MD4Init(MD4_t *Input);
extern void FASTCALL MD4Update(MD4_t *Input,const Byte *BufferPtr,LongWord Length);
extern void FASTCALL MD4Final(Byte *Output,MD4_t *Input);
extern void FASTCALL MD4Quick(Byte *Output,const Byte *BufferPtr,LongWord Length);
extern void FASTCALL MD5Init(MD5_t *Input);
extern void FASTCALL MD5Update(MD5_t *Input,const Byte *BufferPtr,LongWord Length);
extern void FASTCALL MD5Final(Byte *Output,MD5_t *Input);
extern void FASTCALL MD5Quick(Byte *Output,const Byte *BufferPtr,LongWord Length);
extern void * FASTCALL ExtractAString(const char *SrcPtr,LongWord *BufSize,Word Flags);
extern void FASTCALL ExtractAString2(const char *SrcPtr,LongWord *BufSize,Word Flags,char *DestPtr,LongWord DestSize);
extern void FASTCALL FastMemCpy(void *DestPtr,const void *SrcPtr,LongWord Length);
#define FastMemCpyAlign(Dest,Src,Length) memcpy(Dest,Src,Length)
extern void FASTCALL FastMemSet(void *DestPtr,Word Fill,LongWord Length);
extern void FASTCALL FastMemSet16(void *DestPtr,Word Fill,LongWord Length);
extern Word FASTCALL FastStrLen(const char *Input);
extern int FASTCALL FastStrncmp(const char *Input1,const char *Input2,Word MaxLength);

#if defined(__MWERKS__) || defined(__MRC__) || defined(__BEOS__)
extern int FASTCALL memicmp(const char *StrPtr1,const char *StrPtr2,Word Length);
#if !defined(_MSL_NEEDS_EXTRAS)
extern char * FASTCALL strupr(char *Source);
extern char * FASTCALL strlwr(char *Source);
extern char * FASTCALL strdup(const char *source);
extern int FASTCALL stricmp(const char *StrPtr1,const char *StrPtr2);
extern int FASTCALL strnicmp(const char *StrPtr1,const char *StrPtr2,Word Len);
#endif
#endif

/* The point of DebugString is that it goes away in release code */

extern void FASTCALL DebugXString(const char *String);
extern void FASTCALL DebugXshort(short i);
extern void FASTCALL DebugXShort(Short i);
extern void FASTCALL DebugXlong(long i);
extern void FASTCALL DebugXLongWord(LongWord i);
extern void FASTCALL DebugXDouble(double i);
extern void FASTCALL DebugXPointer(const void *i);
extern void ANSICALL DebugXMessage(const char *String,...);
#define DebugXWord(x) DebugXLongWord(x)

#if _DEBUG			/* Allow in debug code */
#define DebugString(x) DebugXString(x)
#define Debugshort(x) DebugXshort(x)
#define DebugShort(x) DebugXShort(x)
#define Debuglong(x) DebugXlong(x)
#define DebugLongWord(x) DebugXLongWord(x)
#define DebugDouble(x) DebugXDouble(x)
#define DebugPointer(x) DebugXPointer(x)
#define DebugWord(x) DebugXLongWord((LongWord)x)
#define DebugMessage DebugXMessage
#else
#define DebugString(x)		/* Remove from release code */
#define Debugshort(x)
#define DebugShort(x)
#define Debuglong(x)
#define DebugLongWord(x)
#define DebugDouble(x)
#define DebugPointer(x)
#define DebugMessage 1 ? (void)0 : DebugXMessage
#define DebugWord(x)
#endif

#define DEBUGTRACE_MEMORYLEAKS 1 /* Test and display memory leaks */
#define DEBUGTRACE_REZLOAD 2	/* Print the name of a resource file being loaded */
#define DEBUGTRACE_FILELOAD 4	/* Print the name of a file being loaded */
#define DEBUGTRACE_WARNINGS 8	/* Print possible errors */
#define DEBUGTRACE_NETWORK 0x10	/* Network commands */
#define DEBUGTRACE_THEWORKS 0x1F	/* GIMME everything! */

typedef Word (FASTCALL *SystemProcessCallBackProc)(const char *Input);

extern Word DebugTraceFlag;	/* Set these flag for debug spew */
extern Word BombFlag;		/* If true then bomb on ANY error */
extern Word IAmExiting;		/* TRUE if in a shutdown state */
extern char ErrorMsg[512];		/* Last message printed */
extern void ANSICALL Fatal(const char *FatalMsg,...);
extern void ANSICALL NonFatal(const char *ErrorMsg,...);
extern Word FASTCALL SetErrBombFlag(Word Flag);
extern void FASTCALL Halt(void);
extern void FASTCALL SaveJunk(const void *Data,LongWord Length);
extern void FASTCALL OkAlertMessage(const char *Title,const char *Message);
extern Word FASTCALL OkCancelAlertMessage(const char *Title,const char *Message);
extern Word FASTCALL SystemProcessFilenames(SystemProcessCallBackProc Proc);
extern Word FASTCALL GetQuickTimeVersion(void);
extern Word FASTCALL LaunchURL(const char *URLPtr);
extern Word FASTCALL BurgerlibVersion(void);

typedef struct LibRef_t LibRef_t;
extern LibRef_t * FASTCALL LibRefInit(const char *LibName);
extern void FASTCALL LibRefDelete(LibRef_t *LibRef);
extern void * FASTCALL LibRefGetProc(LibRef_t *LibRef,const char *ProcName);
extern void *FASTCALL LibRefGetFunctionInLib(const char *LibName,const char *ProcName);

extern void FASTCALL PrintHexDigit(Word Val);
extern void FASTCALL PrintHexByte(Word Val);
extern void FASTCALL PrintHexShort(Word Val);
extern void FASTCALL PrintHexLongWord(LongWord Val);

#if defined(__68K__)
Short SwapUShort(Short:__D0):__D0 = {0xE058};	/* ROR.W #8,D0 */
short SwapShort(short:__D0):__D0 = {0xE058};	/* ROR.W #8,D0 */
LongWord SwapULong(LongWord:__D0):__D0 = {0xE058,0x4840,0xE058};	/* ROR.W #8,D0,SWAP D0,ROR.W #8,D0 */
long SwapLong(long:__D0):__D0 = {0xE058,0x4840,0xE058};	/* ROR.W #8,D0,SWAP D0,ROR.W #8,D0 */
#else			/* All others */
extern Short FASTCALL SwapUShort(Short Val);
extern short FASTCALL SwapShort(short Val);
extern LongWord FASTCALL SwapULong(LongWord Val);
extern long FASTCALL SwapLong(long Val);
#endif
extern double FASTCALL SwapDouble(const double *ValPtr);

#if defined(__WATCOMC__)
#pragma aux SwapLong = "BSWAP EAX" parm [eax] value [eax]
#pragma aux SwapULong = "BSWAP EAX" parm [eax] value [eax]
#pragma aux SwapShort = "ROR AX,8" parm [ax] value [ax]
#pragma aux SwapUShort = "ROR AX,8" parm [ax] value [ax]
#pragma aux Halt = "INT 3" modify exact
#endif

#if defined(__INTEL__)
#define FastMemCpy(x,y,z) memcpy(x,y,z)
#endif

#if 0
#undef strlen			/* For some compilers, this is intrinsic */
#define strlen(x) FastStrLen(x)
#endif

#ifdef __POWERPC__
#define strncmp(x,y,z) FastStrncmp(x,y,z)
#endif

/* Intel convience routines */

#if defined(__INTEL__)

typedef enum {
	IntelFP24=0,
	IntelFP56=2,
	IntelFP64=3
} IntelFP_e;

extern IntelFP_e FASTCALL IntelSetFPPrecision(IntelFP_e Input);

#endif

/* String Handlers */

typedef struct LWString_t {
	char *DataPtr;			/* Pointer to actual string */
	Word StrLength;			/* Length of the string */
	Word BufferLength;		/* Length of the string buffer */
	char Bogus;				/* Bogus string for no memory (Failsafe) */
	char Pad1,Pad2,Pad3;	/* Make sure I am long word aligned */
} LWString_t;

extern void FASTCALL StrStripLeadingSpaces(char* Input);
extern void FASTCALL StrStripLeadingWhiteSpace(char* Input);
extern void FASTCALL StrStripTrailingSpaces(char* Input);
extern void FASTCALL StrStripTrailingWhiteSpace(char* Input);
extern void FASTCALL StrStripLeadingAndTrailingSpaces(char* Input);
extern void FASTCALL StrStripLeadingAndTrailingWhiteSpace(char* Input);
extern void FASTCALL StrStripAllBut(char* Input,const char* ListPtr);
extern void FASTCALL StrStripAll(char* Input,const char* ListPtr);
extern void FASTCALL StrStripTrailing(char* Input, const char* ListPtr);
extern void FASTCALL StrStripLeading(char* Input, const char* ListPtr);
extern char * FASTCALL StrParseToDelimiter(const char *Input);
extern void FASTCALL StrParse(struct LinkedList_t *ListPtr,const char *Input);
extern void FASTCALL StrGetComputerName(char* Output,Word OutputSize);
extern void FASTCALL StrGetUserName(char* Output,Word OutputSize);
extern char* FASTCALL StrGetFileExtension(const char *Input);
extern void FASTCALL StrSetFileExtension(char* Input,const char* NewExtension);
extern char* FASTCALL StrCopy(const char *Input);
extern char* FASTCALL StrCopyPad(const char *Input,Word Padding);
extern void ** FASTCALL StrCopyHandle(const char *Input);
extern void ** FASTCALL StrCopyPadHandle(const char *Input,Word Padding);
extern char* FASTCALL DebugStrCopy(const char *Input,const char *File,Word Line);
extern char* FASTCALL DebugStrCopyPad(const char *Input,Word Padding,const char *File,Word Line);
extern void ** FASTCALL DebugStrCopyHandle(const char *Input,const char *File,Word Line);
extern void ** FASTCALL DebugStrCopyPadHandle(const char *Input,Word Padding,const char *File,Word Line);

#if _DEBUG
#define StrCopy(x) DebugStrCopy(x,__FILE__,__LINE__)
#define StrCopyPad(x,y) DebugStrCopyPad(x,y,__FILE__,__LINE__)
#define StrCopyHandle(x) DebugStrCopyHandle(x,__FILE__,__LINE__)
#define StrCopyPadHandle(x,y) DebugStrCopyPadHandle(x,y,__FILE__,__LINE__)
#endif

/* Linked list handler */

struct LinkedListEntry_t;
typedef void (FASTCALL *LinkedListDeleteProcPtr)(struct LinkedListEntry_t *);
typedef Word (FASTCALL *LinkedListTraverseProcPtr)(void *);

typedef struct LinkedListEntry_t {
	struct LinkedListEntry_t *Next;		/* Pointer to the next entry */
	struct LinkedListEntry_t *Prev;		/* Pointer to the previous entry */
	void *Data;							/* Pointer to the data */
	LinkedListDeleteProcPtr KillProc;	/* Destructor */
} LinkedListEntry_t;

typedef struct LinkedList_t {
	LinkedListEntry_t *First;			/* First entry in the list */
	LinkedListEntry_t *Last;			/* Last entry in the list */
	Word Count;							/* Number of entries in the list */
} LinkedList_t;

#define LINKLIST_ABORT 1
#define LINKLIST_DELETE 2

typedef int (FASTCALL *LinkedListSortProc)(const void *,const void *);

extern void FASTCALL LinkedListEntryDeallocProc(LinkedListEntry_t *Input);
extern void FASTCALL LinkedListEntryDeallocNull(LinkedListEntry_t *Input);
extern void FASTCALL LinkedListInit(LinkedList_t *Input);
extern void FASTCALL LinkedListDestroy(LinkedList_t *Input);
extern LinkedList_t * FASTCALL LinkedListNew(void);
extern void FASTCALL LinkedListDelete(LinkedList_t *Input);
extern void FASTCALL LinkedListDeleteFirstEntry(LinkedList_t *Input);
extern void FASTCALL LinkedListDeleteLastEntry(LinkedList_t *Input);
extern void FASTCALL LinkedListDeleteEntryByData(LinkedList_t *Input,const void *DataPtr);
extern void FASTCALL LinkedListDeleteEntry(LinkedList_t *Input,LinkedListEntry_t *EntryPtr);
extern void FASTCALL LinkedListRemoveEntry(LinkedList_t *Input,LinkedListEntry_t *EntryPtr);
extern Word FASTCALL LinkedListContains(const LinkedList_t *Input,const void *Data);
extern LinkedListEntry_t * FASTCALL LinkedListGetEntry(const LinkedList_t *Input,Word EntryNum);
extern LinkedListEntry_t * FASTCALL LinkedListGetEntryByData(const LinkedList_t *Input,const void *DataPtr);
extern void * FASTCALL LinkedListGetEntryData(const LinkedList_t *Input,Word EntryNum);
extern Word FASTCALL LinkedListFindString(const LinkedList_t *Input,const char *TextPtr);
extern LinkedListEntry_t * FASTCALL LinkedListFindStringEntry(const LinkedList_t *Input,const char *TextPtr);
extern LinkedListEntry_t *FASTCALL LinkedListTraverseForward(LinkedList_t *Input,LinkedListTraverseProcPtr Proc);
extern LinkedListEntry_t *FASTCALL LinkedListTraverseBackward(LinkedList_t *Input,LinkedListTraverseProcPtr Proc);
extern void FASTCALL LinkedListSort(LinkedList_t *Input,LinkedListSortProc Proc);
extern void FASTCALL LinkedListAddEntryEnd(LinkedList_t *Input,LinkedListEntry_t *EntryPtr);
extern void FASTCALL LinkedListAddEntryBegin(LinkedList_t *Input,LinkedListEntry_t *EntryPtr);
extern void FASTCALL LinkedListAddEntryAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,LinkedListEntry_t *NewPtr);
extern void FASTCALL LinkedListAddEntryBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,LinkedListEntry_t *NewPtr);
extern void FASTCALL LinkedListAddNewEntryEnd(LinkedList_t *Input,void *Data);
extern void FASTCALL LinkedListAddNewEntryBegin(LinkedList_t *Input,void *Data);
extern void FASTCALL LinkedListAddNewEntryAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data);
extern void FASTCALL LinkedListAddNewEntryBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data);
extern void FASTCALL LinkedListAddNewEntryProcEnd(LinkedList_t *Input,void *Data,LinkedListDeleteProcPtr Kill);
extern void FASTCALL LinkedListAddNewEntryProcBegin(LinkedList_t *Input,void *Data,LinkedListDeleteProcPtr Kill);
extern void FASTCALL LinkedListAddNewEntryProcAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,LinkedListDeleteProcPtr Kill);
extern void FASTCALL LinkedListAddNewEntryProcBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,LinkedListDeleteProcPtr Kill);
extern void FASTCALL LinkedListAddNewEntryMemEnd(LinkedList_t *Input,void *Data);
extern void FASTCALL LinkedListAddNewEntryMemBegin(LinkedList_t *Input,void *Data);
extern void FASTCALL LinkedListAddNewEntryMemAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data);
extern void FASTCALL LinkedListAddNewEntryMemBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data);
extern void FASTCALL LinkedListAddNewEntryStringEnd(LinkedList_t *Input,const char *Data);
extern void FASTCALL LinkedListAddNewEntryStringBegin(LinkedList_t *Input,const char *Data);
extern void FASTCALL LinkedListAddNewEntryStringAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,const char *Data);
extern void FASTCALL LinkedListAddNewEntryStringBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,const char *Data);
extern void FASTCALL DebugLinkedListAddNewEntryEnd(LinkedList_t *Input,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryBegin(LinkedList_t *Input,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryProcEnd(LinkedList_t *Input,void *Data,LinkedListDeleteProcPtr Kill,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryProcBegin(LinkedList_t *Input,void *Data,LinkedListDeleteProcPtr Kill,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryProcAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,LinkedListDeleteProcPtr Kill,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryProcBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,LinkedListDeleteProcPtr Kill,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryMemEnd(LinkedList_t *Input,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryMemBegin(LinkedList_t *Input,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryMemAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryMemBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,void *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryStringEnd(LinkedList_t *Input,const char *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryStringBegin(LinkedList_t *Input,const char *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryStringAfter(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,const char *Data,const char *File,Word Line);
extern void FASTCALL DebugLinkedListAddNewEntryStringBefore(LinkedList_t *Input,LinkedListEntry_t *EntryPtr,const char *Data,const char *File,Word Line);

#if _DEBUG
#define LinkedListAddNewEntryEnd(x,y) DebugLinkedListAddNewEntryEnd(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryBegin(x,y) DebugLinkedListAddNewEntryBegin(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryAfter(x,y,z) DebugLinkedListAddNewEntryAfter(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryBefore(x,y,z) DebugLinkedListAddNewEntryBefore(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryProcEnd(x,y,z) DebugLinkedListAddNewEntryProcEnd(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryProcBegin(x,y,z) DebugLinkedListAddNewEntryProcBegin(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryProcAfter(x,y,z,w) DebugLinkedListAddNewEntryProcAfter(x,y,z,w,__FILE__,__LINE__)
#define LinkedListAddNewEntryProcBefore(x,y,z,w) DebugLinkedListAddNewEntryProcBefore(x,y,z,w,__FILE__,__LINE__)
#define LinkedListAddNewEntryMemEnd(x,y) DebugLinkedListAddNewEntryMemEnd(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryMemBegin(x,y) DebugLinkedListAddNewEntryMemBegin(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryMemAfter(x,y,z) DebugLinkedListAddNewEntryMemAfter(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryMemBefore(x,y,z) DebugLinkedListAddNewEntryMemBefore(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryStringEnd(x,y) DebugLinkedListAddNewEntryStringEnd(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryStringBegin(x,y) DebugLinkedListAddNewEntryStringBegin(x,y,__FILE__,__LINE__)
#define LinkedListAddNewEntryStringAfter(x,y,z) DebugLinkedListAddNewEntryStringAfter(x,y,z,__FILE__,__LINE__)
#define LinkedListAddNewEntryStringBefore(x,y,z) DebugLinkedListAddNewEntryStringBefore(x,y,z,__FILE__,__LINE__)
#endif

#define LinkedListGetSize(x) (x)->Count
#define LinkedListGetFirst(x) (x)->First
#define LinkedListGetLast(x) (x)->Last
#define LinkedListGetFirstData(x) (x)->First->Data
#define LinkedListGetLastData(x) (x)->Last->Data

/* List handlers */

typedef struct {
	char* FirstItemTextPtr;	/* Pointer to the string array */
	Word ItemSize;			/* Size of each string element */
	Word NumItems;			/* Number of elements */
} ListFixed_t;

typedef struct {
	char** FirstItemTextArrayPtr;	/* Pointer to the "C" string pointer array */
	Word ItemSize;			/* Size of each element */
	Word NumItems;			/* Number of elements */
} ListStatic_t;

typedef struct {
	void **ArrayHandle;		/* Handle to the array of "C" string pointers */
	Word NumItems;			/* Number of elements */
} ListDynamic_t;

typedef struct {
	int MinVal;				/* Lowest value integer */
	int MaxVal;				/* Highest value integer */
	char WorkString[16];	/* ASCII version of an integer (Used bu GetString()) */
} ListIntRange_t;

typedef struct {
	Fixed MinVal;			/* Lowest value fixed */
	Fixed MaxVal;			/* Highest value fixed */
	Fixed StepVal;			/* Step range */
	char WorkString[32];	/* Ascii version of a fixed point number */
} ListFixedRange_t;

typedef struct {
	float MinVal;			/* Lowest value float */
	float MaxVal;			/* Highest value float */
	float StepVal;			/* Step range */
	char WorkString[32];	/* Ascii version of a floating point number */
} ListFloatRange_t;

extern void FASTCALL ListFixedInit(ListFixed_t *Input,char *FirstItem,Word Size,Word Count);
#define ListFixedDestroy(x)
extern Word FASTCALL ListFixedFind(const ListFixed_t *Input,const char *ItemText);
extern char * FASTCALL ListFixedGetString(const ListFixed_t *Input,Word Index);

extern void FASTCALL ListStaticInit(ListStatic_t *Input,char **FirstItem,Word Size,Word Count);
#define ListStaticDestroy(x)
extern Word FASTCALL ListStaticFind(const ListStatic_t *Input,const char *ItemText);
extern char * FASTCALL ListStaticGetString(const ListStatic_t *Input,Word Index);

extern void FASTCALL ListDynamicInit(ListDynamic_t *Input);
extern Word FASTCALL ListDynamicFind(const ListDynamic_t *Input,const char *ItemText);
extern char * FASTCALL ListDynamicGetString(const ListDynamic_t *Input,Word Index);
extern void FASTCALL ListDynamicDestroy(ListDynamic_t *Input);
extern void FASTCALL ListDynamicAdd(ListDynamic_t *Input,char *ItemText);
extern void FASTCALL ListDynamicRemoveString(ListDynamic_t *Input,const char *ItemText);
extern void FASTCALL ListDynamicRemoveIndex(ListDynamic_t *Input,Word Index);

extern void FASTCALL ListIntRangeInit(ListIntRange_t *Input,int MinVal,int MaxVal);
#define ListIntRangeDestroy(x)
extern Word FASTCALL ListIntRangeFind(const ListIntRange_t *Input,const char *ItemText);
extern char * FASTCALL ListIntRangeGetString(ListIntRange_t *Input,Word Index);

extern void FASTCALL ListFixedRangeInit(ListFixedRange_t *Input,Fixed MinVal,Fixed MaxVal,Fixed Step);
#define ListFixedRangeDestroy(x)
extern Fixed FASTCALL ListFixedRangeFind(const ListFixedRange_t *Input,const char *ItemText);
extern char * FASTCALL ListFixedRangeGetString(ListFixedRange_t *Input,Fixed Index);

extern void FASTCALL ListFloatRangeInit(ListFloatRange_t *Input,float MinVal,float MaxVal,float Step);
#define ListFloatRangeDestroy(x)
extern float FASTCALL ListFloatRangeFind(const ListFloatRange_t *Input,const char *ItemText);
extern char * FASTCALL ListFloatRangeGetString(ListFloatRange_t *Input,float Index);

/* Timed code execution handler */

typedef Word (FASTCALL *RunQueueProc_t)(void **,Word EventNum);	/* Run queue code */
typedef Word (FASTCALL *RunQueuePollCallback_t)(void **);	/* Poll callback */

typedef struct RunQueue_t {
	struct RunQueue_t **NextQueue;		/* Next time event code routine in list */
	struct RunQueue_t **PrevQueue;		/* Previous entry for linked list */
	RunQueueProc_t Proc;	/* Event procedure */
	Word TimeQuantum;		/* Time in ticks before execution */
	Word ElapsedTime;		/* Fractional time in ticks */
	Word Priority;			/* Execution priority */
	Word IDNum;				/* ID used by application (-1 for master) */
} RunQueue_t;

enum {RQTIME,RQINIT,RQDESTROY,RQSAVE,RQLOAD,RQDEBUG,RQUSER};

extern RunQueue_t ** FASTCALL MasterRunQueueNew(void);
extern void FASTCALL MasterRunQueueInit(RunQueue_t **MasterQueueHandle);
extern void FASTCALL MasterRunQueueDelete(RunQueue_t **MasterQueueHandle);
extern void FASTCALL MasterRunQueueDestroy(RunQueue_t **MasterQueueHandle);
extern void FASTCALL MasterRunQueueExecute(RunQueue_t **MasterQueueHandle,Word TimeQuantum);
extern void FASTCALL MasterRunQueueDump(RunQueue_t **MasterQueueHandle);

extern Word FASTCALL RunQueueCallProc(RunQueue_t **RunQueueHandle,Word Event);
extern Word FASTCALL RunQueueDefaultProc(void **RunQueueHandle,Word Event);
extern RunQueue_t ** FASTCALL RunQueueNew(RunQueue_t **MasterQueueHandle,Word MemSize,
	Word TimeQuantum,Word Priority,Word IDNum,RunQueueProc_t Proc);
extern void FASTCALL RunQueueDelete(RunQueue_t **RunQueueHandle);
extern Word FASTCALL RunQueueDeleteProc(void **RunQueueHandle,Word Event);
extern void FASTCALL RunQueueDeleteDefer(RunQueue_t **RunQueueHandle);
extern void FASTCALL RunQueueLink(RunQueue_t **MasterQueueHandle,RunQueue_t **RunQueueHandle,Word Priority);
extern void FASTCALL RunQueueUnlink(RunQueue_t **RunQueueHandle);
extern RunQueue_t ** FASTCALL RunQueueFindIDNum(RunQueue_t **RunQueueHandle,Word IDNum);
extern Word FASTCALL RunQueuePoll(RunQueue_t **MasterQueueHandle,RunQueuePollCallback_t CallBack,Word IDNum);

/* Dialog controls */

typedef enum {BUTTON_OUTSIDE,BUTTON_INSIDE,BUTTON_DOWN,BUTTON_RELEASED,BUTTON_CLICKED} DialogControlAction_e;

typedef void (FASTCALL *DialogControlProc)(struct DialogControl_t *Input);
typedef void (FASTCALL *DialogControlDrawProc)(struct DialogControl_t *Input, int x, int y);
typedef void (FASTCALL *DialogControlEventProc)(struct DialogControl_t *Input,DialogControlAction_e type);
typedef DialogControlAction_e (FASTCALL *DialogControlCheckProc)(struct DialogControl_t *Input, int x,int y,Word buttons,Word Key);

typedef void (FASTCALL *DialogProc)(struct Dialog_t *Input);
typedef void (FASTCALL *DialogDrawProc)(struct Dialog_t *Input, int x, int y);
typedef Boolean (FASTCALL *DialogEventProc)(struct Dialog_t *Input,int x,int y,Word buttons,Word Key);

typedef void (FASTCALL *DialogControlGenericListDrawProc)(struct DialogControlGenericList_t *Input, void *Data, int x, int y, int Width, int Height, Boolean highlighted);

typedef struct DialogControl_t {
	LBRect Bounds;							/* Bounds rect for the control */
	Byte Active;							/* TRUE if an active control */
	Byte Invisible;							/* TRUE if invisible (Not drawn) */
	Byte Focus;		 						/* Flag for if the button was down */
	Byte Inside;							/* Mouse currently inside */
	DialogControlDrawProc Draw;					/* Draw the control */
	DialogControlProc Delete;				/* Delete the control */
	DialogControlEventProc Event;			/* An event occured (Callback) */
	DialogControlCheckProc Check;			/* Check if I hit a hot spot */
	void *RefCon;							/* User data */
	Word HotKey;							/* Ascii for the Hot Key */
	int Id;
} DialogControl_t;

typedef struct DialogControlList_t {
	Word NumButtons;						/* Number of VALID buttons */
	Word MemListSize;						/* Size of buttonlist for memory allocation */
	Word Dormant;							/* True if waiting for a mouse up but no control has focus */
	DialogControl_t *FocusControl;			/* Control that has focus */
	DialogControl_t **ControlList;			/* Pointers to the button list */
} DialogControlList_t;

typedef struct Dialog_t {
	DialogControlList_t MyList;
	LBRect Bounds;
	Byte Invisible;
	Word FillColor;
	Word OutlineColor;
	DialogDrawProc Draw;						/* Draw the control */
	DialogProc Delete;						/* Delete the control */
	DialogEventProc Event;					/* An event occured (Callback) */
} Dialog_t;

typedef struct DialogList_t {
	Word NumDialogs;						/* Number of VALID buttons */
	Dialog_t *FrontDialog;					/* Control that has focus */
	Dialog_t **DialogList;					/* Pointers to the button list */
} DialogList_t;

typedef struct DialogControlButton_t {		/* Simple button */
	DialogControl_t Root;					/* Base class */
	int x,y;								/* Coords to draw the shape */
	struct ScreenShape_t *Art[3];				/* Shape number indexes */
} DialogControlButton_t;

typedef struct DialogControlTextButton_t {	/* Simple text string button */
	DialogControl_t Root;					/* Base class */
	//int x;								/* Coords to draw the string */
	const char *TextPtr;					/* Text string */
	struct FontRef_t *FontPtr;				/* Font to draw with */
	struct FontRef_t *FontPtr2;				/* Highlight Font to draw with */
	struct ScreenShape_t **Art;				/* Shapes to use (Array of 4) */
} DialogControlTextButton_t;

typedef struct DialogControlCheckBox_t {	/* Simple check box */
	DialogControl_t Root;					/* Base class */
	DialogControlEventProc Proc;			/* Pass through so I can handle the check events */
	struct ScreenShape_t **Art;				/* Shapes to use (Array of 4) */
//	int CheckY;								/* Coords to draw the checkbox */
//	int TextX,TextY;						/* Coords to draw the text */
	const char *TextPtr;					/* Text string */
	struct FontRef_t *FontPtr;				/* Font to draw with */
	Word Checked;							/* True if checked */
} DialogControlCheckBox_t;

typedef struct DialogControlSliderBar_t {	/* Slider bar */
	DialogControl_t Root;					/* Base class */
	struct ScreenShape_t **Art;				/* Shapes to use (Array of 5) */
	int BarY;								/* Coords to draw the slider bar */
	int ThumbX,ThumbY;						/* Current position of the thumb */
	Word ThumbAnchor;						/* Offset into the thumb for "Grabbing" */
	Word ThumbMinX;							/* Minimum offset from the left side */
	Word BarWidth;							/* Range of pixels the thumb can move */
	Word Range;								/* Range of the value (0-Range inclusive) */
	Word Value;								/* Current value */
} DialogControlSliderBar_t;

typedef struct DialogControlRepeatButton_t {	/* Simple repeater button */
	DialogControl_t Root;					/* Base class */
	int x,y;								/* Coords to draw the shape */
	struct ScreenShape_t *Art[3];				/* Shape number indexes */
	LongWord TimeMark;						/* Time mark when clicked */
	LongWord RepeatDelay;					/* Time before another click */
} DialogControlRepeatButton_t;

typedef struct DialogControlVScrollSlider_t {	/* Slider bar */
	DialogControl_t Root;					/* Base class */
	struct ScreenShape_t **Art;				/* Shapes to use (Array of 11) */
	int ThumbY;								/* Current position of the thumb */
	Word ThumbSize;							/* Size of the thumb in pixels */
	Word ThumbAnchor;						/* Offset into the thumb for "Grabbing" */
	Word BarHeight;							/* Range of pixels the thumb can move */
	Word Range;								/* Range of the value (0-Range inclusive) */
	Word Step;								/* Motion to step if clicked in a dead region */
	Word Value;								/* Current value */
	LongWord TimeMark;						/* Time mark when clicked */
	LongWord RepeatDelay;					/* Time before another click */
} DialogControlVScrollSlider_t;

typedef struct DialogControlVScroll_t {		/* Simple button */
	DialogControl_t Root;					/* Base class */
	DialogControlList_t MyList;				/* List of controls */
	DialogControlVScrollSlider_t *Slider;	/* Slider for up and down */
	Word ButtonStep;						/* Step for button press */
	Word Value;								/* Current value */
} DialogControlVScroll_t;

typedef struct DialogControlTextBox_t {		/* Simple box of text */
	DialogControl_t Root;					/* Base class */
	struct FontRef_t *FontPtr;				/* Font to use */
	const char *TextPtr;					/* Current text */
	struct FontWidthLists_t *TextDescription;	/* Format info for the text */
	struct ScreenShape_t **Art;				/* Art for the scroll bar */
	DialogControlVScroll_t *Slider;			/* Scroll bar if present */
	Word Value;								/* Top visible Y coordinate */
	Word OutlineColor;						/* Color to draw the outline in */
	Boolean ScrollBarNormalArrowStyle;		/* The scroll bar style to use */
	Boolean AllowSlider;
} DialogControlTextBox_t;

typedef struct DialogControlStaticText_t {		/* Simple box of text */
	DialogControl_t Root;					/* Base class */
	struct FontRef_t *FontPtr;				/* Font to use */
	const char *TextPtr;					/* Current text */
	struct FontWidthLists_t *TextDescription;	/* Format info for the text */
} DialogControlStaticText_t;

typedef struct DialogControlTextList_t {	/* Simple list of text */
	DialogControl_t Root;					/* Base class */
	struct FontRef_t *FontPtr;				/* Font to use */
	struct FontRef_t *FontPtr2;				/* Font to use (highlight) */
	DialogControlVScroll_t *Slider;			/* Scroll bar if present */
	struct ScreenShape_t **Art;				/* Scroll bar art */
	LinkedList_t List;						/* List of text */
	Word ScrollValue;						/* Which entry to display for scrolling */
	Word Value;								/* Which entry is valid */
	Word OutlineColor;						/* Color to draw the outline in */
	Word FillColor;							/* Color to draw background with */
	Word SelColor;							/* Color to draw highlighted row with */
	Boolean ScrollBarNormalArrowStyle;		/* The scroll bar style to use */
} DialogControlTextList_t;

typedef struct DialogControlGenericList_t {	/* Simple list of text */
	DialogControl_t Root;					/* Base class */
	DialogControlVScroll_t *Slider;			/* Scroll bar if present */
	struct ScreenShape_t **Art;				/* Scroll bar art */
	LinkedList_t List;						/* List of text */
	Word ScrollValue;						/* Which entry to display for scrolling */
	Word Value;								/* Which entry is valid */
	Word OutlineColor;						/* Color to draw the outline in */
	Word FillColor;							/* Color to draw background with */
	Word SelColor;							/* Color to draw highlighted row with */
	Word CellHeight;
	Boolean ScrollBarNormalArrowStyle;		/* The scroll bar style to use */
	DialogControlGenericListDrawProc CellDraw;
} DialogControlGenericList_t;

typedef struct DialogControlTextMenu_t {	/* Menu control (Used by pop-up menus) */
	DialogControl_t Root;					/* Base class */
	LinkedList_t *ListPtr;					/* List of text (I don't control the list) */
	struct FontRef_t *FontPtr;				/* Font to use */
	struct FontRef_t *FontPtr2;				/* Font to use (highlight) */
	Word Value;								/* Entry selected for highlight */
	Word CursorValue;						/* Entry the the cursor is over */
	int ScrollValue;						/* Scroll factor */
	Word CellHeight;						/* Number of cells high */
	Word NormColor;							/* Color to fill normally */
	Word CursorColor;						/* Color to fill for cursor highlight */
	Word BoxColor1;							/* Color for shadow box */
	Word BoxColor2;							/* Second color for shadow box */
	struct ScreenShape_t **Art;				/* Art for up and down arrows */
	LongWord TimeMark;						/* Time mark when clicked */
} DialogControlTextMenu_t;

typedef struct DialogControlPopupMenu_t {	/* Simple list of text */
	DialogControl_t Root;					/* Base class */
	struct FontRef_t *FontPtr;				/* Font to use */
	struct FontRef_t *FontPtr2;				/* Font to use (highlight) */
	DialogControlTextMenu_t *MenuPtr;		/* Pop up menu */
	LinkedList_t List;						/* List of text */
	Word Value;								/* Which entry is valid */
	Word NormColor;							/* Color to fill normally */
	Word CursorColor;						/* Color to fill for cursor highlight */
	Word BoxColor1;							/* Color for shadow box */
	Word BoxColor2;							/* Second color for shadow box */
	struct ScreenShape_t **Art;				/* Art list for controls */
} DialogControlPopupMenu_t;

#define DIALOGLINEEDIT_MAX_LEN 256
#define DIALOGLINEEDIT_ALPHAONLY 1
#define DIALOGLINEEDIT_NUMBERONLY 2
#define DIALOGLINEEDIT_CAPS 4
#define DIALOGLINEEDIT_SPACEOK 8

typedef struct DialogControlLineEdit_t {	/* Line Edit Control */
	DialogControl_t Root;					/* Base class */
	struct FontRef_t *FontRef;				/* Font to use */
	char Value[DIALOGLINEEDIT_MAX_LEN];	/* Current input */
	Word Length;							/* Length of valid input */
	Word CurPos;							/* Cursor index */
	Word Insert;							/* TRUE if insert mode */
	Word MaxLen;							/* Maximum length */
	Word CursorColor;						/* Color to draw the cursor line with */
	Word Flags;								/* Flags for line edit mode */
	Boolean CursorFlag;						/* Whether to show the cursor */
} DialogControlLineEdit_t;

typedef struct DialogControlPicture_t {		/* Simple button */
	DialogControl_t Root;					/* Base class */
	struct ScreenShape_t *Art;				/* Shape number indexes */
} DialogControlPicture_t;

extern LongWord DialogControlTextBoxOutlineColor;		/* Color for outlines */
extern LongWord DialogControlTextBoxFillColor;
extern LongWord DialogControlTextBoxSelectedRowColor;
extern LongWord DialogControlShadowColor1;
extern LongWord DialogControlShadowColor2;
extern LongWord DialogControlMenuBackgroundColor;
extern LongWord DialogControlMenuSelectColor;

extern DialogList_t DialogMasterList;	/* Convenience; you usually don't need more than one */

extern void FASTCALL DialogControlDelete(DialogControl_t *Input);
extern DialogControlAction_e FASTCALL DialogControlCheck(DialogControl_t *Input,int x,int y,Word buttons,Word Key);
extern void FASTCALL DialogControlDeleteProc(DialogControl_t *Input);
extern void FASTCALL DialogControlMove(DialogControl_t *Input,int xoffset,int yoffset);
extern void FASTCALL DialogControlMoveTo(DialogControl_t *Input,int x,int y);

extern void FASTCALL DialogInit(Dialog_t *Input);
extern void FASTCALL DialogDestroy(Dialog_t *Input);
extern DialogControl_t * FASTCALL DialogCheck(Dialog_t *Input,int x,int y,Word Buttons,Word Key);
extern void FASTCALL DialogDraw(Dialog_t *Input);
extern DialogControl_t * FASTCALL DialogModal(Dialog_t *Input);
extern void FASTCALL DialogMove(Dialog_t *Input,int xoffset,int yoffset);
extern void FASTCALL DialogMoveTo(Dialog_t *Input,int x,int y);
extern LongWord DialogInitParseMacDLOG(Dialog_t *Input, const Byte *DLOGData, char *name);
extern Word DialogInitParseMacDITL(Dialog_t *Input, Byte *DITLData);

extern void FASTCALL DialogListInit(DialogList_t *Input);
extern void FASTCALL DialogListDestroy(DialogList_t *Input);
extern void FASTCALL DialogListAddDialog(DialogList_t *Input,Dialog_t *DialogPtr);
extern void FASTCALL DialogListRemoveDialog(DialogList_t *Input,Dialog_t *DialogPtr);
extern void FASTCALL DialogListDraw(DialogList_t *Input, Boolean RefreshAll);
extern DialogControl_t * FASTCALL DialogListModal(DialogList_t *Input, Boolean RefreshAll, Dialog_t **OutDialog);

extern void FASTCALL DialogControlListInit(DialogControlList_t *Input);
extern void FASTCALL DialogControlListDestroy(DialogControlList_t *Input);
extern DialogControl_t * FASTCALL DialogControlListCheck(DialogControlList_t *Input,int x,int y,Word Buttons,Word Key);
extern void FASTCALL DialogControlListAddControl(DialogControlList_t *Input,DialogControl_t *ControlPtr);
extern void FASTCALL DialogControlListDraw(DialogControlList_t *Input, int x, int y);
extern DialogControl_t * FASTCALL DialogControlListControlById(DialogControlList_t *Input, int Id);
extern DialogControlButton_t * FASTCALL DialogControlListAddNewButton(DialogControlList_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlTextButton_t * FASTCALL DialogControlListAddNewTextButton(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,const char *TextPtr,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlCheckBox_t * FASTCALL DialogControlListAddNewCheckBox(DialogControlList_t *Input,struct ScreenShape_t **ShapeArray,const char *TextPtr,struct FontRef_t *FontPtr,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc,Word Checked);
extern DialogControlSliderBar_t * FASTCALL DialogControlListAddSliderBar(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range);
extern DialogControlRepeatButton_t * FASTCALL DialogControlListAddRepeatButton(DialogControlList_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlVScrollSlider_t * FASTCALL DialogControlListAddVScrollSlider(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step);
extern DialogControlVScroll_t *DialogControlListAddVScroll(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step,Word ButtonStep,Boolean NormalArrowStyle);
extern DialogControlTextBox_t *FASTCALL DialogControlListAddTextBox(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,Word Value,struct FontRef_t *FontPtr,Boolean NormalArrowStyle,Boolean AllowSlider);
extern DialogControlStaticText_t *FASTCALL DialogControlListAddStaticText(DialogControlList_t *Input,const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,struct FontRef_t *FontPtr);
extern DialogControlTextList_t *FASTCALL DialogControlListAddTextList(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc,Boolean ScrollBarNormalArrowStyle);
extern DialogControlTextMenu_t *FASTCALL DialogControlListAddTextMenu(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,int x,int y,struct LinkedList_t *ListPtr,Word Value,DialogControlEventProc EventProc);
extern DialogControlPopupMenu_t *FASTCALL DialogControlListAddPopupMenu(DialogControlList_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc);
extern DialogControlLineEdit_t *FASTCALL DialogControlListAddLineEdit(DialogControlList_t *Input, struct FontRef_t* FontPtr, const LBRect *Bounds, Word MaxLen, LongWord CursorColor, Word Flags, DialogControlEventProc EventProc);
extern DialogControlPicture_t *FASTCALL DialogControlListAddPicture(DialogControlList_t *Input, struct ScreenShape_t *Art, const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);

extern void FASTCALL DialogControlInit(DialogControl_t *Input,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControl_t *FASTCALL DialogControlNew(const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);

extern void FASTCALL DialogControlButtonInit(DialogControlButton_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlButton_t *FASTCALL DialogControlButtonNew(struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern void FASTCALL DialogControlButtonSetShapes(DialogControlButton_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3);

extern void FASTCALL DialogControlTextButtonInit(DialogControlTextButton_t *Input,struct ScreenShape_t **ShapeArray,const char *Text,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlTextButton_t *FASTCALL DialogControlTextButtonNew(struct ScreenShape_t **ShapeArray,const char *Text,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern void FASTCALL DialogControlTextButtonSetText(DialogControlTextButton_t *Input,const char *TextPtr,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2);
extern void FASTCALL DialogControlTextButtonSetArt(DialogControlTextButton_t *Input,struct ScreenShape_t **ShapeArray);

extern void FASTCALL DialogControlCheckBoxInit(DialogControlCheckBox_t *Input,struct ScreenShape_t **ShapeArray,const char *Text,struct FontRef_t *FontPtr,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc,Word Checked);
extern DialogControlCheckBox_t *FASTCALL DialogControlCheckBoxNew(struct ScreenShape_t **ShapeArray,const char *Text,struct FontRef_t *FontPtr,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc,Word Checked);
extern void FASTCALL DialogControlCheckBoxSetText(DialogControlCheckBox_t *Input,struct ScreenShape_t **ShapeArray,const char *Text,struct FontRef_t *FontPtr);

extern void FASTCALL DialogControlSliderBarInit(DialogControlSliderBar_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range);
extern DialogControlSliderBar_t * FASTCALL DialogControlSliderBarNew(struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range);
extern void FASTCALL DialogControlSliderBarSetValue(DialogControlSliderBar_t *Input,Word NewValue);
extern void FASTCALL DialogControlSliderBarSetParms(DialogControlSliderBar_t *Input,Word Range);
extern void FASTCALL DialogControlSliderBarSetArt(DialogControlSliderBar_t *Input,struct ScreenShape_t **ArtArray);

extern void FASTCALL DialogControlRepeatButtonInit(DialogControlRepeatButton_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlRepeatButton_t * FASTCALL DialogControlRepeatButtonNew(struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern void FASTCALL DialogControlRepeatButtonSetArt(DialogControlRepeatButton_t *Input,struct ScreenShape_t *Shape1,struct ScreenShape_t *Shape2,struct ScreenShape_t *Shape3);

extern void FASTCALL DialogControlVScrollSliderInit(DialogControlVScrollSlider_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step);
extern DialogControlVScrollSlider_t * FASTCALL DialogControlVScrollSliderNew(struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step);
extern void FASTCALL DialogControlVScrollSliderSetValue(DialogControlVScrollSlider_t *Input,Word NewValue);
extern void FASTCALL DialogControlVScrollSliderSetParms(DialogControlVScrollSlider_t *Input,Word Range,Word Step);

extern void FASTCALL DialogControlVScrollSetValue(DialogControlVScroll_t *Input,Word NewValue);
extern void FASTCALL DialogControlVScrollSetParms(DialogControlVScroll_t *Input,Word Range,Word Step,Word ButtonStep);
extern void FASTCALL DialogControlVScrollInit(DialogControlVScroll_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step,Word ButtonStep, Boolean NormalArrowStyle);
extern DialogControlVScroll_t * FASTCALL DialogControlVScrollNew(struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,Word Value,Word Range,Word Step,Word ButtonStep,Boolean NormalArrowStyle);

extern void FASTCALL DialogControlTextBoxSetValue(DialogControlTextBox_t *Input,Word NewValue);
extern void FASTCALL DialogControlTextBoxSetText(DialogControlTextBox_t *Input,const char *TextPtr,struct FontRef_t *FontPtr);
extern void FASTCALL DialogControlTextBoxInit(DialogControlTextBox_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,Word Value,struct FontRef_t *FontPtr,Boolean ScrollBarNormalArrowStyle,Boolean AllowSlider);
extern DialogControlTextBox_t * FASTCALL DialogControlTextBoxNew(struct ScreenShape_t **ArtArray,const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,Word Value,struct FontRef_t *FontPtr,Boolean ScrollBarNormalArrowStyle,Boolean AllowSlider);

extern void FASTCALL DialogControlStaticTextSetText(DialogControlStaticText_t *Input,const char *TextPtr,struct FontRef_t *FontPtr);
extern void FASTCALL DialogControlStaticTextInit(DialogControlStaticText_t *Input,const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,struct FontRef_t *FontPtr);
extern DialogControlStaticText_t * FASTCALL DialogControlStaticTextNew(const LBRect *Bounds,DialogControlEventProc EventProc,const char *TextPtr,struct FontRef_t *FontPtr);

extern void FASTCALL DialogControlTextListSetValue(DialogControlTextList_t *Input,Word NewValue);
extern Word FASTCALL DialogControlTextListAddText(DialogControlTextList_t *Input,const char *TextPtr,Word EntryNum);
extern void FASTCALL DialogControlTextListRemoveText(DialogControlTextList_t *Input,Word EntryNum);
extern void FASTCALL DialogControlTextListRemoveAllText(DialogControlTextList_t *Input);
extern Word FASTCALL DialogControlTextListFindText(DialogControlTextList_t *Input,const char *TextPtr);
extern void FASTCALL DialogControlTextListInit(DialogControlTextList_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc,Boolean ScrollBarNormalArrowStyle);
extern DialogControlTextList_t *FASTCALL DialogControlTextListNew(struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc,Boolean ScrollBarNormalArrowStyle);

extern void FASTCALL DialogControlGenericListSetValue(DialogControlGenericList_t *Input,Word NewValue);
extern Word FASTCALL DialogControlGenericListAddRow(DialogControlGenericList_t *Input,void *DataPtr,Word EntryNum);
extern void FASTCALL DialogControlGenericListRemoveRow(DialogControlGenericList_t *Input,Word EntryNum);
extern void FASTCALL DialogControlGenericListRemoveAllRows(DialogControlGenericList_t *Input);
extern void FASTCALL DialogControlGenericListInit(DialogControlGenericList_t *Input,struct ScreenShape_t **ArtArray,const LBRect *Bounds,Word CellHeight,DialogControlEventProc EventProc,DialogControlGenericListDrawProc CellDrawProc,Boolean ScrollBarNormalArrowStyle);
extern DialogControlGenericList_t *FASTCALL DialogControlGenericListNew(struct ScreenShape_t **ArtArray,const LBRect *Bounds,Word CellHeight,DialogControlEventProc EventProc,DialogControlGenericListDrawProc CellDrawProc,Boolean ScrollBarNormalArrowStyle);

extern void FASTCALL DialogControlTextMenuInit(DialogControlTextMenu_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,int x,int y,LinkedList_t *ListPtr,Word Value,DialogControlEventProc EventProc);
extern DialogControlTextMenu_t *FASTCALL DialogControlTextMenuNew(struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,int x,int y,LinkedList_t *ListPtr,Word Value,DialogControlEventProc EventProc);

extern void FASTCALL DialogControlPopupMenuSetValue(DialogControlPopupMenu_t *Input,Word NewValue);
extern Word FASTCALL DialogControlPopupMenuAddText(DialogControlPopupMenu_t *Input,const char *TextPtr,Word EntryNum);
extern void FASTCALL DialogControlPopupMenuRemoveText(DialogControlPopupMenu_t *Input,Word EntryNum);
extern void FASTCALL DialogControlPopupMenuRemoveAllText(DialogControlPopupMenu_t *Input);
extern Word FASTCALL DialogControlPopupMenuFindText(DialogControlPopupMenu_t *Input,const char *TextPtr);
extern void FASTCALL DialogControlPopupMenuInit(DialogControlPopupMenu_t *Input,struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc);
extern DialogControlPopupMenu_t *FASTCALL DialogControlPopupMenuNew(struct ScreenShape_t **ArtArray,struct FontRef_t *FontPtr,struct FontRef_t *FontPtr2,const LBRect *Bounds,DialogControlEventProc EventProc);

extern void FASTCALL DialogControlLineEditReset(DialogControlLineEdit_t *Input);
extern void FASTCALL DialogControlLineEditSetText(DialogControlLineEdit_t *Input,const char *text);
extern void FASTCALL DialogControlLineEditGetText(DialogControlLineEdit_t *Input, char* Buffer, Word BufferSize );
extern void FASTCALL DialogControlLineEditEnableCursor(DialogControlLineEdit_t* Input, Boolean EnableCursor );
extern void FASTCALL DialogControlLineEditSetInsertMode(DialogControlLineEdit_t* Input, Boolean InsertMode );
extern Boolean FASTCALL DialogControlLineEditGetInsertMode(DialogControlLineEdit_t* Input );
extern Boolean FASTCALL DialogControlLineEditOnKeyPress(DialogControlLineEdit_t *Input,Word InKey);
extern void FASTCALL DialogControlLineEditInit(DialogControlLineEdit_t *Input, struct FontRef_t* FontPtr, const LBRect *Bounds, Word MaxLen, LongWord CursorColor, Word Flags, DialogControlEventProc EventProc );
extern DialogControlLineEdit_t *FASTCALL DialogControlLineEditNew(struct FontRef_t* FontPtr, const LBRect *Bounds, Word MaxLen, LongWord CursorColor, Word Flags, DialogControlEventProc EventProc);

extern void FASTCALL DialogControlPictureInit(DialogControlPicture_t *Input,struct ScreenShape_t *Art,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern DialogControlPicture_t *FASTCALL DialogControlPictureNew(struct ScreenShape_t *Art,const LBRect *Bounds,Word HotKey,DialogControlEventProc EventProc);
extern void FASTCALL DialogControlPictureSetArt(DialogControlPicture_t *Input,struct ScreenShape_t *Art);

/* Sound handlers */

#define SOUND_EXCLUSIVE 0x80000000UL
#define SOUND_COOKIE 0x40000000UL

#define SOUNDTYPEBYTE 0		/* WAVE 8 bit data */
#define SOUNDTYPECHAR 1		/* AIFF 8 bit data */
#define SOUNDTYPELSHORT 2	/* Little endian short */
#define SOUNDTYPEBSHORT 3	/* Big endian short */
#define SOUNDTYPEADPCM 4	/* MS ADPCM compression */
#define SOUNDTYPEDVIPCM 5	/* Intel DVI ADPCM compression */
#define SOUNDTYPEMP3 6		/* MP3 Audio */
#define SOUNDTYPEULAW 7		/* MuLaw */
#define SOUNDTYPEALAW 8		/* ALaw */
#define SOUNDTYPEMACE3 9	/* Mace 3:1 */
#define SOUNDTYPEMACE6 10	/* Mace 6:1 */
#define SOUNDTYPEOGG 11		/* OGG/Vorbis Audio */
#define SOUNDTYPESTEREO 0x8000	/* Stereo data */
#define SOUNDTYPEDOUBLEBUFFER 0x4000	/* Double buffered */

/* SystemState defines */

#define SfxActive 1
#define MusicActive 2
#define PauseActive 4

typedef void (FASTCALL *SndCompleteProcPtr)(void *);
typedef char *(FASTCALL *MakeSongProc)(Word);

typedef struct RawSound_t {
	Byte *SoundPtr;			/* Pointer to the data */
	LongWord SoundLength;	/* Length of the data in bytes */
	LongWord LoopStart;		/* Sample to start from */
	LongWord LoopEnd;		/* Sample to end the loop (0 for no looping) */
	SndCompleteProcPtr Proc;	/* Completion routine */
	void *Data;				/* Data to pass for completion routine */
	Word DataType;			/* Type of data found */
	Word SampleRate;		/* Samples per second to play */
	Byte *CompPtr;			/* Used by compression */
	Word Extra1;			/* Used by compression */
	Word Extra2;			/* Used by compression */
	Word Extra3;			/* Used by compression */
	LongWord SampleCount;	/* Output from ParseSoundFileImage */
	float TimeInSeconds;	/* Output from ParseSoundFileImage */
} RawSound_t;

#define MUSICCODECINIT 0
#define MUSICCODECDESTROY 1
#define MUSICCODECDECODE 2
#define MUSICCODECRESET 3

typedef LongWord (FASTCALL *DecodeCallbackProc)(void *,Byte *,LongWord);
typedef LongWord (FASTCALL *DecodeCodecProc)(struct DigitalMusicReadState_t *,Word,Byte *,LongWord);

typedef struct DigitalMusicReadState_t {	/* Private state structure */
	DecodeCallbackProc ReadProc;	/* Read data proc */
	void *CallBackParm;				/* Parm for the read callback */
	DecodeCodecProc CodecProc;		/* Decompressor */
	void *CompressStatePtr;			/* Extra data needed by codec */
	LongWord FileOffset;			/* Offset to where in the file data starts at */
	LongWord SoundLength;			/* Size of data to play (Decompressed) */
	LongWord BytesPlayed;			/* Number of bytes processed */
	Word DataType;					/* Type of input data (Codec index) */
} DigitalMusicReadState_t;

typedef int (FASTCALL *MADImportPtr)(const Byte *DataPtr,LongWord Length,struct MADMusic *MadFile);

extern Word SystemState;	/* Global game state flags */
extern Word SfxVolume;		/* Master volume of game sound effects */
extern Word *SoundCookiePtr;	/* Writeback handle for audio channel */
extern Word PanPosition;	/* Initial pan position of sound to play */
extern Word SoundLoopFlag;	/* True if you want the sound to loop */
extern int SoundFrequencyAdjust;	/* Pitch bend */
extern struct RezHeader_t *SoundRezHeader;	/* Resource file for sound loading */
extern MakeSongProc DigitalMusicNameCallback;

extern void FASTCALL ModMusicInit(void);
extern void FASTCALL ModMusicShutdown(void);
extern void FASTCALL ModMusicImporter(MADImportPtr ImportPtr);
extern Word FASTCALL ModMusicPlay(Word SongNum);
extern Word FASTCALL ModMusicPlayByFilename(const char *FileName);
extern Word FASTCALL ModMusicPlayByPtr(const Byte *DataPtr,LongWord Length);
extern void FASTCALL ModMusicStop(void);
extern void FASTCALL ModMusicPause(void);
extern void FASTCALL ModMusicResume(void);
extern void FASTCALL ModMusicReset(void);
extern Word FASTCALL ModMusicGetVolume(void);
extern void FASTCALL ModMusicSetVolume(Word NewVolume);
extern int FASTCALL ModMusicS3M(const Byte *DataPtr,LongWord Length,struct MADMusic *MadFile);
extern int FASTCALL ModMusicMADI(const Byte *DataPtr,LongWord Length,struct MADMusic *MadFile);
extern int FASTCALL ModMusicIT(const Byte *DataPtr,LongWord Length,struct MADMusic *MadFile);
extern int FASTCALL ModMusicXM(const Byte *DataPtr,LongWord Length,struct MADMusic *MadFile);

extern LongWord FASTCALL DigitalMusicByte(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicChar(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicULaw(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicALaw(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicLShort(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicBShort(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicMace3(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicMace6(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicADPCM(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern LongWord FASTCALL DigitalMusicOgg(DigitalMusicReadState_t *Input,Word Command,Byte *DestPtr,LongWord Length);
extern Word FASTCALL DigitalMusicGetSilenceVal(Word Type);
extern Word FASTCALL DigitalMusicDecode(DigitalMusicReadState_t *Input,Byte *DestBuffer,LongWord Length);
extern Word FASTCALL DigitalMusicReadStateInit(DigitalMusicReadState_t *Output,struct RawSound_t *Input,Byte *ImagePtr,LongWord MaxSize,DecodeCallbackProc Proc,void *Parm);
extern void FASTCALL DigitalMusicReadStateDestroy(DigitalMusicReadState_t *Input);
extern void FASTCALL DigitalMusicReset(DigitalMusicReadState_t *Input);
extern void FASTCALL DigitalMusicInit(void);
extern void FASTCALL DigitalMusicShutdown(void);
extern Word FASTCALL DigitalMusicIsPlaying(void);
extern Word FASTCALL DigitalMusicGetFrequency(void);
extern Word FASTCALL DigitalMusicGetVolume(void);
extern void FASTCALL DigitalMusicSetFilenameProc(MakeSongProc Proc);
extern void FASTCALL DigitalMusicSetFrequency(Word Freq);
extern void FASTCALL DigitalMusicSetVolume(Word Volume);
extern void FASTCALL DigitalMusicPlay(Word SongNum);
extern void FASTCALL DigitalMusicPause(void);
extern void FASTCALL DigitalMusicResume(void);

extern void FASTCALL InitSoundPlayer(void);
extern void FASTCALL KillSoundPlayer(void);
extern void FASTCALL StopASound(LongWord SoundCookie);
extern Word FASTCALL PlayASound(LongWord SoundNum);
extern Word FASTCALL PlayARawSound(RawSound_t *Input);
extern Word FASTCALL ParseSoundFileImage(RawSound_t *Output,const void *Input,LongWord Length);
extern double FASTCALL ConvertAiffExtended(const void *Input);
extern void * FASTCALL FindIffChunk(const void *Input,LongWord Name,LongWord Length);
extern void FASTCALL PauseAllSounds(void);
extern void FASTCALL ResumeAllSounds(void);
extern void FASTCALL StopAllSounds(void);
extern void FASTCALL SetMaxSounds(Word Max);
extern Word FASTCALL GetMaxSounds(void);
extern Word FASTCALL GetNumSoundsPlaying(void);
extern Word FASTCALL GetSfxVolume(void);
extern void FASTCALL SetSfxVolume(Word NewVolume);
extern Word FASTCALL IsASoundPlaying(LongWord SoundCookie);
extern Word FASTCALL GetASoundFrequency(LongWord SoundCookie);
extern void FASTCALL SetASoundFrequency(LongWord SoundCookie,Word Frequency);
extern Word FASTCALL GetASoundVolume(LongWord SoundCookie);
extern void FASTCALL SetASoundVolume(LongWord SoundCookie,Word Volume);
extern Word FASTCALL GetASoundPan(LongWord SoundCookie);
extern void FASTCALL SetASoundPan(LongWord SoundCookie,Word Pan);
extern void FASTCALL SoundSetCallback(LongWord SoundCookie,SndCompleteProcPtr Proc,void *Data);

/* CD Audio Manager */

#define REDBOOKMODEBUSY 0
#define REDBOOKMODESTOPPED 1
#define REDBOOKMODEPLAYING 2
#define REDBOOKMODEPAUSED 3
#define REDBOOKMODEOPENED 4

#define REDBOOKPLAY 0
#define REDBOOKPLAYONCE 1
#define REDBOOKPLAYLOOP 2

#define FRAMESPERSEC 75

#if defined(__WIN95__)	/* Windows 95 version */
typedef struct {			/* Members are private!! */
	LongWord FromMark;		/* Starting mark for loop */
	LongWord ToMark;		/* Destination mark used for resume */
	LongWord OpenDeviceID;	/* Win95 MMSystem device */
	void *ThreadID;			/* ID for the thread processing the CD */
	Word MixerDevice;		/* Device for audio control */
	Word MixerFlags;		/* Volume enable flag for mixer */
	Word LoopingTimerID;	/* Timer proc for looping a CD file */
	Boolean Paused;			/* True if paused (Public) */
	Boolean Active;			/* True if properly initialized (Public) */
	Boolean Timer;			/* True if loop thread is spawned */
	Boolean Mixer;			/* True if volume control is present */
} Redbook_t;

#elif defined(__MAC__)	/* MacOS version */
typedef struct {			/* Members are private!! */
	LongWord FromMark;		/* Starting mark for loop */
	LongWord ToMark;		/* Destination mark used for resume */
	LongWord TimeMark;		/* Timer for thread */
	void *ProcPtr;			/* IOCompletionUPP for async calls */
	Byte ParamData[84*4];	/* MacOS ParamBlockRec (And a pad byte) */
	short OpenDeviceID;		/* MacOS device reference */
	Boolean Paused;			/* True if paused (Public) */
	Boolean Active;			/* True if properly initialized (Public) */
	Boolean Timer;			/* True if loop thread is spawned */
} Redbook_t;
#else					/* DOS version */
typedef struct {			/* Members are private!! */
	LongWord FromMark;		/* Starting mark for loop */
	LongWord ToMark;		/* Destination mark used for resume */
	LongWord TimeMark;		/* Timer for thread */
	Word OpenDeviceID;		/* Dos device number */
	Boolean Paused;			/* True if paused (Public) */
	Boolean Active;			/* True if properly initialized (Public) */
	Boolean Timer;			/* True if loop thread is spawned */
} Redbook_t;

#endif

extern Redbook_t * FASTCALL RedbookNew(void);
extern Word FASTCALL RedbookInit(Redbook_t *Input);
extern void FASTCALL RedbookDelete(Redbook_t *Input);
extern void FASTCALL RedbookDestroy(Redbook_t *Input);
extern void FASTCALL RedbookPlay(Redbook_t *Input,Word Mode,Word Track);
extern void FASTCALL RedbookPlayRange(Redbook_t *Input,Word Mode,Word StartTrack,LongWord StartPosition,Word EndTrack,LongWord EndPosition);
extern Word FASTCALL RedbookGetCurrentTrack(Redbook_t *Input);
extern Word FASTCALL RedbookGetTrackCount(Redbook_t *Input);
extern LongWord FASTCALL RedbookGetTrackLength(Redbook_t *Input,Word Track);
extern LongWord FASTCALL RedbookGetTrackStart(Redbook_t *Input,Word TrackNum);
extern void FASTCALL RedbookStop(Redbook_t *Input);
extern void FASTCALL RedbookGetVolume(Redbook_t *Input,Word *Left,Word *Right);
extern void FASTCALL RedbookSetVolume(Redbook_t *Input,Word Left,Word Right);
extern Word FASTCALL RedbookGetPlayStatus(Redbook_t *Input);
extern void FASTCALL RedbookPause(Redbook_t *Input);
extern void FASTCALL RedbookResume(Redbook_t *Input);
extern LongWord FASTCALL RedbookGetPosition(Redbook_t *Input);
extern void FASTCALL RedbookOpenDrawer(Redbook_t *Input);
extern void FASTCALL RedbookCloseDrawer(Redbook_t *Input);
extern LongWord FASTCALL RedbookMakeTMSF(Word Track,LongWord Position);
extern LongWord FASTCALL RedbookMakePosition(Word Minutes,Word Seconds,Word Frames);
extern void FASTCALL RedbookSetLoopStart(Redbook_t *Input,Word Track,LongWord Position);
extern void FASTCALL RedbookSetLoopEnd(Redbook_t *Input,Word Track,LongWord Position);
extern Word FASTCALL RedbookLogCDByName(Redbook_t *Input,const char *VolumeName);

/* Profile manager */

typedef enum {FPU_NONE,FPU_287,FPU_387,FPU_PENTIUM,
	FPU_601,FPU_603,FPU_68881} FPU_e;
typedef enum {CPU_UNKNOWN,CPU_386,CPU_486,CPU_586,CPU_686,
	CPU_601,CPU_603,CPU_604,CPU_750,
	CPU_68000,CPU_68020,CPU_68030,CPU_68040} CPU_e;
typedef enum {MMX_NONE,MMX_PENTIUM,MMX_K6,MMX_ALTIVEC} MMX_e;
typedef enum {VENDOR_UNKNOWN,VENDOR_INTEL,VENDOR_AMD,VENDOR_UMC,VENDOR_CYRIX,VENDOR_NEXGEN,
	VENDOR_IBM,VENDOR_HITACHI,VENDOR_MOTOROLA,VENDOR_ARM,VENDOR_MIPS} CPUVendor_e;

typedef struct CPUFeatures_t {
	CPU_e CPUFamily;	/* Class of CPU */
	FPU_e FPUFamily;	/* Class of FPU */
	MMX_e MMXFamily;	/* Extended instructions */
	CPUVendor_e Vendor;	/* Who made the chip? */
	char VerboseName[64];	/* Cpu name and feature string */
#if defined(__INTEL__)
	LongWord Features;	/* CPU ID features list */
	Word Revision;		/* CPU stepping flag */
	Word Model;			/* CPU model */
	Word Type;			/* CPU type */
	char VendorID[13];	/* Cpu vendor string */
#endif
} CPUFeatures_t;

typedef struct Profile_t {
	const char *Name;	/* Name of fragment being profiled */
	struct Profile_t *Next;	/* Next in linked list */
	LongWord Mark;		/* Current time mark */
	LongWord TimeIn;	/* Time inside the proc */
	LongWord TimeOut;	/* Time outside the proc */
	Word HitCount;		/* Number of times entered */
	Word RecurseCount;	/* Recursion flag */
	Word Initialized;	/* TRUE if initialized */
} Profile_t;

extern Profile_t *ProfileRoot;	/* Root pointer for linked list of Profile_t's */

extern void FASTCALL CPUFeaturesGet(CPUFeatures_t *Input);
extern Profile_t * FASTCALL ProfileNew(const char *Name);
extern void FASTCALL ProfileInit(Profile_t *Input,const char *Name);
extern void FASTCALL ProfileDelete(Profile_t *Input);
extern void FASTCALL ProfileDestroy(Profile_t *Input);
extern void FASTCALL ProfileEntry(Profile_t *Input);
extern void FASTCALL ProfileExit(Profile_t *Input);
extern double FASTCALL ProfileGetSecondsIn(const Profile_t *Input);
extern double FASTCALL ProfileGetSecondsOut(const Profile_t *Input);
extern double FASTCALL ProfileGetMicrosecondsIn(const Profile_t *Input);
extern double FASTCALL ProfileGetMicrosecondsOut(const Profile_t *Input);
extern void FASTCALL ProfileReset(Profile_t *Input);
extern void FASTCALL ProfileResetAll(void);
extern Word FASTCALL ProfileIsAvailable(void);

/* Flic player */

#define MOVIEDISKBASED 0
#define MOVIERAMBASED 1

typedef struct FlicMovie_t {
	FILE *fp;				/* Input file stream */
	Byte *Buffer;			/* Input file buffer */
	LongWord BufferSize;	/* Size of the input buffer */
	StreamHandle_t MyInput;	/* Ram based input buffer */
	LongWord MovieSpeed;	/* Speed to play a movie */
	LongWord TickStart;		/* Tick the movie began */
	LongWord TickStop;		/* Tick the movie was paused at */
	LongWord FirstFrameMark;	/* Offset to the first movie frame */
	LongWord LoopFrameMark;	/* Offset to the loop frame */
	LongWord FileOffset;	/* Offset into the file for seeking */
	Image_t FrameImage;		/* Current image of the movie */
	Word CurrentFrameNumber;	/* Current frame being shown */
	Word MaxFrameNumber;	/* Frames in the movie */
	Boolean Active;			/* True if this movie is in progress */
	Boolean Paused;			/* True if the movie is paused */
	Boolean Completed;		/* True if the movie has completed */
	Boolean AllowFrameSkipping;	/* True if I can skip frames */
	Boolean Looping;		/* True if it loops */
} FlicMovie_t;

extern FlicMovie_t * FASTCALL FlicMovieNew(const char *FileName,Word Flags,LongWord FileOffset);
extern Word FASTCALL FlicMovieInit(FlicMovie_t *Input,const char *FileName,Word Flags,LongWord FileOffset);
extern void FASTCALL FlicMovieDelete(FlicMovie_t *Input);
extern void FASTCALL FlicMovieDestroy(FlicMovie_t *Input);
#define FlicMovieIsPlaying(Input) ((Input)->Active)
#define FlicMovieGetWidth(Input) ((Input)->FrameImage.Width)
#define FlicMovieGetHeight(Input) ((Input)->FrameImage.Height)
extern Word FASTCALL FlicMovieDataRead(FlicMovie_t *Input,void *Data,LongWord Length);
extern void FASTCALL FlicMovieReset(FlicMovie_t *Input);
extern Word FASTCALL FlicMovieStart(FlicMovie_t *Input);
extern void FASTCALL FlicMovieStop(FlicMovie_t *Input);
extern void FASTCALL FlicMoviePause(FlicMovie_t *Input);
extern void FASTCALL FlicMovieResume(FlicMovie_t *Input);
extern void FASTCALL FlicMovieSetLoopFlag(FlicMovie_t *Input,Word LoopFlag);
extern void FASTCALL FlicMovieSetSpeed(FlicMovie_t *Input,Word FramesPerSecond);
extern void FASTCALL FlicMovieSetToFrame(FlicMovie_t *Input,Word FrameNumber);
extern Image_t * FASTCALL FlicMovieGetImage(FlicMovie_t *Input);
extern Image_t * FASTCALL FlicMovieUpdate(FlicMovie_t *Input);

/* DPaint Anim player */

typedef struct DPaintAnimMovie_t {
	FILE *fp;				/* Input file stream */
	Byte *Buffer;			/* Input file buffer */
	Byte *DictionaryBuffer;	/* Dictionary buffer */
	StreamHandle_t MyInput;	/* Ram based input buffer */
	LongWord MovieSpeed;	/* Speed to play a movie */
	LongWord TickStart;		/* Tick the movie began */
	LongWord TickStop;		/* Tick the movie was paused at */
	LongWord FileOffset;	/* Offset into the file for seeking */
	Word DictionarySize;	/* Size of the dictionary in records */
	Image_t FrameImage;		/* Current image of the movie */
	Word CurrentFrameNumber;	/* Current frame being shown */
	Word MaxFrameNumber;	/* Frames in the movie */
	Boolean Active;			/* True if this movie is in progress */
	Boolean Paused;			/* True if the movie is paused */
	Boolean Completed;		/* True if the movie has completed */
	Boolean AllowFrameSkipping;	/* True if I can skip frames */
	Boolean Looping;		/* True if it loops */
} DPaintAnimMovie_t;

extern DPaintAnimMovie_t * FASTCALL DPaintAnimMovieNew(const char *FileName,Word Flags,LongWord FileOffset);
extern Word FASTCALL DPaintAnimMovieInit(DPaintAnimMovie_t *Input,const char *FileName,Word Flags,LongWord FileOffset);
extern void FASTCALL DPaintAnimMovieDelete(DPaintAnimMovie_t *Input);
extern void FASTCALL DPaintAnimMovieDestroy(DPaintAnimMovie_t *Input);
#define DPaintAnimMovieIsPlaying(Input) ((Input)->Active)
#define DPaintAnimMovieGetWidth(Input) ((Input)->FrameImage.Width)
#define DPaintAnimMovieGetHeight(Input) ((Input)->FrameImage.Height)
extern Word FASTCALL DPaintAnimMovieDataRead(DPaintAnimMovie_t *Input,void *Data,LongWord Length);
extern void FASTCALL DPaintAnimMovieReset(DPaintAnimMovie_t *Input);
extern Word FASTCALL DPaintAnimMovieStart(DPaintAnimMovie_t *Input);
extern void FASTCALL DPaintAnimMovieStop(DPaintAnimMovie_t *Input);
extern void FASTCALL DPaintAnimMoviePause(DPaintAnimMovie_t *Input);
extern void FASTCALL DPaintAnimMovieResume(DPaintAnimMovie_t *Input);
extern void FASTCALL DPaintAnimMovieSetLoopFlag(DPaintAnimMovie_t *Input,Word LoopFlag);
extern void FASTCALL DPaintAnimMovieSetSpeed(DPaintAnimMovie_t *Input,Word FramesPerSecond);
extern Image_t * FASTCALL DPaintAnimMovieGetImage(DPaintAnimMovie_t *Input);
extern Image_t * FASTCALL DPaintAnimMovieUpdate(DPaintAnimMovie_t *Input);

/* GUID Manager */

#ifndef GUID_DEFINED		/* Used to be compatible with Win95 */
#define GUID_DEFINED
typedef struct _GUID {
	LongWord Data1;			/* Same names as Win95 */
	Short Data2;
	Short Data3;
	Byte Data4[8];
} GUID;
#endif

typedef struct EthernetAddress_t {
    Byte eaddr[6];      /* 6 bytes of ethernet hardware address */
} EthernetAddress_t;

extern GUID GUIDBlank;		/* Empty GUID */
extern void FASTCALL GUIDInit(GUID *Output);
extern void FASTCALL GUIDToString(char *Output,const GUID *Input);
extern Word FASTCALL GUIDFromString(GUID *Output,const char *Input);
extern Word FASTCALL GUIDHash(const GUID *Input);
extern Word FASTCALL GUIDIsEqual(const GUID *Input1,const GUID *Input2);
extern int FASTCALL GUIDCompare(const GUID *Input1,const GUID *Input2);
extern void FASTCALL GUIDGetTime(LongWord64_t *Output);
extern Word FASTCALL GUIDGetEthernetAddr(EthernetAddress_t *Output);
extern Word FASTCALL GUIDGenRandomEthernet(EthernetAddress_t *Output);

/* Network Manager */

#define NET_MAXPACKETSIZE 3072
typedef enum {NET_PROVIDER_IPX,NET_PROVIDER_TCP,NET_PROVIDER_APPLETALK,NET_PROVIDER_COUNT} NetProvider_e;
typedef enum {SOCKETMODE_UNUSED,SOCKETMODE_LISTEN,SOCKETMODE_LISTENPACKET,SOCKETMODE_CONNECTED,
	SOCKETMODE_ACCEPTING,SOCKETMODE_COUNT} SocketMode_e;
typedef Word (FASTCALL * NetListenProc)(struct NetHandle_t *);

typedef struct NetAddr_t {	/* Used for a network address */
	Word Provider;			/* Network provider (TCP/IPX/APPLETALK) */
	union {
		struct {			/* TCP/UDP data */
			Word Port;		/* TCP/IP, UDP Port */
			LongWord IP;	/* Internet IP (Network order) */
		} TCP;
		struct {			/* IPX/SPX data */
			Byte Net[4];	/* IPX/SPX Network */
			Byte Node[6];	/* IPX/SPX Node address */
			Word Socket;	/* IPX/SPX Socket */
		} IPX;
		struct {			/* Appletalk data */
			Word Network;	/* Appletalk network */
			Word NodeID;	/* Appletalk node */
			Word Socket;	/* Appletalk socket */
			Word DDPType;	/* Appletalk protocol ID */
		} APPLETALK;
	};
} NetAddr_t;

typedef struct NetPacket_t {
	NetAddr_t Origin;		/* Who sent this packet? */
	LongWord Length;		/* Length of data */
	Byte *Data;				/* Data coming in */
} NetPacket_t;

typedef struct NetHandle_t NetHandle_t;

extern Word FASTCALL NetInit(void);
extern void FASTCALL NetShutdown(void);
extern NetHandle_t * FASTCALL NetHandleNewListenPacket(NetAddr_t *Input);
extern NetHandle_t * FASTCALL NetHandleNewListenStream(NetAddr_t *Input,NetListenProc Proc);
extern NetHandle_t * FASTCALL NetHandleNewConnect(NetAddr_t *Input,Word Timeout);
extern void FASTCALL NetHandleDelete(NetHandle_t *Input);
extern Word FASTCALL NetHandleIsItClosed(NetHandle_t *Input);
extern long FASTCALL NetHandleRead(NetHandle_t *Input,void *Buffer,long BufSize);
extern long FASTCALL NetHandleWrite(NetHandle_t *Input,const void *Buffer,long Length,Word BlockFlag);
extern Word FASTCALL NetHandleSendPacket(NetHandle_t *Input,NetAddr_t *DestAddr,const void *Buffer,Word Length);
extern NetPacket_t * FASTCALL NetHandleGetPacket(NetHandle_t *Input);
extern Word FASTCALL NetStringToTCPAddress(NetAddr_t *Output,const char *TCPName);
extern void FASTCALL NetAddressToString(char *Output,Word Size,NetAddr_t *Input,Word PortFlag);
extern Word FASTCALL NetAddressCompare(const NetAddr_t *First,const NetAddr_t *Second);
extern Word FASTCALL NetIsProviderPresent(NetProvider_e Provider);
extern NetHandle_t * FASTCALL NetGetPacketSendHandle(NetProvider_e Provider);
extern void FASTCALL NetGetPeerAddress(NetAddr_t *Output,NetHandle_t *Input);
extern void FASTCALL NetGetLocalAddress(NetAddr_t *Output,NetHandle_t *Input);
extern NetHandle_t * FASTCALL NetFindHandleFromSocket(LongWord Socket);
extern NetHandle_t * FASTCALL NetFindHandleByMode(SocketMode_e Mode);
extern void FASTCALL NetAddHandleToList(NetHandle_t *Input);
#if defined(__WIN95__)
extern void FASTCALL Win95WinSockToNetAddress(NetAddr_t *Output,struct sockaddr *Input);
extern void FASTCALL Win95NetToWinSockAddress(struct sockaddr *Output,NetAddr_t *Input);
#elif defined(__MAC__)
extern void FASTCALL MacOTToNetAddress(NetAddr_t *Output,struct OTAddress *Input);
extern void FASTCALL MacNetToOTAddress(struct OTAddress *Output,NetAddr_t *Input);
#endif

/* IBM Specific convience routines */

#if defined(__IBM__)
/*********************************

	This is used by the x32 dos extender so I can address
	low memory in an IBM PC

*********************************/

extern void MCGAOn(void);
extern void MCGAOff(void);
extern Word SVGAOn(void);
extern void UpdateSVGA(void *Offscreen);
extern void UpdateSVGARectList(void *Offscreen,LBRectList *RectList);

typedef struct Regs16 {
	Short ax;
	Short bx;
	Short cx;
	Short dx;
	Short si;
	Short di;
	Short bp;
	Short ds;
	Short es;
	Short flags;
} Regs16;

extern Word FASTCALL AreLongFilenamesAllowed(void);
extern int FASTCALL Int86x(Word InterNum,Regs16 *InRegs,Regs16 *OutRegs);
extern int FASTCALL Call86(LongWord Address,Regs16 *InRegs,Regs16 *OutRegs);
extern void CallInt10(Word EAX);
extern Word Int14(Word EAX,Word EDX);
extern Word Int17(Word EAX,Word EDX);
extern void SetBothInts(Word IrqNum,void far *CodePtr);
extern void SetProtInt(Word IrqNum,void far *CodePtr);
extern void SetRealInt(Word IrqNum,LongWord CodePtr);
extern void far *GetProtInt(Word IrqNum);
extern LongWord GetRealInt(Word IrqNum);
extern void *MapPhysicalAddress(void *Input,LongWord Length);
extern void FASTCALL Win95AppendFilenameToDirectory(char *Directory,const char *Filename);
#endif

/**********************************

	Win 95 specific functions

**********************************/

#if defined(__WIN95__)
typedef struct Direct3DTexture_t {
	struct IDirectDrawSurface *Surface;		/* Pointer to the DirectDraw Surface */
	struct IDirect3DTexture *TexturePtr;	/* Pointer to the Direct3D Texture reference */
	LongWord TextureHandle;					/* The Direct3D texture handle for rendering */
} Direct3DTexture_t;

extern Word TickWakeUpFlag;		/* TRUE if I should get tick events */
extern Word Win95NoProcess;		/* Shut down ProcessSystemEvents */
extern Word Win95MouseButton;	/* Mouse button contents */
extern Word Win95MouseX;		/* Virtual mouse X */
extern Word Win95MouseY;		/* Virtual mouse Y */
extern int Win95MouseWheel;		/* Virtual mouse Z */
extern int Win95MouseXDelta;	/* Mouse motion */
extern int Win95MouseYDelta;
extern int Win95LastMouseX;		/* Previous mouse position for delta motion */
extern int Win95LastMouseY;
extern int Win95DestScreenX;	/* X coord to draw screen to */
extern int Win95DestScreenY;	/* Y coord to draw screen to */
extern void *Win95MainWindow;	/* Main window to perform all operations on */
extern struct IDirectDrawSurface *Win95FrontBuffer;	/* Currently displayed screen */
extern struct IDirectDrawSurface *Win95BackBuffer;	/* My work buffer */
extern struct IDirectDrawSurface *Win95WorkBuffer;	/* Which buffer am I using? */
extern struct IDirectDrawSurface *Direct3DZBufferPtr;		/* ZBuffer for 3D */
extern struct IDirectDrawPalette *Win95WindowPalettePtr;	/* Pointer to game palette */
extern struct IDirectDraw *DirectDrawPtr;			/* Pointer to direct draw instance */
extern struct IDirectDraw2 *DirectDraw2Ptr;			/* Pointer to the direct draw 2 instance */
extern struct IDirectDraw4 *DirectDraw4Ptr;			/* Pointer to the direct draw 4 instance */
extern struct IDirectDrawClipper *Win95WindowClipperPtr;	/* Clipper for primary surface */
extern Word Direct3DZBufferBitDepth;					/* Bits per pixel for D3D ZBuffer */
extern Word Win95ApplsActive;	/* True if the application is active */
extern Word FASTCALL Win95ProcessMessage(struct tagMSG *MessagePtr);
extern void FASTCALL ProcessSystemEvents(void);
extern Byte *Win95VideoPointer;	/* Locked videopointer for hardware blits */
extern void *Win95Instance;		/* HINSTANCE of the current app */
extern Boolean Win95UseBackBuffer;	/* True if the backbuffer is used */
extern void *Win95LockedMemory;		/* Copy of the video pointer */
extern void *BurgerHBitMap;			/* HBITMAP for window mode */
extern Byte *BurgerHBitMapPtr;		/* Pointer to HBITMAP Memory */
extern void *BurgerHPalette;		/* HPALETTE for window mode */

extern Word FASTCALL InitWin95Window(const char *Title,void *Instance,long (__stdcall *Proc)(struct HWND__*,Word,Word,long));
extern Word FASTCALL GetDirectXVersion(void);
extern void FASTCALL Win95AppendFilenameToDirectory(char *Directory,const char *Filename);
extern Word FASTCALL Win95AddGroupToProgramMenu(const char *GroupName);
extern Word FASTCALL Win95DeleteGroupFromProgramMenu(const char *GroupName);
extern Word FASTCALL Win95Allow1Instance(const char *LockName);
extern long FASTCALL CallDirectDrawCreate(struct _GUID *lpGUID,struct IDirectDraw **lplpDD,void *pUnkOuter);
extern long FASTCALL CallDirectDrawEnumerateA(int (__stdcall *lpCallback)(struct _GUID *,char *,char *,void*),void *lpContext);
extern long FASTCALL CallDirectDrawEnumerateExA(int (__stdcall *)(struct _GUID *,char*,char*,void*,void *),void *lpContext,LongWord Flags);
extern Word FASTCALL Win95VideoGetGuid(struct _GUID **OutPtr,struct _GUID *Output,Word DevNum);

extern Word FASTCALL Direct3DTextureInitImage(Direct3DTexture_t *Input,const struct Image_t *ImagePtr);
extern Direct3DTexture_t * FASTCALL Direct3DTextureNewImage(const struct Image_t *ImagePtr);
extern void FASTCALL Direct3DTextureDelete(Direct3DTexture_t *Input);
extern void FASTCALL Direct3DTextureDestroy(Direct3DTexture_t *Input);
extern void FASTCALL Direct3DTextureDraw2D(Direct3DTexture_t *Input,int x,int y,Word Width,Word Height);
extern void FASTCALL Direct3DTextureDraw2DSubRect(Direct3DTexture_t *Input,int x,int y,Word Width,Word Height,const float *UVPtr);

#endif

/**********************************

	Mac OS specific functions

**********************************/

#if defined(__MAC__) || defined(__MACOSX__)
#if !defined(__MACOSX__)
typedef Word (FASTCALL *MacEventInterceptProcPtr)(struct EventRecord *MyEventPtr);

extern struct GDevice **VideoDevice;	/* Video device for graphic output */
#if !TARGET_API_MAC_CARBON
extern struct CGrafPort *VideoGWorld;	/* Grafport to offscreen buffer */
extern struct GrafPort *VideoWindow;	/* Window to display to */
#define MacDialogRef struct GrafPort *
#else
extern struct OpaqueGrafPtr *VideoGWorld;	/* Grafport to offscreen buffer */
extern struct OpaqueWindowPtr *VideoWindow;	/* Window to display to */
#define MacDialogRef struct OpaqueDialogPtr*
#endif
#if TARGET_RT_MAC_CFM
extern struct OpaqueDSpContextReference *MacContext;	/* Reference to a reserved DrawSprocket reference */
extern Boolean MacDrawSprocketActive;	/* Has draw sprocket been started up? */
#endif
extern Boolean MacUseBackBuffer;		/* Which buffer is active */
extern Boolean MacSystemTaskFlag;		/* If TRUE, then SystemTask() is called */
extern MacEventInterceptProcPtr MacEventIntercept;	/* Intercept for DoMacEvent() */
extern short MacVRefNum;		/* Volume reference used by Mac OS */
extern long MacDirID;			/* Directory reference used by MacOS */
extern short MacVRefNum2;		/* Volume reference used by copy and rename */
extern long MacDirID2;			/* Directory reference used by copy and rename */
extern short MacCacheVRefNum;	/* Cached VRef num for currently logged directory */
extern long MacCacheDirID;		/* Cached Dir ID for currently logged directory */
extern Word FASTCALL MacMakeOffscreenGWorld(Word Width,Word Height,Word Depth,Word Flags);
extern void * FASTCALL GetFullPathFromMacID(long dirID,short vRefNum);
extern short FASTCALL OpenAMacResourceFile(const char *PathName,char Permission);
extern Word FASTCALL CreateAMacResourceFile(const char *PathName);
extern char *FASTCALL GetFullPathFromMacFSSpec(const struct FSSpec *Input);
extern void FASTCALL MacOSFileSecondsToTimeDate(struct TimeDate_t *Output,LongWord Time);
extern void FASTCALL MacOSPurgeDirCache(void);
extern Word FASTCALL DoMacEvent(Word Mask,struct EventRecord *MyRecord);
extern Word FASTCALL FixMacKey(struct EventRecord *MyRecord);
extern Word FASTCALL MacOSIsTrapAvailable(int TrapNum);
extern Word FASTCALL MacOSIsQuickTimePowerPlugAvailable(void);
extern Word FASTCALL MacOSGetSoundManagerVersion(void);
extern Word FASTCALL MacOSGetInputSprocketVersion(void);
extern Word FASTCALL MacOSGetDrawSprocketVersion(void);
extern Word FASTCALL MacOSGetAppleShareVersion(void);
extern void FASTCALL MacOSSetExtensionsPrefix(Word PrefixNum);
extern long FASTCALL MacOpenFileForRead(const char *Filename);
extern Word FASTCALL MacOpenControlPanel(LongWord type,const char *defaultname,Word Continue);
extern Word FASTCALL MacLaunch(short vref,long dirID, const char *name,Word Continue);
extern void FASTCALL MacOSKillProcess(struct ProcessSerialNumber *victim);
extern void FASTCALL MacOSKillAllProcesses(void);
extern struct LibRef_t * FASTCALL MacOSGetInterfaceLib(void);
extern struct LibRef_t * FASTCALL MacOSDriverLoaderLib(void);
extern Word FASTCALL MacOSDialogControlGetValue(MacDialogRef dialog,Word item);
extern void FASTCALL MacOSDialogControlSetValue(MacDialogRef dialog,Word item,Word Value);
extern void FASTCALL MacOSDialogControlToggleValue(MacDialogRef dialog,Word item);
#if TARGET_API_MAC_CARBON
extern void * FASTCALL MacOSMachOToCFM(void *ProcPtr);
extern void FASTCALL MacOSMachOToCFMDelete(void *MachPtr);
#endif
#endif

extern void FASTCALL MacOSInitTools(void);
extern void FASTCALL MacOSEjectCD(void);
extern Word FASTCALL MacOSIsAltivecPresent(void);
extern Word FASTCALL MacOSGetOSVersion(void);

#if TARGET_API_MAC_CARBON || defined(__MACOSX__)

typedef struct MacOSXFramework_t {	/* Data for a MacOSX library reference */
	struct __CFBundle *LibBundle;	/* Master bundle reference */
} MacOSXFramework_t;

extern MacOSXFramework_t *FASTCALL MacOSGetFoundationFramework(void);
extern Word FASTCALL MacOSXFrameworkInit(MacOSXFramework_t *Input,const char *FrameWorkName);
extern void * FASTCALL MacOSXFrameworkGetProc(MacOSXFramework_t *Input,const char *ProcNam);
extern void FASTCALL MacOSXFrameworkDestroy(MacOSXFramework_t *Input);
extern void FASTCALL MacOSXTimeDateFromUTCTime(struct TimeDate_t *Output,const struct UTCDateTime *Input);
#endif

#endif

#if defined(__INTEL__) && !defined(__BEOS__)
#include <poppack.h>
#elif defined(__MWERKS__)
#pragma options align=reset
#endif

#if defined(__MWERKS__)
#pragma ANSI_strict reset
#pragma enumsalwaysint reset
#endif

#ifdef __cplusplus
}
#ifndef __BURGERNONEW__
inline void * ANSICALL operator new(size_t size)
	{ return AllocAPointerClear(size); }
inline void * ANSICALL operator new(size_t size,const char *FooFile,Word FooLine)
	{ return DebugAllocAPointerClear(size,FooFile,FooLine); }
inline void ANSICALL operator delete(void *p)
	{ DeallocAPointer(p); }
#ifndef __MRC__
inline void * ANSICALL operator new[](size_t size)
	{ return AllocAPointerClear(size); }
inline void * ANSICALL operator new[](size_t size,const char *FooFile,Word FooLine)
	{ return DebugAllocAPointerClear(size,FooFile,FooLine); }
inline void ANSICALL operator delete[](void *p)
	{ DeallocAPointer(p); }
#endif
#if _DEBUG
#define new new(__FILE__,__LINE__)
#endif
#endif
#endif

#endif

