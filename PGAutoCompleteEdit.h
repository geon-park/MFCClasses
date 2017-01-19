#pragma once

#include <atlbase.h>
#include <vector>
#include <list>
#include <string>
#include "PGEnumString.h"

// CPGAutoCompleteEdit

class CPGAutoCompleteEdit : public CEdit
{
	DECLARE_DYNAMIC(CPGAutoCompleteEdit)

public:
	CPGAutoCompleteEdit();
	virtual ~CPGAutoCompleteEdit();

private:
	CPGEnumString* m_pEnum;
	CComPtr<IAutoComplete> m_pac;
	std::vector<std::wstring> m_vsWordList;

public:
	int SetList(const std::vector<std::wstring> vecNewList);
	int SetList(std::vector<std::wstring>&& vecNewList);
	// int SetList(std::list<std::wstring> lstNewList);

	int ClearList();

public:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};