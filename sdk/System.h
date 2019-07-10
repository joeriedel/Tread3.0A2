///////////////////////////////////////////////////////////////////////////////
// System.h
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
//
// Redistribution and use in source and binary forms, 
// with or without modification, are permitted provided 
// that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution. 
//
// Neither the name of the <ORGANIZATION> nor the names of its contributors may be 
// used to endorse or promote products derived from this software without specific 
// prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEM_H
#define SYSTEM_H

///////////////////////////////////////////////////////////////////////////////
// The basic system works by storing a list of objects, and allowing mouse   //
// based interactions and what-not on those objects. All objects in the map  //
// have the ability to expose a property list with basic types (see			 //
// CObjProp). Tread provides a standard type interface to each of these      //
// propert types. All properties can be compound, allowing the user to       //
// select from a list.														 //
//                                                                           //
// The undo system is fully generalized. As long as an object can properly   //
// copy itself, then all operations such as moving and such will have the    //
// ability to be undone. Note that manipulations to the properties of an     //
// object are not undo-able.                                                 //
//                                                                           //
// All objects derrive from the CMapObject class. This class exposes the     //
// objects interface, the user, the maps file system, the undo               //
// system and the rendering system.                                          //
//                                                                           //
// Most classes are derrived from the CLLObject class. By derriving from     //
// this class those objects or any further derrived classes can be held      //
// in the CLinkedList template class. The linked list system is used         //
// extensively in the editor.                                                //
//                                                                           //
// See the following class definitions for further explanations.             //
///////////////////////////////////////////////////////////////////////////////

#include "os.h"
#include "LinkedList.h"
#include "Math3D.h"
#include <fstream>
#include "r_sys.h"
#include "c_tokenizer.h"

#define TREAD_API_VERSION 1

///////////////////////////////////////////////////////////////////////////////
// All our fonts are GL based, and these are the compiled command lists that //
// expose the GL font commands.                                              //
//                                                                           //
// The fonts are created via wgl calls in CMainFrm on startup.               //
///////////////////////////////////////////////////////////////////////////////

#define TEXTURE_NAME_FONT	1
#define VIEWTYPE_FONT		256
#define BOXSIZE_FONT		512

#define ALTKEYDOWN()		(GetAsyncKeyState(VK_MENU)&(1<<16))

///////////////////////////////////////////////////////////////////////////////
// Global axis defines. These should always be unit vectors, and should      //
// always point down a primary axis. They will not work in every place they  //
// are used if they have anything spread across multiple components.         //
///////////////////////////////////////////////////////////////////////////////

OS_FNEXP extern const vec3 sysAxisX;
OS_FNEXP extern const vec3 sysAxisY;
OS_FNEXP extern const vec3 sysAxisZ;

#define CLAMP_VAL(a, low, high) ( ((a)<(low))?(low):((a)>(high))?(high):(a) )
#define MAX_VAL(a, max) ( (a)>(max)?(a):(max) )
#define MIN_VAL(a, min) ( (a)<(min)?(a):(min) )

///////////////////////////////////////////////////////////////////////////////
// Returns the 0-2 axis index of a vectors primary index. The vector is      //
// assumed to have only one non-zero component.                              //
///////////////////////////////////////////////////////////////////////////////
#define QUICK_AXIS( v ) ( (fabsf((v)[0]) > 0) ? 0 : (fabsf((v)[1]) > 0) ? 1 : 2 )

///////////////////////////////////////////////////////////////////////////////
// Returns the sign of a number, -1, 0, or 1.                                //
///////////////////////////////////////////////////////////////////////////////
#define SIGN( f ) ( (f) > 0 ? 1 : (f) < 0 ? -1 : 0 )

///////////////////////////////////////////////////////////////////////////////
// Defines the size in pixels of the 2D and 3D screen space pick area.       //
///////////////////////////////////////////////////////////////////////////////
#define DEFAULT_2D_PICK_SIZE	8
#define DEFAULT_3D_PICK_SIZE	10

///////////////////////////////////////////////////////////////////////////////
// Defines the various cursors that can be set with Sys_SetCursor().         //
///////////////////////////////////////////////////////////////////////////////
#define TC_DEFAULT			-1
#define TC_IDC_ARROW		0
#define TC_IDC_IBEAM		1
#define TC_IDC_WAIT			2
#define TC_IDC_CROSS		3
#define TC_IDC_UPARROW		4
#define TC_IDC_SIZEALL		5
#define TC_IDC_SIZENWSE		6
#define TC_IDC_SIZENESW		7
#define TC_IDC_SIZEWE		8
#define TC_IDC_SIZENS		9
#define TC_NUMCURSORS		10

///////////////////////////////////////////////////////////////////////////////
// The 4 views that are used in the editor all have a type specifier.        //
// Certain function calls to redraw views and stuff can take combinations.   //
///////////////////////////////////////////////////////////////////////////////
#define VIEW_TYPE_NONE		0x00
#define VIEW_TYPE_3D		0x01
#define VIEW_TYPE_FRONT		0x02
#define VIEW_TYPE_BACK		0x04
#define VIEW_TYPE_LEFT		0x08
#define VIEW_TYPE_RIGHT		0x10
#define VIEW_TYPE_TOP		0x20
#define VIEW_TYPE_BOTTOM	0x40
#define VIEW_TYPE_TEXTURE	0x80
#define VIEW_FLAG_ALL		0xFFFFFFFF
#define VIEW_FLAG_2D		(VIEW_FLAG_ALL&~(VIEW_TYPE_3D|VIEW_TYPE_TEXTURE))
#define VIEW_FLAG_MAP		(VIEW_FLAG_ALL&~(VIEW_TYPE_TEXTURE))

///////////////////////////////////////////////////////////////////////////////
// Determines the ratios between micro and milli seconds and whether or not  //
// the editor uses a micro or milli second clock base.                       //
///////////////////////////////////////////////////////////////////////////////
#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_SECOND	1000000
#define MICROSECONDS_PER_MILLISECOND (MICROSECONDS_PER_SECOND/MILLISECONDS_PER_SECOND)
#define MICROSECONDS_TO_MILLISECONDS(x) ( x*(1.0f/MICROSECONDS_PER_MILLISECOND) )
#define MILLISECONDS_TO_MICROSECONDS(x) ( x*MICROSECONDS_PER_MILLISECOND )
#define GAME_TICKS_TO_MICROSECONDS(x)	( x*(1.0f/GAME_TICKS_PER_MICROSECOND) )
#define GAME_TICKS_TO_MILLISECONDS(x)	( x*(1.0f/GAME_TICKS_PER_MILLISECOND) )

#define GAME_TIMEBASE_MICROSECONDS 0

#if GAME_TIMEBASE_MICROSECONDS == 1
#define GAME_TICKS_PER_SECOND 			MICROSECONDS_PER_SECOND
#define GAME_TICKS_PER_MICROSECOND		1
#define GAME_TICKS_PER_MILLISECOND		MICROSECONDS_PER_MILLISECOND
#else
#define GAME_TICKS_PER_SECOND			MILLISECONDS_PER_SECOND
#define GAME_TICKS_PER_MICROSECOND		((vec_t)MILLISECONDS_PER_SECOND/(vec_t)MICROSECONDS_PER_SECOND)
#define GAME_TICKS_PER_MILLISECOND		1
#endif

///////////////////////////////////////////////////////////////////////////////
// Overrides certain event flags. This was originally done for portability   //
// although this editor is probably the least portable piece of code ever    //
// written.                                                                  //
///////////////////////////////////////////////////////////////////////////////
#define MS_LBUTTON		MK_LBUTTON
#define MS_RBUTTON		MK_RBUTTON
#define MS_MBUTTON		MK_MBUTTON
#define MS_SHIFT		MK_SHIFT
#define MS_CONTROL		MK_CONTROL
#define MS_ALT			0x80000000

#define MS_BUTTONS		(MS_LBUTTON|MS_RBUTTON|MS_MBUTTON)

///////////////////////////////////////////////////////////////////////////////
// Degrees to radians and whatnot.                                           //
///////////////////////////////////////////////////////////////////////////////
#define DEGREES_TO_RADIANS(a) 		( ((a)/180.0f)*PI )
#define RADIANS_TO_DEGREES(a) 		( ((a)/PI)*180.0f )
#define QUARTER_CIRCLE_RADIANS		( PI/2 )
#define QUARTER_CIRCLE_DEGREES		( 180.0f/2 )

///////////////////////////////////////////////////////////////////////////////
// All objects in a map must have a corresponding bitflag representing their //
// class type so they can be properly saved and restored from disk.          //
///////////////////////////////////////////////////////////////////////////////
#define MAPOBJ_CLASS_BRUSH			0x00000001
#define MAPOBJ_CLASS_ENTITY			0x00000002
#define MAPOBJ_CLASS_GROUP			0x00000004
#define MAPOBJ_CLASS_FIRST_USER_BIT 0x00004000
#define MAPOBJ_CLASS_ALL			0xFFFFFFFF

///////////////////////////////////////////////////////////////////////////////
// Subclasses are just special class-type bits that can be used to make      //
// minor functionality deviations from the same object type.                 //
//                                                                           //
// For example, entities that own brushes are not drawn. And brushes that    //
// are owned draw in a yellowish color.                                      //
//                                                                           //
// These bits are mainly used so the document can determine if anything that //
// is owned is selected to make alterations to available manipulations.      //
// The same goes for owner objects.                                          //
///////////////////////////////////////////////////////////////////////////////
#define MAPOBJ_SUBCLASS_NONE			0x00000001
#define MAPOBJ_SUBCLASS_OWNED			0x00000002
#define MAPOBJ_SUBCLASS_OWNER			0x00000004
#define MAPOBJ_SUBCLASS_FIRST_USER_BIT  0x00004000

class CMainFrame;
class CChildFrame;
class CTreadDoc;
class CUndoRedoAction;
class CMapView;
class CMapObject;
class CConsoleDialog;
class CObjPropView;

///////////////////////////////////////////////////////////////////////////////
// COrientation defines a rotation in space as a matrix, a quaternion, and   //
// pure euler angles. It also stores the up/left/front vectors for the given //
// orientation.                                                              //
//                                                                           //
// This is used mainly for viewer transforms, although originally I was      //
// going to use it to correctly compound a given set of transforms (which    //
// would obliterate euler angles) but this didn't actually happen.           //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP COrientation
{
public:

	COrientation();
	virtual ~COrientation();

	quat q;
	mat3x3 m;
	vec3 angles;
	vec3 lft;
	vec3 up;
	vec3 frw;
	vec3 delta_angles;
	vec3 org_angles;

	void make_vecs();
	void set_q( const quat& qt );
	void set_angles( const vec3& a );
};

///////////////////////////////////////////////////////////////////////////////
// CObjProp encapsulates a single object property that can be manipulated by //
// the user. It can be any one of a number of different types of properties. //
// However, the property type is only an end user convenience allowing the   //
// editor to facilitate a more natural editing of the property (i.e. color   //
// picker). Properties can contain a list of sub properties that can be      //
// used as a list of choices or options. Choices by definition means that    //
// the user can pick a single one from the list, while options means that    //
// the user can turn each one off or on seperatly (like flags).              //
//                                                                           //
// Regardless of the type of property the user can always manually type in   //
// the value into an edit field.                                             //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CObjProp : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	CObjProp();
	CObjProp( const CObjProp& c );
	virtual ~CObjProp();
	
	CObjProp& operator = ( const CObjProp& p );

	void Copy( const CObjProp& c );
	void Inherit(const CObjProp &c);

	const char* GetName();
	void SetName( const char* name );

	const char* GetDisplayName();
	void SetDisplayName( const char* name );

	int GetType();
	void SetType( int type );

	float GetFloat();
	void SetFloat( float f );
	
	int GetInt();
	void SetInt( int i );
	
	const char* GetString();
	void SetString( const char* string );
	
	vec3 GetVector();
	void SetVector( const vec3& vec );

	void SetSubType( bool bChoicesTrueFlagsFalse );
	bool GetSubType();

	CLinkedList<CObjProp>* GetChoices();
	void AddChoice( CObjProp* choice );

	void SetValue( CObjProp* prop );

	void SetListItem( int item );
	int GetListItem();

	static const int integer;
	static const int floatnum;
	static const int string;
	static const int vector;
	static const int facing;
	static const int color;
	static const int script;

	static CObjProp* FindProp( CLinkedList<CObjProp>* list, const char* name );

private:

	CString m_sVal;
	CString m_sName;
	CString m_sDisplayName;
	int m_nType;
	int m_nListItem;
	bool m_bSubTypeChoices;

	CLinkedList<CObjProp> m_Choices;
};

///////////////////////////////////////////////////////////////////////////////
// CPickObject is the interface exposed to the picking system, used to       //
// select the object by clicking on it. Any object that can be selected via  //
// a mouse click must expose this interface. This interface allows the       //
// object to respond to mouse based user input and commands. The class has   //
// built in support for a parent-child chain of objects, allowing any object //
// in the the chain to defer messages to another object.                     //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CPickObject
{
public:

	DECLARE_TREAD_NEW();

	CPickObject();
	virtual ~CPickObject();

	//
	// Returns the parent of the object. If immediate == false then the highest level
	// parent is returned. If immediate == true then the first parent is returned.
	// If no parent is present then the object returns NULL.
	//
	CPickObject* GetParent( bool immediate = false ); 

	//
	// Sets the immediate parent of the object.
	//
	void SetParent( CPickObject* p );

	//
	// For Manipulators Only: This allows a manipulator to only show up in certain views.
	//
	void SetViewFlags( int flags );
	int  GetViewFlags();

	//
	// The various messages are described below, however, to actually invoke them, these functions should be used.
	// You should never call an On* function directly (unless directed to do so by your doctor).
	// If message_parent == true, then the message will go to the highest level parent.
	//
	bool Select    ( CTreadDoc* pDoc, CPickObject* pSrc = 0, bool message_parent = true );
	bool Deselect  ( CTreadDoc* pDoc, CPickObject* pSrc = 0, bool message_parent = true );
	void MouseDown ( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	void MouseMove ( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	void MouseUp   ( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	void MouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	void MouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	bool PopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc = 0, bool message_parent = true );
	
	//
	// Called when the object is selected. This is typically generated by an object when OnMouseDown() is called.
	// All objects that extend CPickObject should select themselves when this routine is called.
	// When called the pSrc parameter will be the sub-object where the message originall originated from, or null if
	// it is this object.
	//
	virtual bool OnSelect( CTreadDoc* pDoc, CPickObject* pSrc );
	//
	// Called to deselect the object. This should also be implemented by anything that extends CPickObject. See
	// Select() for parameter details.
	//
	virtual bool OnDeselect( CTreadDoc* pDoc, CPickObject* pSrc );

	//
	// The following are the various mouse events that can occur. The pView parameter is the view that the event
	// occured in. The nMX, nMY parameters are the screen space coordinates of the event. nButtons are the
	// buttons states (MS_LBUTTON|MS_RBUTTON|MS_MBUTTON) that were present when the even occured.
	//
	virtual void OnMouseDown( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	virtual void OnMouseUp( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	virtual void OnMouseMove( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	//
	// Mouse enter and leave message means that the mouse moved over the object without any mouse buttons down.
	// These message only occur for manipulators.
	//
	virtual void OnMouseEnter( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );
	virtual void OnMouseLeave( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

	//
	// The object should display its context menu when it recieves this message. If it doesn't have
	// one then the message should be ignored.
	//
	virtual bool OnPopupMenu( CMapView* pView, int nMX, int nMY, int nButtons, CPickObject* pSrc );

private:

	CPickObject* m_parent;
	int m_viewflags;
};

///////////////////////////////////////////////////////////////////////////////
// CManipulator defines the base object class for things that can manipulate //
// an object in response to mouse input. For example, the axis gizmo of a    //
// selected object is a collection of 3 different manipulator objects each   //
// programmed to drag the object on each axis when clicked and dragged.      //
//                                                                           //
// To create a manipulator you need to override this class and respond to    //
// the appropriate CPickObject messages for your new manipulator. You will   //
// also need to add and remove your manipulator from the map via the         //
// document functions AddManipulatorToMap and DetachManipulator.             //
//                                                                           //
// You also need to override OnDraw() to render the manipulator with the     //
// appropriate GL commands. When OnDraw() is called the correct GL states    //
// have been set for solid fill (nontextured) drawing.                       //
//                                                                           //
// You should always render your objects using glVertex3* calls, or any      //
// 3D rendering calls. You should not try to render your object two          //
// dimensionally as it will not work (even with 2D views). 2D views setup    //
// an orthographic viewing matrix, and each different view type draws from   //
// a different side, so you should be passing your full 3D coordinates to    //
// GL when you draw.                                                         //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CManipulator : public CPickObject, public CLLObject
{
public:
	
	DECLARE_TREAD_NEW();

	bool m_bSelected;

	CManipulator();
	virtual ~CManipulator();

	virtual void OnDraw( CMapView* pView );

friend class CTreadDoc;
private:

	void* m_pMapList;
};

///////////////////////////////////////////////////////////////////////////////
// CRenderMesh encapsulates the majority of the interface between high level //
// objects and the rendering sub-system. Meshes can be created once and      //
// passsed repeatedly to the rendering system, so don't create it every time //
// you render an object (unless you have to). By setting different fields in //
// the class you can control how a given object is drawn. A render mesh has  //
// the built in capability to render solid, textured, or with a shader. Only //
// map objects will preview correctly (in preview mode) when their mesh      //
// specifies a shader (manipulators and others are not animated).            //
//                                                                           //
// The mesh supports a high density mesh drawn with glDrawElements for high  //
// speed rendering, or it supports a slower more flexible polygon list. Both //
// types can be used at the same time. The polygon lists always references   //
// indices into the mesh's XYZ point list.                                   //
//                                                                           //
// Please see ents.cpp or brush.cpp for working examples of the mesh use,    //
// and how all it's subtleties work.                                          //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CRenderMesh
{
public:

	DECLARE_TREAD_NEW();

	CRenderMesh();
	virtual ~CRenderMesh();
	
	//
	// Free's any allocated memory on xyz, normals[], st, rgba, tris.
	//
	void FreeMesh();

	//
	// Allocates:
	//
	// xyz[num_pts]
	// normals[0][num_pts]
	// normals[1][num_pts]
	// normals[2][num_pts]
	// st[num_pts]
	// rgba[num_pts]
	// tris[num_tris*3]
	//
	void AllocMesh( int num_pts, int num_tris );

	//
	// cmds can be either GL_TRIANGLES or GL_LINES, anything else is undefined. GL_LINES causes
	// the tris field to be used as indices for a line loop, with N entries drawing N-1 lines. A 
	// -1 can be used to specify a break in the loop. num_tris will specifiy how many indices there 
	// are in tris, instead of the assumed num_tris*3 for triangles.
	//
	int cmds;

	//
	// Specifies the number of unique vertices in the mesh. Each vertex has an xyz, a normal, an st, and a color.
	//
	int num_pts;

	//
	// Specifies the vertices.
	//
	vec3* xyz;

	//
	// Specifies the normals (normals[0]), binormals (normals[1], and tangents (normals[2]) of the vertex (for bump mapping). 
	// The tangent and binormal can be generated from R_GetTXSpaceVecs(). NOTE: normals[1] and normals[2] cannot be NULL if
	// the mesh uses a shader that specifies bump mapping.
	//
	vec3* normals[3];

	//
	// Specifies the texture coordinate for the vertex.
	//
	vec2* st;

	//
	// Specifies the packed RGBA colors of the vertices.
	//
	unsigned int* rgba;

	//
	// Specifies the wireframe color of the mesh in the 2D views.
	//
	unsigned int color2d;

	//
	// Specifies the color of the non-selected 3D mesh.
	//
	unsigned int color3d;

	//
	// Specifies the wireframe color of the mesh in the 3D views.
	//
	unsigned int wireframe3d;

	//
	// Specifies the diffuse material reflection in the textured 3D view when shading is on.
	//
	float fcolor3d[4];

	//
	// Specifies the number of triangles in the mesh (or the number if line loop points).
	//
	int num_tris;

	//
	// Specifies the indices into the vertex arrays. Should have num_tris*3 indices for triangles. 
	//
	unsigned short* tris;
	//unsigned int* tris;

	//
	// Specifies the shader that the mesh should be drawn with. Can be NULL.
	//
	CShader* shader;

	//
	// Specifies the texture that the mesh should be drawn with. This is only used if the shader field is NULL.
	// If this field is NULL then texturing is disabled and the mesh draws with solid color.
	//
	struct RTex_s* texture;

	//
	// The pick object that is associated with this mesh. This field ties the picking system together. When a mesh is
	// moused, this field specifies the pick object to talk to. If this is null, the mesh cannot be picked.
	//
	CPickObject* pick;

	//
	// If true, the mesh should be drawn solid in the 2D views.
	//
	bool solid2d;

	//
	// If true, then the mesh can be drawn selected.
	//
	bool allow_selected;

	//
	// If true then the mesh can be drawn in wireframe mode.
	//
	bool allow_wireframe;

	//
	// If cmds == GL_LINES, then this field specifies the line thickness in pixels.
	//
	int line_size;

	//
	// The mesh encapsulates a flexible polygon type to drawn a list of arbitrary poly's instead of just
	// a triangle or line mesh.
	//
	class poly_s : public CLLObject
	{
	public:
		poly_s() : CLLObject()
		{
			num_pts = 0;
			indices = 0;
			shader = 0;
			pick = 0;
			selected = 0;
			texture = 0;
			selcolor = 0xFFFFFFFF;
		}

		virtual ~poly_s()
		{
			if( indices )
				delete[] indices;
		}

		//
		// Defines the number of points on the polygon.
		//
		int num_pts;

		//
		// Defines the indices into the mesh's vertices for the polygon.
		//
		unsigned short* indices;

		// Specifies the shader the polygon is to be drawn with. Can be NULL.
		//
		CShader* shader;

		//
		// Specifies the texture the polygon is to be drawn with. Can be NULL.
		//
		struct RTex_s* texture; // If shader is null, then this can be used. if this is null also, then texure is disabled.

		//
		// If true the polygon is to be drawn using the selected color.
		//
		bool selected;

		//
		// The RGBA packed color to draw the polygon when it's selected.
		//
		unsigned int selcolor;

		//
		// The pick object to talk to for mouse events on this polygon.
		//
		CPickObject* pick;
	};

	//
	// The linked list of poly's to draw.
	//
	CLinkedList<poly_s> polys;
};

///////////////////////////////////////////////////////////////////////////////
// Each Undo/Redo object must have a class number for disk serialization.    //
///////////////////////////////////////////////////////////////////////////////
#define UNDOREDO_CLASS_NONE			0
#define UNDOREDO_CLASS_GENERIC		1
#define UNDOREDO_CLASS_DESELECT		2
#define UNDOREDO_CLASS_CREATE		3
#define UNDOREDO_CLASS_DELETE		4
#define UNDOREDO_CLASS_PASTE		5
#define UNDOREDO_CLASS_LINK			6
#define UNDOREDO_CLASS_UNLINK		7
#define UNDOREDO_FIRST_USER         64

///////////////////////////////////////////////////////////////////////////////
// The undo redo system is generalized to the point that if an object        //
// properly derrives the CMapObject class, then all it's manipulations will  //
// automatically be recorded and undo-redo able. The object must properly    //
// behave and respond to the Add/Remove messages properly.                   //
//                                                                           //
// To make a specific undo/redo class that handles a specialized action you  //
// must derrive from the CUndoRedoAction, and integrate it accordingly into  //
// the CTreadDoc class. Then, before the operation is executed you need      //
// to make an instance of your class and process any necessary information   //
// to restore the states before and after the action.                        //
//                                                                           //
// Great care must be made to ensure that each actions undo/redo class be as //
// absolutely independant of the state of the map as possible. It should     //
// try to be loosly bound to an object (via UID's) incase some type of       //
// corruption takes place.                                                   //
//                                                                           //
// Note that the undo history is saved with the map so the undo actions      //
// must be able to serialize their data to and from the disk.                //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CUndoRedoAction : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	CUndoRedoAction();
	virtual ~CUndoRedoAction();

	//
	// Sets the title of this particular action. This will show up in the edit menu.
	//
	void SetTitle( const char* title );

	//
	// Retrieves the title of the action.
	//
	const char* GetTitle();

	//
	// Called by the system to apply the undo operation to the specified document.
	//
	virtual void Undo( CTreadDoc* pDoc ) = 0;
	//
	// Called by the system to apply the redo operation to the specified document.
	// This should effectively put the document back in the state it was before Undo()
	// was called. NOTE: You are guaranteed to have Undo() called before any Redo(). You
	// are also guaranteed to have neither called several times in succession.
	//
	virtual void Redo( CTreadDoc* pDoc ) = 0;

	//
	// Returns the class bits of this action.
	//
	virtual int GetClass() = 0;

	//
	// Writes the given action to the specified file.
	// TRUE equals success.
	//
	virtual bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );

	//
	// Reads the given action from the specified file.
	// TRUE equals success.
	//
	virtual bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	//
	// Called to have the action object connect any entities it contains to the
	// current set of entity definitions. The action should simply call all entities
	// OnConnectToEntDefs().
	//
	virtual void ReconnectEntDefs(CTreadDoc* doc);

	//
	// Static member to create an UndoRedoAction based on class bits.
	//
	static CUndoRedoAction* UndoRedoFromClass( int classbits );

	//
	// Static members to read/write an UndoRedoAction to/from a file.
	//
	static CUndoRedoAction* ReadUndoRedoFromFile( CFile* file, CTreadDoc* doc, int nVersion );
	static bool WriteUndoRedoToFile( CFile* file, CTreadDoc* doc, int nVersion, CUndoRedoAction* obj );

private:

	CString m_sTitle;
};

///////////////////////////////////////////////////////////////////////////////
// CGenericUndoRedoAction will handle any and all kinds of full generic      //
// actions on a object. It assumes that the objects being manipulated are in //
// the documents selection list. It uses a simple brute force method of      //
// making a duplicate copy of all the objects on the selection list and has  //
// the ability to simply swap between the before and after sets. The action  //
// is bound to the objects via their UID's.                                  //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CGenericUndoRedoAction : public CUndoRedoAction
{
public:
	
	CGenericUndoRedoAction();
	virtual ~CGenericUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );

	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );

private:

	CLinkedList<CMapObject> m_UndoList;
	CLinkedList<CMapObject> m_RedoList;

	int* m_objs;
	int m_numobjs;

	void MakeRedoList( CTreadDoc* pDoc );
};

///////////////////////////////////////////////////////////////////////////////
// CDeselectUndoRedoAction allows the user to undo/redo a deselection. Note  //
// that a selection typically is a deselection of one or more objects and    //
// the selection of another.                                                 //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CDeselectUndoRedoAction : public CUndoRedoAction
{
public:

	CDeselectUndoRedoAction();
	virtual ~CDeselectUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );

private:

	int* m_nUndoUIDs, *m_nRedoUIDs;
	int m_nNumUndoUIDs, m_nNumRedoUIDs;
};

///////////////////////////////////////////////////////////////////////////////
// CCreateUndoRedoAction allows the user to undo/redo the creation of an     //
// object via the ALT+RCLICK method.                                         //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CCreateUndoRedoAction : public CUndoRedoAction
{
public:

	CCreateUndoRedoAction();
	virtual ~CCreateUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );

	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CMapObject* pObj, CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );

private:

	int* m_nUndoUIDs;
	int m_nNumUndoUIDs;
	CMapObject* m_pRedoObj;
	int m_nCreateUID;
};

///////////////////////////////////////////////////////////////////////////////
// CDeleteUndoRedoAction allows the user to undo/redo the deletion of an     //
// object or set of objects.                                                 //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CDeleteUndoRedoAction : public CUndoRedoAction
{
public:

	CDeleteUndoRedoAction();
	virtual ~CDeleteUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );
	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );

private:

	int* m_nUndoUIDs;
	int m_nNumUndoUIDs;
	int* m_nObjs;
	int m_nNumObjs;

	CLinkedList<CMapObject> m_UndoList;
};

///////////////////////////////////////////////////////////////////////////////
// CPasteUndoRedoAction allows the user to undo/redo pasting of the          //
// clipboard into their map.                                                 //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CPasteUndoRedoAction : public CUndoRedoAction
{
public:

	CPasteUndoRedoAction();
	virtual ~CPasteUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );
	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CLinkedList<CMapObject>* pPaste, CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );

private:

	int* m_nUndoUIDs;
	int m_nNumUndoUIDs;

	int* m_nObjUIDs;
	int m_nNumObjUIDs;

	CLinkedList<CMapObject> m_RedoList;
};

///////////////////////////////////////////////////////////////////////////////
// CLinkUndoRedoAction allows the user to undo/redo the linking of brushes   //
// to an entity.                                                             //
///////////////////////////////////////////////////////////////////////////////

class CEntity;
class OS_CLEXP CLinkUndoRedoAction : public CUndoRedoAction
{
public:

	CLinkUndoRedoAction();
	virtual ~CLinkUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );
	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CEntity* entity ); // called to init *after* link has been done.

private:

	CEntity* m_ent;
	int m_uid;
};

///////////////////////////////////////////////////////////////////////////////
// CUnlinkUndoRedoAction allows the user to undo/redo the unlinking of       //
// brushes from any entity.                                                  //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CUnlinkUndoRedoAction : public CUndoRedoAction
{
public:

	CUnlinkUndoRedoAction();
	virtual ~CUnlinkUndoRedoAction();

	void Undo( CTreadDoc* pDoc );
	void Redo( CTreadDoc* pDoc );
	void ReconnectEntDefs(CTreadDoc* doc);

	int GetClass();

	bool WriteToFile( CFile* file, CTreadDoc* doc, int nVersion );
	bool ReadFromFile( CFile* file, CTreadDoc* doc, int nVersion );

	void InitUndo( CEntity* entity ); // called to init *after* unlink.

private:

	CEntity* m_ent;
	int m_uid;
};

///////////////////////////////////////////////////////////////////////////////
// CUndoRedoManager manages the undo/redo history of a given map (document). //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CUndoRedoManager
{
public:

	CUndoRedoManager();
	virtual ~CUndoRedoManager();

	int GetUndoHistoryCount();

	void AddUndoItem( CUndoRedoAction* item );
	void ClearUndoStack();

	void UpdateRedoMenu( CCmdUI* pUI );
	void UpdateUndoMenu( CCmdUI* pUI );

	void PerformUndo( CTreadDoc* pDoc );
	void PerformRedo( CTreadDoc* pDoc );

	bool WriteUndoHistoryToFile( CFile* file, CTreadDoc* doc, int version );
	bool ReadUndoHistroryFromFile( CFile* file, CTreadDoc* doc, int version );

	void ReconnectEntDefs(CTreadDoc* doc);

	static void CopyObjectList( CLinkedList<CMapObject>* pSrc, CLinkedList<CMapObject>* pDst, CTreadDoc* pDoc );
	static void DuplicateListIntoDocument( CLinkedList<CMapObject>* pSrc, CTreadDoc* pDoc );

	static int* GetObjectUIDs( CLinkedList<CMapObject>* pList, int* num );
	static void DeleteUIDs(int *ids);
	static int* AllocateUIDs(int num);
	static void SelectObjectsByUIDs( CTreadDoc* pDoc, int* uids, int num_uids );

private:

	int m_nNum;

	CLinkedList<CUndoRedoAction> m_UndoRedo;
	CUndoRedoAction* m_pUndo, *m_pRedo;

	void PurgeFromPos( CUndoRedoAction* pos );
};

///////////////////////////////////////////////////////////////////////////////
// CObjectGroup encapsulates the functionality of creating and managing      //
// groups of objects in a map. Objects are bound by their UID's.             //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CObjectGroup : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	CObjectGroup();
	virtual ~CObjectGroup();

	//
	// Used for marking a group during extended operations.
	//
	bool IsMarked();
	void SetMarked( bool marked = true );

	//
	// Makes a group from the specified list of objects.
	//
	void MakeList( CLinkedList<CMapObject>* pList );

	//
	// Return true if the group is selected.
	//
	bool IsSelected( CTreadDoc* pDoc );

	//
	// Selects all objects in the group.
	//
	void SelectObjects( CTreadDoc* pDoc );

	//
	// Deselects all objects in the group.
	//
	void DeselectObjects( CTreadDoc* pDoc );

	//
	// Removes the group and orphans the objects.
	//
	void RemoveObject( int uid );

	//
	// Returns true if this group doesn't have any objects.
	//
	bool IsNullGroup( CTreadDoc* pDoc );

	//
	// Returns true if the group is visible.
	//
	bool IsVisible();

	//
	// Shows/Hides all objects in the group.
	//
	void SetVisible( CTreadDoc* pDoc, bool vis = true );

	//
	// Detaches all objects in the group, but remembers their UID's.
	//
	void OrphanObjects( CTreadDoc* pDoc );

	//
	// Sets the name of the group.
	//
	void SetName( const char* name );

	//
	// Gets the name of the group.
	//
	const char* GetName();

	//
	// Set's the UID of the group.
	//
	void SetUID( int uid );

	//
	// Get's the UID of the group.
	//
	int GetUID();

	//
	// Reads/Write the group to/from disk.
	void WriteToFile( CFile* file, int nVersion );
	void ReadFromFile( CFile* file, int nVersion );

	//
	// Set's the associated tree-view item.
	//
	void SetTreeItem( HTREEITEM hItem );
	HTREEITEM GetTreeItem();

private:

	int* m_nUIDs;
	int m_nNumUIDs;
	CString m_sName;
	bool m_bVisible;
	int m_nUID;
	HTREEITEM m_hTreeItem;
	bool m_bMarked;
};

///////////////////////////////////////////////////////////////////////////////
// CObjFilter encodes an object filter for selection of object via their     //
// properties.                                                               //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CObjectFilter
{
public:

	DECLARE_TREAD_NEW();

	CObjectFilter();
	virtual ~CObjectFilter();

	void SetBrushFilters( bool on );

	struct
	{
		bool nodraw;
		bool solid;
		bool window;
		bool water;
		bool areaportal;
		bool detail;
		bool noshadow;
		bool always_shadow;
		bool monster_clip;
		bool player_clip;
		bool corona_block;
		bool camera_clip;
		bool skybox;
		bool skyportal;
	} BrushFilter;

};

///////////////////////////////////////////////////////////////////////////////
// All map objects must declare their association via the OBJECT_TYPE_ bits. //
// This allows lights to properly preview that are character or world only.  //
///////////////////////////////////////////////////////////////////////////////

#define OBJECT_TYPE_WORLD		0x00000004
#define OBJECT_TYPE_BMODEL		0x00000008
#define OBJECT_TYPE_JMODEL		0x00000010
#define OBJECT_TYPE_SKEL		0x00000020
#define OBJECT_TYPE_FIRST_USER  0x00004000

///////////////////////////////////////////////////////////////////////////////
// CMapObject is the base class for all objects that can be manipulated in   //
// a map by the user. Most of the undo-redo system has automatic support of  //
// any object derrived correctly from this class. You can fill in the        //
// necessary routines to make your new object save/restore with the map file //
// and allow the user to visually manipulate its geometry and properties.    //
//                                                                           //
// See ents.cpp or brush.cpp for a working example.                          //
///////////////////////////////////////////////////////////////////////////////

class CEntity;
class OS_CLEXP CMapObject : public CPickObject, public CLLObject // for linked lists.
{
public:

	CMapObject();
	CMapObject( const CMapObject& mo );
	virtual ~CMapObject();

	//
	// Extended object manipulation marker.
	//
	void SetEntOwnerCall( bool owner_call );
	void SetEntGroupCall( bool group_call );

	//
	// Get/Set the UID of the group this object belongs too. -1 == no group.
	//
	int GetGroupUID();
	void SetGroupUID( int uid );

	//
	// Get/Set the owner of this object (for linked brushes). -1 == no owner.
	//
	int GetOwnerUID();
	void SetOwnerUID( int uid );

	//
	// Maps the objects owners UID to an entity. Returns NULL if there is no owner.
	//
	CEntity* GetOwner( CTreadDoc* pDoc );
	
	//
	// Maps the objects group UID to a group. Returns NULL if there is no owner.
	//
	CObjectGroup* GetGroup( CTreadDoc* pDoc );

	//
	// Called by an owner object to notify that brush contents may need to
	// be altered.
	//
	virtual void UpdateOwnerContents( CTreadDoc* pDoc );

	//
	// Returns true if the object is selected.
	//
	virtual bool IsSelected();

	//
	// Returns the 3D axis aligned bounding box (AABB) of the object in world space.
	//
	virtual void GetWorldMinsMaxs( vec3* pMins, vec3* pMaxs ) = 0;

	//
	// Returns the 3D AABB of the object in object space.
	//
	virtual void GetObjectMinsMaxs( vec3* pMins, vec3* pMaxs ) = 0;
	
	//
	// Get the world space origin of the object.
	//
	virtual vec3 GetObjectWorldPos() = 0;

	//
	// Set the world space origin of the object.
	//
	virtual void SetObjectWorldPos( const vec3& pos, CTreadDoc* pDoc );

	//
	// Transform or set the transform of the object to the given matrix.
	//
	virtual void SetObjectTransform( const mat3x3& m, CTreadDoc* pDoc );
	
	//
	// Nudge the object by the given vector (offsets the origin).
	//
	virtual void Nudge( const vec3& amt, CTreadDoc* doc );

	//
	// Scale the object around the given origin.
	//
	virtual void ScaleObject( const vec3& scale, const vec3& origin, const vec3& mins_before, const vec3& maxs_before, const vec3& mins_after, const vec3& maxs_after, CTreadDoc* pDoc );

	//
	// Called when requested to select ourselves. The default selects us.
	//
	virtual bool OnSelect( CTreadDoc* pDoc, CPickObject* pSrc );
	
	//
	// Called when requested to deselect ourselves. The default deselects us.
	//
	virtual bool OnDeselect( CTreadDoc* pDoc, CPickObject* pSrc );

	//
	// Returns the objects name.
	//
	virtual const char* GetName();

	//
	// Gets the objects name.
	//
	virtual void SetName( const char* szName );

	//
	// Apply the specified shader to the object.
	//
	virtual void SetShaderName( const char* szShaderName, CTreadDoc* pDoc );

	//
	// Reconnect the object with the specified shader (if the object uses it).
	//
	virtual void RepaintShader( const char* szShader, CTreadDoc* pDoc );
	
	//
	// Read/Write the object to/from a file.
	//
	virtual bool WriteToFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );
	virtual bool ReadFromFile( CFile* pFile, CTreadDoc* pDoc, int nVersion );

	//
	// Get the list of properties of the object. Doesn't have to return anything.
	//
	virtual CLinkedList<CObjProp>* GetPropList( CTreadDoc* pDoc );

	//
	// Sets an objects property. The object should copy the value of the passed property.
	//
	virtual void SetProp( CTreadDoc* pDoc, CObjProp* prop );

	//
	// Notifies the object it's been added to the specified document. This may be called
	// when an object is deselected. Use IsAttached() to determine if it's already been added.
	//
	virtual void OnAddToMap( CTreadDoc* pDoc );

	//
	// Notifies the object it's been removed from the specified document. The object
	// should null any ties it had to the document by the time this function returns.
	//
	virtual void OnRemoveFromMap( CTreadDoc* pDoc );

	//
	// Notifies the object that it's been added to the currently selected object list of the
	// specified document.
	//
	virtual void OnAddToSelection( CTreadDoc* pDoc );

	//
	// Returns true if the object is visible and should be drawn. This is also reflected in the
	// object list.
	//
	bool IsVisible();

	//
	// Sets the object to visible or invisible.
	//
	void SetVisible( CTreadDoc* pDoc, bool bVisible = true );

	//
	// Called to notify the object that its visibility has changed. 
	//
	virtual void OnSetVisible( bool bVisible, CTreadDoc* pDoc );

	//
	// Gets the class bits of the object.
	//
	virtual int GetClass() = 0;

	//
	// Gets the subclass bits of the object. 0 by default.
	//
	virtual int GetSubClass();

	//
	// Clones the object (i.e makes an exact duplicate). However the clone should not think that
	// it's attached to a document. Clone should also not copy things like UID, or tree-view positions, or the name.
	//
	virtual CMapObject* Clone() = 0;

	//
	// Copies the UID, the tree-view position, and the name of the specified object.
	//
	virtual void CopyState( CMapObject* src, CTreadDoc* pDoc );

	//
	// Returns the number of render meshes the object would like drawn for the given view.
	//
	virtual int GetNumRenderMeshes( CMapView* pView );

	//
	// Returns the indexed (0-(meshcount-1)) mesh for the given view that the object wants drawn.
	// Can return NULL.
	//
	virtual CRenderMesh* GetRenderMesh( int num, CMapView* pView );

	//
	// Returns the root name of the object. This is used when creating names for new objects. They are typically
	// ordered sequentially (rootname 1, rootname 2) etc, etc.
	//
	virtual const char* GetRootName();

	//
	// Requests that the object select itself if it uses the specified shader.
	//
	virtual void SelectByShader( const char* szShader, CTreadDoc* pDoc );

	//
	// Flips the object around the given origin on the given axis.
	//
	virtual void FlipObject( const vec3& origin, const vec3& axis );

	//
	// Returns true if the object wants to be accessable in the tree-view.
	//
	virtual bool CanAddToTree();

	//
	// Sets/Gets the objects unique identifier.
	//
	void SetUID( int uid );
	int GetUID();

	//
	// Sets/Gets the object associated tree-view item.
	//
	void SetTreeItem( HTREEITEM hItem );
	HTREEITEM GetTreeItem();

	//
	// Requests that the object snap its geometry and or origin to the specified grid on the specified axis.
	//
	virtual void SnapToGrid( float fGridSize, bool x, bool y, bool z );

	//
	// Notifies the object that the entity definitions and models have been reparsed and it should
	// update any linkages to those objects now.
	//
	virtual void OnConnectToEntDefs(CTreadDoc* doc);

	//
	// return true if the object has any errors, along with a message describing what the problem is.
	//
	virtual bool CheckObject( CString& message, CTreadDoc* pDoc );

#define OBJFILTER_IGNORE		0
#define OBJFILTER_HIDE			1
#define OBJFILTER_SHOW			2

	//
	// Returns an OBJECTFILTER_* flag based on whether object is filter by the specified filter set.
	//
	virtual int GetFilterState( CTreadDoc* doc, CObjectFilter* filter );

	//
	// Animation states are handled automatically by the CMapObject class. When an object is being animating
	// its states are saved and later restored when the animation is done.
	//
	// Tells the object that it is going to be animated and should save/restore its states.
	//
	void SetInAnimationMode( CTreadDoc* pDoc, bool animating = true );

	//
	// Returns true if the object is in animation mode.
	//
	bool IsInAnimationMode();

	//
	// Notifies the object that it is entering or leaving animation mode.
	//
	virtual void OnAnimationMode( CTreadDoc* pDoc, bool animating = true );

	//
	// Sets the animation transforms on the object.
	//
	void SetAnimTransform( CTreadDoc* pDoc, const vec3& pos, const vec3& angles );

	//
	// Gets the total animation angles that the user has specified.
	//
	void GetAnimAngles( vec3* angles );

	//
	// Restores the original animation transform before the object entered animation mode.
	//
	void RestoreAnimTransform( CTreadDoc* pDoc );

	//
	// Returns an OBJECT_TYPE_* flag based on the engine rendering type of the object.
	// This allows lights that only effect certain types of objects to preview correctly.
	//
	virtual int GetObjectTypeBits();

	//
	// Returns true if the object is attached to a map (present in the document).
	//
	bool IsAttached();

protected:

	//
	// Called to duplicate the object exactly. This is called with the purposes of restoring the object
	// states late (i.e. Track Animations).
	//
	virtual void CopyObject( CMapObject* obj, CTreadDoc* pDoc );

private:

	friend class CTreadDoc;

	int m_nUID;
	int m_nGroupUID;
	int m_nOwnerUID;
	CString m_sName;
	HTREEITEM m_hTreeItem;

	void* m_pMapList;
	bool m_bSelected;
	bool m_bVisible;
	bool m_bAttached;
	bool m_bEntOwnerCall;
	bool m_bEntGroupCall;

	vec3 m_anim_angles;
	bool m_bAnimating;
	CMapObject* m_anim_obj;
};

///////////////////////////////////////////////////////////////////////////////
// The ObjectMenu system allows an object to define and respond to a         //
// contextually driven menu system.                                          //
//                                                                           //
// See ents.cpp or brush.cpp for a working example.                          //
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// CObjectMenu encapsulates a single contextual menu. You can use            //
// Sys_DisplayObjectMenu() to display the menu.                              //
///////////////////////////////////////////////////////////////////////////////

class CObjectMenuItem : public CLLObject
{
public:

	DECLARE_TREAD_NEW();

	CObjectMenuItem();
	virtual ~CObjectMenuItem();

	int id;
	CString sMenuString;
};

class CObjectSubMenu : public CLLObject
{
public:

	CObjectSubMenu();
	virtual ~CObjectSubMenu();

	CMenu* m_pMenu;
};

class OS_CLEXP CObjectMenu
{
public:

	DECLARE_TREAD_NEW();

	CObjectMenu();
	virtual ~CObjectMenu();

	//
	// Adds a menu item with the specified id, and the specified title to the menu. If
	// absolute_id == true then the id is not adjusted to fit into the contextual menu space
	// and MFC must be mapped to intercept that id. All this really means is that you can call
	// another menu ID that you specified in your resource and it won't come through the OnUpdateCmdUI() or
	// OnMenuItem() functions of this object.
	//
	// Menu items should be numbered sequentially starting at 0. If you are    
	// using an ID from MFC or resource.h, then pass that define as the ID and  
	// make sure absolute_id == true.                                           
	//
	virtual void AddMenuItem( int id, const char* string, bool absolute_id = false );

	//
	// Called to set the CCmdUI state of the specified menu item.
	//
	virtual void OnUpdateCmdUI( int id, CCmdUI* pUI );

	//
	// Notification that the specified menu item has been selected.
	//
	virtual void OnMenuItem( int id );

	//
	// These should not be called by you ever.
	//
	static CMenu* FindMenuItem( CMenu* pBaseMenu, const char* title );
	static void BuildMenuFromObjectMenu( CMenu* pMenu, CObjectMenu* pObjMenu );

private:

	CLinkedList<CObjectSubMenu> m_SubMenuList;
	CLinkedList<CObjectMenuItem> m_MenuList;
};

///////////////////////////////////////////////////////////////////////////////
// The editors creation tree-view graphically maps creation functions to     //
// tree items. When the user ALT+RCLICK's a mouse in a map view, the         //
// creation function for the specified type is called to create that object  //
// and hand it off to the system for processing.                             //
//                                                                           //
// To add the ability to ALT+RCLICK your own object type you need to add     //
// a call to Sys_RegisterObjectCreator() inside the Sys_InitCreators()       //
// function. When the editor startes it called Sys_InitCreators() to         //
// register all the various ALT+RCLICK objects.                              //
///////////////////////////////////////////////////////////////////////////////

class OS_CLEXP CObjectCreator : public CLLObject
{
public:
	virtual ~CObjectCreator() {}

	virtual void Release() = 0;
	virtual const char *Name() = 0;
	virtual CMapObject *CreateObject(CTreadDoc *doc) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                              //
///////////////////////////////////////////////////////////////////////////////

class CPluginGame;
class CPluginFileExport;
class CPluginFileImport;
class CPluginMenuOption;
class CPluginMapCompiler;

class CPlugin : public CLLObject
{
public:
	virtual ~CPlugin() {}

	virtual void Release() = 0;

	virtual CPluginGame *GamePlugin(int i) = 0;

	virtual CPluginFileImport *FileImporter(int i) = 0;

	virtual CObjectCreator *ObjectCreator(int i) = 0;

	virtual CMapObject *ObjectForClass(int classbits, int subclassbits) = 0;
};

class CPluginGame : public CLLObject
{
public:
	virtual ~CPluginGame() {}

	virtual void Initialize(PrefFileSection_t *prefs) = 0;
	virtual void SaveSettings(PrefFileSection_t *prefs) = 0;
	virtual void Release() = 0;
	
	virtual const char *Name() = 0;
	virtual bool AllowBrushes() = 0;
	
	virtual bool WantsConfiguration() = 0;
	virtual void Configure() = 0;
	
	virtual int FileVersion() = 0;
	virtual void GetExportFile(const char *filename, char *buff, int buffSize) = 0;
	virtual CPluginFileExport *NativeMapExporter() = 0;

	virtual CPluginFileExport *FileExporter(int i) = 0;
	
	virtual CLinkedList<CShader> *ShaderList() = 0;
	
	virtual void CompileMap(CTreadDoc *doc, bool run) = 0;
	virtual bool LeakTraceExists(CTreadDoc *doc) = 0;
	virtual bool LoadLeakTrace(CTreadDoc *doc) = 0;
	virtual CLinkedList<CObjProp> *GetWorldspawnProps(CTreadDoc *doc) = 0;
	
	virtual CMapObject *ObjectForClass(int classbits, int subclassbits) = 0;
	virtual CUndoRedoAction *UndoRedoActionForClass(int classbits) = 0;

	virtual CObjectCreator *ObjectCreator(int i) = 0;

	virtual void RunGame(const char *mapFile) = 0;

	virtual void UpdateVertexMode(CTreadDoc *doc) = 0;
	virtual void UpdateSelectionBounds(CTreadDoc *doc) = 0;
	virtual void EnterVertexMode(CTreadDoc *doc, bool enter) = 0;
	virtual void EnterFaceMode(CTreadDoc *doc, bool enter) = 0;
	virtual void ProcessVertexBoxSelection(CTreadDoc *doc, int count, CPickObject **list, bool select) = 0;
	virtual void BindUserData(CTreadDoc *doc) = 0;
	virtual void ReleaseUserData(CTreadDoc *doc) = 0;

	virtual bool ReadMapHeader(CFile *file, CTreadDoc *doc, int version) = 0;
	virtual void WriteMapHeader(CFile *file, CTreadDoc *doc, int version) = 0;
};

class CPluginFileImport : public CLLObject
{
public:
	virtual ~CPluginFileImport() {}

	virtual void Release() = 0;
	virtual bool Import(const char *filename, CTreadDoc *doc) = 0;
	virtual const char *Type() = 0;
	virtual const char *Extension() = 0;
	virtual const char *PluginGame() = 0;
};

class CPluginFileExport : public CLLObject
{
public:
	virtual ~CPluginFileExport() {}

	virtual void Release() = 0;
	virtual bool Export(const char *filename, bool forCompile, CTreadDoc *doc) = 0;
	virtual const char *Type() = 0;
	virtual const char *Extension() = 0;
};

class CPluginMapCompiler
{
public:
	virtual ~CPluginMapCompiler() {}

	virtual void Abort() = 0;
};

typedef int (*PLUGIN_APIVERSION_FP)();
typedef CPlugin* (*PLUGIN_CREATE_FP)(int);

#if defined(__BUILDING_TREAD__)

//
// Initialize various global editor system (jmodel/skel caches) etc.
//
void Sys_Init(void);

//
// Free various global editor systems.
//
void Sys_Shutdown(void);

//
// Reload models and entity definitions.
//
void Sys_ReloadStuff(void);

CLinkedList<CPlugin> *Sys_Plugins();
CLinkedList<CPluginGame> *Sys_PluginGames();
CPluginGame *Sys_FindPluginGame(const char *name);
CLinkedList<CPluginFileImport> *Sys_PluginImporters();
CLinkedList<CPluginFileExport> *Sys_PluginExporters();

class CGameDef;
CLinkedList<CGameDef> *Sys_GameDefs();

CGameDef *Sys_FindGameDef(const char *name);
void Sys_SetGameDef(const char *name);
CString Sys_GameDef();

//
// Private, don't call.
//
void Sys_OnPopupMenu( int id );

//
// Private, don't call.
//
void Sys_UpdatePopupMenu( int id, CCmdUI* pUI );

//
// For propview.
//
void Sys_RegisterPropView( CObjPropView* view );
CObjPropView* Sys_GetPropView(void);

//
// For console.
//
void Sys_SetConsole( CConsoleDialog* console );

//
// For mainframe.
//
void Sys_RegisterMainFrame( CMainFrame* pFrame );

//
// Get the current frame that holds the 4 map views.
//
CChildFrame* Sys_GetActiveFrame(void);

//
// Creates an object of the currently selection creation type at the specified screen position
// of the specified view.
//
void Sys_CreateSelectedObject(CMapView* pView, int mx, int my);

//
// Gets the CMainFrame application object.
//
CMainFrame* Sys_GetMainFrame(void);

//
// Builds Plugin Menu Items
//
void Sys_BuildPluginMenu(CMenu *root, CTreadDoc *doc);

void Sys_OnPluginCommand(int id);

//
// Import/Export globals.
//
void Sys_SetImporter(CPluginFileImport *plugin);
CPluginFileImport *Sys_Importer();
void Sys_SetExporter(CPluginFileExport *plugin);
CPluginFileExport *Sys_Exporter();

#endif

OS_FNEXP void Sys_DisplayWindowsError(DWORD code);
OS_FNEXP void Sys_GetDirectory(const char *path, char *buff, int buffSize);
OS_FNEXP void Sys_GetFilename(const char *path, char *buff, int buffSize);
OS_FNEXP bool Sys_FileExists(const char *path);
OS_FNEXP bool Sys_WriteFile(const char *path, const void *data, int len);
OS_FNEXP int  Sys_ToWideString(const char *str, wchar_t **wstring);
OS_FNEXP void Sys_FreeWideString(wchar_t *wstring);

OS_FNEXP void Sys_RepaintAllShaders();
OS_FNEXP void Sys_SaveSettings();

//
// Status bar
//
OS_FNEXP void Sys_SetStatusBarRange(int nLower, int nUpper);
OS_FNEXP void Sys_SetStatusBarPos(int nPos);
OS_FNEXP void Sys_OffsetStatusBarPos(int nPos);
OS_FNEXP void Sys_SetStatusBarStep(int nStep);
OS_FNEXP void Sys_StepStatusBar();

//
// Displays the requested object menu at the requested position in the requested view.
//
OS_FNEXP void Sys_DisplayObjectMenu( CMapView* pView, int mx, int my, CObjectMenu* pObjMenu );

//
// Browses for a folder. The fact that I have this function is a testament to how lame MFC programming is. I miss c#.
//
OS_FNEXP bool Sys_BrowseForFolder(const char *title, char *inoutDir, int buffLen);

//
// Prints a line to the system console.
//
OS_FNEXP void Sys_printf( const char* fmt, ... );

//
// Adjusts the given view to make the texture visible.
//
OS_FNEXP void Sys_AdjustToViewTexture( CShader* shader );

//
// Resets the texture view position.
//
OS_FNEXP void Sys_ResetTextureView( void );

//
// Shows/Hides the mouse cursor.
//
OS_FNEXP void Sys_ShowCursor( bool show=true );

//
// Gets the active map (document).
//
OS_FNEXP CTreadDoc* Sys_GetActiveDocument(void);

//
// Redraws the specified view types of the active document.
//
OS_FNEXP void Sys_RedrawWindows( int type = VIEW_FLAG_MAP );

//
// Gets the angles to look at the specified position from the specified position.
//
OS_FNEXP vec3 Sys_LookAngles( const vec3& pos, const vec3& look, const vec3& up );

//
// Grid snaps the specified number via the specified grid size.
//
OS_FNEXP float Sys_Snapf( float map_coord, float grid_size );

//
// Grid snaps the specified vector via the specified grid size.
//
OS_FNEXP vec3 Sys_SnapVec3( const vec3& v, float grid_size );

//
// Sets the specified views mouse cursor to the specified cursor.
//
OS_FNEXP void Sys_SetCursor(CMapView* pView, int nCursor);

//
// Global qsort function.
//
OS_FNEXP void Sys_Sort( int in1, int in2, int* min, int* max );

struct SysScaleInfo_t
{
	vec3 pos;
	vec3 scale;
	vec3 mins_before;
	vec3 maxs_before;
	vec3 mins_after;
	vec3 maxs_after;
	CTreadDoc* doc;
};

//
// WalkList() function to scale an object.
//
OS_FNEXP int Sys_Scale( CMapObject* obj, void* parm, void* parm2 );

struct SysRotateInfo_t
{
	mat3x3 m;
	vec3 org;
	CTreadDoc* doc;
};

//
// WalkList() function to rotate an object.
//
OS_FNEXP int Sys_Rotate( CMapObject* obj, void* parm, void* parm2 );

//
// WalkList() function to translate an object.
//
OS_FNEXP int Sys_Translate( CMapObject* obj, void* parm, void* parm2 );

//
// WalkList() function to nudge an object.
//
OS_FNEXP int Sys_Nudge( CMapObject* obj, void* parm, void* parm2 );

//
// Called to begin/drag/end a selection drag.
//
OS_FNEXP void Sys_BeginDragSel( CMapView* pView, int mx, int my, int buttons );
OS_FNEXP void Sys_EndDragSel( CMapView* pView, int mx, int my, int buttons );
OS_FNEXP bool Sys_DragSel( CMapView* pView, int mx, int my, int buttons );

//
// Reconnects all objects in all open maps using the specified shader.
//
OS_FNEXP void Sys_RepaintObjectsWithShader( const char* szShader );

//
// Start/End mouse capture.
//
OS_FNEXP void Sys_SetMouseCapture( CMapView* pView );

//
// Returns a random number between min and max.
//
OS_FNEXP float ENT_Random( float min, float max );

#endif