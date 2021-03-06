#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <windef.h>

#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#define GENERIC_IOCTL_DEVICE	0x00222233

#define IOCTL_READ		CTL_CODE(GENERIC_IOCTL_DEVICE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_WRITE		CTL_CODE(GENERIC_IOCTL_DEVICE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MODULE_ADDRESS	CTL_CODE(GENERIC_IOCTL_DEVICE, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

struct READ_REQUEST
{
	DWORD process_id;
	PVOID from;
	PVOID to;
	SIZE_T size;
};

struct WRITE_REQUEST
{
	DWORD process_id;
	PVOID to;
	PVOID from;
	SIZE_T size;
};

struct MODULE_REQUEST
{
	DWORD process_id;
	WCHAR name[256];
	PVOID address;
};

#endif
