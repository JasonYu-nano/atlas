// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

// Use minimal api instead of Windows.h to avoid include useless stuff.

#define WIN32_LEAN_AND_MEAN
//NOGDICAPMASKS     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//NOVIRTUALKEYCODES - VK_*
//NOWINMESSAGES     - WM_*, EM_*, LB_*, CB_*
//NOWINSTYLES       - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//NOSYSMETRICS      - SM_*
//NOMENUS           - MF_*
//NOICONS           - IDI_*
//NOKEYSTATES       - MK_*
//NOSYSCOMMANDS     - SC_*
//NORASTEROPS       - Binary and Tertiary raster ops
//NOSHOWWINDOW      - SW_*
//OEMRESOURCE       - OEM Resource values
//NOATOM            - Atom Manager routines
//NOCLIPBOARD       - Clipboard routines
//NOCOLOR           - Screen colors
//NOCTLMGR          - Control and Dialog routines
//NODRAWTEXT        - DrawText() and DT_*
//NOGDI             - All GDI defines and routines
#define NOKERNEL          //- All KERNEL defines and routines
//NOUSER            - All USER defines and routines
//NONLS             - All NLS defines and routines
//NOMB              - MB_* and MessageBox()
//NOMEMMGR          - GMEM_*, LMEM_*, GHND, LHND, associated routines
//NOMETAFILE        - typedef METAFILEPICT
#define NOMINMAX          //- Macros min(a,b) and max(a,b)
//NOMSG             - typedef MSG and associated routines
//NOOPENFILE        - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
//NOSCROLL          - SB_* and scrolling routines
//NOSERVICE         - All Service Controller routines, SERVICE_ equates, etc.
//NOSOUND           - Sound driver routines
//NOTEXTMETRIC      - typedef TEXTMETRIC and associated routines
//NOWH              - SetWindowsHook and WH_*
//NOWINOFFSETS      - GWL_*, GCL_*, associated routines
//NOCOMM            - COMM driver routines
//NOKANJI           - Kanji support stuff.
//NOHELP            - Help engine interface.
//NOPROFILER        - Profiler interface.
//NODEFERWINDOWPOS  - DeferWindowPos routines
//NOMCX             - Modem Configuration Extensions

#include <Windows.h>

#ifdef MoveFile
#undef MoveFile
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

#ifdef CopyFile
#undef CopyFile
#endif


