// Confidential and only for the information of the intended recipient.
// Copyright 2023 Improbable Worlds Limited.

#ifndef LATTICE_SYSTEM_ERROR_H
#define LATTICE_SYSTEM_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifndef DLL_PUBLIC
#ifdef _WIN32
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __attribute__((visibility("default")))
#endif  // _WIN32
#endif  // DLL_PUBLIC

/*
 * ERROR HANDLING
 * -----------------------------------------------------------------------------
 */

typedef enum System_StatusCode {
  SYSTEM_STATUS_CODE_SUCCESS = 0,
  SYSTEM_STATUS_CODE_NO_WRITE_ACCESS = 10,
  SYSTEM_STATUS_CODE_INVALID_DATA_SIZE = 11,
  SYSTEM_STATUS_CODE_ITERATION_ALREADY_COMPLETED = 12,
  SYSTEM_STATUS_CODE_INVALID_COMPONENT = 13,
  SYSTEM_STATUS_CODE_ALREADY_EXISTS = 14,
  SYSTEM_STATUS_CODE_OUT_OF_MEMORY = 15,
  SYSTEM_STATUS_CODE_MISSING_COMPONENT = 16,
  SYSTEM_STATUS_CODE_NO_TICK_RATE_CONFIG = 17,
  SYSTEM_STATUS_CODE_INVALID_ARGUMENT = 18,
  SYSTEM_STATUS_CODE_OUT_OF_RANGE = 19,
  SYSTEM_STATUS_CODE_NOT_IMPLEMENTED = 98,
  SYSTEM_STATUS_CODE_ERROR = 99,  // Generic error code
  SYSTEM_STATUS_CODE_ABORT =
      100,  // System has suffered an unrecoverable error and requires the simulation to abort
} System_StatusCode;

/*
 * Convert a System Status code to a human-readable string
 */
DLL_PUBLIC const char* System_StatusCodeToString(System_StatusCode status_code);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // LATTICE_SYSTEM_ERROR_H
