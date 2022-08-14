#pragma once
#include <string>

#ifdef _WIN32	
#include <Windows.h>
#include <commdlg.h>
#endif

namespace Shard3D {
	class MessageDialogs {
	public:
		enum DialogResult{
			RESERROR = -1,
//#ifdef _WIN32	
			RESOK			=	1,
			RESCANCEL		=	2,
			RESABORT		=	3,
			RESRETRY		=	4,
			RESIGNORE		=	5,
			RESYES			=	6,
			RESNO			=	7,
			 
			RESCLOSE		=	8,
			RESHELP			=	9,
					
			RESTRYAGAIN		=	10,
			RESCONTINUE		=	11,
			
			RESTIMEOUT		=	32000
//#endif // since other OS use different values
			
		};
		enum DialogOptions {
//#ifdef _WIN32
			OPTOK                      = 0x00000000L,
			OPTOKCANCEL                = 0x00000001L, 
			OPTABORTRETRYIGNORE        = 0x00000002L, 
			OPTYESNOCANCEL             = 0x00000003L, 
			OPTYESNO                   = 0x00000004L, 
			OPTRETRYCANCEL             = 0x00000005L, 
				   				 
			OPTCANCELTRYCONTINUE       = 0x00000006L, 
								   				 
			OPTICONERROR               = 0x00000010L, 
			OPTICONQUESTION            = 0x00000020L, 
			OPTICONEXCLAMATION         = 0x00000030L, 
			OPTICONINFO				   = 0x00000040L, 
								   				 
			OPTUSERICON                = 0x00000080L, 
								   				 
			OPTDEFBUTTON1              = 0x00000000L, 
			OPTDEFBUTTON2              = 0x00000100L, 
			OPTDEFBUTTON3              = 0x00000200L, 
								   				 
			OPTDEFBUTTON4              = 0x00000300L, 
								   				 								   				 
			OPTAPPLMODAL               = 0x00000000L, 
			OPTSYSTEMMODAL             = 0x00001000L, 
			OPTTASKMODAL               = 0x00002000L, 
								   				 
			OPTHELP                    = 0x00004000L, // Help Button
								   								   
			OPTNOFOCUS                 = 0x00008000L,
			OPTSETFOREGROUND           = 0x00010000L,
			OPTDEFAULT_DESKTOP_ONLY    = 0x00020000L,
								   							   
			OPTTOPMOST                 = 0x00040000L,
			OPTRIGHT                   = 0x00080000L,
			OPTRTLREADING              = 0x00100000L
//#endif // since other OS use different values
		};
		// wrapper exists to keep code clean when potentially dealing with other OS'es (future proofing)

		static DialogResult show(const char* text, const char* caption = "", uint32_t options = OPTOKCANCEL | OPTDEFBUTTON1) {
#ifdef _WIN32
			return (DialogResult)(uint32_t)(MessageBoxA(
				nullptr,
				text, caption, options
			));
#endif
#ifdef __linux__ 
			SHARD3D_NOIMPL;
#endif
			return RESERROR;
		}	
	};
	class FileDialogs {
	public:
		// no string returned if cancelled
		static std::string openFile(const char* filter = "All files (*.*)\0*.*\0") {
#ifdef _WIN32
			OPENFILENAMEA ofn;
			CHAR szFile[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = nullptr; //glfwGetWin32Window((GLFWwindow*)EngineWindow::getGLFWwindow());
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetOpenFileNameA(&ofn) == TRUE) {
				return ofn.lpstrFile;
			}
#endif
#ifdef __linux__ 
			SHARD3D_WARN("unsupported function");
#endif
			return std::string();

		}
		static std::string saveFile(const char* filter = "All files (*.*)\0*.*\0") {
#ifdef _WIN32
			OPENFILENAMEA ofn;
			CHAR szFile[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = nullptr; //glfwGetWin32Window((GLFWwindow*)EngineWindow::getGLFWwindow());
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetSaveFileNameA(&ofn) == TRUE) {
				return ofn.lpstrFile;
			}
#endif
#ifdef __linux__ 
			SHARD3D_WARN("unsupported function");
#endif
			return std::string();
		}
	};
}