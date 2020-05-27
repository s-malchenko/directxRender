#pragma once

// Graphics exception
#define GFX_EXCEPT_NOINFO(hr) GraphicsException(__FILE__, __LINE__, (hr))
#define GFX_THROW_NOINFO(hrcall) { if (HRESULT _hResult; FAILED(_hResult = (hrcall))) throw GraphicsException(__FILE__, __LINE__, _hResult); }

#ifndef NDEBUG
#define GFX_EXCEPT(hr) GraphicsException(__FILE__, __LINE__, (hr), GraphicsException::infoManager.GetMessages())
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) DeviceRemovedException(__FILE__, __LINE__, _hResult, GraphicsException::infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
{ \
	GraphicsException::infoManager.Set(); \
	if (HRESULT _hResult; FAILED(_hResult = (hrcall))) \
	{ \
		throw GraphicsException(__FILE__, __LINE__, _hResult, GraphicsException::infoManager.GetMessages()); \
	} \
}

#define GFX_THROW_INFO_VOID(voidcall) \
{ \
	GraphicsException::infoManager.Set(); \
	(voidcall); \
	auto info = GraphicsException::infoManager.GetMessages(); \
	if (!info.empty()) \
	{ \
		throw GraphicsException(__FILE__, __LINE__, E_FAIL, info); \
	} \
}

#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(_hResult) DeviceRemovedException(__FILE__, __LINE__, _hResult)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_THROW_INFO_VOID(voidcall) (voidcall)
#endif
