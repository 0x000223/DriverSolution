#pragma once
#include "imports.hpp"
#include "structs.hpp"

#ifndef ROUTINES_HPP
#define ROUTINES_HPP

#include "include.hpp"

NTSTATUS read_memory(READ_REQUEST* request);

NTSTATUS write_memory(WRITE_REQUEST* request);

NTSTATUS get_module_address(MODULE_REQUEST* request);

#endif