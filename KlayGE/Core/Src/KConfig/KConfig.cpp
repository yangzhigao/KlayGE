// KConfig.cpp
// KlayGE Configuration tool implement file
// Ver 3.11.0
// Copyright(C) Minmin Gong, 2010
// Homepage: http://www.klayge.org
//
// 3.11.0
// First release (2010.8.15)
//
// CHANGE LIST
/////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/ResLoader.hpp>
#include <KlayGE/Util.hpp>

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <commctrl.h>
#include <sstream>
#include "resource.h"

using namespace KlayGE;

ContextCfg cfg;
bool save_cfg = false;

INT_PTR CALLBACK Graphics_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Audio_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Input_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Show_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum nTabDialogs
{
	GRAPHICS_TAB,
	AUDIO_TAB,
	INPUT_TAB,
	SHOW_TAB,
	NTABS
};

HWND hTab = NULL; // Handle to tab control.
HWND hTabDlg[NTABS] = {0}; // Array of handle to tab dialogs.
int iCurSelTab = 0;

HWND hOKButton;
HWND hCancelButton;

std::basic_string<TCHAR> tab_dlg_titles[] =
{
	TEXT("Graphics"),
	TEXT("Audio"),
	TEXT("Input"),
	TEXT("Show")
};

std::basic_string<TCHAR> tab_dlg_ids[] =
{
	TEXT("GraphicsTab"),
	TEXT("AudioTab"),
	TEXT("InputTab"),
	TEXT("ShowTab")
};

DLGPROC tab_dlg_procs[] =
{
	Graphics_Tab_DlgProc,
	Audio_Tab_DlgProc,
	Input_Tab_DlgProc,
	Show_Tab_DlgProc
};

INT_PTR CALLBACK Graphics_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hFactoryCombo = GetDlgItem(hDlg, IDC_FACTORY_COMBO);
			HMODULE mod_d3d11 = LoadLibrary(TEXT("D3D11.dll"));
			if (mod_d3d11)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D3D11")));
				FreeLibrary(mod_d3d11);
			}
			HMODULE mod_d3d10 = LoadLibrary(TEXT("D3D10.dll"));
			if (mod_d3d10)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D3D10")));
				FreeLibrary(mod_d3d10);
			}
			HMODULE mod_d3d9 = LoadLibrary(TEXT("D3D9.dll"));
			if (mod_d3d9)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D3D9")));
				FreeLibrary(mod_d3d10);
			}
			HMODULE mod_gl = LoadLibrary(TEXT("OpenGL32.dll"));
			if (mod_gl)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("OpenGL")));
				FreeLibrary(mod_gl);
			}
			HMODULE mod_gles2 = LoadLibrary(TEXT("libGLESv2.dll"));
			if (mod_gles2)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("OpenGLES2")));
				FreeLibrary(mod_gles2);
			}

			TCHAR buf[256];
			int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCOUNT, 0, 0));
			int sel = 0;
			for (int i = 0; i < n; ++ i)
			{
				SendMessage(hFactoryCombo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buf));

				std::string str;
				Convert(str, buf);
				if (str == cfg.render_factory_name)
				{
					sel = i;
				}
			}
			SendMessage(hFactoryCombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hResCombo = GetDlgItem(hDlg, IDC_RES_COMBO);
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1920x1080")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1680x1050")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1280x1024")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1280x960")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1280x800")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1280x720")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1024x768")));
			SendMessage(hResCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("800x600")));

			TCHAR buf[256];
			int n = static_cast<int>(SendMessage(hResCombo, CB_GETCOUNT, 0, 0));
			int sel = 0;
			for (int i = 0; i < n; ++ i)
			{
				SendMessage(hResCombo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buf));

				std::ostringstream oss;
				oss << cfg.graphics_cfg.width << "x" << cfg.graphics_cfg.height;

				std::string str;
				Convert(str, buf);
				if (str == oss.str())
				{
					sel = i;
				}
			}
			SendMessage(hResCombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hClrFmtCombo = GetDlgItem(hDlg, IDC_CLR_FMT_COMBO);
			SendMessage(hClrFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("ARGB8")));
			SendMessage(hClrFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("ABGR8")));
			SendMessage(hClrFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("A2BGR10")));

			int sel = 0;
			switch (cfg.graphics_cfg.color_fmt)
			{
			case EF_ARGB8:
				sel = 0;
				break;

			case EF_ABGR8:
				sel = 1;
				break;

			case EF_A2BGR10:
				sel = 2;
				break;

			default:
				sel = 0;
				break;
			}
			SendMessage(hClrFmtCombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hDepthFmtCombo = GetDlgItem(hDlg, IDC_DEPTH_FMT_COMBO);
			SendMessage(hDepthFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D16")));
			SendMessage(hDepthFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D24S8")));
			SendMessage(hDepthFmtCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("D32F")));

			int sel = 0;
			switch (cfg.graphics_cfg.depth_stencil_fmt)
			{
			case EF_D16:
				sel = 0;
				break;

			case EF_D24S8:
				sel = 1;
				break;

			case EF_D32F:
				sel = 2;
				break;

			default:
				sel = 0;
				break;
			}
			SendMessage(hDepthFmtCombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hAACombo = GetDlgItem(hDlg, IDC_AA_COMBO);
			SendMessage(hAACombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("No")));
			SendMessage(hAACombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("2")));
			SendMessage(hAACombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("4")));
			SendMessage(hAACombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("8")));

			int sel = 0;
			switch (cfg.graphics_cfg.sample_count)
			{
			case 1:
				sel = 0;
				break;

			case 2:
				sel = 1;
				break;

			case 4:
				sel = 2;
				break;

			case 8:
				sel = 3;
				break;

			default:
				sel = 0;
				break;
			}
			SendMessage(hAACombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hFSCombo = GetDlgItem(hDlg, IDC_FS_COMBO);
			SendMessage(hFSCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("Yes")));
			SendMessage(hFSCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("No")));
			SendMessage(hFSCombo, CB_SETCURSEL, cfg.graphics_cfg.full_screen ? 0 : 1, 0);
		}
		{
			HWND hSyncCombo = GetDlgItem(hDlg, IDC_SYNC_COMBO);
			SendMessage(hSyncCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("0")));
			SendMessage(hSyncCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("1")));
			SendMessage(hSyncCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("2")));
			SendMessage(hSyncCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("4")));

			int sel = 0;
			switch (cfg.graphics_cfg.sync_interval)
			{
			case 0:
				sel = 0;
				break;

			case 1:
				sel = 1;
				break;

			case 2:
				sel = 2;
				break;

			case 4:
				sel = 3;
				break;

			default:
				sel = 0;
				break;
			}
			SendMessage(hSyncCombo, CB_SETCURSEL, sel, 0);
		}
		{
			HWND hStereoCombo = GetDlgItem(hDlg, IDC_STEREO_COMBO);
			SendMessage(hStereoCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("Yes")));
			SendMessage(hStereoCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("No")));
			SendMessage(hStereoCombo, CB_SETCURSEL, 1, 0);
			SendMessage(hStereoCombo, CB_SETCURSEL, cfg.graphics_cfg.stereo_mode ? 0 : 1, 0);
		}
		{
			HWND hMBFramesEdit = GetDlgItem(hDlg, IDC_MB_FRAMES_EDIT);

			std::ostringstream oss;
			oss << cfg.graphics_cfg.motion_frames;
			std::basic_string<TCHAR> str;
			Convert(str, oss.str());

			SetWindowText(hMBFramesEdit, str.c_str());
		}
		{
			HWND hStereoSepEdit = GetDlgItem(hDlg, IDC_STEREO_SEP_EDIT);

			std::ostringstream oss;
			oss << cfg.graphics_cfg.stereo_separation;
			std::basic_string<TCHAR> str;
			Convert(str, oss.str());

			SetWindowText(hStereoSepEdit, str.c_str());
		}
		return TRUE;

	default:
		return FALSE;
	}
}

INT_PTR CALLBACK Audio_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hFactoryCombo = GetDlgItem(hDlg, IDC_FACTORY_COMBO);
			HMODULE mod_al = LoadLibrary(TEXT("OpenAL32.dll"));
			if (mod_al)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("OpenAL")));
				FreeLibrary(mod_al);
			}
			HMODULE mod_ds = LoadLibrary(TEXT("dsound.dll"));
			if (mod_ds)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("DSound")));
				FreeLibrary(mod_ds);
			}

			TCHAR buf[256];
			int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCOUNT, 0, 0));
			int sel = 0;
			for (int i = 0; i < n; ++ i)
			{
				SendMessage(hFactoryCombo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buf));

				std::string str;
				Convert(str, buf);
				if (str == cfg.audio_factory_name)
				{
					sel = i;
				}
			}
			SendMessage(hFactoryCombo, CB_SETCURSEL, sel, 0);
		}
		return TRUE;

	default:
		return FALSE;
	}
}

INT_PTR CALLBACK Input_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hFactoryCombo = GetDlgItem(hDlg, IDC_FACTORY_COMBO);
			HMODULE mod_dinput = LoadLibrary(TEXT("dinput8.dll"));
			if (mod_dinput)
			{
				SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("DInput")));
				FreeLibrary(mod_dinput);
			}

			TCHAR buf[256];
			int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCOUNT, 0, 0));
			int sel = 0;
			for (int i = 0; i < n; ++ i)
			{
				SendMessage(hFactoryCombo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buf));

				std::string str;
				Convert(str, buf);
				if (str == cfg.input_factory_name)
				{
					sel = i;
				}
			}
			SendMessage(hFactoryCombo, CB_SETCURSEL, sel, 0);
		}
		return TRUE;

	default:
		return FALSE;
	}
}

INT_PTR CALLBACK Show_Tab_DlgProc(HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hFactoryCombo = GetDlgItem(hDlg, IDC_FACTORY_COMBO);
			SendMessage(hFactoryCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("DShow")));

			TCHAR buf[256];
			int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCOUNT, 0, 0));
			int sel = 0;
			for (int i = 0; i < n; ++ i)
			{
				SendMessage(hFactoryCombo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buf));

				std::string str;
				Convert(str, buf);
				if (str == cfg.show_factory_name)
				{
					sel = i;
				}
			}
			SendMessage(hFactoryCombo, CB_SETCURSEL, sel, 0);
		}
		return TRUE;

	default:
		return FALSE;
	}
}

INT_PTR CreateTabDialogs(HWND hWnd, HINSTANCE hInstance)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	hTab = CreateWindowEx(
				WS_EX_CONTROLPARENT,
				WC_TABCONTROL,
				TEXT(""),
				WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD | TCS_FOCUSONBUTTONDOWN,
				rcClient.left + 5,
				rcClient.top + 5,
				rcClient.right - rcClient.left - 10,
				rcClient.bottom - rcClient.top - 45,
				hWnd,
				NULL,
				hInstance,
				NULL);

	ShowWindow(hTab, SW_SHOWNORMAL);

	TCITEM tci;
	tci.mask       = TCIF_TEXT;
	tci.iImage     = -1;

	RECT rc;
	GetClientRect(hTab, &rc);
	TabCtrl_AdjustRect(hTab, false, &rc);
	rc.top += 20;

	for (int i = 0; i < NTABS; ++ i)
	{
		tci.pszText    = const_cast<TCHAR*>(tab_dlg_titles[i].c_str());
		tci.cchTextMax = static_cast<int>(tab_dlg_titles[i].size());
		TabCtrl_InsertItem(hTab, i, &tci);

		hTabDlg[i] = CreateDialogParam(hInstance, tab_dlg_ids[i].c_str(), hTab, tab_dlg_procs[i], 0);
		MoveWindow(hTabDlg[i], rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
	}

	ShowWindow(hTabDlg[GRAPHICS_TAB], SW_SHOWNORMAL);

	return FALSE;
}

INT_PTR CreateButtons(HWND hWnd, HINSTANCE hInstance)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	hOKButton = CreateWindowEx(
				WS_EX_CONTROLPARENT,
				WC_BUTTON,
				TEXT("OK"),
				WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				rcClient.right - 72 * 2 - 15,
				rcClient.bottom - 27,
				72,
				22,
				hWnd,
				NULL,
				hInstance,
				NULL);
	hCancelButton = CreateWindowEx(
				WS_EX_CONTROLPARENT,
				WC_BUTTON,
				TEXT("Cancel"),
				WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				rcClient.right - 72 - 5,
				rcClient.bottom - 27,
				72,
				22,
				hWnd,
				NULL,
				hInstance,
				NULL);

	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		if (reinterpret_cast<HWND>(lParam) == hOKButton)
		{
			{
				{
					HWND hFactoryCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_FACTORY_COMBO);
					int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCURSEL, 0, 0));
					TCHAR buf[256];
					SendMessage(hFactoryCombo, CB_GETLBTEXT, n, reinterpret_cast<LPARAM>(buf));
					Convert(cfg.render_factory_name, buf);
				}
				{
					HWND hResCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_RES_COMBO);
					int n = static_cast<int>(SendMessage(hResCombo, CB_GETCURSEL, 0, 0));
					TCHAR buf[256];
					SendMessage(hResCombo, CB_GETLBTEXT, n, reinterpret_cast<LPARAM>(buf));
					std::string str;
					Convert(str, buf);
					std::string::size_type p = str.find('x');
					std::istringstream(str.substr(0, p)) >> cfg.graphics_cfg.width;
					std::istringstream(str.substr(p + 1, str.size())) >> cfg.graphics_cfg.height;
				}
				{
					HWND hClrFmtCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_CLR_FMT_COMBO);
					int n = static_cast<int>(SendMessage(hClrFmtCombo, CB_GETCURSEL, 0, 0));
					switch (n)
					{
					case 0:
						cfg.graphics_cfg.color_fmt = EF_ARGB8;
						break;

					case 1:
						cfg.graphics_cfg.color_fmt = EF_ABGR8;
						break;

					case 2:
						cfg.graphics_cfg.color_fmt = EF_A2BGR10;
						break;

					default:
						cfg.graphics_cfg.color_fmt = EF_ARGB8;
						break;
					}
				}
				{
					HWND hDepthFmtCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_DEPTH_FMT_COMBO);
					int n = static_cast<int>(SendMessage(hDepthFmtCombo, CB_GETCURSEL, 0, 0));
					switch (n)
					{
					case 0:
						cfg.graphics_cfg.depth_stencil_fmt = EF_D16;
						break;

					case 1:
						cfg.graphics_cfg.depth_stencil_fmt = EF_D24S8;
						break;

					case 2:
						cfg.graphics_cfg.depth_stencil_fmt = EF_D32F;
						break;

					default:
						cfg.graphics_cfg.depth_stencil_fmt = EF_D16;
						break;
					}
				}
				{
					HWND hAACombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_AA_COMBO);
					int n = static_cast<int>(SendMessage(hAACombo, CB_GETCURSEL, 0, 0));
					switch (n)
					{
					case 0:
						cfg.graphics_cfg.sample_count = 1;
						cfg.graphics_cfg.sample_quality = 0;
						break;

					case 1:
						cfg.graphics_cfg.sample_count = 2;
						cfg.graphics_cfg.sample_quality = 0;
						break;

					case 2:
						cfg.graphics_cfg.sample_count = 4;
						cfg.graphics_cfg.sample_quality = 0;
						break;

					case 3:
						cfg.graphics_cfg.sample_count = 8;
						cfg.graphics_cfg.sample_quality = 0;
						break;

					default:
						cfg.graphics_cfg.sample_count = 1;
						cfg.graphics_cfg.sample_quality = 0;
						break;
					}
				}
				{
					HWND hFSCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_FS_COMBO);
					int n = static_cast<int>(SendMessage(hFSCombo, CB_GETCURSEL, 0, 0));
					cfg.graphics_cfg.full_screen = (0 == n) ? 1 : 0;
				}
				{
					HWND hSyncCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_SYNC_COMBO);
					int n = static_cast<int>(SendMessage(hSyncCombo, CB_GETCURSEL, 0, 0));
					switch (n)
					{
					case 0:
						cfg.graphics_cfg.sync_interval = 0;
						break;

					case 1:
						cfg.graphics_cfg.sync_interval = 1;
						break;

					case 2:
						cfg.graphics_cfg.sync_interval = 2;
						break;

					case 3:
						cfg.graphics_cfg.sync_interval = 4;
						break;

					default:
						cfg.graphics_cfg.sync_interval = 0;
						break;
					}
				}
				{
					HWND hStereoCombo = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_STEREO_COMBO);
					int n = static_cast<int>(SendMessage(hStereoCombo, CB_GETCURSEL, 0, 0));
					cfg.graphics_cfg.stereo_mode = (0 == n) ? 1 : 0;
				}
				{
					HWND hMBFramesEdit = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_MB_FRAMES_EDIT);
					TCHAR buf[256];
					GetWindowText(hMBFramesEdit, buf, sizeof(buf) / sizeof(buf[0]));
					std::basic_stringstream<TCHAR>(buf) >> cfg.graphics_cfg.motion_frames;
				}
				{
					HWND hStereoSepEdit = GetDlgItem(hTabDlg[GRAPHICS_TAB], IDC_STEREO_SEP_EDIT);
					TCHAR buf[256];
					GetWindowText(hStereoSepEdit, buf, sizeof(buf) / sizeof(buf[0]));
					std::basic_stringstream<TCHAR>(buf) >> cfg.graphics_cfg.stereo_separation;
				}
			}
			{
				HWND hFactoryCombo = GetDlgItem(hTabDlg[AUDIO_TAB], IDC_FACTORY_COMBO);
				int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCURSEL, 0, 0));
				TCHAR buf[256];
				SendMessage(hFactoryCombo, CB_GETLBTEXT, n, reinterpret_cast<LPARAM>(buf));
				Convert(cfg.audio_factory_name, buf);
			}
			{
				HWND hFactoryCombo = GetDlgItem(hTabDlg[INPUT_TAB], IDC_FACTORY_COMBO);
				int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCURSEL, 0, 0));
				TCHAR buf[256];
				SendMessage(hFactoryCombo, CB_GETLBTEXT, n, reinterpret_cast<LPARAM>(buf));
				Convert(cfg.input_factory_name, buf);
			}
			{
				HWND hFactoryCombo = GetDlgItem(hTabDlg[SHOW_TAB], IDC_FACTORY_COMBO);
				int n = static_cast<int>(SendMessage(hFactoryCombo, CB_GETCURSEL, 0, 0));
				TCHAR buf[256];
				SendMessage(hFactoryCombo, CB_GETLBTEXT, n, reinterpret_cast<LPARAM>(buf));
				Convert(cfg.show_factory_name, buf);
			}

			save_cfg = true;
			::PostQuitMessage(0);
		}
		else if (reinterpret_cast<HWND>(lParam) == hCancelButton)
		{
			save_cfg = false;
			::PostQuitMessage(0);
		}
		break;

	case WM_CLOSE:
		save_cfg = false;
		::PostQuitMessage(0);
		return 0;

	case WM_NOTIFY:
		switch (reinterpret_cast<LPNMHDR>(lParam)->code)
		{
		case TCN_SELCHANGE:
			{
				int iSelTab = TabCtrl_GetCurSel(hTab);
				if (iCurSelTab != iSelTab)
				{
					ShowWindow(hTabDlg[iCurSelTab], SW_HIDE);
					ShowWindow(hTabDlg[iSelTab], SW_SHOWNORMAL);
					iCurSelTab = iSelTab;
				}
				return TRUE;
			}
		}
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool UIConfiguration(HINSTANCE hInstance)
{
	::WNDCLASSEX wc;
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= ::LoadIcon(hInstance, TEXT("IDI_KLAYGEICON"));
	wc.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= TEXT("KConfig");
	wc.hIconSm			= wc.hIcon;
	::RegisterClassEx(&wc);

	int cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);
	int width = 420;
	int height = 470;

	HWND hWnd = ::CreateWindow(wc.lpszClassName, TEXT("KlayGE Configuration Tool"),
		WS_CAPTION | WS_SYSMENU, (cx - width) / 2, (cy - height) / 2,
		width, height, 0, 0, hInstance, NULL);

	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::UpdateWindow(hWnd);

	CreateTabDialogs(hWnd, hInstance);
	CreateButtons(hWnd, hInstance);

	MSG msg;
	memset(&msg, 0, sizeof(msg));
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return save_cfg;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
	std::string cfg_path = ResLoader::Instance().Locate("KlayGE.cfg");
	Context::Instance().LoadCfg(cfg_path);
	cfg = Context::Instance().Config();

	if (UIConfiguration(hInstance))
	{
		Context::Instance().Config(cfg);
		Context::Instance().SaveCfg(cfg_path);
	}

	return 0;
}
