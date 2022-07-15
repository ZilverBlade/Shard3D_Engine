#include "../s3dtpch.h"
#include "video_decode.hpp"
#include <filesystem>
#include "../singleton.hpp"

namespace VideoPlaybackEngine {
#ifdef _WIN32
	static inline IMFPMediaPlayer* g_pPlayer = nullptr;      // The MFPlay player object.	
	static inline MediaPlayerCallback* g_pPlayerCB = nullptr;
	static inline int g_bHasVideo = 0;
#endif

	EngineH264Video::EngineH264Video() : playbackstate(false), hwndwin(nullptr) {}
	EngineH264Video::~EngineH264Video() {}

	void EngineH264Video::destroyPlayback() {
		g_pPlayer->Stop();
		Shard3D::SafeRelease(&g_pPlayer);
		PostQuitMessage(0);
	}

	void EngineH264Video::createVideoSession(GLFWwindow* window, const std::string& mediaFile) {
		SHARD3D_INFO("Creating video session");
		wchar_t absolutePath = (wchar_t)std::filesystem::absolute(mediaFile).string().c_str();

		PlayVideo(L"assets\\videodata\\shard3d.wmv");
	}
	// WINDOWS
#ifdef _WIN32
	void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT* pEvent)
	{
		// The media item was created successfully.
		//std::cout << "OnItemCreated\n";
	
		if (g_pPlayer)
		{
			BOOL    bHasVideo = FALSE;
			BOOL    bIsSelected = FALSE;
	
			// Check if the media item contains video.
			HRESULT hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);
			//std::cout << "bHasVideo "<< bHasVideo << "\n";
			//std::cout << "bIsSelected " << bIsSelected << "\n";
	
			if (SUCCEEDED(hr))
			{
				g_bHasVideo = bHasVideo && bIsSelected;
	
				// Set the media item on the player. This method completes
				// asynchronously.
				hr = g_pPlayer->SetMediaItem(pEvent->pMediaItem);
				Shard3D::LOGGER::getDebugLogger()->trace("Succeded playing '{0}'", hr);
			}
	
			if (FAILED(hr))
			{
				SHARD3D_WARN("Failed playing '{0}'", hr);
			}
		}
	}
	void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT* /*pEvent*/)
	{
		//std::cout << "OnMediaItemSet\n";
	
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
		
		case MFP_EVENT_TYPE_PLAYBACK_ENDED:
			Shard3D::LOGGER::getDebugLogger()->trace("Playback ended, stopping media");
			g_pPlayer->Stop();
			Shard3D::SafeRelease(&g_pPlayer);
			*playbackstateptr = false;
			CloseWindow(*hwndptr);
			DestroyWindow(*hwndptr);
			glfwWaitEvents();
			glfwDestroyWindow(glfwwinptr);

			g_pPlayerCB = nullptr;
			break;
		}
	}

	inline void EngineH264Video::PlayVideo(PCWSTR pszURL) {
		HRESULT hr = S_OK;



		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		GLFWwindow* pWindow = glfwCreateWindow(
			1280,
			720,
			"media.wmv",
			nullptr,
			nullptr
		);
		hwndwin = glfwGetWin32Window(pWindow);

		SetParent(hwndwin, glfwGetWin32Window(Shard3D::Singleton::engineWindow.getGLFWwindow()));

		long style = GetWindowLong(hwndwin, GWL_STYLE);
		style &= ~WS_POPUP; // remove popup style
		style |= WS_CHILDWINDOW; // add childwindow style
		SetWindowLong(hwndwin, GWL_STYLE, style);


		ShowWindow(hwndwin, SW_SHOW);


		// Create the MFPlayer object.
		if (g_pPlayer == NULL)
		{
			g_pPlayerCB = new (std::nothrow) MediaPlayerCallback();
			g_pPlayerCB->playbackstateptr = &playbackstate;
			g_pPlayerCB->hwndptr = &hwndwin;
			g_pPlayerCB->glfwwinptr = pWindow;
			if (g_pPlayerCB == NULL)
			{
				return;
			}
			SHARD3D_LOG("creating media");
			hr = MFPCreateMediaPlayer(
				pszURL,			// path
				TRUE,           // Start playback automatically?
				_MFP_CREATION_OPTIONS::MFP_OPTION_FREE_THREADED_CALLBACK,              // Flags
				g_pPlayerCB,    // Callback pointer
				hwndwin,           // Video window
				&g_pPlayer
			);
			//int len = WideCharToMultiByte(CP_ACP, 0, pszURL, wcslen(pszURL), NULL, 0, NULL, NULL);
			//char* buffer = new char[len + 1];
			//WideCharToMultiByte(CP_ACP, 0, pszURL, wcslen(pszURL), buffer, len, NULL, NULL);
			//buffer[len] = '\0';

			//SHARD3D_LOG("Loaded video file '{0}'", buffer);
			//std::cout << "HR code " << hr << "\n";
		}

		// Create a new media item for this URL.

		if (SUCCEEDED(hr))
		{
			hr = g_pPlayer->CreateMediaItemFromURL(pszURL, FALSE, 0, NULL);
			playbackstate = true;
		}

		// The CreateMediaItemFromURL method completes asynchronously.
		// The application will receive an MFP_EVENT_TYPE_MEDIAITEM_CREATED
		// event. See MediaPlayerCallback::OnMediaPlayerEvent().
		//std::cout << hr << " code for media creation\n";
		return;
	}
	
#endif
}