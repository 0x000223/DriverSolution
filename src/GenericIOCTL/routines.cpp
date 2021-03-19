#include "routines.hpp"

NTSTATUS read_memory(READ_REQUEST* request)
{
	DbgPrint("[GenericIOCTL] : read_memory\n");

	PEPROCESS process;

	if(PsLookupProcessByProcessId(
		reinterpret_cast<HANDLE>(request->process_id), &process) == STATUS_INVALID_CID)
	{
		DbgPrint("[GenericIOCTL] : Failed to find process object\n");
		return STATUS_CANCELLED;
	}

	__try
	{
		ProbeForRead(request->from, request->size, 1);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at ProbeForRead\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	__try
	{
		ProbeForWrite(request->to, request->size, 1);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at ProbeForWrite\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	__try
	{
		size_t copied;

		MmCopyVirtualMemory
		(
			process,
			request->from,
			PsGetCurrentProcess(),
			request->to,
			request->size,
			KernelMode,
			&copied
		);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at MmCopyVirtualMemory\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	ObDereferenceObject(process);
	
	return STATUS_SUCCESS;
}

NTSTATUS write_memory(WRITE_REQUEST* request)
{
	DbgPrint("[GenericIOCTL] : write_memory\n");

	PEPROCESS process;

	if(PsLookupProcessByProcessId(
		reinterpret_cast<HANDLE>(request->process_id), &process) == STATUS_INVALID_CID)
	{
		DbgPrint("[GenericIOCTL] : Failed to find process object\n");
		return STATUS_CANCELLED;
	}

	__try
	{
		ProbeForRead(request->from, request->size, 1);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at ProbeForRead\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	__try
	{
		ProbeForWrite(request->to, request->size, 1);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at ProbeForWrite\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	__try
	{
		size_t copied;

		MmCopyVirtualMemory
		(
			PsGetCurrentProcess(),
			request->from,
			process,
			request->to,
			request->size,
			KernelMode,
			&copied
		);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[GenericIOCTL] : Exception at MmCopyVirtualMemory\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	ObDereferenceObject(process);

	return STATUS_SUCCESS;
}

NTSTATUS get_module_address(MODULE_REQUEST* request)
{
	DbgPrint("[GenericIOCTL] : get_module_address\n");

	PEPROCESS process;

	if(PsLookupProcessByProcessId(
		reinterpret_cast<HANDLE>(request->process_id), &process) == STATUS_INVALID_CID)
	{
		DbgPrint("[GenericIOCTL] : Failed to find process object\n");
		return STATUS_CANCELLED;
	}

	UNICODE_STRING module_name;
	RtlInitUnicodeString(&module_name, request->name);

	KAPC_STATE apc;
	KeStackAttachProcess(process, &apc);
	
	auto* peb = PsGetProcessPeb(process);
	if(!peb)
	{
		DbgPrint("[GenericIOCTL] : Failed to get process object\n");
		ObDereferenceObject(process);
		return STATUS_CANCELLED;
	}

	for(auto* list = peb->Ldr->InLoadOrderModuleList.Flink; 
        list != &peb->Ldr->InLoadOrderModuleList; 
        list = list->Flink)
    {
	    auto* entry { CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) };

    	if(!RtlCompareUnicodeString(&entry->BaseDllName, &module_name, TRUE))
    	{
    		request->address = entry->DllBase;
    		break;
    	}
    }

	KeUnstackDetachProcess(&apc);

	ObDereferenceObject(process);

	return STATUS_SUCCESS;
}
