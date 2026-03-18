# Hide data inside unallocated partition - Technical concepts
An unallocated partition refers to a portion of a storage device (such as a hard disk drive or solid-state drive) that is not assigned to any partition or volume. 
This space is not formatted with a file system and is therefore unusable by the operating system for data storage until it is allocated and formatted. Unallocated 
partitions can be easily identified through Disk Management, as shown in the image below.
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 174548.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/c9f87ea192c3d2a22cfa5493733a5ff8d0787e4f/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20174548.png)

Because unallocated partitions are unusable by the operating system, they will not appear in File Explorer or `diskpart` command and therefore, attackers can exploit this to hide data inside these unallocated partitions.

Look at the previous image, on a physical disk, partitions are arranged consecutively, and each partition has its own size and starting offset (in bytes). We can quickly gather information about the size and start offset of each partition on a specific disk, also the unallocated (free) size on each disks by using the following command:
```
C:\> diskpart
DISKPART> select disk 0
DISKPART> list partition
DISKPART> list disk
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 182801.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/de31c8e606905bd2776459bdeb6407efa2b20ed4/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20182801.png)

As we know, unallocated partitions are not displayed in File Explorer and the `diskpart` command, but we can detect them based on the size and start offset of the partitions. Taking the image above as an example, if we take the start offset (in bytes) of Partition 3 plus the size of Partition 3 (in bytes), we will have the start offset (in bytes) of the next partition. Since the partitions are arranged consecutively, if the calculated result is smaller than the start offset of the next partition, it is highly likely that we have found an unallocated partition.

Assuming an attacker has discovered an unallocated partition, the only thing they need to do is hide the data inside it. The operating system cannot recognize unallocated partitions because they are not assigned any drive letters; therefore, the only way an attacker can access these partitions is through the physical disk's Win32 device namespace (usually in the form `\\.\PhysicalDriveX`). And because we will be accessing the physical disk directly, from an attack perspective, we must find the final piece of the puzzle – the sector offset of the unallocated partition. Simply put, just divide the starting offset (in bytes) of the unallocated partition by the number of bytes per sector (usually 512 bytes) and we will get the sector offset.

# References
[1] What Is Unallocated Partition and How to Fix It Readily - https://www.m3datarecovery.com/wiki/unallocated-partition.html

[2] Naming Files, Paths, and Namespaces - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file

---
