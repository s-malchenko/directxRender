#include "DxgiInfoManager.h"

#include "Graphics.h"
#include "GfxMacros.h"
#include "HrException.h"
#include "Util.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

DxgiInfoManager::DxgiInfoManager()
{
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	if (!hModDxgiDebug)
	{
		throw HR_EXCEPT_LASTERR();
	}

	typedef HRESULT(CALLBACK* PROC)(REFIID, void**);
	const auto DxgiGetDebugInterface = reinterpret_cast<PROC>((GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	if (!DxgiGetDebugInterface)
	{
		throw HR_EXCEPT_LASTERR();
	}

	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &infoQueue));
}

void DxgiInfoManager::Set()
{
	next = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (auto i = next; i < end; ++i)
	{
		size_t length;
		GFX_THROW_NOINFO(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &length));
		auto buffer = std::make_unique<byte[]>(length);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(buffer.get());
		GFX_THROW_NOINFO(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &length));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}
