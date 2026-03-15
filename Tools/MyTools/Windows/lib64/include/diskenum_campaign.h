#ifndef DISKENUM_CAMPAIGN_H
#define DISKENUM_CAMPAIGN_H

#include <windows.h>
#include <stdio.h>
#include <setupapi.h>
#include <string.h>
#include <ShlObj.h>
#include <ntddvol.h>
#include "utils.h"

#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "utils.lib")

typedef struct _DISK_INFO {
	WCHAR DevicePath[MAX_PATH];
	WCHAR SymbolicLink[MAX_PATH];
	WCHAR Status[15];
	LONGLONG TotalSizeInByte;
	LONGLONG UnallocatedSizeInByte;
	DWORD BytesPerSector;
	BOOL IsGpt;
	BOOL IsDynamic;
} DISK_INFO, *PDISK_INFO;

typedef struct _DISK_LIST {
	DWORD DiskCount;
	BOOL Success;
	DWORD LastError;
	DISK_INFO* Disks;
} DISK_LIST;

typedef struct _PARTITION_INFO {
	DWORD NumberId;
	WCHAR Type[32];
	LONGLONG StartOffsetInByte;
	LONGLONG EndOffsetInByte;
	LONGLONG SizeInByte;
} PARTITION_INFO, *PPARTITION_INFO;

typedef struct _PARTITION_LIST {
	DWORD PartitionCount;
	BOOL Success;
	DWORD LastError;
	PARTITION_INFO* Partitions;
} PARTITION_LIST;

typedef struct _VOLUME_EXTENT_INFO {
	DWORD DiskNumber;
	LONGLONG StartingOffset;
	LONGLONG ExtentLength;
} VOLUME_EXTENT_INFO, *PVOLUME_EXTENT_INFO;

typedef struct _VOLUME_EXTENTS_LIST {
	DWORD ExtentCount;
	BOOL Success;
	DWORD LastError;
	VOLUME_EXTENT_INFO* Extents;
} VOLUME_EXTENTS_LIST;

typedef struct _VOLUME_INFO {
	WCHAR VolumeGuid[MAX_PATH];
	WCHAR DriveLetter[10];
	WCHAR Label[MAX_PATH];
	WCHAR FileSystem[MAX_PATH];
	LONGLONG SizeInByte;
	LONGLONG FreeSizeInByte;
	DWORD BytesPerSector;
	DWORD SectorsPerCluster;
	DWORD BytesPerCluster;
	DWORD TotalClusters;
	DWORD FreeClusters;
	DWORD DiskNumber;
	LONGLONG StartOffsetInByte;
}VOLUME_INFO, *PVOLUME_INFO;

typedef struct _VOLUME_LIST {
	DWORD VolumeCount;
	BOOL Success;
	DWORD LastError;
	VOLUME_INFO* Volumes;
}VOLUME_LIST;

typedef enum _VOLUME_TYPE {
	VOLUME_UNKNOWN = 0,
	VOLUME_SIMPLE,
	VOLUME_SPANNED,
	VOLUME_STRIPED,
	VOLUME_MIRRORED
} VOLUME_TYPE;

typedef struct _FILE_EXTENT_INFO {
	LONGLONG Vcn;
	LONGLONG StartLcn;
	LONGLONG ClusterCount;
} FILE_EXTENT_INFO, *PFILE_EXTENT_INFO;

typedef struct _FILE_EXTENTS_LIST {
	DWORD ExtentCount;
	BOOL Success;
	DWORD LastError;
	FILE_EXTENT_INFO* Extents;
} FILE_EXTENTS_LIST;

typedef struct _FILE_INFO {
	WCHAR FileName[MAX_PATH];
	LONGLONG FileSize;
	LONGLONG AllocatedSize;
} FILE_INFO, *PFILE_INFO;

typedef struct _FILE_LIST {
	DWORD FileCount;
	BOOL Success;
	DWORD LastError;
	FILE_INFO* Files;
} FILE_LIST;

typedef struct _SLACK_POINT {
	DWORD DiskNumber;
	DWORD DiskNumberStripe128;
	DWORD DiskNumberStripe256;
	DWORD DiskNumberStripe512;
	DWORD DiskNumberStripe1024;
	DWORD NumberOfslackSectors;
	LONGLONG PhysicalOffset;
	//LONGLONG PhysicalOffsetStripe128;
	//LONGLONG PhysicalOffsetStripe256;
	//LONGLONG PhysicalOffsetStripe512;
	//LONGLONG PhysicalOffsetStripe1024;
} SLACK_POINT;


/// <summary>
/// Sort a list of physical disks using Bubble sort.
/// </summary>
/// <param name="list">[in] A list of physical disk. See fGetPhysicalDisks.</param>
void fSortPhysicalDisks(DISK_LIST list);

/// <summary>
/// Get the size of all used extents of a specific disk.
/// </summary>
/// <param name="dwTargetDiskNumber">[in] A disk number</param>
/// <returns>(LONGLONG ) The size of used extents in byte.</returns>
LONGLONG fGetTotalUsedExtents(DWORD dwTargetDiskNumber);

/// <summary>
/// Check if the specified disk is dynamic disk.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsDynamicDisk(HANDLE hDevice);

/// <summary>
/// Check if the specified disk is GPT.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsGptDisk(HANDLE hDevice);

/// <summary>
/// Get bytes per sector of a specfic disk.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <returns>(DWORD) Bytes per sector.</returns>
DWORD fGetBytesPerSectorByHandle(HANDLE hDevice);

/// <summary>
/// Get the status of specific disk.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <param name="outStatus">[out] An output buffer to contain the status.</param>
/// <param name="bufferSize">[in] The size of output buffer.</param>
void fGetDiskStatusByHandle(HANDLE hDevice, WCHAR* outStatus, DWORD bufferSize);

/// <summary>
/// Get the size of physical disk in byte.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <returns>(LONGLONG) A size in byte.</returns>
LONGLONG fGetDiskSizeByHandle(HANDLE hDevice);

/// <summary>
/// Free allocated memory of DISK_LIST.
/// </summary>
/// <param name="list">[in] A list of physical disks which is returned by fGetPhysicalDisks.</param>
void fFreeDiskList(DISK_LIST* list);

/// <summary>
/// This function calculates the size in byte of unallocated spaces in a specific disk.
/// </summary>
/// <param name="hDevice">[in] A handle to a physical disk. This handle must be created from device interface path.</param>
/// <param name="symbolicLink">[in] The symbolic link of a disk should be \\.\PhysicalDriveX (X is a disk number).</param>
/// <param name="totalDiskSize">[in] The total size of a disk. See fGetDiskSizeByHandle.</param>
/// <returns>(LONGLONG) A size in byte.</returns>
LONGLONG fGetUnallocatedSize(HANDLE hDevice, LPCWSTR symbolicLink, LONGLONG totalDiskSize);

/// <summary>
/// Get informations of all available physical disks on machine.
/// </summary>
/// <returns>Returns (DISK_LIST) a list of physical disks</returns>
DISK_LIST fGetPhysicalDisks();

/// <summary>
/// Free allocated memory for PARTITION_LIST.
/// </summary>
/// <param name="list">[in] A list of partitions which is returned by fGetPartitionOfDisk.</param>
void fFreePartitionList(PARTITION_LIST* list);

/// <summary>
/// Get a friendly name for a partition.
/// </summary>
/// <param name="p">[in] A pointer to PartitionEntry field of PARTITION_INFORMATION_EX.</param>
/// <param name="outBuffer">[out] The output buffer which receives the friendly name for a partition.</param>
/// <param name="outBufferSize">[in] The output buffer size</param>
/// <returns>TRUE/FALSE</returns>
BOOL fGetFriendlyPartitionName(PARTITION_INFORMATION_EX* p, WCHAR* outBuffer, DWORD outBufferSize);

/// <summary>
/// Get a list of partitions from a specified physical disk.
/// </summary>
/// <param name="diskIndex">[in] A disk number.</param>
/// <returns>(PARTITION_LIST) A list of partitions.</returns>
PARTITION_LIST fGetPartitionsOfDisk(DWORD diskIndex);

/// <summary>
/// Check if the given volume is actually existing on specified disk.
/// </summary>
/// <param name="volumeName">[in] A volume name which can be take from FindFirstVolumeW or FindNextVolumeW.</param>
/// <param name="diskNumber">[in] A disk number.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsVolumeOnDisk(LPCWSTR volumeName, DWORD diskNumber);

/// <summary>
/// Free allocated heap memory for VOLUME_LIST.
/// </summary>
/// <param name="list">[in] A volume list which is returned by fGetVolumesOfDisk.</param>
void fFreeVolumeList(VOLUME_LIST* list);

/// <summary>
/// Free allocated heap memory for VOLUME_EXTENTS_LIST.
/// </summary>
/// <param name="list">[in] A volume extents list which is returned by fGetVolumeExtents.</param>
void fFreeVolumeExtentsList(VOLUME_EXTENTS_LIST* list);

/// <summary>
/// Get all available volumes of a specified disk.
/// </summary>
/// <param name="diskIndex">[in] A disk number.</param>
/// <returns>(VOLUME_LIST) A list of volumes on a specified disk.</returns>
VOLUME_LIST fGetVolumesOfDisk(DWORD diskIndex);

/// <summary>
/// Get all available extents of a specified volume.
/// </summary>
/// <param name="volumePath">[in] A volume path which has format of \\.\X:</param>
/// <returns>(VOLUME_EXTENTS_LIST) A list of extents of a specified volume.</returns>
VOLUME_EXTENTS_LIST fGetVolumeExtents(LPCWSTR volumePath);

/// <summary>
/// Get all available dynamic disks on the machine.
/// </summary>
/// <param name="totalPhysicalDisks">[in] A list of physical disks which can be taken from fGetPhysicalDisks.</param>
/// <returns>(DISK_LIST) A list of dynamic disks.</returns>
DISK_LIST fGetDynamicDisks(DISK_LIST totalPhysicalDisks);

/// <summary>
/// Identfy the type of dynamic NTFS volume (Simple, Spanned, Striped, Mirrored)
/// </summary>
/// <param name="volumeExtentsList">[in] A pointer of VOLUME_EXTENS_LIST which can be taken from fGetVolumeExtents.</param>
/// <returns>(VOLUME_TYPE) The type of volume.</returns>
VOLUME_TYPE fGetDynamicVolumeType(VOLUME_EXTENTS_LIST* volumeExtentsList);

/// <summary>
/// Get all available dynamic simple volumes on the machine.
/// </summary>
/// <returns>(VOLUME_LIST) A list of dynamic simple volumes.</returns>
VOLUME_LIST fGetSimpleVolumeList();

/// <summary>
/// Get all available dynamic spanned volumes on the machine.
/// </summary>
/// <returns>(VOLUME_LIST) A list of dynamic spanned volumes.</returns>
VOLUME_LIST fGetSpannedVolumeList();

/// <summary>
/// Get all available dynamic mirrored volumes on the machine.
/// </summary>
/// <returns>(VOLUME_LIST) A list of dynamic mirrored volumes.</returns>
VOLUME_LIST fGetMirroredVolumeList();

/// <summary>
/// Get all available dynamic striped volumes on the machine.
/// </summary>
/// <returns>(VOLUME_LIST) A list of dynamic striped volumes.</returns>
VOLUME_LIST fGetStripedVolumeList();

/// <summary>
/// Get informations about one or more files. These informations will be stored at FILE_INFO struct.
/// </summary>
/// <param name="path">[in] File path or directory path.</param>
/// <returns>(FILE_LIST) A list of files.</returns>
FILE_LIST fGetFileInfoList(LPCWSTR path);

/// <summary>
/// Get all available LCNs which are allocated for the specified file.
/// </summary>
/// <param name="filePath">[in] File path.</param>
/// <returns>(FILE_EXTENTS_LIST) A list of allocated LCNs.</returns>
FILE_EXTENTS_LIST fGetFileExtents(LPCWSTR filePath);

/// <summary>
/// Get the size of a specified file in byte.
/// </summary>
/// <param name="filePath">[in] A file path.</param>
/// <returns>(LONGLONG) the size of file in byte.</returns>
LONGLONG fGetAllocatedSize(LPCWSTR filePath);

/// <summary>
/// Free allocated heap memory for FILE_LIST.
/// </summary>
/// <param name="list">[in] A list of files which can be taken from fGetFileInfoList.</param>
void fFreeFileInfoList(FILE_LIST* list);

/// <summary>
/// Free allocated heap memory for FILE_EXTENTS_LIST.
/// </summary>
/// <param name="list">[in] A list of LCNs which can be taken from fGetFileExtents.</param>
void fFreeFileExtentsList(FILE_EXTENTS_LIST* list);

/// <summary>
/// This function scans a direcory path to gather all available files recursively.
/// </summary>
/// <param name="searchPath">[in] An input directory path.</param>
/// <param name="pList">[in, out] An output FILE_LIST buffer which receives all gathered files.</param>
/// <param name="capacity">[out] An output size of FILE_LIST buffer.</param>
void fScanDirectory(LPCWSTR searchPath, FILE_LIST* pList, DWORD* capacity);

/// <summary>
/// Calculate the slack sector from the start LCN of a file.
/// </summary>
/// <param name="numberOfAllocatedClusters">[in] The number of allocated clusters (VCN).</param>
/// <param name="startLcn">[in] The start LCN of a file</param>
/// <param name="fileSize">[in] The size of a file</param>
/// <param name="driveLetter">[in] The drive letter in which the file is located.</param>
/// <returns>(SLACK_POINT) The informations about the slack sectors.</returns>
SLACK_POINT fGetSlackSectors(DWORD numberOfAllocatedClusters, LONGLONG startLcn, LONGLONG fileSize, WCHAR driveLetter);

/// <summary>
/// Check if the given volume is NTFS.
/// </summary>
/// <param name="driveLetter">[in] A drive letter.</param>
/// <returns>TRUE/FALSE</returns>
BOOL fIsNtfsVolume(WCHAR driveLetter);

#endif