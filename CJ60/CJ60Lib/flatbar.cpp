////////////////////////////////////////////////////////////////
// CFlatToolBar 1997 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
#include "StdAfx.h"
#include "ModulVer.h"
#include "FlatBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// CFlatToolBar--does flat tool bar in MFC.
//
IMPLEMENT_DYNAMIC(CFlatToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CFlatToolBar, CToolBar)
	//{{AFX_MSG_MAP(CFlatToolBar)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_NCCREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////
// FixTB code from August 98 article:
////////////////////////////////////////////////////////////////

//////////////////
// ********PD** My stuff below

static int GetVerComCtl32()
{
	DLLVERSIONINFO dvi;
	return CModuleVersion::DllGetVersion(_T("comctl32.dll"), dvi) ?
		(dvi.dwMajorVersion*100 + dvi.dwMinorVersion) : 0;
}

// class global
int CFlatToolBar::iVerComCtl32 = GetVerComCtl32();

CFlatToolBar::CFlatToolBar()
{
	// **PD**
	// default: show dropdown arrows for vertical toolbar
	m_bShowDropdownArrowWhenVertical = TRUE;
}

CFlatToolBar::~CFlatToolBar()
{
}

//////////////////
// **PD**
// This is the all-important function that gets the true size of a button,
// instead of using m_sizeButton. And it's virtual, so you can override if
// my algorithm doesn't work, as will surely be the case in some circumstances.
//
CSize CFlatToolBar::GetButtonSize(TBBUTTON* pData, int iButton)
{
	// Get the actual size of the button, not what's in m_sizeButton.
	// Make sure to do SendMessage instead of calling MFC's GetItemRect,
	// which has all sorts of bad side-effects! (Go ahead, take a look at it.)
	// 
	CRect rc;
	SendMessage(TB_GETITEMRECT, iButton, (LPARAM)&rc);
	CSize sz = rc.Size();

	////////////////
	// Now must do special case for various versions of comctl32.dll,
	//
	DWORD dwStyle = pData[iButton].fsStyle;
	if ((pData[iButton].fsState & TBSTATE_WRAP)) {
		if (dwStyle & TBSTYLE_SEP) {
			// this is the last separator in the row (eg vertically docked)
			// fudge the height, and ignore the width. TB_GETITEMRECT will return
			// size = (8 x 22) even for a separator in vertical toolbar
			//
			if (iVerComCtl32 <= 470)
				sz.cy -= 3;		// empircally good fudge factor
			else if (iVerComCtl32 != 471)
				sz.cy = sz.cx;
			sz.cx = 0;			// separator takes no width if it's the last one

		} else if (dwStyle & TBSTYLE_DROPDOWN &&
			!m_bShowDropdownArrowWhenVertical) {
			// ignore width of dropdown
			sz.cx = 0;
		}
	}
	return sz;
}

////////////////////////////////////////////////////////////////
// ******* Stuff below is copied from MFC; my mods marked **PD**

#ifdef _MAC
	#define CX_OVERLAP  1
#else
	#define CX_OVERLAP  0
#endif

CSize CFlatToolBar::CalcSize(TBBUTTON* pData, int nCount)
{
	ASSERT(pData != NULL && nCount > 0);

	CPoint cur(0,0);
	CSize sizeResult(0,0);
	int cyTallestOnRow = 0;

	for (int i = 0; i < nCount; i++)
	{
		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		// **PD** Load actual size of button into local var
		// that obscures CToolBar::m_sizeButton.
		CSize m_sizeButton = GetButtonSize(pData, i);

		// **PD** I also changed the logic below to be more correct.
		cyTallestOnRow = max(cyTallestOnRow, m_sizeButton.cy);
		sizeResult.cx = max(cur.x + m_sizeButton.cx, sizeResult.cx);
		sizeResult.cy = max(cur.y + m_sizeButton.cy, sizeResult.cy);

		cur.x += m_sizeButton.cx - CX_OVERLAP;

		if (pData[i].fsState & TBSTATE_WRAP)
		{
			cur.x = 0;
			cur.y += cyTallestOnRow;
			cyTallestOnRow = 0;
			if (pData[i].fsStyle & TBSTYLE_SEP)
				cur.y += m_sizeButton.cy;
		}
	}
	return sizeResult;
}

int CFlatToolBar::WrapToolBar(TBBUTTON* pData, int nCount, int nWidth)
{
	ASSERT(pData != NULL && nCount > 0);

	int nResult = 0;
	int x = 0;
	for (int i = 0; i < nCount; i++)
	{
		pData[i].fsState &= ~TBSTATE_WRAP;

		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		int dx, dxNext;

		// **PD** Load actual size of button into local var
		// that obscures CToolBar::m_sizeButton.
		CSize m_sizeButton = GetButtonSize(pData, i);

		dx = m_sizeButton.cx;
		dxNext = dx - CX_OVERLAP;

		if (x + dx > nWidth)
		{
			BOOL bFound = FALSE;
			for (int j = i; j >= 0  &&  !(pData[j].fsState & TBSTATE_WRAP); j--)
			{
				// Find last separator that isn't hidden
				// a separator that has a command ID is not
				// a separator, but a custom control.
				if ((pData[j].fsStyle & TBSTYLE_SEP) &&
					(pData[j].idCommand == 0) &&
					!(pData[j].fsState & TBSTATE_HIDDEN))
				{
					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
			}
			if (!bFound)
			{
				for (int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
				{
					// Never wrap anything that is hidden,
					// or any custom controls
					if ((pData[j].fsState & TBSTATE_HIDDEN) ||
						((pData[j].fsStyle & TBSTYLE_SEP) &&
						(pData[j].idCommand != 0)))
						continue;

					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
				if (!bFound)
					x += dxNext;
			}
		}
		else
			x += dxNext;
	}
	return nResult + 1;
}

//////////////////////////////////////////////////////////////////////////
// **PD**
// Functions below are NOT modified. They're only here because they
// call the modified functions above, which are NOT virtual.
//////////////////////////////////////////////////////////////////////////

void  CFlatToolBar::SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert)
{
	ASSERT(pData != NULL && nCount > 0);

	if (!bVert)
	{
		int nMin, nMax, nTarget, nCurrent, nMid;

		// Wrap ToolBar as specified
		nMax = nLength;
		nTarget = WrapToolBar(pData, nCount, nMax);

		// Wrap ToolBar vertically
		nMin = 0;
		nCurrent = WrapToolBar(pData, nCount, nMin);

		if (nCurrent != nTarget)
		{
			while (nMin < nMax)
			{
				nMid = (nMin + nMax) / 2;
				nCurrent = WrapToolBar(pData, nCount, nMid);

				if (nCurrent == nTarget)
					nMax = nMid;
				else
				{
					if (nMin == nMid)
					{
						WrapToolBar(pData, nCount, nMax);
						break;
					}
					nMin = nMid;
				}
			}
		}
		CSize size = CalcSize(pData, nCount);
		WrapToolBar(pData, nCount, size.cx);
	}
	else
	{
		CSize sizeMax, sizeMin, sizeMid;

		// Wrap ToolBar vertically
		WrapToolBar(pData, nCount, 0);
		sizeMin = CalcSize(pData, nCount);

		// Wrap ToolBar horizontally
		WrapToolBar(pData, nCount, 32767);
		sizeMax = CalcSize(pData, nCount);

		while (sizeMin.cx < sizeMax.cx)
		{
			sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
			WrapToolBar(pData, nCount, sizeMid.cx);
			sizeMid = CalcSize(pData, nCount);

			if (nLength < sizeMid.cy)
			{
				if (sizeMin == sizeMid)
				{
					WrapToolBar(pData, nCount, sizeMax.cx);
					return;
				}
				sizeMin = sizeMid;
			}
			else if (nLength > sizeMid.cy)
				sizeMax = sizeMid;
			else
				return;
		}
	}
}

struct _AFX_CONTROLPOS
{
	int nIndex, nID;
	CRect rectOldPos;
};

CSize CFlatToolBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);

	int nCount;
	TBBUTTON* pData;
	CSize sizeResult(0,0);

	// Load Buttons
	{
		nCount = SendMessage(TB_BUTTONCOUNT, 0, 0);
		if (nCount != 0)
		{
			int i;
			pData = new TBBUTTON[nCount];
			for (i = 0; i < nCount; i++)
				GetButton(i, &pData[i]); // **PD** renamed from _GetButton
		}
	}

	if (nCount > 0)
	{
		if (!(m_dwStyle & CBRS_SIZE_FIXED))
		{
			BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

			if (bDynamic && (dwMode & LM_MRUWIDTH))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else if (bDynamic && (dwMode & LM_HORZDOCK))
				SizeToolBar(pData, nCount, 32767);
			else if (bDynamic && (dwMode & LM_VERTDOCK))
				SizeToolBar(pData, nCount, 0);
			else if (bDynamic && (nLength != -1))
			{
				CRect rect; rect.SetRectEmpty();
				CalcInsideRect(rect, (dwMode & LM_HORZ));
				BOOL bVert = (dwMode & LM_LENGTHY);
				int nLen = nLength + (bVert ? rect.Height() : rect.Width());

				SizeToolBar(pData, nCount, nLen, bVert);
			}
			else if (bDynamic && (m_dwStyle & CBRS_FLOATING))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else
				SizeToolBar(pData, nCount, (dwMode & LM_HORZ) ? 32767 : 0);
		}

		sizeResult = CalcSize(pData, nCount);

		if (dwMode & LM_COMMIT)
		{
			_AFX_CONTROLPOS* pControl = NULL;
			int nControlCount = 0;
			BOOL bIsDelayed = m_bDelayedButtonLayout;
			m_bDelayedButtonLayout = FALSE;

			for(int i = 0; i < nCount; i++)
				if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					nControlCount++;

			if (nControlCount > 0)
			{
				pControl = new _AFX_CONTROLPOS[nControlCount];
				nControlCount = 0;

				for(int i = 0; i < nCount; i++)
				{
					if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					{
						pControl[nControlCount].nIndex = i;
						pControl[nControlCount].nID = pData[i].idCommand;

						CRect rect;
						GetItemRect(i, &rect);
						ClientToScreen(&rect);
						pControl[nControlCount].rectOldPos = rect;

						nControlCount++;
					}
				}
			}

			if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
				m_nMRUWidth = sizeResult.cx;
			for (int i = 0; i < nCount; i++)
				SetButton(i, &pData[i]); // **PD** renamed from _SetButton

			if (nControlCount > 0)
			{
				for (int i = 0; i < nControlCount; i++)
				{
					CWnd* pWnd = GetDlgItem(pControl[i].nID);
					if (pWnd != NULL)
					{
						CRect rect;
						pWnd->GetWindowRect(&rect);
						CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
						GetItemRect(pControl[i].nIndex, &rect);
						pt = rect.TopLeft() + pt;
						pWnd->SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
					}
				}
				delete[] pControl;
			}
			m_bDelayedButtonLayout = bIsDelayed;
		}
		delete[] pData;
	}

	//BLOCK: Adjust Margins
	{
		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();

		CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}
	return sizeResult;
}

CSize CFlatToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout(dwMode);
}

CSize CFlatToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}
	return CalcLayout(dwMode, nLength);
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar attribute access

// **PD** I renamed this from _GetButton.
//
void CFlatToolBar::GetButton(int nIndex, TBBUTTON* pButton) const
{
	CToolBar* pBar = (CToolBar*)this;
	VERIFY(pBar->SendMessage(TB_GETBUTTON, nIndex, (LPARAM)pButton));
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
}

// **PD** I renamed this from _SetButton.
//
void CFlatToolBar::SetButton(int nIndex, TBBUTTON* pButton)
{
	// get original button state
	TBBUTTON button;
	VERIFY(SendMessage(TB_GETBUTTON, nIndex, (LPARAM)&button));

	// prepare for old/new button comparsion
	button.bReserved[0] = 0;
	button.bReserved[1] = 0;
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
	pButton->bReserved[0] = 0;
	pButton->bReserved[1] = 0;

	// nothing to do if they are the same
	if (memcmp(pButton, &button, sizeof(TBBUTTON)) != 0)
	{
		// don't redraw everything while setting the button
		DWORD dwStyle = GetStyle();
		ModifyStyle(WS_VISIBLE, 0);
		VERIFY(SendMessage(TB_DELETEBUTTON, nIndex, 0));
		VERIFY(SendMessage(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
		ModifyStyle(0, dwStyle & WS_VISIBLE);

		// invalidate appropriate parts
		if (((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
			((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
		{
			// changing a separator
			Invalidate(FALSE);
		}
		else
		{
			// invalidate just the button
			CRect rect;
			if (SendMessage(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
				InvalidateRect(rect, FALSE);    // don't erase background
		}
	}
}

////////////////////////////////////////////////////////////////
// End FixTb code.
////////////////////////////////////////////////////////////////

////////////////
// Load override modifies the style after loading toolbar.
//
BOOL CFlatToolBar::LoadToolBar(LPCTSTR lpszResourceName)
{
	if (!CToolBar::LoadToolBar(lpszResourceName))
		return FALSE;
	ModifyStyle(0, TBSTYLE_FLAT); // make it flat
	return TRUE;
}

//#define ILLUSTRATE_DISPLAY_BUG			 // remove comment to see the bug

//////////////////
// MFC doesn't handle moving a TBSTYLE_FLAT toolbar correctly.
// The simplest way to fix it is to repaint the old rectangle and
// toolbar itself whenever the toolbar moves.
// 
void CFlatToolBar::OnWindowPosChanging(LPWINDOWPOS lpwp)
{
	CToolBar::OnWindowPosChanging(lpwp);

#ifndef ILLUSTRATE_DISPLAY_BUG

	if (!(lpwp->flags & SWP_NOMOVE)) {	 // if moved:
		CRect rc;								 //   Fill rectangle with..
		GetWindowRect(&rc);					 //   ..my (toolbar) rectangle.
		CWnd* pParent = GetParent();		 //   get parent (dock bar/frame) win..
		pParent->ScreenToClient(&rc);		 //   .. and convert to parent coords

		// Ask parent window to paint the area beneath my old location.
		// Typically, this is just solid grey. The area won't get painted until
		// I send WM_NCPAINT after the move, in OnWindowPosChanged below.
		//
		pParent->InvalidateRect(&rc);		 // paint old rectangle
	}
#endif
}

//////////////////
// Now toolbar has moved: repaint old area
//
void CFlatToolBar::OnWindowPosChanged(LPWINDOWPOS lpwp)
{
	CToolBar::OnWindowPosChanged(lpwp);

#ifndef ILLUSTRATE_DISPLAY_BUG
	if (!(lpwp->flags & SWP_NOMOVE)) {	 // if moved:
		// Now paint my non-client area at the new location.
		// This is the extra bit of border space surrounding the buttons.
		// Without this, you will still have a partial display bug (try it!)
		//
		SendMessage(WM_NCPAINT);
	}
#endif
}

////////////////////////////////////////////////////////////////
// The following stuff is to make the command update UI mechanism
// work properly for flat tool bars. The main idea is to convert
// a "checked" button state into a "pressed" button state. Changed 
// lines marked with "PD"

void CFlatOrCoolBarCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;
	if (!bOn)
	{
		nNewStyle |= TBBS_DISABLED;
		// WINBUG: If a button is currently pressed and then is disabled
		// COMCTL32.DLL does not unpress the button, even after the mouse
		// button goes up!  We work around this bug by forcing TBBS_PRESSED
		// off when a button is disabled.
		nNewStyle &= ~TBBS_PRESSED;
	}
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}

// Take your pick:
#define MYTBBS_CHECKED TBBS_CHECKED			// use "checked" state
//#define MYTBBS_CHECKED TBBS_PRESSED		// use pressed state

//////////////////
// This is the only function that has changed: instead of TBBS_CHECKED,
// I use TBBS_PRESSED--PD
//
void CFlatOrCoolBarCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nOldStyle = pToolBar->GetButtonStyle(m_nIndex); // PD
	UINT nNewStyle = nOldStyle &
				~(MYTBBS_CHECKED | TBBS_INDETERMINATE); // PD
	if (nCheck == 1)
		nNewStyle |= MYTBBS_CHECKED; // PD
	else if (nCheck == 2)
		nNewStyle |= TBBS_INDETERMINATE;

	// Following is to fix display bug for TBBS_CHECKED:
	// If new state is unchecked, repaint--but only if style actually changing.
	// (Otherwise will end up with flicker)
	// 
	if (nNewStyle != nOldStyle) {
		ASSERT(!(nNewStyle & TBBS_SEPARATOR));
		pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
		pToolBar->Invalidate();
	}
}

void CFlatOrCoolBarCmdUI::SetText(LPCTSTR)
{
	// ignore for now, but you should really set the text
}

//////////////////
// This function is mostly copied from CToolBar/BARTOOL.CPP. The only thing
// that's different is I instantiated a CFlatOrCoolBarCmdUI instead of
// CToolCmdUI.
//
void CFlatToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CFlatOrCoolBarCmdUI state; // <<<< This is the only line that's different--PD
	state.m_pOther = this;

	state.m_nIndexMax = (UINT)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		// get button state
		TBBUTTON button;
		VERIFY(DefWindowProc(TB_GETBUTTON, state.m_nIndex, (LPARAM)&button));
		// TBSTATE_ENABLED == TBBS_DISABLED so invert it
		button.fsState ^= TBSTATE_ENABLED;

		state.m_nID = button.idCommand;

		// ignore separators
		if (!(button.fsStyle & TBSTYLE_SEP))
		{
			// allow the toolbar itself to have update handlers
			if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

//////////////////
// Make the parent frame my owner. This is important for status bar
// prompts to work. Note that when you create the CCoolToolBar in
// CYourCoolBar::OnCreateBands, you must also set CBRS_FLYBY in the
// the CCoolToolBar style!
//

BOOL CFlatToolBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CFrameWnd* pFrame = GetParentFrame();
	ASSERT_VALID(pFrame);
	SetOwner(pFrame);
	return CToolBar::OnNcCreate(lpCreateStruct);
}
