#include "../s3dtpch.h"
#include "video_decode.hpp"
#include <filesystem>
#include "../engine_logger.hpp"

namespace VideoPlaybackEngine {
	EngineH264Video::EngineH264Video() {}
	EngineH264Video::~EngineH264Video() {}

	void EngineH264Video::destroyPlayback() {
		Shard3D::SafeRelease(&g_pPlayer);
		PostQuitMessage(0);
	}

	void EngineH264Video::createVideoSession(GLFWwindow* window, const std::string& mediaFile) {
		SHARD3D_INFO("Creating video session");
		PCWSTR absolutePath = std::filesystem::absolute(mediaFile).c_str();
		PlayVideo(glfwGetWin32Window(window), L"D:\\video.wmv");
	}

	// WINDOWS
#ifdef _WIN32
	void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT* pEvent)
	{
		// The media item was created successfully.
		std::cout << "OnItemCreated\n";

		if (g_pPlayer)
		{
			BOOL    bHasVideo = FALSE;
			BOOL    bIsSelected = FALSE;

			// Check if the media item contains video.
			HRESULT hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);
			std::cout << "bHasVideo "<< bHasVideo << "\n";
			std::cout << "bIsSelected " << bIsSelected << "\n";

			if (SUCCEEDED(hr))
			{
				g_bHasVideo = bHasVideo && bIsSelected;

				// Set the media item on the player. This method completes
				// asynchronously.
				hr = g_pPlayer->SetMediaItem(pEvent->pMediaItem);
				SHARD3D_INFO("Succeded playing this file. {0}", hr);
			}

			if (FAILED(hr))
			{
				SHARD3D_WARN("Failed playing this file. {0}", hr);
			}
		}
	}
	void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT* /*pEvent*/)
	{
		std::cout << "OnMediaItemSet\n";

		HRESULT hr = g_pPlayer->Play();
		if (FAILED(hr))
		{
			SHARD3D_WARN("IMFPMediaPlayer::Play failed. {0}", hr);
		}
	}
	IFACEMETHODIMP_(void) MediaPlayerCallback::OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader)
	{
		std::cout << "OnMediaPlayerEvent\n";
		if (FAILED(pEventHeader->hrEvent))
		{
			SHARD3D_WARN("Playback failed {0}", pEventHeader->hrEvent);
			return;
		}

		switch (pEventHeader->eEventType)
		{
		case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
			OnMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
			break;

		case MFP_EVENT_TYPE_MEDIAITEM_SET:
			OnMediaItemSet(MFP_GET_MEDIAITEM_SET_EVENT(pEventHeader));
			break;
		}
	}

	HRESULT EngineH264Video::PlayVideo(HWND hwnd, PCWSTR pszURL) {
		HRESULT hr = S_OK;

		// Create the MFPlayer object.
		if (g_pPlayer == NULL)
		{
			g_pPlayerCB = new (std::nothrow) MediaPlayerCallback();

			if (g_pPlayerCB == NULL)
			{
				return E_OUTOFMEMORY;
			}

			hr = MFPCreateMediaPlayer(
				pszURL,			// path
				TRUE,           // Start playback automatically?
				0,              // Flags
				g_pPlayerCB,    // Callback pointer
				hwnd,           // Video window
				&g_pPlayer
			);
			int len = WideCharToMultiByte(CP_ACP, 0, pszURL, wcslen(pszURL), NULL, 0, NULL, NULL);
			char* buffer = new char[len + 1];
			WideCharToMultiByte(CP_ACP, 0, pszURL, wcslen(pszURL), buffer, len, NULL, NULL);
			buffer[len] = '\0';

			std::cout << buffer << " is the video file\n";
			std::cout << "HR code " << hr << "\n";
		}

		// Create a new media item for this URL.

		if (SUCCEEDED(hr))
		{
			hr = g_pPlayer->CreateMediaItemFromURL(pszURL, FALSE, 0, NULL);
		}

		// The CreateMediaItemFromURL method completes asynchronously.
		// The application will receive an MFP_EVENT_TYPE_MEDIAITEM_CREATED
		// event. See MediaPlayerCallback::OnMediaPlayerEvent().
		std::cout << hr << " code for media creation\n";
		return hr;
	}
	
#endif
}