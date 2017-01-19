#include "stdafx.h"
#include "PGWallpaper.h"

#include <Shlobj.h>

CPGWallpaper::CPGWallpaper() : m_pWallpaper(nullptr)
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	Init();
}

CPGWallpaper::~CPGWallpaper()
{
	Release();
	CoUninitialize();
}

HRESULT CPGWallpaper::Enable(BOOL bEnable/* = TRUE*/)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->Enable(bEnable);
}

HRESULT CPGWallpaper::GetMonitorRECT(UINT nIndex, RECT* rc)
{
	std::shared_ptr<WCHAR> pPathAt = GetMonitorDevicePathAt(nIndex);
	if (nullptr == pPathAt)
		return E_FAIL;

	return m_pWallpaper->GetMonitorRECT(pPathAt.get(), rc);
}

HRESULT CPGWallpaper::GetMonitorDevicePathCount(UINT* count)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->GetMonitorDevicePathCount(count);
}

HRESULT CPGWallpaper::GetBackgroundColor(COLORREF* color)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->GetBackgroundColor(color);
}

HRESULT CPGWallpaper::SetBackgroundColor(COLORREF newColor)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->SetBackgroundColor(newColor);
}

HRESULT CPGWallpaper::GetPosition(DESKTOP_WALLPAPER_POSITION* position)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->GetPosition(position);
}

HRESULT CPGWallpaper::SetPosition(DESKTOP_WALLPAPER_POSITION position)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;
	return m_pWallpaper->SetPosition(position);
}

HRESULT CPGWallpaper::GetWallpaper(UINT monitorIndex, std::wstring& wallpaper)
{
	std::shared_ptr<WCHAR> pPathAt = GetMonitorDevicePathAt(monitorIndex);
	if (nullptr == pPathAt)
		return E_FAIL;

	LPWSTR szWallpaper = nullptr;
	HRESULT hr = m_pWallpaper->GetWallpaper(pPathAt.get(), &szWallpaper);
	if (FAILED(hr))
		return hr;

	wallpaper = szWallpaper;
	::CoTaskMemFree(szWallpaper);

	return hr;
}

HRESULT CPGWallpaper::SetWallpaper(UINT monitorIndex, LPCWSTR wallpaper)
{
	std::shared_ptr<WCHAR> pPathAt = GetMonitorDevicePathAt(monitorIndex);
	if (nullptr == pPathAt)
		return E_FAIL;

	return m_pWallpaper->SetWallpaper(pPathAt.get(), wallpaper);
}

HRESULT CPGWallpaper::AdvanceSlideshow(UINT monitorIndex, DESKTOP_SLIDESHOW_DIRECTION direction)
{
	std::shared_ptr<WCHAR> pPathAt = GetMonitorDevicePathAt(monitorIndex);
	if (nullptr == pPathAt)
		return E_FAIL;

	return m_pWallpaper->AdvanceSlideshow(pPathAt.get(), direction);
}

HRESULT CPGWallpaper::GetSlideshow(IShellItemArray** items)
{
	return E_NOTIMPL;
}

HRESULT CPGWallpaper::SetSlideshow(LPCWSTR szSlidePath)
{
	LPITEMIDLIST  pidl;
	LPSHELLFOLDER pDesktopFolder;
	ULONG chEaten;
	
	std::shared_ptr<WCHAR> szPath(new WCHAR[wcslen(szSlidePath) + 1]);
	lstrcpy(szPath.get(), szSlidePath);
	HRESULT hr = ::SHGetDesktopFolder(&pDesktopFolder);
	if (FAILED(hr))
		return hr;

	hr = pDesktopFolder->ParseDisplayName(nullptr, nullptr, szPath.get(), &chEaten, &pidl, nullptr);
	if (FAILED(hr))
		return hr;

	IShellItemArray* pIShell = nullptr;
	hr = SHCreateShellItemArrayFromIDLists(1, const_cast<LPCITEMIDLIST*>(&pidl), &pIShell);
	m_pWallpaper->SetSlideshow(pIShell);

	return hr;
}

HRESULT CPGWallpaper::GetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS* options, UINT* slideshowTick)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;

	return m_pWallpaper->GetSlideshowOptions(options, slideshowTick);
}

HRESULT CPGWallpaper::SetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS options, UINT slideshowTick)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;

	return m_pWallpaper->SetSlideshowOptions(options, slideshowTick);
}

HRESULT CPGWallpaper::GetStatus(DESKTOP_SLIDESHOW_STATE* state)
{
	if (nullptr == m_pWallpaper)
		return E_POINTER;

	return m_pWallpaper->GetStatus(state);
}

std::shared_ptr<WCHAR> CPGWallpaper::GetMonitorDevicePathAt(UINT nIndex)
{
	if (nullptr == m_pWallpaper)
		return nullptr;
	
	UINT nCount = 0;
	HRESULT hr = GetMonitorDevicePathCount(&nCount);
	if (FAILED(hr) || nIndex >= nCount)
		return nullptr;

	LPWSTR szPathAt = nullptr;
	hr = m_pWallpaper->GetMonitorDevicePathAt(nIndex, &szPathAt);
	if (FAILED(hr))
		return nullptr;
	
	return std::shared_ptr<WCHAR>(szPathAt, CoTaskDeleter());
}

HRESULT CPGWallpaper::Init()
{
	return m_pWallpaper.CoCreateInstance(CLSID_DesktopWallpaper);
}

inline void CPGWallpaper::Release()
{
	if (nullptr != m_pWallpaper)
	{
		m_pWallpaper.Release();
		m_pWallpaper = nullptr;
	}
}