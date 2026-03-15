#include "diskenum_campaign.h"
#include "utils.h"

static const GUID PARTITION_BASIC_DATA_GUID = { 0xebd0a0a2 , 0xb9e5, 0x4433, { 0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7} };
static const GUID PARTITION_ENTRY_UNUSED_GUID = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
static const GUID PARTITION_SYSTEM_GUID = { 0xc12a7328, 0xf81f, 0x11d2, { 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b} };
static const GUID PARTITION_MSFT_RESERVED_GUID = { 0xe3c9e316, 0x0b5c, 0x4db8, { 0x81, 0x7d, 0xf9, 0x2d, 0xf0, 0x02, 0x15, 0xae} };
static const GUID PARTITION_LDM_METADATA_GUID = { 0x5808C8AA, 0x7E8F, 0x42E0, { 0x85, 0xD2, 0xE1, 0xE9, 0x04, 0x34, 0xCF, 0xB3 } };
static const GUID PARTITION_LDM_DATA_GUID = { 0xAF9B6001, 0x1400, 0x11D2, { 0x93, 0x82, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x92} };
static const GUID PARTITION_LDM_DYNAMIC_DATA_GUID = { 0xAF9B60A0, 0x1431, 0x4F62, { 0xBC, 0x68, 0x33, 0x11, 0x71, 0x4A, 0x69, 0xAD } };
static const GUID PARTITION_MSFT_RECOVERY_GUID = { 0xde94bba4, 0x06d1, 0x4d40, { 0xa1, 0x6a, 0xbf, 0xd5, 0x01, 0x79, 0xd6, 0xac } };

void
fSortPhysicalDisks(DISK_LIST list) {
    if (list.Disks == NULL || list.DiskCount < 2) return;

    for (DWORD i = 0; i < list.DiskCount - 1; i++) {
        for (DWORD j = 0; j < list.DiskCount - i - 1; j++) {
            DWORD diskNum1 = _wtoi(&list.Disks[j].SymbolicLink[17]);
            DWORD diskNum2 = _wtoi(&list.Disks[j + 1].SymbolicLink[17]);

            if (diskNum1 > diskNum2) {
                DISK_INFO temp = list.Disks[j];
                list.Disks[j] = list.Disks[j + 1];
                list.Disks[j + 1] = temp;
            }
        }
    }
}

LONGLONG
fGetTotalUsedExtents(DWORD dwTargetDiskNumber) {
    HANDLE hVolumeFind = INVALID_HANDLE_VALUE;
    HANDLE hVolDev = INVALID_HANDLE_VALUE;
    PVOLUME_DISK_EXTENTS pExtents = NULL;

    WCHAR volName[MAX_PATH];
    LONGLONG totalUsed = -1;
    DWORD bufferSize = 512;
    DWORD errorCode;

    pExtents = (PVOLUME_DISK_EXTENTS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (pExtents == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    hVolumeFind = FindFirstVolumeW(volName, ARRAYSIZE(volName));
    if (hVolumeFind == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute FindFirstVolumeW at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    totalUsed = 0;
    do {
        size_t len = wcslen(volName);
        if (len > 0 && volName[len - 1] == L'\\') volName[len - 1] = L'\0';

        hVolDev = CreateFileW(volName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        volName[len - 1] = L'\\';

        if (hVolDev != INVALID_HANDLE_VALUE) {
            DWORD bytesRet = 0;

            while (TRUE) {
                if (DeviceIoControl(hVolDev, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, pExtents, bufferSize, &bytesRet, NULL)) {
                    for (DWORD i = 0; i < pExtents->NumberOfDiskExtents; i++) {
                        if (pExtents->Extents[i].DiskNumber == dwTargetDiskNumber) {
                            totalUsed += pExtents->Extents[i].ExtentLength.QuadPart;
                        }
                    }
                    break;
                }

                if (GetLastError() == ERROR_MORE_DATA || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    bufferSize += 512;
                    PVOLUME_DISK_EXTENTS pTemp = (PVOLUME_DISK_EXTENTS)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pExtents, bufferSize);
                    if (!pTemp) {
                        errorCode = GetLastError();
                        wprintf(L"\033[1;31m");
                        wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                        fPrintErrorMessage(errorCode);
                        wprintf(L"\033[0m\n");
                        goto CleanUp;
                    }
                    pExtents = pTemp;
                }
                else {
                    break;
                }
            }
            CloseHandle(hVolDev);
            hVolDev = INVALID_HANDLE_VALUE;
        }

    } while (FindNextVolumeW(hVolumeFind, volName, ARRAYSIZE(volName)));

CleanUp:
    if (pExtents) HeapFree(GetProcessHeap(), 0, pExtents);
    if (hVolDev != INVALID_HANDLE_VALUE) CloseHandle(hVolDev);
    if (hVolumeFind != INVALID_HANDLE_VALUE) FindVolumeClose(hVolumeFind);

    return totalUsed;
}

BOOL
fIsDynamicDisk(HANDLE hDevice) {
    DWORD errorCode;
    BOOL isDynamic = FALSE;
    DWORD capacity = 4;
    DWORD bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * capacity);
    DWORD bytesReturned = 0;
    PDRIVE_LAYOUT_INFORMATION_EX pLayout = NULL;

    pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (pLayout == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    while (TRUE)
    {
        if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pLayout, bufferSize, &bytesReturned, NULL)
            && bytesReturned >= sizeof(DRIVE_LAYOUT_INFORMATION_EX)) break;

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            DWORD newCapacity = capacity + 4;
            bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * newCapacity);
            PDRIVE_LAYOUT_INFORMATION_EX pTemp = (PDRIVE_LAYOUT_INFORMATION_EX)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pLayout, bufferSize);
            if (pTemp == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapRealloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                goto CleanUp;
            }
            pLayout = pTemp;
            capacity = newCapacity;
        }
        else {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Unknown error at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
    }

    if (pLayout) {
        if (pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
            for (DWORD i = 0; i < pLayout->PartitionCount; i++) {
                if (pLayout->PartitionEntry[i].Mbr.PartitionType == 0x42) {
                    isDynamic = TRUE;
                    goto CleanUp;
                }
            }
        }
        else if (pLayout->PartitionStyle == PARTITION_STYLE_GPT) {
            for (DWORD i = 0; i < pLayout->PartitionCount; i++) {
                if (IsEqualGUID(&pLayout->PartitionEntry[i].Gpt.PartitionType, &PARTITION_LDM_METADATA_GUID) || IsEqualGUID(&pLayout->PartitionEntry[i].Gpt.PartitionType, &PARTITION_LDM_DATA_GUID)) {
                    isDynamic = TRUE;
                    goto CleanUp;
                }
            }
        }
    }
CleanUp:
    if (pLayout) HeapFree(GetProcessHeap(), 0, pLayout);
    return isDynamic;
}

BOOL
fIsGptDisk(HANDLE hDevice) {
    BOOL isGpt = FALSE;
    DWORD errorCode;
    DWORD capacity = 4;
    DWORD bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * capacity);
    DWORD bytesReturned = 0;
    PDRIVE_LAYOUT_INFORMATION_EX pLayout = NULL;

    pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (!pLayout) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    while (TRUE)
    {
        if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pLayout, bufferSize, &bytesReturned, NULL)
            && bytesReturned >= sizeof(DRIVE_LAYOUT_INFORMATION_EX)) break;

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            DWORD newCapacity = capacity + 4;
            bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * newCapacity);
            PDRIVE_LAYOUT_INFORMATION_EX pTemp = (PDRIVE_LAYOUT_INFORMATION_EX)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pLayout, bufferSize);
            if (pTemp == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                goto CleanUp;
            }
            pLayout = pTemp;
            capacity = newCapacity;
        }
        else {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Unknown error at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
    }

    if (pLayout && pLayout->PartitionStyle == PARTITION_STYLE_GPT) {
        isGpt = TRUE;
        goto CleanUp;
    }

CleanUp:
    if (pLayout) HeapFree(GetProcessHeap(), 0, pLayout);
    return isGpt;
}

void
fGetDiskStatusByHandle(HANDLE hDisk, WCHAR* outStatus, DWORD bufferSize) {
    GET_DISK_ATTRIBUTES diskAttr = { 0 };
    diskAttr.Version = sizeof(GET_DISK_ATTRIBUTES);
    DWORD bytesReturned = 0;
    DWORD errorCode;

    if (DeviceIoControl(hDisk, IOCTL_DISK_GET_DISK_ATTRIBUTES, NULL, 0, &diskAttr, sizeof(diskAttr), &bytesReturned, NULL
    ) && bytesReturned >= sizeof(GET_DISK_ATTRIBUTES)) {
        if (diskAttr.Attributes & DISK_ATTRIBUTE_OFFLINE) {
            swprintf_s(outStatus, bufferSize, L"Offline");
        }
        else if (diskAttr.Attributes & DISK_ATTRIBUTE_READ_ONLY) {
            swprintf_s(outStatus, bufferSize, L"Read Only");
        }
        else {
            swprintf_s(outStatus, bufferSize, L"Online");
        }
    }
    else {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute DeviceIoControl at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        swprintf_s(outStatus, bufferSize, L"Unknown");
    }
}

DWORD
fGetBytesPerSectorByHandle(HANDLE hDevice)
{
    DISK_GEOMETRY_EX diskGeometry = { 0 };
    DWORD bytesReturned = 0;
    DWORD errorCode;

    if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &diskGeometry, sizeof(diskGeometry), &bytesReturned, NULL)
        && bytesReturned >= sizeof(diskGeometry))
    {
        return diskGeometry.Geometry.BytesPerSector;
    }
    else {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute DeviceIoControl at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
    }
    return 0;
}

LONGLONG
fGetDiskSizeByHandle(HANDLE hDevice)
{
    DISK_GEOMETRY_EX diskGeometry = { 0 };
    DWORD bytesReturned = 0;
    DWORD errorCode;

    if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &diskGeometry, sizeof(diskGeometry), &bytesReturned, NULL)
        && bytesReturned == sizeof(diskGeometry))
    {
        return diskGeometry.DiskSize.QuadPart;
    }
    else {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute DeviceIoControl at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
    }
    return -1;
}

void
fFreeDiskList(DISK_LIST* list) {
    if (list != NULL && list->Disks != NULL) {
        HeapFree(GetProcessHeap(), 0, list->Disks);
        list->Disks = NULL;
        list->DiskCount = 0;
        list->Success = FALSE;
        list->LastError = 0;
    }
}

LONGLONG
fGetUnallocatedSize(HANDLE hDevice, LPCWSTR symbolicLink, LONGLONG totalDiskSize) {
    DWORD targetDiskNumber;
    swscanf_s(symbolicLink, L"\\\\.\\PhysicalDrive%lu", &targetDiskNumber);

    LONGLONG unallocatedSize = -1;
    DWORD capacity = 4;
    DWORD bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * capacity);
    DWORD bytesReturned = 0;
    PDRIVE_LAYOUT_INFORMATION_EX pLayout = NULL;
    PPARTITION_INFORMATION_EX pi = NULL;
    DWORD errorCode;

    pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (pLayout == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    while (pLayout != NULL)
    {
        if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pLayout, bufferSize, &bytesReturned, NULL)) {
            unallocatedSize = 0;
            break;
        }

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            DWORD newCapacity = capacity + 4;
            bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * newCapacity);
            PDRIVE_LAYOUT_INFORMATION_EX temp = (PDRIVE_LAYOUT_INFORMATION_EX)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pLayout, bufferSize);
            if (temp == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                goto CleanUp;
            }
            pLayout = temp;
            capacity = newCapacity;
        }
        else {
            errorCode = GetLastError();
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Unknown error at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
    }

    if (pLayout) {
        DWORD actualCount = 0;
        pi = (PPARTITION_INFORMATION_EX)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PARTITION_INFORMATION_EX) * pLayout->PartitionCount);
        if (pi == NULL) {
            errorCode = GetLastError();
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }

        if (fIsDynamicDisk(hDevice)) {
            if (pLayout->PartitionStyle == PARTITION_STYLE_GPT || pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
                LONGLONG usedExtents = fGetTotalUsedExtents(targetDiskNumber);
                unallocatedSize = totalDiskSize - usedExtents;
                goto CleanUp;
            }
        }
        else {
            if (pLayout->PartitionStyle == PARTITION_STYLE_GPT || pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
                for (DWORD i = 0; i < pLayout->PartitionCount; i++) {
                    if (pLayout->PartitionEntry[i].PartitionLength.QuadPart > 0) {
                        pi[actualCount++] = pLayout->PartitionEntry[i];
                    }
                }

                if (actualCount == 0) {
                    HeapFree(GetProcessHeap(), 0, pi);
                    unallocatedSize = totalDiskSize;
                    goto CleanUp;
                }

                for (DWORD i = 0; i < actualCount - 1; i++) {
                    for (DWORD j = i + 1; j < actualCount; j++) {
                        if (pi[j].StartingOffset.QuadPart < pi[i].StartingOffset.QuadPart) {
                            PARTITION_INFORMATION_EX tmp = pi[i];
                            pi[i] = pi[j];
                            pi[j] = tmp;
                        }
                    }
                }

                for (DWORD i = 0; i < actualCount - 1; i++) {
                    LONGLONG endOfThis = pi[i].StartingOffset.QuadPart + pi[i].PartitionLength.QuadPart;
                    LONGLONG startOfNext = pi[i + 1].StartingOffset.QuadPart;
                    if (startOfNext > endOfThis) {
                        unallocatedSize += (startOfNext - endOfThis);
                    }
                }

                LONGLONG endOfLast = pi[actualCount - 1].StartingOffset.QuadPart + pi[actualCount - 1].PartitionLength.QuadPart;

                if (pLayout->PartitionStyle == PARTITION_STYLE_GPT) {
                    LONGLONG usableEnd = pLayout->Gpt.StartingUsableOffset.QuadPart + pLayout->Gpt.UsableLength.QuadPart;
                    if (usableEnd > endOfLast) {
                        unallocatedSize += usableEnd - endOfLast;
                    }
                }
                else if (pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
                    if (totalDiskSize > endOfLast) {
                        LONGLONG tailGap = totalDiskSize - endOfLast;
                        if (tailGap >= (1024 * 1024)) {
                            unallocatedSize += tailGap;
                        }
                    }
                }
            }
        }
    }

CleanUp:
    if (pLayout != NULL) HeapFree(GetProcessHeap(), 0, pLayout);
    if (pi != NULL) HeapFree(GetProcessHeap(), 0, pi);

    return unallocatedSize;
}

DISK_LIST
fGetPhysicalDisks() {
    DISK_LIST list = { 0 };
    list.DiskCount = 0;
    list.Disks = NULL;
    list.LastError = 0;
    list.Success = FALSE;

    DWORD capacity = 4;
    DWORD memberIndex = 0;
    DWORD errorCode;
    DWORD failedDevice = 0;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA spDeviceInterfaceData;
    spDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute SetupDiGetClassDevs at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    list.Disks = (DISK_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, capacity * sizeof(DISK_INFO));
    if (list.Disks == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = GetLastError();
        goto CleanUp;
    }

    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK, index, &spDeviceInterfaceData); index++) {
        if (memberIndex >= capacity) {
            DWORD newCapacity = capacity + 4;
            DISK_INFO* temp = (DISK_INFO*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, list.Disks, newCapacity * sizeof(DISK_INFO));
            if (temp == NULL) {
                break;
            }
            list.Disks = temp;
            capacity = newCapacity;
        }

        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &spDeviceInterfaceData, NULL, 0, &requiredSize, NULL);

        PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredSize);
        if (pDetail) {
            pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
            if (SetupDiGetDeviceInterfaceDetailW(hDevInfo, &spDeviceInterfaceData, pDetail, requiredSize, NULL, NULL)) {

                wcscpy_s(list.Disks[memberIndex].DevicePath, MAX_PATH, pDetail->DevicePath);

                HANDLE hDevice = CreateFileW(pDetail->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                if (hDevice != INVALID_HANDLE_VALUE) {
                    STORAGE_DEVICE_NUMBER sdn;
                    DWORD bytesReturned;

                    if (DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &bytesReturned, NULL)) {
                        swprintf_s(list.Disks[memberIndex].SymbolicLink, MAX_PATH, L"\\\\.\\PhysicalDrive%d", sdn.DeviceNumber);

                        list.Disks[memberIndex].TotalSizeInByte = fGetDiskSizeByHandle(hDevice);
                        list.Disks[memberIndex].UnallocatedSizeInByte = fGetUnallocatedSize(hDevice, list.Disks[memberIndex].SymbolicLink, list.Disks[memberIndex].TotalSizeInByte);
                        list.Disks[memberIndex].BytesPerSector = fGetBytesPerSectorByHandle(hDevice);

                        WCHAR tempStatus[15];
                        fGetDiskStatusByHandle(hDevice, tempStatus, _countof(tempStatus));
                        wcscpy_s(list.Disks[memberIndex].Status, _countof(list.Disks[memberIndex].Status), tempStatus);

                        list.Disks[memberIndex].IsGpt = fIsGptDisk(hDevice);
                        list.Disks[memberIndex].IsDynamic = fIsDynamicDisk(hDevice);

                        memberIndex++;
                        failedDevice = 0;
                    }
                    CloseHandle(hDevice);
                }
                else {
                    failedDevice++;
                    if (failedDevice > 10) {
                        wprintf(L"\033[1;31m[-] Too many consecutive failures. Stopping enumeration.\033[0m\n");
                        HeapFree(GetProcessHeap(), 0, pDetail);
                        break;
                    }
                }
            }
            HeapFree(GetProcessHeap(), 0, pDetail);
        }
    }

    list.DiskCount = memberIndex;
    list.Success = TRUE;
    list.LastError = ERROR_SUCCESS;

CleanUp:
    if (!list.Success && list.Disks != NULL) {
        HeapFree(GetProcessHeap(), 0, list.Disks);
        list.Disks = NULL;
    }

    if (hDevInfo != INVALID_HANDLE_VALUE) SetupDiDestroyDeviceInfoList(hDevInfo);
    return list;
}

void
fFreePartitionList(PARTITION_LIST* list) {
    if (list != NULL && list->Partitions != NULL) {
        HeapFree(GetProcessHeap(), 0, list->Partitions);
        list->Partitions = NULL;
        list->PartitionCount = 0;
        list->Success = FALSE;
        list->LastError = 0;
    }
}

BOOL
fGetFriendlyPartitionName(PARTITION_INFORMATION_EX* p, WCHAR* outBuffer, DWORD outBufferSize) {
    if (!p || !outBuffer || outBufferSize == 0) return FALSE;

    BOOL isHidden = FALSE;
    const WCHAR* baseName = L"Unknown";

    if (p->PartitionStyle == PARTITION_STYLE_MBR) {
        BYTE t = p->Mbr.PartitionType;
        if (t == 0x17 || t == 0x1B || t == 0x1C || t == 0x27 || t == 0x12) isHidden = TRUE;

        switch (t) {
        case 0x00: baseName = L"Unused Entry"; break;
        case 0x01: baseName = L"FAT12"; break;
        case 0x04: baseName = L"FAT16 (<= 32 MB)"; break;
        case 0x06: baseName = L"FAT16 (>= 32MB)"; break;
        case 0x0B: baseName = L"FAT32 (CHS)"; break;
        case 0x0C: baseName = L"Primary FAT32 (LBA)"; break;
        case 0x0E: baseName = L"Primary FAT16 (LBA)"; break;
        case 0x05: baseName = L"Extended"; break;
        case 0x07: baseName = L"NTFS"; break;
        case 0x42: baseName = L"LDM"; break;
        case 0x80: baseName = L"Primary NTFT"; break;
        case 0xC0: baseName = L"Valid NTFT"; break;
        case 0x27: baseName = L"Recovery"; break;
        case 0x12: baseName = L"OEM/Service"; break;
        default:   break;
        }
    }
    else if (p->PartitionStyle == PARTITION_STYLE_GPT) {
        if (p->Gpt.Attributes & GPT_BASIC_DATA_ATTRIBUTE_HIDDEN) {
            isHidden = TRUE;
        }

        if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_LDM_METADATA_GUID)) {
            baseName = L"Dynamic Reserved";
        }
        else if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_LDM_DYNAMIC_DATA_GUID)) {
            baseName = L"Dynamic Data";
        }
        else if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_BASIC_DATA_GUID)) baseName = L"Basic Data";
        else if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_SYSTEM_GUID)) baseName = L"System EFI";
        else if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_MSFT_RESERVED_GUID)) baseName = L"Reserved MSR";
        else if (IsEqualGUID(&p->Gpt.PartitionType, &PARTITION_MSFT_RECOVERY_GUID)) baseName = L"Recovery";
    }

    if (isHidden) {
        swprintf_s(outBuffer, outBufferSize, L"%s (Hidden)", baseName);
    }
    else {
        swprintf_s(outBuffer, outBufferSize, L"%s", baseName);
    }

    return TRUE;
}

PARTITION_LIST
fGetPartitionsOfDisk(DWORD diskIndex) {
    PARTITION_LIST list = { 0 };
    list.PartitionCount = 0;
    list.LastError = 0;
    list.Partitions = NULL;
    list.Success = FALSE;

    DWORD errorCode;
    WCHAR path[20];
    swprintf_s(path, _countof(path), L"\\\\.\\PhysicalDrive%d", diskIndex);

    HANDLE hDisk = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute CreateFileW at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        return list;
    }

    DWORD capacity = 4;
    DWORD bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * capacity);
    DWORD bytesReturned = 0;
    PDRIVE_LAYOUT_INFORMATION_EX pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (!pLayout) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    while (TRUE) {
        if (DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pLayout, bufferSize, &bytesReturned, NULL)
            && bytesReturned >= sizeof(DRIVE_LAYOUT_INFORMATION_EX)) break;

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            capacity += 4;
            bufferSize = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry) + (sizeof(PARTITION_INFORMATION_EX) * capacity);
            PDRIVE_LAYOUT_INFORMATION_EX pTemp = (PDRIVE_LAYOUT_INFORMATION_EX)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pLayout, bufferSize);
            if (pTemp == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                list.LastError = errorCode;
                goto CleanUp;
            }
            pLayout = pTemp;
        }
        else {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Unknown error at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            list.LastError = errorCode;
            goto CleanUp;
        }
    }

    if (pLayout) {
        list.Partitions = (PARTITION_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PARTITION_INFO) * pLayout->PartitionCount);
        if (list.Partitions == NULL) {
            list.LastError = GetLastError();
            goto CleanUp;
        }

        if (list.Partitions) {
            DWORD validCount = 0;
            for (DWORD i = 0; i < pLayout->PartitionCount; i++) {
                PARTITION_INFORMATION_EX* p = &pLayout->PartitionEntry[i];
                if (p->PartitionLength.QuadPart == 0) continue;
                if (p->PartitionLength.QuadPart > 0) {
                    list.Partitions[validCount].NumberId = p->PartitionNumber;
                    fGetFriendlyPartitionName(p, list.Partitions[validCount].Type, _countof(list.Partitions[validCount].Type));
                    list.Partitions[validCount].StartOffsetInByte = p->StartingOffset.QuadPart;
                    list.Partitions[validCount].EndOffsetInByte = p->StartingOffset.QuadPart + p->PartitionLength.QuadPart - 1;
                    list.Partitions[validCount].SizeInByte = p->PartitionLength.QuadPart;

                    validCount++;
                }
            }
            list.PartitionCount = validCount;
        }
    }

    list.LastError = ERROR_SUCCESS;
    list.Success = TRUE;

CleanUp:
    if (!list.Success) {
        if (list.Partitions) {
            HeapFree(GetProcessHeap(), 0, list.Partitions);
            list.Partitions = NULL;
        }
    }
    if (hDisk != INVALID_HANDLE_VALUE) CloseHandle(hDisk);
    if (pLayout) HeapFree(GetProcessHeap(), 0, pLayout);
    return list;
}

BOOL
fIsVolumeOnDisk(LPCWSTR volumeName, DWORD diskNumber) {
    BOOL isOnDisk = FALSE;
    WCHAR tempPath[MAX_PATH];
    wcscpy_s(tempPath, MAX_PATH, volumeName);
    size_t len = wcslen(tempPath);
    DWORD capacity = 8;
    HANDLE hVol = INVALID_HANDLE_VALUE;
    DWORD errorCode;
    DWORD bufferSize = FIELD_OFFSET(VOLUME_DISK_EXTENTS, Extents) + (sizeof(DISK_EXTENT) * capacity);
    PVOLUME_DISK_EXTENTS extents = NULL;
    DWORD bytesReturned;

    if (len > 0 && tempPath[len - 1] == L'\\') {
        tempPath[len - 1] = L'\0';
    }

    hVol = CreateFileW(tempPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVol == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute CreateFileW at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    extents = (PVOLUME_DISK_EXTENTS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (extents == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }

    while (TRUE) {
        if (DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, extents, bufferSize, &bytesReturned, NULL)) {
            for (DWORD i = 0; i < extents->NumberOfDiskExtents; i++) {
                if (extents->Extents[i].DiskNumber == diskNumber) {
                    isOnDisk = TRUE;
                    break;
                }
            }
            break;
        }

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            DWORD newCapacity = capacity + 4;
            bufferSize = FIELD_OFFSET(VOLUME_DISK_EXTENTS, Extents) + (sizeof(DISK_EXTENT) * newCapacity);
            VOLUME_DISK_EXTENTS* temp = (VOLUME_DISK_EXTENTS*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, extents, bufferSize);
            if (!temp) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                goto CleanUp;
            }
            extents = temp;
            capacity = newCapacity;
        }
        else {
            break;
        }
    }

CleanUp:
    if (extents) HeapFree(GetProcessHeap(), 0, extents);
    if (hVol != INVALID_HANDLE_VALUE) CloseHandle(hVol);
    return isOnDisk;
}

void
fFreeVolumeList(VOLUME_LIST* list) {
    if (list != NULL && list->Volumes != NULL) {
        HeapFree(GetProcessHeap(), 0, list->Volumes);
        list->Volumes = NULL;
        list->VolumeCount = 0;
        list->Success = FALSE;
        list->LastError = 0;
    }
}

void
fFreeVolumeExtentsList(VOLUME_EXTENTS_LIST* list) {
    if (list && list->Extents) {
        HeapFree(GetProcessHeap(), 0, list->Extents);
        list->Extents = NULL;
        list->ExtentCount = 0;
        list->LastError = 0;
    }
}

VOLUME_LIST
fGetVolumesOfDisk(DWORD diskIndex) {
    VOLUME_LIST list = { 0 };
    list.VolumeCount = 0;
    list.Volumes = NULL;
    list.Success = FALSE;
    list.LastError = 0;
    WCHAR volumeName[MAX_PATH];
    HANDLE hVol = INVALID_HANDLE_VALUE;
    DWORD capacity = 4;
    DWORD bufferOfVolumes = sizeof(VOLUME_INFO) * capacity;
    DWORD memberIndex = 0;
    DWORD errorCode;

    list.Volumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferOfVolumes);
    if (list.Volumes == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    hVol = FindFirstVolumeW(volumeName, ARRAYSIZE(volumeName));
    if (hVol == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute FindFirstVolumeW at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }
    else {
        do {
            if (fIsVolumeOnDisk(volumeName, diskIndex)) {
                if (memberIndex >= capacity) {
                    DWORD newCapacity = capacity + 4;
                    bufferOfVolumes = sizeof(VOLUME_INFO) * newCapacity;
                    VOLUME_INFO* temp = (VOLUME_INFO*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, list.Volumes, bufferOfVolumes);
                    if (temp == NULL) {
                        errorCode = GetLastError();
                        wprintf(L"\033[1;31m");
                        wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                        fPrintErrorMessage(errorCode);
                        wprintf(L"\033[0m\n");
                        list.LastError = errorCode;
                        goto CleanUp;
                    }
                    list.Volumes = temp;
                    capacity = newCapacity;
                }

                list.Volumes[memberIndex].DiskNumber = diskIndex;
                wcscpy_s(list.Volumes[memberIndex].VolumeGuid, _countof(volumeName), volumeName);

                DWORD returnedLength = 0;
                WCHAR volumePathNames[MAX_PATH] = L"";
                if (GetVolumePathNamesForVolumeNameW(volumeName, volumePathNames, ARRAYSIZE(volumePathNames), &returnedLength) && returnedLength > 0) {
                    wcscpy_s(list.Volumes[memberIndex].DriveLetter, _countof(volumePathNames), volumePathNames);
                }
                else {
                    wcscpy_s(list.Volumes[memberIndex].DriveLetter, _countof(volumePathNames), L"(None)");
                }

                WCHAR label[MAX_PATH] = { 0 };
                WCHAR fileSystem[MAX_PATH] = { 0 };
                if (GetVolumeInformationW(volumeName, label, ARRAYSIZE(label), NULL, NULL, NULL, fileSystem, ARRAYSIZE(fileSystem))) {
                    wcscpy_s(list.Volumes[memberIndex].Label, _countof(list.Volumes[memberIndex].Label), label);
                    wcscpy_s(list.Volumes[memberIndex].FileSystem, _countof(list.Volumes[memberIndex].FileSystem), fileSystem);
                }
                else {
                    wcscpy_s(list.Volumes[memberIndex].Label, _countof(list.Volumes[memberIndex].Label), L"(None)");
                    wcscpy_s(list.Volumes[memberIndex].FileSystem, _countof(list.Volumes[memberIndex].FileSystem), L"Unknown");
                }

                ULARGE_INTEGER totalSize;
                ULARGE_INTEGER freeSize;
                if (GetDiskFreeSpaceExW(volumeName, NULL, &totalSize, &freeSize)) {
                    list.Volumes[memberIndex].SizeInByte = totalSize.QuadPart;
                    list.Volumes[memberIndex].FreeSizeInByte = freeSize.QuadPart;
                }
                else {
                    list.Volumes[memberIndex].SizeInByte = 0;
                    list.Volumes[memberIndex].FreeSizeInByte = 0;
                }

                DWORD lpSectorsPerCluster;
                DWORD lpBytesPerSectors;
                DWORD lpNumberOfFreeClusters;
                DWORD lpTotalNumberOfClusters;
                if (GetDiskFreeSpaceW(volumeName, &lpSectorsPerCluster, &lpBytesPerSectors, &lpNumberOfFreeClusters, &lpTotalNumberOfClusters)) {
                    list.Volumes[memberIndex].BytesPerSector = lpBytesPerSectors;
                    list.Volumes[memberIndex].SectorsPerCluster = lpSectorsPerCluster;
                    list.Volumes[memberIndex].BytesPerCluster = lpBytesPerSectors * lpSectorsPerCluster;
                    list.Volumes[memberIndex].TotalClusters = lpTotalNumberOfClusters;
                    list.Volumes[memberIndex].FreeClusters = lpNumberOfFreeClusters;
                }
                else {
                    list.Volumes[memberIndex].BytesPerSector = 0;
                    list.Volumes[memberIndex].SectorsPerCluster = 0;
                    list.Volumes[memberIndex].BytesPerCluster = 0;
                    list.Volumes[memberIndex].TotalClusters = 0;
                    list.Volumes[memberIndex].FreeClusters = 0;
                }

                if (list.Volumes[memberIndex].DriveLetter != NULL && iswalpha(list.Volumes[memberIndex].DriveLetter[0])) {
                    WCHAR volumePath[10];
                    swprintf_s(volumePath, _countof(volumePath), L"\\\\.\\%c:", list.Volumes[memberIndex].DriveLetter[0]);

                    VOLUME_EXTENTS_LIST extents = fGetVolumeExtents(volumePath);
                    if (extents.Success && extents.ExtentCount > 0) {
                        list.Volumes[memberIndex].StartOffsetInByte = extents.Extents[0].StartingOffset;
                    }
                    else {
                        list.Volumes[memberIndex].StartOffsetInByte = 0;
                        if (extents.LastError != 0) {
                            wprintf(L"\033[1;31m[-] Cannot get volume extents for %s. Error: %lu\033[0m\n", volumePath, extents.LastError);
                        }
                    }
                    fFreeVolumeExtentsList(&extents);
                }

                memberIndex++;
            }
        } while (FindNextVolumeW(hVol, volumeName, ARRAYSIZE(volumeName)));
        list.VolumeCount = memberIndex;
    }
    list.LastError = ERROR_SUCCESS;
    list.Success = TRUE;

CleanUp:
    if (!list.Success) {
        if (list.Volumes) {
            HeapFree(GetProcessHeap(), 0, list.Volumes);
            list.Volumes = NULL;
        }
    }
    if (hVol != INVALID_HANDLE_VALUE) FindVolumeClose(hVol);

    return list;
}

VOLUME_EXTENTS_LIST
fGetVolumeExtents(LPCWSTR volumePath) {
    VOLUME_EXTENTS_LIST list = { 0 };
    list.Success = FALSE;
    list.ExtentCount = 0;
    list.Extents = NULL;
    list.LastError = 0;

    DWORD bytesReturned = 0;
    DWORD capacity = 4;
    DWORD bufferSize = FIELD_OFFSET(VOLUME_DISK_EXTENTS, Extents) + sizeof(DISK_EXTENT) * capacity;
    PVOLUME_DISK_EXTENTS volumeExtents = NULL;
    DWORD errorCode;

    HANDLE hVol = INVALID_HANDLE_VALUE;

    volumeExtents = (PVOLUME_DISK_EXTENTS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (volumeExtents == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    hVol = CreateFileW(volumePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVol == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"%s\n", volumePath);
        wprintf(L"[ERROR] Failed to execute CreateFileW at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    while (TRUE) {
        if (DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, volumeExtents, bufferSize, &bytesReturned, NULL)
            && bytesReturned >= FIELD_OFFSET(VOLUME_DISK_EXTENTS, Extents)) break;

        errorCode = GetLastError();
        if (errorCode == ERROR_MORE_DATA || errorCode == ERROR_INSUFFICIENT_BUFFER) {
            DWORD newCapacity = capacity + 4;
            bufferSize = FIELD_OFFSET(VOLUME_DISK_EXTENTS, Extents) + sizeof(DISK_EXTENT) * newCapacity;
            PVOLUME_DISK_EXTENTS temp = (PVOLUME_DISK_EXTENTS)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, volumeExtents, bufferSize);
            if (temp == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapReAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                list.LastError = errorCode;
                goto CleanUp;
            }
            volumeExtents = temp;
            capacity = newCapacity;
        }
        else {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Unknown error at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            fPrintErrorMessage(errorCode);
            wprintf(L"\033[0m\n");
            list.LastError = errorCode;
            goto CleanUp;
        }
    }

    list.Extents = (VOLUME_EXTENT_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VOLUME_EXTENT_INFO) * volumeExtents->NumberOfDiskExtents);
    if (list.Extents == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    list.ExtentCount = volumeExtents->NumberOfDiskExtents;
    for (DWORD i = 0; i < volumeExtents->NumberOfDiskExtents; i++) {
        list.Extents[i].DiskNumber = volumeExtents->Extents[i].DiskNumber;
        list.Extents[i].StartingOffset = volumeExtents->Extents[i].StartingOffset.QuadPart;
        list.Extents[i].ExtentLength = volumeExtents->Extents[i].ExtentLength.QuadPart;
    }

    list.LastError = ERROR_SUCCESS;
    list.Success = TRUE;


CleanUp:
    if (!list.Success) {
        if (list.Extents) {
            HeapFree(GetProcessHeap(), 0, list.Extents);
            list.Extents = NULL;
        }
    }

    if (volumeExtents) HeapFree(GetProcessHeap(), 0, volumeExtents);
    if (hVol != INVALID_HANDLE_VALUE) CloseHandle(hVol);
    return list;
}

DISK_LIST
fGetDynamicDisks(DISK_LIST totalPhysicalDisks) {
    DISK_LIST list = { 0 };
    list.LastError = 0;
    list.Success = FALSE;
    list.DiskCount = 0;
    list.Disks = NULL;

    DWORD memberIndex = 0;
    DWORD errorCode;

    DWORD dynamicDiskCount = 0;
    if (totalPhysicalDisks.DiskCount == 0) {
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] The list of physical disks is empty.");
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }
    else {
        for (DWORD i = 0; i < totalPhysicalDisks.DiskCount; i++) {
            if (totalPhysicalDisks.Disks[i].IsDynamic == TRUE) dynamicDiskCount++;
        }
    }

    if (dynamicDiskCount == 0) {
        list.LastError = ERROR_SUCCESS;
        list.DiskCount = 0;
        list.Success = TRUE;
        wprintf(L"[INFO] No dynamic disk found.\n");
        return list;
    }

    list.Disks = (DISK_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DISK_INFO) * dynamicDiskCount);
    if (list.Disks == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        list.LastError = errorCode;
        goto CleanUp;
    }

    for (DWORD i = 0; i < totalPhysicalDisks.DiskCount; i++) {
        if (totalPhysicalDisks.Disks[i].IsDynamic == TRUE) {
            list.Disks[memberIndex] = totalPhysicalDisks.Disks[i];
            memberIndex++;
        }
    }

    list.DiskCount = dynamicDiskCount;
    list.LastError = ERROR_SUCCESS;
    list.Success = TRUE;

CleanUp:
    if (!list.Success) {
        if (list.Disks) {
            HeapFree(GetProcessHeap(), 0, list.Disks);
            list.Disks = NULL;
        }
    }

    return list;
}

VOLUME_TYPE
fGetDynamicVolumeType(VOLUME_EXTENTS_LIST* volumeExtentsList) {
    if (volumeExtentsList == NULL || volumeExtentsList->ExtentCount == 0) {
        return VOLUME_UNKNOWN;
    }

    const unsigned char ntfsStripedSignature[] = "\x00\x68\x01\x68\x02\x68\x03\x68\x04\x68\x05\x68\x06\x68\x07\x68\x08\x68\x09\x68\x0A\x68\x0B\x68\x0C\x68\x0D\x68\x0E\x68\x0F\x68";

    typedef struct _MIN_DISK_EXTENT {
        DWORD UniqueDiskNumber;
        LONGLONG MinStartOffset;
        LONGLONG TotalExtentLength;
    } MIN_DISK_EXTENT;

    DWORD errorCode;
    MIN_DISK_EXTENT* res = (MIN_DISK_EXTENT*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MIN_DISK_EXTENT) * volumeExtentsList->ExtentCount);
    if (res == NULL) {
        errorCode = GetLastError();
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        fPrintErrorMessage(errorCode);
        wprintf(L"\033[0m\n");
        return VOLUME_UNKNOWN;
    }

    DWORD uniqueDiskCount = 0;

    for (DWORD i = 0; i < volumeExtentsList->ExtentCount; i++) {
        DWORD currentDisk = volumeExtentsList->Extents[i].DiskNumber;
        LONGLONG currentOffset = volumeExtentsList->Extents[i].StartingOffset;
        LONGLONG currentTotalLength = volumeExtentsList->Extents[i].ExtentLength;

        BOOL found = FALSE;
        for (DWORD j = 0; j < uniqueDiskCount; j++) {
            if (res[j].UniqueDiskNumber == currentDisk) {
                if (currentOffset < res[j].MinStartOffset) {
                    res[j].MinStartOffset = currentOffset;
                }

                res[j].TotalExtentLength += currentTotalLength;
                found = TRUE;
                break;
            }
        }

        if (!found) {
            res[uniqueDiskCount].UniqueDiskNumber = currentDisk;
            res[uniqueDiskCount].MinStartOffset = currentOffset;
            res[uniqueDiskCount].TotalExtentLength = currentTotalLength;
            uniqueDiskCount++;
        }
    }


    if (uniqueDiskCount == 1) {
        return VOLUME_SIMPLE;
    }
    else if (uniqueDiskCount >= 2) {
        for (DWORD i = 0; i < uniqueDiskCount; i++) {
            if (res[0].TotalExtentLength != res[i].TotalExtentLength) {
                return VOLUME_SPANNED;
            }
            else {
                continue;
            }
        }

        BYTE buffer1[512] = { 0 };
        BYTE buffer2[512] = { 0 };
        WCHAR diskPath1[32] = L"\0";
        WCHAR diskPath2[32] = L"\0";

        swprintf_s(diskPath1, 32, L"\\\\.\\PhysicalDrive%lu", res[0].UniqueDiskNumber);
        swprintf_s(diskPath2, 32, L"\\\\.\\PhysicalDrive%lu", res[1].UniqueDiskNumber);

        HANDLE hDisk1 = CreateFileW(diskPath1, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        HANDLE hDisk2 = CreateFileW(diskPath2, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);

        if (hDisk1 != INVALID_HANDLE_VALUE && hDisk2 != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER pos1, pos2;
            pos1.QuadPart = res[0].MinStartOffset;
            pos2.QuadPart = res[1].MinStartOffset;

            if (SetFilePointerEx(hDisk1, pos1, NULL, FILE_BEGIN) && SetFilePointerEx(hDisk2, pos2, NULL, FILE_BEGIN)) {

                DWORD bytesRead1, bytesRead2;
                BOOL success1 = ReadFile(hDisk1, buffer1, sizeof(buffer1), &bytesRead1, NULL);
                BOOL success2 = ReadFile(hDisk2, buffer2, sizeof(buffer2), &bytesRead2, NULL);

                if (success1 && success2 && bytesRead1 == 512 && bytesRead2 == 512) {
                    if (memcmp(buffer1, buffer2, 32) == 0) {
                        CloseHandle(hDisk1);
                        CloseHandle(hDisk2);
                        return VOLUME_MIRRORED;
                    }

                    if (memcmp(buffer1, ntfsStripedSignature, 32) == 0 || memcmp(buffer2, ntfsStripedSignature, 32) == 0) {
                        CloseHandle(hDisk1);
                        CloseHandle(hDisk2);
                        return VOLUME_STRIPED;
                    }

                    CloseHandle(hDisk1);
                    CloseHandle(hDisk2);
                    return VOLUME_SPANNED;
                }
            }
            CloseHandle(hDisk1);
            CloseHandle(hDisk2);
        }
    }
    return VOLUME_UNKNOWN;
}

VOLUME_LIST
fGetSimpleVolumeList() {
    VOLUME_LIST simpleVolumeList = { 0 };
    simpleVolumeList.LastError = 0;
    simpleVolumeList.Success = FALSE;
    simpleVolumeList.VolumeCount = 0;
    simpleVolumeList.Volumes = NULL;

    VOLUME_INFO* pTotalVolumes = NULL;
    DISK_LIST totalPhysicalDiskList = fGetPhysicalDisks();
    DISK_LIST dynamicDiskList = fGetDynamicDisks(totalPhysicalDiskList);
    DWORD errorCode;

    if (!totalPhysicalDiskList.Success) {
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute fGetPhysicalDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }
    else {
        fSortPhysicalDisks(totalPhysicalDiskList);
        if (!dynamicDiskList.Success) {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Failed to execute fGetDynamicDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
        else {
            fSortPhysicalDisks(dynamicDiskList);

            DWORD totalVolumeCount = 0;

            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    totalVolumeCount += tempList.VolumeCount;
                    fFreeVolumeList(&tempList);
                }
            }

            if (totalVolumeCount == 0) goto CleanUp;

            DWORD bufferSize = sizeof(VOLUME_INFO) * totalVolumeCount;
            pTotalVolumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
            if (pTotalVolumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                simpleVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            DWORD currentCount = 0;
            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    for (DWORD j = 0; j < tempList.VolumeCount; j++) {
                        pTotalVolumes[currentCount++] = tempList.Volumes[j];
                    }
                }
                fFreeVolumeList(&tempList);
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);

                if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_SIMPLE) {
                    currentCount++;
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }

            if (currentCount == 0) goto CleanUp;

            simpleVolumeList.Volumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VOLUME_INFO) * currentCount);
            if (simpleVolumeList.Volumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                simpleVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);

                if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_SIMPLE) {
                    simpleVolumeList.Volumes[currentCount++] = pTotalVolumes[i];
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }
            simpleVolumeList.LastError = ERROR_SUCCESS;
            simpleVolumeList.Success = TRUE;
            simpleVolumeList.VolumeCount = currentCount;
        }
    }

CleanUp:
    fFreeDiskList(&totalPhysicalDiskList);
    fFreeDiskList(&dynamicDiskList);

    if (!simpleVolumeList.Success) {
        if (simpleVolumeList.Volumes) {
            HeapFree(GetProcessHeap(), 0, simpleVolumeList.Volumes);
            simpleVolumeList.Volumes = NULL;
        }
    }

    if (pTotalVolumes) HeapFree(GetProcessHeap(), 0, pTotalVolumes);

    return simpleVolumeList;
}

VOLUME_LIST
fGetSpannedVolumeList() {
    VOLUME_LIST spannedVolumeList = { 0 };
    spannedVolumeList.LastError = 0;
    spannedVolumeList.Success = FALSE;
    spannedVolumeList.VolumeCount = 0;
    spannedVolumeList.Volumes = NULL;

    VOLUME_INFO* pTotalVolumes = NULL;
    DISK_LIST totalPhysicalDiskList = fGetPhysicalDisks();
    DISK_LIST dynamicDiskList = fGetDynamicDisks(totalPhysicalDiskList);
    DWORD errorCode;

    if (!totalPhysicalDiskList.Success) {
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute fGetPhysicalDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }
    else {
        fSortPhysicalDisks(totalPhysicalDiskList);
        if (!dynamicDiskList.Success) {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Failed to execute fGetDynamicDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
        else {
            fSortPhysicalDisks(dynamicDiskList);

            DWORD totalVolumeCount = 0;

            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    totalVolumeCount += tempList.VolumeCount;
                    fFreeVolumeList(&tempList);
                }
            }

            if (totalVolumeCount == 0) goto CleanUp;

            DWORD bufferSize = sizeof(VOLUME_INFO) * totalVolumeCount;
            pTotalVolumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
            if (pTotalVolumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                spannedVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            DWORD currentCount = 0;
            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    for (DWORD j = 0; j < tempList.VolumeCount; j++) {
                        pTotalVolumes[currentCount++] = tempList.Volumes[j];
                    }
                }
                fFreeVolumeList(&tempList);
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_SPANNED) {
                            currentCount++;
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }

            if (currentCount == 0) goto CleanUp;

            spannedVolumeList.Volumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VOLUME_INFO) * currentCount);
            if (spannedVolumeList.Volumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                spannedVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_SPANNED) {
                            spannedVolumeList.Volumes[currentCount++] = pTotalVolumes[i];
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }
            spannedVolumeList.LastError = ERROR_SUCCESS;
            spannedVolumeList.Success = TRUE;
            spannedVolumeList.VolumeCount = currentCount;
        }
    }

CleanUp:
    fFreeDiskList(&totalPhysicalDiskList);
    fFreeDiskList(&dynamicDiskList);

    if (!spannedVolumeList.Success) {
        if (spannedVolumeList.Volumes) {
            HeapFree(GetProcessHeap(), 0, spannedVolumeList.Volumes);
            spannedVolumeList.Volumes = NULL;
        }
    }

    if (pTotalVolumes) HeapFree(GetProcessHeap(), 0, pTotalVolumes);

    return spannedVolumeList;
}

VOLUME_LIST
fGetMirroredVolumeList() {
    VOLUME_LIST mirroredVolumeList = { 0 };
    mirroredVolumeList.LastError = 0;
    mirroredVolumeList.Success = FALSE;
    mirroredVolumeList.VolumeCount = 0;
    mirroredVolumeList.Volumes = NULL;

    VOLUME_INFO* pTotalVolumes = NULL;
    DISK_LIST totalPhysicalDiskList = fGetPhysicalDisks();
    DISK_LIST dynamicDiskList = fGetDynamicDisks(totalPhysicalDiskList);
    DWORD errorCode;

    if (!totalPhysicalDiskList.Success) {
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute fGetPhysicalDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }
    else {
        fSortPhysicalDisks(totalPhysicalDiskList);
        if (!dynamicDiskList.Success) {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Failed to execute fGetDynamicDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
        else {
            fSortPhysicalDisks(dynamicDiskList);

            DWORD totalVolumeCount = 0;

            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    totalVolumeCount += tempList.VolumeCount;
                    fFreeVolumeList(&tempList);
                }
            }

            if (totalVolumeCount == 0) goto CleanUp;

            DWORD bufferSize = sizeof(VOLUME_INFO) * totalVolumeCount;
            pTotalVolumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
            if (pTotalVolumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                mirroredVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            DWORD currentCount = 0;
            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    for (DWORD j = 0; j < tempList.VolumeCount; j++) {
                        pTotalVolumes[currentCount++] = tempList.Volumes[j];
                    }
                }
                fFreeVolumeList(&tempList);
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_MIRRORED) {
                            currentCount++;
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }

            if (currentCount == 0) goto CleanUp;

            mirroredVolumeList.Volumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VOLUME_INFO) * currentCount);
            if (mirroredVolumeList.Volumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                mirroredVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_MIRRORED) {
                            mirroredVolumeList.Volumes[currentCount++] = pTotalVolumes[i];
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }
            mirroredVolumeList.LastError = ERROR_SUCCESS;
            mirroredVolumeList.Success = TRUE;
            mirroredVolumeList.VolumeCount = currentCount;
        }
    }

CleanUp:
    fFreeDiskList(&totalPhysicalDiskList);
    fFreeDiskList(&dynamicDiskList);

    if (!mirroredVolumeList.Success) {
        if (mirroredVolumeList.Volumes) {
            HeapFree(GetProcessHeap(), 0, mirroredVolumeList.Volumes);
            mirroredVolumeList.Volumes = NULL;
        }
    }

    if (pTotalVolumes) HeapFree(GetProcessHeap(), 0, pTotalVolumes);

    return mirroredVolumeList;
}

VOLUME_LIST
fGetStripedVolumeList() {
    VOLUME_LIST stripedVolumeList = { 0 };
    stripedVolumeList.LastError = 0;
    stripedVolumeList.Success = FALSE;
    stripedVolumeList.VolumeCount = 0;
    stripedVolumeList.Volumes = NULL;

    VOLUME_INFO* pTotalVolumes = NULL;
    DISK_LIST totalPhysicalDiskList = fGetPhysicalDisks();
    DISK_LIST dynamicDiskList = fGetDynamicDisks(totalPhysicalDiskList);
    DWORD errorCode;

    if (!totalPhysicalDiskList.Success) {
        wprintf(L"\033[1;31m");
        wprintf(L"[ERROR] Failed to execute fGetPhysicalDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
        wprintf(L"\033[0m\n");
        goto CleanUp;
    }
    else {
        fSortPhysicalDisks(totalPhysicalDiskList);
        if (!dynamicDiskList.Success) {
            wprintf(L"\033[1;31m");
            wprintf(L"[ERROR] Failed to execute fGetDynamicDisks at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
            wprintf(L"\033[0m\n");
            goto CleanUp;
        }
        else {
            fSortPhysicalDisks(dynamicDiskList);

            DWORD totalVolumeCount = 0;

            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    totalVolumeCount += tempList.VolumeCount;
                    fFreeVolumeList(&tempList);
                }
            }

            if (totalVolumeCount == 0) goto CleanUp;

            DWORD bufferSize = sizeof(VOLUME_INFO) * totalVolumeCount;
            pTotalVolumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
            if (pTotalVolumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                stripedVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            DWORD currentCount = 0;
            for (DWORD i = 0; i < dynamicDiskList.DiskCount; i++) {
                int diskNumber = _wtoi(&dynamicDiskList.Disks[i].SymbolicLink[17]);
                VOLUME_LIST tempList = fGetVolumesOfDisk(diskNumber);
                if (tempList.Success) {
                    for (DWORD j = 0; j < tempList.VolumeCount; j++) {
                        pTotalVolumes[currentCount++] = tempList.Volumes[j];
                    }
                }
                fFreeVolumeList(&tempList);
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_STRIPED) {
                            currentCount++;
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }

            if (currentCount == 0) goto CleanUp;

            stripedVolumeList.Volumes = (VOLUME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VOLUME_INFO) * currentCount);
            if (stripedVolumeList.Volumes == NULL) {
                errorCode = GetLastError();
                wprintf(L"\033[1;31m");
                wprintf(L"[ERROR] Failed to execute HeapAlloc at line %d (%S) in %S. ", __LINE__, __FUNCTION__, __FILE__);
                fPrintErrorMessage(errorCode);
                wprintf(L"\033[0m\n");
                stripedVolumeList.LastError = errorCode;
                goto CleanUp;
            }

            currentCount = 0;
            for (DWORD i = 0; i < totalVolumeCount; i++) {
                if (pTotalVolumes[i].DriveLetter[0] == L'\0') continue;

                WCHAR volumePath[15];
                swprintf_s(volumePath, 15, L"\\\\.\\%c:", pTotalVolumes[i].DriveLetter[0]);
                VOLUME_EXTENTS_LIST volumeExtentList = fGetVolumeExtents(volumePath);
                BOOL isMultiDisks = FALSE;

                if (volumeExtentList.ExtentCount > 1) {
                    for (DWORD j = 0; j < volumeExtentList.ExtentCount; j++) {
                        if (volumeExtentList.Extents[j].DiskNumber != pTotalVolumes[i].DiskNumber) {
                            isMultiDisks = TRUE;
                            break;
                        }
                    }

                    if (isMultiDisks) {
                        if (fGetDynamicVolumeType(&volumeExtentList) == VOLUME_STRIPED) {
                            stripedVolumeList.Volumes[currentCount++] = pTotalVolumes[i];
                        }
                    }
                }
                fFreeVolumeExtentsList(&volumeExtentList);
            }
            stripedVolumeList.LastError = ERROR_SUCCESS;
            stripedVolumeList.Success = TRUE;
            stripedVolumeList.VolumeCount = currentCount;
        }
    }

CleanUp:
    fFreeDiskList(&totalPhysicalDiskList);
    fFreeDiskList(&dynamicDiskList);

    if (!stripedVolumeList.Success) {
        if (stripedVolumeList.Volumes) {
            HeapFree(GetProcessHeap(), 0, stripedVolumeList.Volumes);
            stripedVolumeList.Volumes = NULL;
        }
    }

    if (pTotalVolumes) HeapFree(GetProcessHeap(), 0, pTotalVolumes);

    return stripedVolumeList;
}

FILE_LIST
fGetFileInfoList(LPCWSTR path) {
    FILE_LIST res = { 0 };
    res.FileCount = 0;
    res.Files = NULL;
    res.LastError = 0;
    res.Success = FALSE;

    DWORD capacity = 0;

    DWORD fileAttr = GetFileAttributesW(path);
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        res.LastError = GetLastError();
        return res;
    }

    if (!(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
        res.Files = (FILE_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FILE_INFO));
        if (res.Files != NULL) {
            wcscpy_s(res.Files[0].FileName, MAX_PATH, path);

            WIN32_FILE_ATTRIBUTE_DATA data;
            if (GetFileAttributesExW(path, GetFileExInfoStandard, &data)) {
                LARGE_INTEGER fs;
                fs.LowPart = data.nFileSizeLow;
                fs.HighPart = data.nFileSizeHigh;
                res.Files[0].FileSize = fs.QuadPart;
            }

            res.Files[0].AllocatedSize = fGetAllocatedSize(path);

            res.FileCount = 1;
            res.Success = TRUE;
            res.LastError = ERROR_SUCCESS;
        }
        else {
            res.LastError = GetLastError();
            return res;
        }
    }
    else {
        fScanDirectory(path, &res, &capacity);
        res.Success = TRUE;
        res.LastError = ERROR_SUCCESS;
    }

CleanUp:
    if (!res.Success) {
        if (res.Files) {
            HeapFree(GetProcessHeap(), 0, res.Files);
            res.Files = NULL;
        }
    }

    return res;
}

FILE_EXTENTS_LIST
fGetFileExtents(LPCWSTR filePath) {
    FILE_EXTENTS_LIST res = { 0 };
    res.Success = FALSE;
    res.Extents = NULL;
    res.ExtentCount = 0;
    res.LastError = 0;

    STARTING_VCN_INPUT_BUFFER input = { 0 };
    input.StartingVcn.QuadPart = 0;
    DWORD bytesReturned = 0;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE* buffer = NULL;
    BOOL bIoctlSuccess = FALSE;
    PRETRIEVAL_POINTERS_BUFFER ptrs = NULL;

    hFile = CreateFileW(filePath, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        res.LastError = GetLastError();
        return res;
    }

    DWORD bufferSize = FIELD_OFFSET(RETRIEVAL_POINTERS_BUFFER, Extents) + sizeof(((PRETRIEVAL_POINTERS_BUFFER)0)->Extents[0]) * 20;
    buffer = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSize);
    if (!buffer) {
        res.LastError = ERROR_OUTOFMEMORY;
        CloseHandle(hFile);
        return res;
    }

    while (TRUE) {
        if (DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS, &input, sizeof(input), buffer, bufferSize, &bytesReturned, NULL)) break;

        DWORD err = GetLastError();
        if (err == ERROR_MORE_DATA || err == ERROR_INSUFFICIENT_BUFFER) {
            bufferSize *= 2;
            BYTE* newBuffer = (BYTE*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffer, bufferSize);

            if (!newBuffer) {
                res.LastError = ERROR_OUTOFMEMORY;
                goto CleanUp;
            }
            buffer = newBuffer;
        }
        else if (err == ERROR_HANDLE_EOF) {
            res.Success = TRUE;
            res.LastError = ERROR_SUCCESS;
            goto CleanUp;
        }
        else {
            res.LastError = err;
            goto CleanUp;
        }
    }

    ptrs = (PRETRIEVAL_POINTERS_BUFFER)buffer;

    if (ptrs->ExtentCount > 0) {
        res.Extents = (FILE_EXTENT_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptrs->ExtentCount * sizeof(FILE_EXTENT_INFO));

        if (res.Extents) {
            DWORD validCount = 0;
            LONGLONG currentVcn = ptrs->StartingVcn.QuadPart;

            for (DWORD i = 0; i < ptrs->ExtentCount; i++) {
                LONGLONG lcn = ptrs->Extents[i].Lcn.QuadPart;
                LONGLONG nextVcn = ptrs->Extents[i].NextVcn.QuadPart;

                if (lcn != -1LL) {
                    res.Extents[validCount].Vcn = currentVcn;
                    res.Extents[validCount].StartLcn = lcn;
                    res.Extents[validCount].ClusterCount = nextVcn - currentVcn;
                    validCount++;
                }
                currentVcn = nextVcn;
            }

            res.ExtentCount = validCount;
            res.Success = TRUE;
            res.LastError = 0;
        }
        else {
            res.LastError = ERROR_OUTOFMEMORY;
        }
    }
    else {
        res.LastError = ERROR_NO_MORE_ITEMS;
    }

CleanUp:
    if (buffer) HeapFree(GetProcessHeap(), 0, buffer);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    if (!res.Success && res.Extents) {
        HeapFree(GetProcessHeap(), 0, res.Extents);
        res.Extents = NULL;
    }

    return res;
}

LONGLONG
fGetAllocatedSize(LPCWSTR filePath) {
    FILE_STANDARD_INFO fsi;
    LONGLONG result = -1;
    HANDLE hFile = CreateFileW(filePath, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return result;
    }
    
    if (GetFileInformationByHandleEx(hFile, FileStandardInfo, &fsi, sizeof(fsi))) {
        result = fsi.AllocationSize.QuadPart;
    }
    CloseHandle(hFile);
    return result;
}

void
fFreeFileInfoList(FILE_LIST* list) {
    if (list && list->Files) {
        HeapFree(GetProcessHeap(), 0, list->Files);
        list->Files = NULL;
        list->FileCount = 0;
        list->LastError = 0;
        list->Success = FALSE;
    }
}

void
fFreeFileExtentsList(FILE_EXTENTS_LIST* list) {
    if (list && list->Extents) {
        HeapFree(GetProcessHeap(), 0, list->Extents);
        list->Extents = NULL;
        list->ExtentCount = 0;
        list->LastError = 0;
        list->Success = FALSE;
    }
}

void
fScanDirectory(LPCWSTR searchPath, FILE_LIST* pList, DWORD* capacity) {
    WCHAR findPath[MAX_PATH];
    swprintf_s(findPath, MAX_PATH, L"%s\\*", searchPath);

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(findPath, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

        WCHAR fullPath[MAX_PATH];
        swprintf_s(fullPath, MAX_PATH, L"%s\\%s", searchPath, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            fScanDirectory(fullPath, pList, capacity);
        }
        else {
            if (pList->FileCount >= *capacity) {
                DWORD newCapacity = (*capacity == 0) ? 10 : *capacity * 2;

                FILE_INFO* temp = NULL;
                if (pList->Files == NULL) {
                    temp = (FILE_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, newCapacity * sizeof(FILE_INFO));
                }
                else {
                    temp = (FILE_INFO*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pList->Files, newCapacity * sizeof(FILE_INFO));
                }

                if (!temp) {
                    FindClose(hFind);
                    return;
                }
                pList->Files = temp;
                *capacity = newCapacity;
            }

            PFILE_INFO pCurrent = &pList->Files[pList->FileCount];
            wcscpy_s(pCurrent->FileName, MAX_PATH, fullPath);

            LARGE_INTEGER fs;
            fs.LowPart = fd.nFileSizeLow;
            fs.HighPart = fd.nFileSizeHigh;
            pCurrent->FileSize = fs.QuadPart;

            pCurrent->AllocatedSize = fGetAllocatedSize(fullPath);

            pList->FileCount++;
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

SLACK_POINT
fGetSlackSectors(DWORD numberOfAllocatedClusters, LONGLONG startLcn, LONGLONG fileSize, WCHAR driveLetter) {
    SLACK_POINT finalSlack = { 0 };
    WCHAR rootPath[10];
    swprintf_s(rootPath, _countof(rootPath), L"%c:\\", driveLetter);

    DWORD sectorsPerCluster = 0;
    DWORD bytesPerSector = 0;
    DWORD numberOfFreeClusters = 0;
    DWORD totalNumberOfClusters = 0;
    if (!GetDiskFreeSpaceW(rootPath, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters)) return finalSlack;

    DWORD bytesPerCluster = bytesPerSector * sectorsPerCluster;

    WCHAR volumePath[10];
    swprintf_s(volumePath, _countof(volumePath), L"\\\\.\\%c:", driveLetter);      

    LONGLONG startSectorOffsetOfStartLcn = startLcn * sectorsPerCluster;
    LONGLONG startSectorOffsetOfLastLcn = (startLcn + numberOfAllocatedClusters - 1) * sectorsPerCluster;
    LONGLONG remaining = fileSize % bytesPerCluster;
    LONGLONG usedSectorsInLastLcn = (remaining + bytesPerSector - 1) / bytesPerSector;
    LONGLONG slackSectorsInLastLcn = sectorsPerCluster - usedSectorsInLastLcn;
    LONGLONG startSlackSectorOffsetOfLastLcn = startSectorOffsetOfLastLcn + usedSectorsInLastLcn;

    finalSlack.NumberOfslackSectors = (DWORD)slackSectorsInLastLcn;
    finalSlack.PhysicalOffset = startSlackSectorOffsetOfLastLcn;
    
    return finalSlack;
}

BOOL
fIsNtfsVolume(WCHAR driveLetter) {
    DWORD errorCode;
    WCHAR label[MAX_PATH] = { 0 };
    WCHAR fileSystem[MAX_PATH] = { 0 };
    WCHAR rootPath[10];
    swprintf_s(rootPath, _countof(rootPath), L"%c:\\", driveLetter);

    if (!GetVolumeInformationW(rootPath, NULL, 0, NULL, NULL, NULL, fileSystem, ARRAYSIZE(fileSystem))) {
        return FALSE;
    }

    if (_wcsicmp(fileSystem, L"NTFS") == 0) return TRUE;

    return FALSE;
}