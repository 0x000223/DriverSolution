#pragma once
#include "include.hpp"

#ifndef IMPORTS_HPP
#define IMPORTS_HPP

EXTERN_C_START

NTKERNELAPI NTSTATUS NTAPI MmCopyVirtualMemory
(
	IN  PEPROCESS		fromProcess,
	IN  PVOID			fromAddress,
	IN  PEPROCESS		toProcess,
	OUT PVOID			toAddress,
	IN  SIZE_T			bufferSize,
	IN  KPROCESSOR_MODE	previousMode,
	OUT PSIZE_T			numberOfBytesCopied
);

NTKERNELAPI PPEB NTAPI PsGetProcessPeb
(
	IN  PEPROCESS process
);

NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process
(
	IN  PEPROCESS process
);

EXTERN_C_END

#endif