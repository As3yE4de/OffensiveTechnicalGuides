#include "diskenum_campaign.h"
#include "utils.h"

#define VERSION L"Betav0.0.0.1"
#define AUTHOR L"As3yE4de"
#define GITHUB L"https://github.com/As3yE4de"

#pragma comment(lib, "diskenum_campaign")
#pragma comment(lib, "utils")

typedef struct _TOOL_ARGS {
	BOOL diskArg;
	BOOL diskPartitionArg;
	BOOL diskVolumeArg;
	WCHAR* volumeExtentArg;
	BOOL volumeSimpleArg;
	BOOL volumeSpannedArg;
	BOOL volumeStripedArg;
	BOOL volumeMirroredArg;
	WCHAR* slackArg;
	BOOL Success;
	DWORD LastError;
} TOOL_ARGS, *PTOOL_ARGS;

void fPrintBanner() {
	wprintf(L"%c%c%c ________  .__        __    ___________                     \n", 178, 177, 176);
	wprintf(L"%c%c%c \\______ \\ |__| _____|  | __\\_   _____/ ____  __ __  _____  \n", 178, 177, 176);
	wprintf(L"%c%c%c  |    |  \\|  |/  ___/  |/ / |    __)_ /    \\|  |  \\/     \\ \n", 178, 177, 176);
	wprintf(L"%c%c%c  |    `   \\  |\\___ \\|    <  |        \\   |  \\  |  /  Y Y  \\\n", 178, 177, 176);
	wprintf(L"%c%c%c /_______  /__/____  >__|_ \\/_______  /___|  /____/|__|_|  /\n", 178, 177, 176);
	wprintf(L"%c%c%c         \\/        \\/     \\/        \\/     \\/            \\/ \n", 178, 177, 176);
	wprintf(L"%c%c%c\n", 178, 177, 176);
	wprintf(L"%c%c%c\n", 178, 177, 176);
	wprintf(L"%c%c%c [INFO] Description: A tool for enumerating physical disks and volumes on Windows.\n", 178, 177, 176);
	wprintf(L"%c%c%c [INFO] Version: %s\n", 178, 177, 176, VERSION);
	wprintf(L"%c%c%c [INFO] Author: %s - AIC (%s)\n", 178, 177, 176, AUTHOR, GITHUB);
	wprintf(L"%c%c%c [INFO] Tested Environments: Windows 10 (x64)\n\n", 178, 177, 176);
}

void fUsage(WCHAR* tool) {
	wprintf(L"\nUSAGE\n");
	wprintf(L"\n\t%s <COMMAND(S)>\n", tool);
	wprintf(L"\nCOMMANDS\n");
	wprintf(L"\n\t%-35s\t%s\n", L"--disk", L"List physical disks");
	wprintf(L"\n\t%-35s\t%s\n", L"--disk-partition", L"List all partitions of disks");
	wprintf(L"\n\t%-35s\t%s\n", L"--disk-volume", L"List all volumes of disks");
	wprintf(L"\n\t%-35s\t%s\n", L"--volume-extent=<DRIVE_LETTER>", L"List all extents of a specified volume. The drive letter must be a ");
	wprintf(L"\n\t%-35s\t%s\n", L"", L"single character (for example: .\\DiskEnum.exe --disk-volume=C)");
	wprintf(L"\n\t%-35s\t%s\n", L"--volume-simple", L"List all dynamic simple volumes (only NTFS)");
	wprintf(L"\n\t%-35s\t%s\n", L"--volume-spanned", L"List all dynamic spanned volumes (only NTFS)");
	wprintf(L"\n\t%-35s\t%s\n", L"--volume-striped", L"List all dynamic striped volumes (only NTFS)");
	wprintf(L"\n\t%-35s\t%s\n", L"--volume-mirrored", L"List all dynamic mirrored volumes (only NTFS)");
	wprintf(L"\n\t%-35s\t%s\n", L"--slack=<PATH>", L"Find slack space of files. The given path can be a file path or a ");
	wprintf(L"\n\t%-35s\t%s\n", L"", L"directory path");
}

TOOL_ARGS fParseToolArgs(int argc, WCHAR* argv[]) {
	TOOL_ARGS args = { 0 };
	args.diskArg = FALSE;
	args.diskPartitionArg = FALSE;
	args.diskVolumeArg = FALSE;
	args.volumeExtentArg = NULL;
	args.volumeSimpleArg = FALSE;
	args.volumeSpannedArg = FALSE;
	args.volumeStripedArg = FALSE;
	args.volumeMirroredArg = FALSE;
	args.slackArg = NULL;
	args.Success = FALSE;
	args.LastError = ERROR_INVALID_PARAMETER;

	if (argc < 2) return args;

	for (DWORD i = 1; i < argc; i++) {
		if (wcscmp(argv[i], L"--disk") == 0) {
			args.diskArg = TRUE;
		}
		else if (wcscmp(argv[i], L"--disk-partition") == 0) {
			args.diskPartitionArg = TRUE;
		}
		else if (wcscmp(argv[i], L"--disk-volume") == 0) {
			args.diskVolumeArg = TRUE;
		}
		else if (wcsncmp(argv[i], L"--volume-extent=", 16) == 0) {
			args.volumeExtentArg = argv[i] + 16;
		}
		else if (wcscmp(argv[i], L"--volume-simple") == 0) {
			args.volumeSimpleArg = TRUE;
		}
		else if (wcscmp(argv[i], L"--volume-spanned") == 0) {
			args.volumeSpannedArg = TRUE;
		}
		else if (wcscmp(argv[i], L"--volume-striped") == 0) {
			args.volumeStripedArg = TRUE;
		}
		else if (wcscmp(argv[i], L"--volume-mirrored") == 0) {
			args.volumeMirroredArg = TRUE;
		}
		else if (wcsncmp(argv[i], L"--slack=", 8) == 0) {
			args.slackArg = argv[i] + 8;
		}
		else {
			return args;
		}
	}

	args.Success = TRUE;
	args.LastError = ERROR_SUCCESS;

	return args;
}

int wmain(int argc, WCHAR* argv[]) {
	fEnableConsoleColor();
	if (!IsUserAnAdmin()) {
		printf("\033[1;31m");
		printf("[-] Must run as administrator. QUITTING!\n");
		printf("\033[0m");
		return 1;
	}
	fPrintBanner();

	TOOL_ARGS args = fParseToolArgs(argc, argv);
	if (!args.Success && args.LastError == ERROR_INVALID_PARAMETER) {
		fUsage(argv[0]);
		return 1;
	}

	if (args.Success && args.LastError == ERROR_SUCCESS) {
		if (args.diskArg) {
			WCHAR formattedSize[64];
			DISK_LIST physicalDisks = fGetPhysicalDisks();
			if (!physicalDisks.Success || physicalDisks.Disks == NULL || physicalDisks.DiskCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get physical disks on this machine!\033[0m\n");
				fFreeDiskList(&physicalDisks);
				return 1;
			}

			fSortPhysicalDisks(physicalDisks);
			wprintf(L"[INFO] Enumerating physical disks...\n");
			for (DWORD i = 0; i < physicalDisks.DiskCount; i++) {
				DISK_INFO* d = &physicalDisks.Disks[i];
				wprintf(L"\033[1;32m");
				wprintf(L"[+] Found #%d disk(s)\n", i + 1);
				wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Device Interface Path", d->DevicePath);
				wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Symbolic Link", d->SymbolicLink);
				wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Status", d->Status);
				fFormatBytes(d->TotalSizeInByte, formattedSize, _countof(formattedSize));
				wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, d->TotalSizeInByte);
				fFormatBytes(d->UnallocatedSizeInByte, formattedSize, _countof(formattedSize));
				wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Unallocated Size", formattedSize, d->UnallocatedSizeInByte);
				wprintf(L" %c%c%c%c %-26s : %d\n", 192, 196, 196, 16, L"Bytes Per Sector", d->BytesPerSector);
				wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"GPT", (d->IsGpt ? L"Yes" : L"No"));
				wprintf(L" %c%c%c%c %-26s : %s\n\n", 192, 196, 196, 16, L"Dynamic", (d->IsDynamic ? L"Yes" : L"No"));
				wprintf(L"\033[0m");
			}
			fFreeDiskList(&physicalDisks);
		}

		if (args.diskPartitionArg) {
			WCHAR formattedSize[64];
			DISK_LIST physicalDisks = fGetPhysicalDisks();
			if (!physicalDisks.Success || physicalDisks.Disks == NULL || physicalDisks.DiskCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get physical disks on this machine!\033[0m\n");
				fFreeDiskList(&physicalDisks);
				return 1;
			}

			fSortPhysicalDisks(physicalDisks);

			for (DWORD i = 0; i < physicalDisks.DiskCount; i++) {
				wprintf(L"[INFO] Enumerating partitions on disk \\\\.\\PhysicalDrive%lu...\n", i);
				PARTITION_LIST partitionList = fGetPartitionsOfDisk(i);
				if (!partitionList.Success || partitionList.Partitions == NULL || partitionList.PartitionCount == 0) {
					wprintf(L"\033[1;31m[-] Cannot get partitions on this machine!\033[0m\n");
					fFreeDiskList(&physicalDisks);
					fFreePartitionList(&partitionList);
					return 1;
				}

				if (partitionList.Partitions == NULL) continue;

				LONGLONG currentOffset = 0;
				for (DWORD j = 0; j < partitionList.PartitionCount; j++) {
					PARTITION_INFO* p = &partitionList.Partitions[j];
					if (p->SizeInByte == 0) continue;
					if (p->StartOffsetInByte > currentOffset) {
						LONGLONG gap = p->StartOffsetInByte - currentOffset;
						wprintf(L"\033[1;33m");
						wprintf(L" %c%c%c%c [+] Found Partition Gap\n", 192, 196, 196, 16);
						fFormatBytes(gap, formattedSize, _countof(formattedSize));
						wprintf(L" %c     %c%c%c%c %-26s : %s (%lld in Bytes)\n", 179, 192, 196, 196, 16, L"Gap Size", formattedSize, gap);
						wprintf(L" %c     %c%c%c%c %-26s : %lld\n", 179, 192, 196, 196, 16, L"Starting Offset In Bytes", currentOffset);
						wprintf(L" %c     %c%c%c%c %-26s : %lld\n\n", 179, 192, 196, 196, 16, L"Ending Offset In Bytes)", p->StartOffsetInByte - 1);
						wprintf(L"\033[0m");
					}

					if (p->SizeInByte > 0) {
						wprintf(L"\033[1;32m");
						wprintf(L" %c%c%c%c [+] Found #%d partition(s)\n", 192, 196, 196, 16, j + 1);
						wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Partition ID", p->NumberId);
						wprintf(L" %c     %c%c%c%c %-26s : %s\n", 179, 192, 196, 196, 16, L"Type", p->Type);
						wprintf(L" %c     %c%c%c%c %-26s : %lld\n", 179, 192, 196, 196, 16, L"Starting Offset In Bytes", p->StartOffsetInByte);
						wprintf(L" %c     %c%c%c%c %-26s : %lld\n", 179, 192, 196, 196, 16, L"Ending Offset In Bytes", p->EndOffsetInByte);
						fFormatBytes(p->SizeInByte, formattedSize, _countof(formattedSize));
						wprintf(L" %c     %c%c%c%c %-26s : %s (%lld Bytes)\n\n", 179, 192, 196, 196, 16, L"Total Size", formattedSize, p->SizeInByte);
						wprintf(L"\033[0m");
					}
					currentOffset = p->EndOffsetInByte + 1;
				}
				if (currentOffset < physicalDisks.Disks[i].TotalSizeInByte) {
					wprintf(L"\033[1;33m");
					wprintf(L" %c%c%c%c [+] Found Partition Gap\n", 192, 196, 196, 16);
					fFormatBytes(physicalDisks.Disks[i].TotalSizeInByte - currentOffset, formattedSize, _countof(formattedSize));
					wprintf(L"     %c%c%c%c %-26s : %s (%lld in Bytes)\n", 192, 196, 196, 16, L"Gap Size", formattedSize, physicalDisks.Disks[i].TotalSizeInByte - currentOffset);
					wprintf(L"     %c%c%c%c %-26s : %lld\n", 192, 196, 196, 16, L"Starting Offset (in bytes)", currentOffset);
					wprintf(L"     %c%c%c%c %-26s : %lld\n\n\n", 192, 196, 196, 16, L"Ending Offset (in bytes)", physicalDisks.Disks[i].TotalSizeInByte - 1);
					wprintf(L"\033[0m");
				}
				fFreePartitionList(&partitionList);
			}
			fFreeDiskList(&physicalDisks);
		}

		if (args.diskVolumeArg) {
			WCHAR formattedSize[64];
			DISK_LIST physicalDisks = fGetPhysicalDisks();
			if (!physicalDisks.Success || physicalDisks.Disks == NULL || physicalDisks.DiskCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get physical disks on this machine!\033[0m\n");
				fFreeDiskList(&physicalDisks);
				return 1;
			}

			fSortPhysicalDisks(physicalDisks);

			for (DWORD i = 0; i < physicalDisks.DiskCount; i++) {
				DWORD diskNumber = _wtoi(&physicalDisks.Disks[i].SymbolicLink[17]);
				wprintf(L"[INFO] Enumerating volumes on disk %s\n", physicalDisks.Disks[i].SymbolicLink);
				VOLUME_LIST volumes = fGetVolumesOfDisk(diskNumber);
				if (!volumes.Success || volumes.Volumes == NULL || volumes.VolumeCount == 0) {
					wprintf(L"\033[1;31m[-] Cannot get volumes on this machine!\033[0m\n");
					fFreeVolumeList(&volumes);
					continue;
				}

				if (volumes.Volumes != NULL) {
					for (DWORD j = 0; j < volumes.VolumeCount; j++) {
						VOLUME_INFO* v = &volumes.Volumes[j];
						if (j == volumes.VolumeCount - 1) {
							wprintf(L"\033[1;32m");
							wprintf(L" %c%c%c%c [+] Found #%d volume(s)\n", 192, 196, 196, 16, j + 1);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
							wprintf(L"       %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
							wprintf(L"       %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
							wprintf(L"       %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Label", v->Label);
							wprintf(L"       %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File System", v->FileSystem);
							fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
							wprintf(L"       %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
							fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
							wprintf(L"       %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
							wprintf(L"       %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
							if (v->StartOffsetInByte != 0) {
								wprintf(L"       %c%c%c%c %-26s : %lld\n\n", 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
							}
							else {
								wprintf(L"\n");
							}
							wprintf(L"\033[0m");
						}
						else {
							wprintf(L"\033[1;32m");
							wprintf(L" %c%c%c%c [+] Found #%d volume(s)\n", 192, 196, 196, 16, j + 1);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
							wprintf(L" %c     %c%c%c%c %-26s : %s\n", 179, 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
							wprintf(L" %c     %c%c%c%c %-26s : %s\n", 179, 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
							wprintf(L" %c     %c%c%c%c %-26s : %s\n", 179, 192, 196, 196, 16, L"Label", v->Label);
							wprintf(L" %c     %c%c%c%c %-26s : %s\n", 179, 192, 196, 196, 16, L"File System", v->FileSystem);
							fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
							wprintf(L" %c     %c%c%c%c %-26s : %s (%lld Bytes)\n", 179, 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
							fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
							wprintf(L" %c     %c%c%c%c %-26s : %s (%lld Bytes)\n", 179, 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
							wprintf(L" %c     %c%c%c%c %-26s : %lu\n", 179, 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
							if (v->StartOffsetInByte != 0) {
								wprintf(L" %c     %c%c%c%c %-26s : %lld\n\n", 179, 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
							}
							else {
								wprintf(L"\n");
							}
							wprintf(L"\033[0m");
						}
					}
					fFreeVolumeList(&volumes);
				}
			}
			fFreeDiskList(&physicalDisks);
		}

		if (args.volumeExtentArg != NULL) {
			if (fIsSingleAlphabet(args.volumeExtentArg)) {
				WCHAR volumePath[10];
				swprintf_s(volumePath, _countof(volumePath), L"\\\\.\\%c:", args.volumeExtentArg[0]);

				VOLUME_EXTENTS_LIST res = fGetVolumeExtents(volumePath);
				if (!res.Success || res.Extents == NULL || res.ExtentCount == 0) {
					wprintf(L"\033[1;31m[-] Cannot get extents on volume %s!\033[0m\n", volumePath);
					fFreeVolumeExtentsList(&res);
					return 1;
				}

				wprintf(L"[INFO] Enumerating extents on volume %s\n", volumePath);
				wprintf(L"\033[1;32m");

				for (DWORD i = 0; i < res.ExtentCount; i++) {
					const VOLUME_EXTENT_INFO* e = &res.Extents[i];

					if (i == res.ExtentCount - 1) {
						wprintf(L" %c%c%c%c [+] Found #%d Extent(s)\n", 192, 196, 196, 16, i + 1);
						wprintf(L"       %c%c%c%c %-35s : %lu (\\\\.\\PhysicalDrive%d)\n", 192, 196, 196, 16, L"Disk Number", e->DiskNumber, e->DiskNumber);
						wprintf(L"       %c%c%c%c %-35s : %lld\n", 192, 196, 196, 16, L"Starting Offset In Byte", e->StartingOffset);
						wprintf(L"       %c%c%c%c %-35s : %lld\n", 192, 196, 196, 16, L"Extent Length In Byte", e->ExtentLength);
					}
					else {
						wprintf(L" %c%c%c%c [+] Found #%d Extent(s)\n", 192, 196, 196, 16, i + 1);
						wprintf(L" %c     %c%c%c%c %-35s : %lu (\\\\.\\PhysicalDrive%d)\n", 179, 192, 196, 196, 16, L"Disk Number", e->DiskNumber, e->DiskNumber);
						wprintf(L" %c     %c%c%c%c %-35s : %lld\n", 179, 192, 196, 196, 16, L"Starting Offset In Byte", e->StartingOffset);
						wprintf(L" %c     %c%c%c%c %-35s : %lld\n", 179, 192, 196, 196, 16, L"Extent Length In Byte", e->ExtentLength);
					}
				}
				wprintf(L"\033[0m");

				fFreeVolumeExtentsList(&res);
			}
		}

		if (args.volumeSimpleArg) {
			WCHAR formattedSize[64];
			VOLUME_LIST list = fGetSimpleVolumeList();
			if (!list.Success || list.Volumes == NULL || list.VolumeCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get simple volumes on this machine!\033[0m\n");
				fFreeVolumeList(&list);
				return 1;
			}

			if (list.Volumes != NULL && list.VolumeCount > 0) {
				wprintf(L"\033[1;32m");
				for (DWORD i = 0; i < list.VolumeCount; i++) {
					VOLUME_INFO* v = &list.Volumes[i];
					wprintf(L"[+] Found #%d Simple Volume(s)\n", i + 1);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Label", v->Label);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File System", v->FileSystem);
					fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
					fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
					if (v->StartOffsetInByte != 0) {
						wprintf(L" %c%c%c%c %-26s : %lld\n\n", 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
					}
					else {
						wprintf(L"\n");
					}
				}
				wprintf(L"\033[0m");
			}
			fFreeVolumeList(&list);
		}

		if (args.volumeSpannedArg) {
			WCHAR formattedSize[64];
			VOLUME_LIST list = fGetSpannedVolumeList();
			if (!list.Success || list.Volumes == NULL || list.VolumeCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get spanned volumes on this machine!\033[0m\n");
				fFreeVolumeList(&list);
				return 1;
			}

			if (list.Volumes != NULL && list.VolumeCount > 0) {
				wprintf(L"\033[1;32m");
				for (DWORD i = 0; i < list.VolumeCount; i++) {
					VOLUME_INFO* v = &list.Volumes[i];
					wprintf(L"[+] Found #%d Spanned Volume(s)\n", i + 1);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Label", v->Label);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File System", v->FileSystem);
					fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
					fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
					if (v->StartOffsetInByte != 0) {
						wprintf(L" %c%c%c%c %-26s : %lld\n\n", 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
					}
					else {
						wprintf(L"\n");
					}
				}
				wprintf(L"\033[0m");
			}
			fFreeVolumeList(&list);
		}

		if (args.volumeMirroredArg) {
			WCHAR formattedSize[64];
			VOLUME_LIST list = fGetMirroredVolumeList();
			if (!list.Success || list.Volumes == NULL || list.VolumeCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get mirrored volumes on this machine!\033[0m\n");
				fFreeVolumeList(&list);
				return 1;
			}

			if (list.Volumes != NULL && list.VolumeCount > 0) {
				wprintf(L"\033[1;32m");
				for (DWORD i = 0; i < list.VolumeCount; i++) {
					VOLUME_INFO* v = &list.Volumes[i];
					wprintf(L"[+] Found #%d Mirrored Volume(s)\n", i + 1);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Label", v->Label);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File System", v->FileSystem);
					fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
					fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
					if (v->StartOffsetInByte != 0) {
						wprintf(L" %c%c%c%c %-26s : %lld\n\n", 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
					}
					else {
						wprintf(L"\n");
					}
				}
				wprintf(L"\033[0m");
			}
			fFreeVolumeList(&list);
		}

		if (args.volumeStripedArg) {
			WCHAR formattedSize[64];
			VOLUME_LIST list = fGetStripedVolumeList();
			if (!list.Success || list.Volumes == NULL || list.VolumeCount == 0) {
				wprintf(L"\033[1;31m[-] Cannot get striped volumes on this machine!\033[0m\n");
				fFreeVolumeList(&list);
				return 1;
			}

			if (list.Volumes != NULL && list.VolumeCount > 0) {
				wprintf(L"\033[1;32m");
				for (DWORD i = 0; i < list.VolumeCount; i++) {
					VOLUME_INFO* v = &list.Volumes[i];
					wprintf(L"[+] Found #%d Striped Volume(s)\n", i + 1);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Disk Number", v->DiskNumber);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Volume GUID", v->VolumeGuid);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Drive Letter", v->DriveLetter);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"Label", v->Label);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File System", v->FileSystem);
					fFormatBytes(v->SizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Total Size", formattedSize, v->SizeInByte);
					fFormatBytes(v->FreeSizeInByte, formattedSize, _countof(formattedSize));
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Free Size", formattedSize, v->FreeSizeInByte);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Sector", v->BytesPerSector);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Sectors Per Cluster", v->SectorsPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Bytes Per Cluster", v->BytesPerCluster);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Total Clusters", v->TotalClusters);
					wprintf(L" %c%c%c%c %-26s : %lu\n", 192, 196, 196, 16, L"Free Clusters", v->FreeClusters);
					if (v->StartOffsetInByte != 0) {
						wprintf(L" %c%c%c%c %-26s : %lld\n\n", 192, 196, 196, 16, L"Start Offset in byte", v->StartOffsetInByte);
					}
					else {
						wprintf(L"\n");
					}
				}
				wprintf(L"\033[0m");
			}
			fFreeVolumeList(&list);
		}

		if (args.slackArg != NULL) {
			wprintf(L"\033[1;31mWARNING: We cannot raw write data to the system drive (C: drive). Therefore, all slack space of files which ");
			wprintf(L"are within C: drive are for reference only.\n\033[0m");

			if (fIsNtfsVolume(args.slackArg[0])) {
				WCHAR formattedSize[64];
				FILE_LIST fileList = fGetFileInfoList(args.slackArg);
				for (DWORD i = 0; i < fileList.FileCount; i++) {
					LONGLONG startLcn = 0;
					DWORD numberOfAllocatedClusters = 0;
					const FILE_INFO* f = &fileList.Files[i];
					wprintf(L"\033[1;32m");
					wprintf(L"[+] Found #%d file(s)\n", i + 1);
					wprintf(L" %c%c%c%c %-26s : %s\n", 192, 196, 196, 16, L"File Name", f->FileName);
					fFormatBytes(f->FileSize, formattedSize, 64);
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Size", formattedSize, f->FileSize);
					fFormatBytes(f->AllocatedSize, formattedSize, 64);
					wprintf(L" %c%c%c%c %-26s : %s (%lld Bytes)\n", 192, 196, 196, 16, L"Allocated Size", formattedSize, f->AllocatedSize);
					FILE_EXTENTS_LIST fExt = fGetFileExtents(f->FileName);
					for (DWORD j = 0; j < fExt.ExtentCount; j++) {
						const FILE_EXTENT_INFO* fe = &fExt.Extents[j];
						wprintf(L" %c\t %c%c%c%c VCN: 0x%llx ~ Clusters: 0x%llx(%lld) ~ LCN: 0x%llx(%lld)\n", 179, 192, 196, 196, 16, fe->Vcn, fe->ClusterCount, fe->ClusterCount, fe->StartLcn, fe->StartLcn);

						if (j == fExt.ExtentCount - 1) {
							startLcn = fe->StartLcn;
							numberOfAllocatedClusters = fe->ClusterCount;
						}
					}
					LONGLONG fileSize = f->FileSize;
					SLACK_POINT injectableSectors = fGetSlackSectors(numberOfAllocatedClusters, startLcn, fileSize, args.slackArg[0]);

					if (injectableSectors.NumberOfslackSectors > 0 && f->FileSize > 1024) {
						wprintf(L"\033[1;33m");
						wprintf(L" %c%c%c%c %-26s : %d\n", 192, 196, 196, 16, L"Available Slack Sector(s)", injectableSectors.NumberOfslackSectors);
						wprintf(L"\t %c%c%c%c%s \\\\.\\%c: ==> at %lld (Offset In Sector)\n", 192, 196, 196, 16, L"Injectable Point On", args.slackArg[0], injectableSectors.PhysicalOffset);
						wprintf(L"\033[0m");
					}
					fFreeFileExtentsList(&fExt);
				}
				wprintf(L"\033[0m");
				fFreeFileInfoList(&fileList);
			}
			else {
				wprintf(L"\033[1;31m[-] Volume %c: is not NTFS. QUITTING!\n\033[0m", args.slackArg[0]);
			}
		}
	}

	return 0;
}