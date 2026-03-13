# Hide data inside file slack space - Technical concept
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

The reason we need to understand the clustering structure within a file is because this is how attackers find writable slack space. As we have known, the file slack space begins at the last cluster of a file, and the last cluster is in the last extent of the file. Therefore, to write data (hide data) in the slack space, we need to find the sector offset of the last LCN (last cluster) in the last extent and then, calculate the sector offset of the first sector in slack space. Let's briefly discuss sectors and why we need sector offsets.<br>
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Images/test.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/a78722cfec2cbb7dc6a75f92bb6288863a5d9527/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Images/test.png)

At the physical level, the smallest unit of a physical disk is a sector and all volumes within the physical disk will use this sector size. A sector is usually 512 bytes, and we can use the following command to check the size of a sector:
```
C:\> wmic diskdrive get BytesPerSector, DeviceID
```
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Images/Screenshot 2026-03-13 104514.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/5f07edeeda396a04c404f68eb5551cb138faefb2/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Images/Screenshot%202026-03-13%20104514.png)

The DeviceID `\\.\PHYSICALDRIVE0`, `\\.\PHYSICALDRIVE1`, `\\.\PHYSICALDRIVE2` in the above image are Disk 0, Disk 1, and Disk 2, respectively. Because the smallest unit of a physical disk is a sector, when we want to write raw data to a physical disk, we must write it in sectors and therefore, we must use the sector offset. Simply put, there is no way to write data directly to file slack space through OS's file system, we have to use raw writting method to access volume directly (using `\\.\<DRIVE_LETTER>`) without going through the file system (bypass the file system).
## Procedure
Let's assume we want to hide `secret.txt` in file slack spaces of `file1.txt` and `file2.txt`.
<img width="827" height="328" alt="image" src="https://github.com/user-attachments/assets/3dca7d2d-f67e-47df-b647-b7918c9a0018" />

The first informations we need to gather are bytes per sector and bytes per cluster of volume E:, so we use the following command:
```
C:\> fsutil fsinfo ntfsinfo E:
```
<img width="883" height="370" alt="image" src="https://github.com/user-attachments/assets/5ca605e5-c5f4-403c-b296-34b1cca995a1" />

We have:
- Bytes per sector: 512 bytes
- Bytes per cluster: 4096 bytes
- The number of sector in a cluster: 4096 / 512 = 8 sectors

Let's view the LCNs in `file1.txt` and `file2.txt` by using command:
```
C:\> fsutil file queryExtents E:\file1.txt
C:\> fsutil file queryExtents E:\file2.txt
```
<img width="767" height="102" alt="image" src="https://github.com/user-attachments/assets/8093401f-c6cc-4a4a-b5bb-4acaaf0c2a3f" />

The start LCN of `file1.txt ` is 0x25 (37) and the start LCN of `file2.txt` is 0x58f (1423). Now we need the sector offset of these LCNs by multiplying them by the number of sector in a cluster which is 8:
- `file1.txt`: 37 * 8 = 296
- `file2.txt`: 1423 * 8 = 11384

We should double-check before proceeding further by using command:
```
C:\> dd if=\\.\E: bs=512 skip=37 count=1 | powershell -Command "$input | Format-Hex"
C:\> dd if=\\.\E: bs=512 skip=1423 count=1 | powershell -Command "$input | Format-Hex"
```
<img width="1160" height="753" alt="image" src="https://github.com/user-attachments/assets/d8678b74-669a-4272-a460-6673d15f15ca" />

<img width="1108" height="741" alt="image" src="https://github.com/user-attachments/assets/f489da4a-fb47-4710-b02e-64d72de45d3c" />

Then, we have to find the last LCN of those files, as we have seen, `file1.txt` only has 1 extent that means this extent is also the last extent and the number of allocated clusters for this file is only one that means the start LCN is also the last LCN. Look at `file2.txt`, similar to `file1.txt`, it has 1 extent and the number of allocated clusters is 2, to find the sector offset of the last LCN (the second cluster) of `file2.txt`, we use `(1423 + 2 - 1) * 8` = `11392`. So, we have the following information:
- The sector offset of last LCN in `file1.txt`: 296
- The sector offset of last LCN in `file2.txt`: 11392

Next, we have to find the number of used sectors and the number of free sectors in the last LCN of those files by using the following formulas:
- `file1.txt`:
  - (ceil)Used sectors = (Real size of file % bytes per cluster) / bytes per sector = (3926 % 4096) / 512 = 7.66796875 = 8 sectors
  - Free sectors = The number of sector in a cluster - Used sectors = 8 - 8 = 0 sector
- `file2.txt`:
  - (ceil)Used sectors = (Real size of file % bytes per cluster) / bytes per sector = (4832 % 4096) / 512 = 1.4375 = 2 sectors
  - Free sectors = The number of sector in a cluster - Used sectors = 8 - 2 = 6 sectors

Let's confirm our calculation with `dd` tool and use the skip values `303` for `file1.txt` and `11393` for `file2.txt`:
```
C:\> dd if=\\.\E: bs=512 skip=303 count=1 | powershell -Command "$input | Format-Hex"
C:\> dd if=\\.\E: bs=512 skip=11393 count=1 | powershell -Command "$input | Format-Hex"
```
<img width="1112" height="696" alt="image" src="https://github.com/user-attachments/assets/b511d6c0-0dab-4ae0-b0eb-eb519df11e89" />

<img width="1108" height="640" alt="image" src="https://github.com/user-attachments/assets/b971fdac-04ab-427f-a75f-d9b83d422262" />

From the above results, we are only able to hide data inside the slack space of `file2.txt`:
- The sector offset of the first sector in the slack is `11394`.
- The number of sectors we can hide is 6 sectors.

Now, let's hide our `secret.txt` which is 41 bytes (1 sector) in `file2.txt`'s slack space, by using command:
```
C:\> dd if=C:\testSlackSpace\secret.txt of=\\.\E: bs=512 seek=11394 count=1
```
<img width="1122" height="731" alt="image" src="https://github.com/user-attachments/assets/e9d9f55e-d3fd-4735-ae10-11918e58b73a" />




## Tested environment
- Windows 10 x64
## References
[1] Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

[2] File System - Cluster (File | Sector Cluster) - https://www.datacadamia.com/file/cluster

[3] Clusters and Extents - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/clusters-and-extents

[4] Naming Files, Paths, and Namespaces - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
