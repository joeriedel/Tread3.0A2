///////////////////////////////////////////////////////////////////////////////
// ObjPropView.h
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

#if !defined(AFX_OBJPROPVIEW_H__CCAB8E4F_E3E1_45CA_8C8E_8C164F1C0F7A__INCLUDED_)
#define AFX_OBJPROPVIEW_H__CCAB8E4F_E3E1_45CA_8C8E_8C164F1C0F7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjPropView.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CObjPropView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "AutoCompleteComboBox.h"
#include "FilterEdit.h"
#include "ObjGroupTree.h"
#include "ObjectCreatorTree.h"
#include "System.h"
#include "ColorStatic.h"

class CObjPropView : public CFormView
{
protected:
	CObjPropView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CObjPropView)

// Form Data
public:
	//{{AFX_DATA(CObjPropView)
	enum { IDD = IDD_PROPERTIES };
	CListCtrl	m_lcSubList;
	CFilterEdit	m_Nudge;
	CComboBox	m_cbTextureFilter;
	CFilterEdit	m_String;
	CComboBox	m_Props;
	CString	m_sName;
	//}}AFX_DATA

// Attributes
public:

private:

	bool m_bUserChange;
	bool m_worldspawn;

	bool PropsDiffer();
	void ClearPropControls();
	void EnablePropControls( bool enable=true );
	void LoadProperties( CTreadDoc* pDoc, bool worldspawn );
	void MakePropList( CLinkedList<CMapObject>* pList, CTreadDoc* pDoc );
	void LoadPropList();
	void HidePropControls();
	void OnSelProp();
	void UpdateApply();
	void UpdateSetName();
	void ApplyProps( CTreadDoc* pDoc );
	void OnPropValChange();
	void LoadTextureFilters();
	void UpdateTextureFilterApply();
	void UpdateSubItemStates( CObjProp* prop );

	void ClearDocControls();
	void EnableDocControls( bool enable = true );
	
	void AddProp( CObjProp* prop, CLinkedList<CMapObject>* objlist, CTreadDoc* pDoc );

	class CDProp : public CObjProp
	{
	public:

		CDProp();
		CDProp( const CDProp& prop );
		virtual ~CDProp();

		bool bSupress;
	};

	CDProp* m_pSelProp;
	CString m_sSelProp;
	CLinkedList<CDProp> m_PropList;
	CLinkedList<CDProp> m_VirginPropList; // virgin property list.
	CTreadDoc* m_pDoc;
	CImageList m_ImageList;
	CImageList m_SubImageList;
	CObjGroupTree m_Tree;
	CObjectCreatorTree m_createTree;
	CColorStatic m_colorstatic;

	static void ColorDialogCallback( COLORREF clr, void* data );

// Operations
public:

	void SetDoc( CTreadDoc* pDoc );
	CTreadDoc* GetDoc();

	void LoadSelection();
	void LoadDocument();
	void LoadWorldspawn();
	void PropChange( CTreadDoc* pDoc, const char* name );
	void TrySelectProp( const char* szName );
	void ExternalPropChange( CObjProp* prop );
	void ExternalApply();
	void TempApplySingleProp( CObjProp* prop, CTreadDoc* pDoc );

	void TreeAddObject( CMapObject* obj );
	void TreeRemoveObject( CMapObject* obj );
	void TreeAddGroup( CObjectGroup* obj );
	void TreeRemoveGroup( CObjectGroup* obj );
	void TreeUpdateObjectState( CMapObject* obj );
	void TreeUpdateGroupState( CObjectGroup* obj );
	void TreeLoadObjects( CTreadDoc* pDoc );
	void TreeShowGroup( CObjectGroup* gr );
	void TreeEnableDraw( bool draw = true );
	void TreeSelectObject( int uid );
	
	void ObjTreeLoad();
	void UpdateExpandPropsButton();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjPropView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CObjPropView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CObjPropView)
	afx_msg void OnChangeStringEdit();
	afx_msg void OnSelchangeProperties();
	afx_msg void OnApply();
	afx_msg void OnChangeName();
	afx_msg void OnSetName();
	afx_msg void OnSelchangeTextureFilterCombo();
	afx_msg void OnTextureFilterSetButton();
	afx_msg void OnEditchangeTextureFilterCombo();
	afx_msg void OnChangeNudgeEdit();
	afx_msg void OnClickChoiceList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColorStatic();
	afx_msg void OnEditScript();
	afx_msg void OnShowPropertiesDialog();
	afx_msg void OnCloseupProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJPROPVIEW_H__CCAB8E4F_E3E1_45CA_8C8E_8C164F1C0F7A__INCLUDED_)
