///////////////////////////////////////////////////////////////////////////////
// TrackAnimDialog.h
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

#if !defined(AFX_TRACKANIMDIALOG_H__5F434001_08B9_4A6A_B00E_CD7B93B8B2BC__INCLUDED_)
#define AFX_TRACKANIMDIALOG_H__5F434001_08B9_4A6A_B00E_CD7B93B8B2BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrackAnimDialog.h : header file
//

#include "LinkedList.h"
#include "Math3d.h"

/////////////////////////////////////////////////////////////////////////////
// CTrackAnimDialog dialog
class CSplineTrack;
class CSplineSegment;
class CSplineKeyFrame;
class CTreadDoc;
class CMapView;

class CTrackAnimDialog : public CDialog
{
private:

	CSplineTrack* m_track;
	CSplineSegment* m_seg;
	CTreadDoc* m_doc;
	CListBox m_Rots;
	CListBox m_Fovs;
	CListBox m_Events;
	CSliderCtrl m_Slider;
	CButton m_NoMotionCheck;
	CButton m_AnimateCameraCheck;
	bool m_bPlayback;

	void LoadInterface();
	void LoadKeyList( CLinkedList<CSplineKeyFrame>* keys, CListBox* listbox );

	void LoadTotalTime();
	void LoadCurrentTime();
	void LoadSegmentTimes();
	void LoadSliderTimes();
	void LoadSliderPos();

	void DeleteSelRot();
	void DeleteSelFov();
	void DeleteSelEvent();

	int m_ticks;

	bool m_bIgnoreUpdate;

	void UpdateApply();
	void AnimateForTime();

	vec3 GetSelectionRotation();

	CMapView* Get3DView();

	CMapView* m_view;

// Construction
public:
	CTrackAnimDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrackAnimDialog)
	enum { IDD = IDD_TRACK_DIALOG };
	//}}AFX_DATA

	void Load( CSplineTrack* track, CSplineSegment* seg, CTreadDoc* doc );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackAnimDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrackAnimDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateTotalTime();
	afx_msg void OnUpdateTotalSegTime();
	afx_msg void OnUpdateEndTime();
	afx_msg void OnApplyButton();
	afx_msg void OnAddEvent();
	afx_msg void OnAddFov();
	afx_msg void OnAddRot();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateCurrentTime();
	afx_msg void OnDblclkRotKeys();
	afx_msg void OnDblclkFovKeys();
	afx_msg void OnDblclkEventKeys();
	afx_msg void OnNoMotionCheck();
	afx_msg void OnPlayStopButton();
	afx_msg void OnAnimateCameraCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKANIMDIALOG_H__5F434001_08B9_4A6A_B00E_CD7B93B8B2BC__INCLUDED_)
