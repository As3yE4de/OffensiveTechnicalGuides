# Hide data inside file slack space - Method #1
## Description
Hiding data in file slack space is a technique that exploits the Windows operating system's ability to allocate disk space for a file. Logically, when we create a file on a volume, the smallest unit that the file system can allocate is a cluster, not a byte. On Windows 10, the cluster size on NTFS volume is typically 4096 bytes, but depending on the file system being used on the specific volume, the size of a cluster can vary. We can also be able to determine the size of a cluster on a specific volume by using command (e.g. NTFS C: volume):
```
C:\> fsutil fsinfo ntfsinfo C:
```
<img width="887" height="377" alt="image" src="https://github.com/user-attachments/assets/673cad6e-ebf2-4e4c-bd2e-003d4c6fbea9" />

But the `fsutil` command only supports NTFS and REFS file systems, in order to determine the size of a cluster of the older file system (e.g. FAT, exFAT), we can use the following command:
```
C:\> wmic volume where "DriveLetter='F:'" get BlockSize, FileSystem, Label
```
<img width="1043" height="57" alt="image" src="https://github.com/user-attachments/assets/098a5a38-2b54-4281-ad21-89b8cc365c86" />

As we mentioned earlier, the smallest unit a file system can allocate for a file is a cluster. This means that if we create a file smaller than 4096 bytes (4KB), a cluster will still be allocated for that file, and if we create a file larger than 4096 bytes, for example, a 4097-byte file, two clusters will be allocated for this file. Look at the below images:

<img width="987" height="427" alt="image" src="https://github.com/user-attachments/assets/a270e6bb-3921-4e16-af4e-f46ea589b788" />

At the `file1.txt`, the *Size* information is the real size of this file which is 3926 bytes and the *Size on disk* information is the allocated size which is a cluster (4096 bytes). Similarly for the `file2.txt`, the real size is 4832 bytes and the number of allocated clusters is 2 (8192 bytes). So, where is the file slack space? Simply put, we just need to use the byte-allocated clusters minus the actual byte size of the file.
- Slack space for `file1.txt`: 4096 - 3926 = 170 bytes
- Slack space for `file2.txt`: 8192 - 4832 = 3360 bytes

But before we proceed to hide data in file slack space, let's look at how clusters are arranged within a file by using the following command:
```
C:\> fsutil file queryExtents <FILE_PATH>
```
<img width="750" height="101" alt="image" src="https://github.com/user-attachments/assets/ccc6835a-f194-4dbd-9683-71f6e4080595" />

- VCN (Virtual Cluster Number): Any cluster in a file has a VCN, which is its relative offset from the beginning of the file.
- Clusters: The number of allocated clusters within an extent.
- LCN (Logical Cluster Number): A LCN describes the offset of a cluster from some arbitrary point within the volume.

In our case, both files `file1.txt` and `file2.txt` have only one extent (an extent is a run of contiguous clusters). If a file is large enough, it may be fragmented and its extents will be more than one, like the below image:<br>
<img width="953" height="128" alt="image" src="https://github.com/user-attachments/assets/651c59b1-30a7-4e34-8fdc-3aa095a170cd" />

The reason we need to understand the clustering structure within a file is because this is how attackers find writable slack space. As we have known, the file slack space begins at the last cluster of a file, and the last cluster is in the last extent of the file. Therefore, to write data (hide data) in the slack space, we need to find the real sector offset (not relative sector offset) of the last LCN (last cluster) in the last extent and then, calculate the real sector offset of the first sector in slack space. Let's briefly discuss sectors and why we need sector offsets.<br>
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Images/test.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/a78722cfec2cbb7dc6a75f92bb6288863a5d9527/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Images/test.png)

At the physical level, the smallest unit of a physical disk is a sector and all volumes within the physical disk will use this sector size. A sector is usually 512 bytes, and we can use the following command to check the size of a sector:
```
C:\> wmic diskdrive get BytesPerSector, DeviceID
```
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Images/Screenshot 2026-03-13 104514.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/5f07edeeda396a04c404f68eb5551cb138faefb2/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Images/Screenshot%202026-03-13%20104514.png)

The DeviceID `\\.\PHYSICALDRIVE0`, `\\.\PHYSICALDRIVE1`, `\\.\PHYSICALDRIVE2` are Disk 0, Disk 1, and Disk 2, respectively. These DeviceIDs are the entries for us to write data directly to the slack space. Because the smallest unit of a physical disk is a sector, when we want to write raw data to a physical disk, we must write it in sectors and therefore, we must use the real sector offset. Simply put, there is no way to write data directly to file slack space, we have to use raw writting method to bypass the file system.

## Procedure
## Tested environment
- Windows 10 x64
## References
[1] Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

[2] File System - Cluster (File | Sector Cluster) - https://www.datacadamia.com/file/cluster

[3] Clusters and Extents - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/clusters-and-extents
