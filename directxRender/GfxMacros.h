#pragma once

// Graphics exception
#define GFX_EXCEPT_NOINFO(hr) Graphics::Exception(__FILE__, __LINE__, (hr))
#define GFX_THROW_NOINFO(hrcall) { if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw Graphics::Exception(__FILE__, __LINE__, _hResult); }

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::Exception(__FILE__, __LINE__, (hr), infoManager.GetMessages())
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult, infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
{ \
	infoManager.Set(); \
	if (HRESULT _hResult; FAILED(_hResult = (hrcall))) \
	{ \
		throw Graphics::Exception(__FILE__, __LINE__, _hResult, infoManager.GetMessages()); \
	} \
}

#define GFX_THROW_INFO_VOID(voidcall) \
{ \
	infoManager.Set(); \
	(voidcall); \
	auto info = infoManager.GetMessages(); \
	if (!info.empty()) \
	{ \
		throw Graphics::Exception(__FILE__, __LINE__, E_FAIL, info); \
	} \
}

#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) Graphics::DeviceRemovedException(__FILE__, __LINE__, _hResult)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_THROW_INFO_VOID(voidcall) (voidcall)
#endif
