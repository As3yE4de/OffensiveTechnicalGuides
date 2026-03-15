#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <stdio.h>

/// <summary>
/// Print error message to the console.
/// </summary>
/// <param name="errorCode">[in] An error code can be taken from GetLastError().</param>
void fPrintErrorMessage(DWORD errorCode);

/// <summary>
/// Attempt to make your console more colorful.
/// </summary>
void fEnableConsoleColor();

/// <summary>
/// Check if the input string is numeric (all characters are numbers).
/// </summary>
/// <param name="string">[in] A string needed to check.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsNumericString(LPCWSTR string);

/// <summary>
/// Check if the input string is a single alphabetical character.
/// </summary>
/// <param name="string">[in] A string needed to check.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsSingleAlphabet(LPCWSTR string);

/// <summary>
/// Attempt to enable SeBackupPrivilege.
/// </summary>
/// <returns>TRUE/FALSE</returns>
BOOL fEnableSeBackupPrivilege();

/// <summary>
/// Attempt to enable SeRestorePrivilege.
/// </summary>
/// <returns>TRUE/FALSE</returns>
BOOL fEnableSeRestorePrivilege();

/// <summary>
/// Format the given bytes into a friendly unit.
/// </summary>
/// <param name="bytes">[in] Input byte value</param>
/// <param name="outBuffer">[out] Output buffer which receives the friendly unit.</param>
/// <param name="bufferSize">[in] Output buffer size.</param>
void fFormatBytes(LONGLONG bytes, WCHAR* outBuffer, DWORD bufferSize);

#endif