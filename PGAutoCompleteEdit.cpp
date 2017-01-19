// PGAutoCompleteEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "PGAutoCompleteEdit.h"

// CPGAutoCompleteEdit

IMPLEMENT_DYNAMIC(CPGAutoCompleteEdit, CEdit)

CPGAutoCompleteEdit::CPGAutoCompleteEdit() : m_pEnum(nullptr), m_pac(nullptr), m_vsWordList()
{
}

CPGAutoCompleteEdit::~CPGAutoCompleteEdit()
{
}

BEGIN_MESSAGE_MAP(CPGAutoCompleteEdit, CEdit)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CPGAutoCompleteEdit message handlers

void CPGAutoCompleteEdit::PreSubclassWindow()
{
	HRESULT hr = m_pac.CoCreateInstance(CLSID_AutoComplete);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IAutoComplete2> pAC2(m_pac);
		hr = pAC2->SetOptions(ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND);
		pAC2.Release();

		m_pEnum = new CPGEnumString(m_vsWordList);
		hr = m_pac->Init(m_hWnd, m_pEnum, NULL, NULL);
		m_pac->Enable(TRUE);
	}

	CEdit::PreSubclassWindow();
}

BOOL CPGAutoCompleteEdit::PreTranslateMessage(MSG* pMsg)
{
	return CEdit::PreTranslateMessage(pMsg);
}


BOOL CPGAutoCompleteEdit::OnEraseBkgnd(CDC* pDC)
{
	return CEdit::OnEraseBkgnd(pDC);
}

void CPGAutoCompleteEdit::OnClose()
{
	m_pac->Enable(FALSE);
	m_pac.Release();

	CEdit::OnClose();
}

int CPGAutoCompleteEdit::SetList(const std::vector<std::wstring> vecNewList)
{
	m_vsWordList = vecNewList;

	return 0;
}

int CPGAutoCompleteEdit::SetList(std::vector<std::wstring>&& vecNewList)
{
	m_vsWordList = vecNewList;

	return 0;
}

int CPGAutoCompleteEdit::ClearList()
{
	std::vector<std::wstring> vTemp;
	m_vsWordList.swap(vTemp);

	return 0;
}