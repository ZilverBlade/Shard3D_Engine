#pragma once

#ifdef _WIN32       // WIN32 playback
#include <windows.h>
#include <windowsx.h>
#include <mfplay.h>
#include <mferror.h>
#include <strsafe.h>
#include <Shlwapi.h>
#endif

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../utils/engine_utils.hpp"
namespace VideoPlaybackEngine {

#ifdef _WIN32
    class MediaPlayerCallback : public IMFPMediaPlayerCallback
    {
        friend class EngineH264Video;
        long m_cRef; // Reference count
        bool* playbackstateptr = nullptr;
        HWND* hwndptr = nullptr;
        GLFWwindow* glfwwinptr = nullptr;
    public:
        
        MediaPlayerCallback() : m_cRef(1)
        {
        }

        IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
        {
            static const QITAB qit[] =
            {
                QITABENT(MediaPlayerCallback, IMFPMediaPlayerCallback),
                { 0 },
            };
            return QISearch(this, qit, riid, ppv);
        }

        IFACEMETHODIMP_(ULONG) AddRef()
        {
            return InterlockedIncrement(&m_cRef);
        }

        IFACEMETHODIMP_(ULONG) Release()
        {
            ULONG count = InterlockedDecrement(&m_cRef);
            if (count == 0)
            {
                delete this;
                return 0;
            }
            return count;
        }

        // IMFPMediaPlayerCallback methods
        IFACEMETHODIMP_(void) OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader);
    };
#endif


	class EngineH264Video {
	public:
		EngineH264Video();
		~EngineH264Video();
        void destroyPlayback();
		void createVideoSession(GLFWwindow* window, const std::string& mediaFile);
        bool isPlaying() { return playbackstate; }

	private:
        bool playbackstate; // to prevent modification of this value
		// WINDOWS
        friend class MediaPlayerCallback;
#ifdef _WIN32
        HWND hwndwin;
		inline void PlayVideo(PCWSTR pszURL);
#endif
	};

}