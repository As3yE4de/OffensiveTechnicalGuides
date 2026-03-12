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

<img width="1003" height="436" alt="image" src="https://github.com/user-attachments/assets/a145e3c2-c7dc-4f68-93f2-6211d10dc060" />

At the `file1.txt`, the *Size* information is the real size of this file which is 3926 bytes and the *Size on disk* information is the allocated size which is a cluster (4096 bytes). Similarly for the `file2.txt`, the real size is 4832 bytes and the number of allocated clusters is 2 (8192 bytes). So, where is the file slack space? Simply put, we just need to use the byte-allocated clusters minus the actual byte size of the file.
- Slack space for `file1.txt`: 4096 - 3926 = 170 bytes
- Slack space for `file2.txt`: 8192 - 4832 = 3360 bytes

j

## Procedure
## Tested environment
- Windows 10 x64
## References
[1] Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

