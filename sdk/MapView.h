///////////////////////////////////////////////////////////////////////////////
// MapView.h
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

#if !defined(AFX_MAPVIEW_H__56713F75_A0CE_4EEB_A146_2A901E7A6ECF__INCLUDED_)
#define AFX_MAPVIEW_H__56713F75_A0CE_4EEB_A146_2A901E7A6ECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapView.h : header file
//

#include "r_sys.h"
#include "system.h"

class CTreadDoc;

/////////////////////////////////////////////////////////////////////////////
// CMapView view
class OS_CLEXP CMapView : public CView
{
private:

	bool m_bScrollDrag;
	
	float m_fGridSize;
	bool m_bGridSnap;

	DWORD m_lastclicktime;

	HGLRC m_hglrc;
	CDC* m_pActiveDC;
	bool m_bPaintDC;
	bool m_bAltPressed;
	bool m_bPressedLast;
	bool m_bShiftPressedLast;
	bool m_bDoMove;
	bool m_bDragManipulator;
	bool m_bSelBoxDrag;
	bool m_bSelBoxAlt;
	bool m_bSelBoxCtrl;

	int m_nViewType;
	int m_nLastViewType;
	vec3 m_orbit;
	glState_t m_glState;
	CPickObject* m_lpick;
	CPickObject* m_rpick;
	CPickObject* m_trackpick;

	bool SetupGL();	
	
	class CViewParmsMenu : public CObjectMenu
	{
	public:

		CViewParmsMenu();
		virtual ~CViewParmsMenu();

		CMapView* pView;

		void OnUpdateCmdUI( int id, CCmdUI* pUI );
		void OnMenuItem( int id );
	};

	CViewParmsMenu m_ViewParmsMenu;

public:

	bool SwapBuffers();
	void RedrawWindow();
	void CalcSize();

	float GetGridSize();
	void SetGridSize( float fSize );
	void SetGridSnap( bool on = true );
	bool GetGridSnap();

	HCURSOR m_hcursor;

	void SetViewType( int nType );
	int GetViewType();
	void SetLastViewType( int nLastViewType );
	int GetLastViewType();

	glState_t* GetGLState();
	bool BindGL();
	bool UnbindGL();
	void CenterCursor();
	void SetScale( float scale );
	CTreadDoc* GetDocument();

	const char* GetViewTypeString();

	void WinXYToVec3( float x, float y, vec3* out );
	void MapXYToWinXY( float mx, float my, float* x, float* y );
	void WinXYToMapXY( float winx, float winy, float* x, float* y );
	void GetWinSelBox( int *minx, int *maxx, int* miny, int* maxy );

	void KillTrackPicks( CPickObject* obj );

	struct TexView_s
	{
		float fYofs;
		float fMax;
		CShader* pHoverShader;
	} TexView;

	struct View_s
	{
		float fWinWidth;;
		float fWinHeight;
		float fWinCenterX;
		float fWinCenterY;
		float fMapWidth;
		float fMapHeight;
		float fMapCenterX;
		float fMapCenterY;
		float fMapLeft;
		float fMapRight;
		float fMapTop;
		float fMapBottom;
		float fMapMinX;
		float fMapMaxX;
		float fMapMinY;
		float fMapMaxY;
		float fScale;
		float fInvScale;

		bool bGridVisible;
		bool bShowWireframe;
		bool bSelectionWireframe;
		bool bShowEntityNames;
		bool bShaded;
		bool bSelBoxVisible;
		bool bShowIcons;

		RECT selbox;

		vec3 pos3d;
		float fov;
		float clipdist;

		COrientation or2d;
		COrientation or3d;

	} View;

protected:
	CMapView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMapView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMapView)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPVIEW_H__56713F75_A0CE_4EEB_A146_2A901E7A6ECF__INCLUDED_)
