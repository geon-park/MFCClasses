#ifndef __PG_WALLPAPER_H__
#define __PG_WALLPAPER_H__

#include <atlbase.h>
#include <ShObjIdl.h>
#include <vector>
#include <string>
#include <memory>

struct CoTaskDeleter
{
	void operator()(LPWSTR p)
	{
		::CoTaskMemFree(p);
	}
};

class CPGWallpaper
{
public:
	CPGWallpaper();
	~CPGWallpaper();

	HRESULT Enable(BOOL bEnable = TRUE);

	HRESULT GetMonitorRECT(UINT nIndex, RECT* rc);
	HRESULT GetMonitorDevicePathCount(UINT* count);

	HRESULT GetBackgroundColor(COLORREF* color);
	HRESULT SetBackgroundColor(COLORREF newColor);

	HRESULT GetPosition(DESKTOP_WALLPAPER_POSITION* position);
	HRESULT SetPosition(DESKTOP_WALLPAPER_POSITION position);

	HRESULT GetWallpaper(UINT monitorIndex, std::wstring& wallpaper);
	HRESULT SetWallpaper(UINT monitorIndex, LPCWSTR wallpaper);

	HRESULT AdvanceSlideshow(UINT monitorIndex, DESKTOP_SLIDESHOW_DIRECTION direction);
	HRESULT GetSlideshow(IShellItemArray** items);
	HRESULT SetSlideshow(IShellItemArray* items);
	HRESULT SetSlideshow(LPCWSTR szSlidePath);

	HRESULT GetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS* options, UINT* slideshowTick);
	HRESULT SetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS options, UINT slideshowTick);

	HRESULT GetStatus(DESKTOP_SLIDESHOW_STATE* state);

private:
	std::shared_ptr<WCHAR> GetMonitorDevicePathAt(UINT nIndex);
	HRESULT Init();
	void Release();

private:
	CComPtr<IDesktopWallpaper> m_pWallpaper;
};

#endif