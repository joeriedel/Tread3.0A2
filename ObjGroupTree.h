///////////////////////////////////////////////////////////////////////////////
// ObjGroupTree.h
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

#if !defined(AFX_OBJGROUPTREE_H__1651EA96_F0C3_4A2F_9BFF_4DE942FD060F__INCLUDED_)
#define AFX_OBJGROUPTREE_H__1651EA96_F0C3_4A2F_9BFF_4DE942FD060F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjGroupTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjGroupTree window
class CTreadDoc;
class CMapObject;
class CObjectGroup;

class CObjGroupTree : public CTreeCtrl
{
// Construction
public:
	CObjGroupTree();

private:

	CTreadDoc* m_pDoc;

	HTREEITEM m_pRoot;

	HTREEITEM FindItemByUID( int uid, HTREEITEM pRoot );

	static int AddObjectProc( CMapObject* obj, void* p, void* p2 );
	static int ClearObjectTreeItemProc( CMapObject* obj, void* p, void* p2 );
	static int ClearGroupTreeItemProc( CObjectGroup* gr, void* p, void* p2 );
	
	void DeleteGroup();

// Attributes
public:

// Operations
public:

	void SetDoc( CTreadDoc* pDoc );

	void BuildTree();
	void SelectItemUID( int uid );
	void AddObject( CMapObject* obj );
	void RemoveObject( CMapObject* obj );
	void AddGroup( CObjectGroup* gr );
	void RemoveGroup( CObjectGroup* gr );

	void ObjectStateChange( CMapObject* obj );
	void GroupStateChange( CObjectGroup* obj );

	void ShowGroup( CObjectGroup* obj );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjGroupTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjGroupTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjGroupTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJGROUPTREE_H__1651EA96_F0C3_4A2F_9BFF_4DE942FD060F__INCLUDED_)
