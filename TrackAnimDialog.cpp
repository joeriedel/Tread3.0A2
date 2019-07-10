///////////////////////////////////////////////////////////////////////////////
// TrackAnimDialog.cpp
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

#include "stdafx.h"
#include "Tread.h"
#include "TrackAnimDialog.h"
#include "System.h"
#include "SplineTrack.h"
#include "TreadDoc.h"
#include "MainFrm.h"
#include "KeyframeDialog.h"
#include "ScriptKeyframeDialog.h"
#include "childfrm.h"
#include "mapview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrackAnimDialog dialog


CTrackAnimDialog::CTrackAnimDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTrackAnimDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrackAnimDialog)
	//}}AFX_DATA_INIT

	m_track = 0;
	m_seg = 0;
	m_doc = 0;
	m_ticks = 0;
	m_bIgnoreUpdate = false;
	m_bPlayback = true;
	m_view = 0;
}


void CTrackAnimDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrackAnimDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrackAnimDialog, CDialog)
	//{{AFX_MSG_MAP(CTrackAnimDialog)
	ON_EN_UPDATE(IDC_TOTAL_TIME, OnUpdateTotalTime)
	ON_EN_UPDATE(IDC_TOTAL_SEG_TIME, OnUpdateTotalSegTime)
	ON_EN_UPDATE(IDC_END_TIME, OnUpdateEndTime)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_ADD_EVENT, OnAddEvent)
	ON_BN_CLICKED(IDC_ADD_FOV, OnAddFov)
	ON_BN_CLICKED(IDC_ADD_ROT, OnAddRot)
	ON_WM_HSCROLL()
	ON_EN_UPDATE(IDC_CURRENT_TIME, OnUpdateCurrentTime)
	ON_LBN_DBLCLK(IDC_ROT_KEYS, OnDblclkRotKeys)
	ON_LBN_DBLCLK(IDC_FOV_KEYS, OnDblclkFovKeys)
	ON_LBN_DBLCLK(IDC_EVENT_KEYS, OnDblclkEventKeys)
	ON_BN_CLICKED(IDC_NO_MOTION_CHECK, OnNoMotionCheck)
	ON_BN_CLICKED(IDC_PLAY_STOP_BUTTON, OnPlayStopButton)
	ON_BN_CLICKED(IDC_ANIMATE_CAMERA_CHECK, OnAnimateCameraCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackAnimDialog message handlers

void CTrackAnimDialog::LoadKeyList( CLinkedList<CSplineKeyFrame>* keys, CListBox* listbox )
{
	int i;
	CSplineKeyFrame* k;
	CString s;

	listbox->ResetContent();

	if( keys )
	{
		for( k = keys->ResetPos(); k; k = keys->GetNextItem() )
		{
			s.Format( "T:%.3f", ((float)k->ticks)/1000.0f );
			i = listbox->AddString( s );
			listbox->SetItemData( i, (DWORD)k );
		}
	}
}

void CTrackAnimDialog::LoadInterface()
{
	// this will kill the playback if necessary.
	//
	m_bPlayback = false;

	//
	// disable/enable.
	//
	GetDlgItem( IDC_TOTAL_TIME )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_NO_MOTION_CHECK )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_ROT_KEYS )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_FOV_KEYS )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_EVENT_KEYS )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_ADD_ROT )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_ADD_FOV )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_ADD_EVENT )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_ANIMATE_CAMERA_CHECK )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_CURRENT_TIME )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_PLAYBACK_POS )->EnableWindow( m_track != 0 );
	GetDlgItem( IDC_PLAY_STOP_BUTTON )->EnableWindow( m_track != 0 );
	
	GetDlgItem( IDC_END_TIME )->EnableWindow( m_seg != 0 );
	GetDlgItem( IDC_TOTAL_SEG_TIME )->EnableWindow( m_seg != 0 );


	//
	// load.
	//
	if( m_track )
	{
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_ROTS), &m_Rots );
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_FOVS), &m_Fovs );
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_EVENTS), &m_Events );

		m_NoMotionCheck.SetCheck( m_track->HasMotion() ? 0 : 1 );
	}
	else
	{
		LoadKeyList( 0, &m_Rots );
		LoadKeyList( 0, &m_Fovs );
		LoadKeyList( 0, &m_Events );
	}

	LoadSegmentTimes();
	LoadTotalTime();
	LoadCurrentTime();
	LoadSliderTimes();

	UpdateApply();
}

void CTrackAnimDialog::LoadSliderTimes()
{
	m_Slider.SetRange( 0, 10000 );
	m_Slider.SetTicFreq( 500 );
	m_Slider.SetPageSize( 500 );
	m_Slider.SetLineSize( 100 );
}

void CTrackAnimDialog::LoadSliderPos()
{
	if( !m_track )
		return;

	float frac;

	frac = ((float)m_ticks)/m_track->GetTotalTicks();
	int ticks = CeilingFastInt(frac*10000.0f);
	m_Slider.SetPos( ticks );
}

void CTrackAnimDialog::LoadTotalTime()
{
	CString s;

	if( m_track )
	{
		s.Format("%.3f", ((float)m_track->GetTotalTicks())/1000.0f );
	}

	GetDlgItem( IDC_TOTAL_TIME )->SetWindowText( s );
}

void CTrackAnimDialog::UpdateApply()
{
	GetDlgItem( IDC_APPLY_BUTTON )->EnableWindow(FALSE);

	if( !m_seg || !m_track )
	{
		return;
	}

	CString s;
	int ticks;

	GetDlgItem( IDC_TOTAL_TIME )->GetWindowText( s );
	ticks = CeilingFastInt(atof(s)*1000.0f);

	if( ticks != m_track->GetTotalTicks() )
	{
		GetDlgItem( IDC_APPLY_BUTTON )->EnableWindow(TRUE);
		return;
	}

	GetDlgItem( IDC_END_TIME )->GetWindowText(s);
	ticks = CeilingFastInt(atof(s)*1000.0f);
	if( ticks != m_seg->end )
	{
		GetDlgItem( IDC_APPLY_BUTTON )->EnableWindow(TRUE);
		return;
	}

	GetDlgItem( IDC_TOTAL_SEG_TIME )->GetWindowText(s);
	ticks = CeilingFastInt(atof(s)*1000.0f);
	if( ticks != m_seg->ticks )
	{
		GetDlgItem( IDC_APPLY_BUTTON )->EnableWindow(TRUE);
		return;
	}
}

void CTrackAnimDialog::LoadSegmentTimes()
{
	CString s;

	s = "Start Time:";

	if( m_seg )
	{
		s.Format( "Start Time: %.3f", ((float)m_seg->start)/1000.0f );
	}

	GetDlgItem( IDC_START_TIME_STATIC )->SetWindowText( s );

	s = "";

	if( m_seg )
	{
		s.Format( "%.3f", ((float)m_seg->end)/1000.0f );
	}

	m_bIgnoreUpdate = true;
	GetDlgItem( IDC_END_TIME )->SetWindowText( s );

	s = "";

	if( m_seg )
	{
		s.Format( "%.3f", ((float)m_seg->ticks)/1000.0f );
	}

	m_bIgnoreUpdate = true;
	GetDlgItem( IDC_TOTAL_SEG_TIME )->SetWindowText( s );
}

void CTrackAnimDialog::LoadCurrentTime()
{
	CString s;
	s.Format( "%.3f", ((float)m_ticks)/1000.0f );

	m_bIgnoreUpdate = true;
	GetDlgItem( IDC_CURRENT_TIME )->SetWindowText( s );
}

void CTrackAnimDialog::Load( CSplineTrack* track, CSplineSegment* seg, CTreadDoc* doc )
{
	if( m_track != track )
		m_ticks = 0;

	m_track = track;
	m_seg = seg;
	m_doc = doc;

	LoadInterface();
}

void CTrackAnimDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CTrackAnimDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

BOOL CTrackAnimDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Rots.SubclassDlgItem( IDC_ROT_KEYS, this );
	m_Fovs.SubclassDlgItem( IDC_FOV_KEYS, this );
	m_Events.SubclassDlgItem( IDC_EVENT_KEYS, this );
	m_Slider.SubclassDlgItem( IDC_PLAYBACK_POS, this );
	m_NoMotionCheck.SubclassDlgItem( IDC_NO_MOTION_CHECK, this );
	m_AnimateCameraCheck.SubclassDlgItem( IDC_ANIMATE_CAMERA_CHECK, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrackAnimDialog::OnUpdateTotalTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	if( m_bIgnoreUpdate )
	{
		m_bIgnoreUpdate = false;
		return;
	}

	UpdateApply();
}

void CTrackAnimDialog::OnUpdateTotalSegTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	if( m_bIgnoreUpdate )
	{
		m_bIgnoreUpdate = false;
		return;
	}

	UpdateApply();
}

void CTrackAnimDialog::OnUpdateEndTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	if( m_bIgnoreUpdate )
	{
		m_bIgnoreUpdate = false;
		return;
	}

	UpdateApply();
}

void CTrackAnimDialog::OnApplyButton() 
{
	// TODO: Add your control notification handler code here
	
	CString s;
	int ticks;

	GetDlgItem( IDC_TOTAL_TIME )->GetWindowText( s );
	ticks = CeilingFastInt(atof(s)*1000.0f);
	
	if( ticks != m_track->GetTotalTicks() )
	{
		//
		// this is all we must change...
		//
		float t1, t2;

		t1 = m_track->GetTotalTicks();

		if( t1 <= 0.0f )
			goto done;

		t2 = ticks;

		t1 = t2/t1;

		m_track->ScaleTimes( t1 );
		goto done;
	}

	GetDlgItem( IDC_END_TIME )->GetWindowText(s);
	ticks = CeilingFastInt(atof(s)*1000.0f);
	if( ticks != m_seg->end )
	{
		if( ticks <= m_seg->start )
		{
			::MessageBox( 0, "The spline segments end time must be greater than it's start time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
			goto done;
		}

		m_seg->end = ticks;
		m_seg->ticks = m_seg->end-m_seg->start;
		m_track->CalcSplineTimes();
		goto done;
	}

	GetDlgItem( IDC_TOTAL_SEG_TIME )->GetWindowText(s);
	ticks = CeilingFastInt(atof(s)*1000.0f);
	if( ticks != m_seg->ticks )
	{
		m_seg->ticks = ticks;
		m_track->CalcSplineTimes();
		goto done;
	}

done:

	LoadKeyList( m_track->GetKeyFrames( KEYFRAME_ROTS ), &m_Rots );
	LoadKeyList( m_track->GetKeyFrames( KEYFRAME_FOVS ), &m_Fovs );
	LoadKeyList( m_track->GetKeyFrames( KEYFRAME_EVENTS ), &m_Events );

	LoadTotalTime();
	LoadSegmentTimes();

	if( m_ticks > m_track->GetTotalTicks() )
		m_ticks = m_track->GetTotalTicks();

	LoadSliderPos();
	LoadCurrentTime();

	GetDlgItem( IDC_APPLY_BUTTON )->EnableWindow(FALSE);
}

void CTrackAnimDialog::OnAddEvent() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	CScriptKeyframeDialog dlg;
	CString s;

	s.Format("%.3f", ((float)m_ticks)/1000.0f );
	dlg.SetTime( s );
	//dlg.m_sScript = "//\r\n// Insert Script Code Here\r\n//\r\n";
	dlg.m_sScript = "";
	
	if( dlg.DoModal() != IDOK )
		return;

	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( m_track->KeyFrameExists( ticks, KEYFRAME_EVENTS ) )
	{
		::MessageBox( 0, "An event already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->AddKeyFrame( ticks, vec3::zero, dlg.m_sScript, KEYFRAME_EVENTS );
	LoadKeyList( m_track->GetKeyFrames( KEYFRAME_EVENTS ), &m_Events );
}

void CTrackAnimDialog::OnAddFov() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	CKeyframeDialog dlg;
	CString s;

	s.Format("%.3f", ((float)m_ticks)/1000.0f );

	dlg.SetTitle( "Add Field Of View Keyframe" );
	dlg.SetFilter( FILTER_FLOAT );
	dlg.SetTime( s );
	dlg.SetInputLineTitle("Field Of View");
	dlg.SetValue( "90" );

	if( dlg.DoModal() != IDOK )
		return;

	vec3 v;
	s = dlg.GetValue();

	v = vec3::zero;
	sscanf( s, "%f", &v.x );
	
	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( m_track->KeyFrameExists( ticks, KEYFRAME_FOVS ) )
	{
		::MessageBox( 0, "A keyframe already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->AddKeyFrame( ticks , v, "", KEYFRAME_FOVS );
	LoadKeyList( m_track->GetKeyFrames(KEYFRAME_FOVS), &m_Fovs );
	AnimateForTime();
}

void CTrackAnimDialog::OnAddRot() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	CKeyframeDialog dlg;
	CString s;

	s.Format("%.3f", ((float)m_ticks)/1000.0f );

	dlg.SetTitle( "Add Rotation Keyframe" );
	dlg.SetFilter( FILTER_MULTI_FLOAT );
	dlg.SetTime( s );
	dlg.SetInputLineTitle("Rotation");

	{
		vec3 r = GetSelectionRotation();
		s.Format(" %.2f %.2f %.2f", r.x, r.y, r.z );
		dlg.SetValue( s );
	}

	if( dlg.DoModal() != IDOK )
		return;

	vec3 v;
	s = dlg.GetValue();

	sscanf( s, "%f %f %f", &v.x, &v.y, &v.z );
	
	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( m_track->KeyFrameExists( ticks, KEYFRAME_ROTS ) )
	{
		::MessageBox( 0, "A keyframe already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->AddKeyFrame( ticks , v, "", KEYFRAME_ROTS );
	LoadKeyList( m_track->GetKeyFrames(KEYFRAME_ROTS), &m_Rots );
	AnimateForTime();
}

void CTrackAnimDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if( nSBCode == TB_THUMBPOSITION || nSBCode == TB_ENDTRACK )
		return;

	if( !m_track )
		return;

	nPos = (UINT)m_Slider.GetPos();

	float timefrac = ((float)nPos)/10000.0f;
	m_ticks = CeilingFastInt(((float)m_track->GetTotalTicks())*timefrac);

	m_bIgnoreUpdate = true;
	LoadCurrentTime();

	m_view = Get3DView();
	AnimateForTime();
}

void CTrackAnimDialog::OnUpdateCurrentTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	if( m_bIgnoreUpdate )
	{
		m_bIgnoreUpdate = false;
		return;
	}

	CString s;
	GetDlgItem( IDC_CURRENT_TIME )->GetWindowText( s );

	m_ticks = CeilingFastInt( atof(s)*1000.0f );
	LoadSliderPos();

	m_view = Get3DView();
	AnimateForTime();
}

void CTrackAnimDialog::AnimateForTime()
{
	vec3 angles, pos;
	float fov;

	if( !m_track || !m_doc )
		return;

	m_track->EvaluateSpline( m_ticks, &pos, &angles, &fov );	

	if( m_AnimateCameraCheck.GetCheck() )
	{
		if( !m_view )
			return;

		m_view->View.pos3d = pos;
	
		m_view->View.fov = fov;
		m_view->View.or3d.set_angles( angles );

		m_view->RedrawWindow();
	}
	else
	{
		//
		// animate the objects.
		//
		float num;
		vec3 tr, v;
		CMapObject* obj;

		num = 0.0f;
		tr = vec3::zero;

		for( obj = m_doc->GetSelectedObjectList()->ResetPos(); obj; obj = m_doc->GetSelectedObjectList()->GetNextItem() )
		{
			//if( obj->GetClass() == MAPOBJ_CLASS_SPLINETRACK )
			//	continue;

			obj->RestoreAnimTransform( m_doc );
			v = obj->GetObjectWorldPos();

			tr += v;
			num += 1.0f;
		}

		tr = tr / num;
		pos -= tr;

		//
		// rotate everybody around the origin.
		//
		mat3x3 m = build_rotation_matrix( 0.0f, 1.0f, 0.0f, DEGREES_TO_RADIANS( angles[1] )) *
				   build_rotation_matrix( 1.0f, 0.0f, 0.0f, DEGREES_TO_RADIANS( angles[0] )) *
				   build_rotation_matrix( 0.0f, 0.0f, 1.0f, DEGREES_TO_RADIANS( angles[2] ));
		
		for( obj = m_doc->GetSelectedObjectList()->ResetPos(); obj; obj = m_doc->GetSelectedObjectList()->GetNextItem() )
		{
			//if( obj->GetClass() == MAPOBJ_CLASS_SPLINETRACK )
			//	continue;

			v = obj->GetObjectWorldPos();
			v -= tr;
			v *= m;
			v += tr+pos;
			obj->SetAnimTransform( m_doc, v, angles );
		}

		Sys_RedrawWindows();
	}
}

CMapView* CTrackAnimDialog::Get3DView()
{
	int i;
	CMapView* view;

	for(i = 0; i < 4; i++)
	{
		view = m_doc->GetChildFrame()->GetMapView(i);
		if( view->GetViewType() == VIEW_TYPE_3D )
			break;
		view = 0;
	}

	return view;
}

void CTrackAnimDialog::DeleteSelRot()
{
	if( !m_track )
		return;

	int sel = m_Rots.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Rots.GetItemData( sel );
	if( k )
	{
		m_track->RemoveKeyFrame( k->ticks, KEYFRAME_ROTS );
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_ROTS), &m_Rots );
	}

	AnimateForTime();
}

void CTrackAnimDialog::DeleteSelFov()
{
	if( !m_track )
		return;

	int sel = m_Fovs.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Fovs.GetItemData( sel );
	if( k )
	{
		m_track->RemoveKeyFrame( k->ticks, KEYFRAME_FOVS );
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_FOVS), &m_Fovs );
	}

	AnimateForTime();
}

void CTrackAnimDialog::DeleteSelEvent()
{
	if( !m_track )
		return;

	int sel = m_Events.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Events.GetItemData( sel );
	if( k )
	{
		m_track->RemoveKeyFrame( k->ticks, KEYFRAME_EVENTS );
		LoadKeyList( m_track->GetKeyFrames(KEYFRAME_EVENTS), &m_Events );
	}

	AnimateForTime();
}

BOOL CTrackAnimDialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
		//
		// is this a repeat?
		//
		if( pMsg->lParam&(1<<30) )
		{
			return true;
		}

		if( pMsg->wParam == 0xBC )
		{
			SendMessage( WM_COMMAND, ID_TOOLS_ANIMATIONMODE );
		}
		else
		if( pMsg->wParam == VK_DELETE )
		{
			if( GetFocus() == &m_Rots )
			{
				DeleteSelRot();
				return true;
			}
			else
			if( GetFocus() == &m_Fovs )
			{
				DeleteSelFov();
				return true;
			}
			else
			if( GetFocus() == &m_Events )
			{
				DeleteSelEvent();
				return true;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTrackAnimDialog::OnDblclkRotKeys() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	int sel = m_Rots.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Rots.GetItemData( sel );

	CKeyframeDialog dlg;
	CString s, s2;

	s.Format( "Edit Rotation Keyframe @ %.3f", ((float)k->ticks)/1000.0f );
	s2.Format("%.3f", ((float)k->ticks)/1000.0f );

	dlg.SetTitle( s );
	dlg.SetFilter( FILTER_MULTI_FLOAT );
	dlg.SetTime( s2 );
	dlg.SetInputLineTitle("Rotation");

	s.Format( "%.2f %.2f %.2f", k->v.x, k->v.y, k->v.z );
	dlg.SetValue( s );

	if( dlg.DoModal() != IDOK )
		return;

	vec3 v;
	s = dlg.GetValue();

	sscanf( s, "%f %f %f", &v.x, &v.y, &v.z );
	
	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( ticks != k->ticks && m_track->KeyFrameExists( ticks, KEYFRAME_ROTS ) )
	{
		::MessageBox( 0, "A keyframe already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->SetKeyFrameInfo( k->ticks, ticks, v, "", KEYFRAME_ROTS, KEYFRAME_MASK_VEC|KEYFRAME_MASK_TICKS );

	LoadKeyList( m_track->GetKeyFrames(KEYFRAME_ROTS), &m_Rots );
}

void CTrackAnimDialog::OnDblclkFovKeys() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	int sel = m_Fovs.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Fovs.GetItemData( sel );

	CKeyframeDialog dlg;
	CString s, s2;

	s.Format( "Edit Field of View Keyframe @ %.3f", ((float)k->ticks)/1000.0f );
	s2.Format("%.3f", ((float)k->ticks)/1000.0f );

	dlg.SetTitle( s );
	dlg.SetFilter( FILTER_FLOAT );
	dlg.SetTime( s2 );
	dlg.SetInputLineTitle("Field of View");

	s.Format( "%.2f", k->v.x );
	dlg.SetValue( s );

	if( dlg.DoModal() != IDOK )
		return;

	vec3 v;
	s = dlg.GetValue();

	v = vec3::zero;
	sscanf( s, "%f", &v.x );
	
	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( ticks != k->ticks && m_track->KeyFrameExists( ticks, KEYFRAME_FOVS ) )
	{
		::MessageBox( 0, "A keyframe already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->SetKeyFrameInfo( k->ticks, ticks, v, "", KEYFRAME_FOVS, KEYFRAME_MASK_VEC|KEYFRAME_MASK_TICKS );
	LoadKeyList( m_track->GetKeyFrames(KEYFRAME_FOVS), &m_Fovs );
}

void CTrackAnimDialog::OnDblclkEventKeys() 
{
	// TODO: Add your control notification handler code here
	if( !m_track )
		return;

	int sel = m_Events.GetCurSel();
	if( sel < 0 )
		return;

	CSplineKeyFrame* k = (CSplineKeyFrame*)m_Events.GetItemData( sel );

	CScriptKeyframeDialog dlg;
	CString s;

	s.Format( "%.3f", ((float)k->ticks)/1000.0f );
	
	dlg.SetTime( s );
	dlg.m_sScript = k->s;

	if( dlg.DoModal() != IDOK )
		return;

	int ticks = CeilingFastInt(atof(dlg.GetTime())*1000.0f);
	if( ticks != k->ticks && m_track->KeyFrameExists( ticks, KEYFRAME_EVENTS ) )
	{
		::MessageBox( 0, "An event already exists at the specified time!", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	m_track->SetKeyFrameInfo( k->ticks, ticks, vec3::zero, dlg.m_sScript, KEYFRAME_EVENTS, KEYFRAME_MASK_EVENT|KEYFRAME_MASK_TICKS );
	LoadKeyList( m_track->GetKeyFrames(KEYFRAME_EVENTS), &m_Events );
}

void CTrackAnimDialog::OnNoMotionCheck() 
{
	// TODO: Add your control notification handler code here
	m_track->SetMotion( m_NoMotionCheck.GetCheck() != 0 ? false : true );
}

void CTrackAnimDialog::OnPlayStopButton() 
{
	// TODO: Add your control notification handler code here
	if( m_bPlayback || !m_track || !m_seg )
	{
		//
		// cancel the playback.
		// 
		m_bPlayback = false;
		return;
	}

	GetDlgItem( IDC_PLAY_STOP_BUTTON )->SetWindowText("Stop");

	//
	// start playback.
	//
	m_bPlayback = true;

	int totalticks = 0;
	unsigned int last_micro = 0;
	unsigned int cur_micro = 0;
	unsigned int elapsed = 0;

	m_view = Get3DView();

	if( !m_view )
	{
		::MessageBox( 0, "Please make one of your views a Perspective.", "Error!", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	totalticks = m_track->GetTotalTicks();

	last_micro = ReadTickMicroseconds();
	cur_micro = last_micro;

	m_ticks = 0;
	m_doc->ClearAllTrackPicks();

	for(; m_bPlayback;)
	{
		cur_micro = ReadTickMicroseconds();
		elapsed = cur_micro-last_micro;
		
		last_micro = cur_micro;

		m_ticks += (elapsed/MICROSECONDS_PER_MILLISECOND);
		
		AnimateForTime();
		LoadCurrentTime();
		LoadSliderPos();

		//
		// continue windows message pump.
		//
		Sys_GetMainFrame()->PumpMessages();

		if( m_ticks >= totalticks )
			break;
	}

	m_bPlayback = false;
	GetDlgItem( IDC_PLAY_STOP_BUTTON )->SetWindowText("Play");
}

vec3 CTrackAnimDialog::GetSelectionRotation()
{
	if( m_AnimateCameraCheck.GetCheck() )
	{
		CMapView* v = Get3DView();
		//vec3 r = euler_from_matrix( v->View.or3d.m );
		//return vec3( r.z*180.0f/PI, r.x*180.0f/PI, r.y*180.0f/PI );
		return v->View.or3d.org_angles + v->View.or3d.delta_angles;
	}
	else
	{
		//
		// get the first selected object.
		//
		if( m_doc && m_doc->ObjectsAreSelected() )
		{
			CMapObject* obj;

			for( obj = m_doc->GetSelectedObjectList()->ResetPos(); obj; obj = m_doc->GetSelectedObjectList()->GetNextItem() )
			{
				/*if( obj->GetClass() != MAPOBJ_CLASS_SPLINETRACK )*/
				{
					vec3 a;
					obj->GetAnimAngles( &a );
					return a;
				}
			}
		}
	}

	return vec3::zero;
}

void CTrackAnimDialog::OnAnimateCameraCheck() 
{
	// TODO: Add your control notification handler code here
	m_view = Get3DView();
	AnimateForTime();
}
