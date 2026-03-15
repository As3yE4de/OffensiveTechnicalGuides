#include "utils.h"

void
fPrintErrorMessage(DWORD dwErrorCode) {
	LPWSTR strBuffer = NULL;
	DWORD dwSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, dwErrorCode, 0, (LPWSTR)&strBuffer, 0, NULL);

	if (dwSize && strBuffer)
	{
		wprintf(L"%lu (0x%08lX): %s\n", dwErrorCode, dwErrorCode, strBuffer);
		LocalFree(strBuffer);
	}
	else
	{
		wprintf(L"%lu (0x%08lX): Unknown error\n", dwErrorCode, dwErrorCode);
	}
}

void
fEnableConsoleColor()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		wprintf(L"[ERROR] Cannot process ANSI color! ");
		fPrintErrorMessage(GetLastError());
		return;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		wprintf(L"[ERROR] Cannot get console mode! ");
		fPrintErrorMessage(GetLastError());
		return;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode)) {
		wprintf(L"[ERROR] Cannot enable ANSI color: SetConsoleMode failed.");
		fPrintErrorMessage(GetLastError());
		return;
	}
}

BOOL
fIsNumericString(LPCWSTR string) {
	if (string == NULL || *string == L'\0') return FALSE;
	for (DWORD i = 0; string[i] != L'\0'; i++) {
		if (!iswdigit(string[i])) return FALSE;
	}

	return TRUE;
}

BOOL
fIsSingleAlphabet(LPCWSTR string) {
	if (string == NULL || *string == L'\0') return FALSE;
	if (!iswalpha(string[0])) return FALSE;
	if (string[1] != L'\0') return FALSE;

	return TRUE;
}

BOOL
fEnableSeBackupPrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE;

	if (!LookupPrivilegeValueW(NULL, SE_BACKUP_NAME, &tp.Privileges[0].Luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL) || GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

BOOL
fEnableSeRestorePrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE;

	if (!LookupPrivilegeValueW(NULL, SE_RESTORE_NAME, &tp.Privileges[0].Luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL) || GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

void
fFormatBytes(LONGLONG bytes, WCHAR* outBuffer, DWORD bufferSize) {
	double size = (double)bytes;
	int unitIndex = 0;
	const WCHAR* units[] = { L"Bytes", L"KB", L"MB", L"GB", L"TB" };

	while (size >= 1024 && unitIndex < 4) {
		size /= 1024;
		unitIndex++;
	}

	swprintf_s(outBuffer, bufferSize, L"%.2f %s", size, units[unitIndex]);
}