#include "include.hpp"
#include "routines.hpp"

EXTERN_C void		driver_unload(PDRIVER_OBJECT driver_objcet);
EXTERN_C NTSTATUS	create_close(PDEVICE_OBJECT device_object, PIRP irp);
EXTERN_C NTSTATUS	device_control(PDEVICE_OBJECT device_object, PIRP irp);
EXTERN_C NTSTATUS	not_supported(PDEVICE_OBJECT device_object, PIRP irp);

EXTERN_C
NTSTATUS driver_entry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	DbgPrint("[GenericIOCTL] : driver_entry()\n");

	for(unsigned index = 0; index < IRP_MJ_MAXIMUM_FUNCTION; index++)
	{
		driver_object->MajorFunction[index] = not_supported;
	}

	driver_object->DriverUnload = driver_unload;
	driver_object->MajorFunction[IRP_MJ_CREATE] = create_close;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = create_close;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control;
	
	UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\GenericIOCTL");
	UNICODE_STRING symbol_link = RTL_CONSTANT_STRING(L"\\??\\GenericIOCTL");

	PDEVICE_OBJECT device_object { };

	auto status = IoCreateDevice
	(
		driver_object,
		0,
		&device_name,
		GENERIC_IOCTL_DEVICE,
		0,
		FALSE,
		&device_object
	);

	if(!NT_SUCCESS(status))
	{
		DbgPrint("[GenericIOCTL] : Failed to create device\n");
		return status;
	}

	status = IoCreateSymbolicLink(&symbol_link, &device_name);
	if(!NT_SUCCESS(status))
	{
		DbgPrint("[GenericIOCTL] : Failed to create symbol link\n");
		IoDeleteDevice(device_object);
		return status;
	}
	
	return STATUS_SUCCESS;
}

EXTERN_C
void driver_unload(PDRIVER_OBJECT driver_object)
{
	DbgPrint("[GenericIOCTL] : driver_unload()\n");
	
	UNICODE_STRING symbol_link = RTL_CONSTANT_STRING(L"\\??\\GenericIOCTL");

	IoDeleteSymbolicLink(&symbol_link);

	IoDeleteDevice(driver_object->DeviceObject);
}

EXTERN_C
NTSTATUS create_close(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	DbgPrint("[GenericIOCTL] : create_close()\n");

	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}

EXTERN_C
NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	DbgPrint("[GenericIOCTL] : device_control\n");

	NTSTATUS status;
	ULONG size;

	auto* input = irp->AssociatedIrp.SystemBuffer;

	switch(IoGetCurrentIrpStackLocation(irp)->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_READ:
		{
			status = read_memory(static_cast<READ_REQUEST*>(input));
			size = sizeof(READ_REQUEST);
			break;
		}
	case IOCTL_WRITE:
		{
			status = write_memory(static_cast<WRITE_REQUEST*>(input));
			size = sizeof(WRITE_REQUEST);
			break;
		}
	case IOCTL_MODULE_ADDRESS:
		{
			status = get_module_address(static_cast<MODULE_REQUEST*>(input));
			size = sizeof(MODULE_REQUEST);
			break;
		}
		default:
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			size = NULL;
			break;
		}
	}

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = size;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

EXTERN_C
NTSTATUS not_supported(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);
	
	DbgPrint("[GenericIOCTL] : Request is not supported!\n");

	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}
