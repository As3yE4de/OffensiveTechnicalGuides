# Hide data inside VHD on Windows - Technical concepts
A VHD (Virtual Hard Disk) is a disk image that allows encapsulation of the hard disk into an individual file for use by the operating system as a virtual disk 
in all the same ways physical hard disks are used. These virtual disks are capable of hosting native file systems (NTFS, FAT, exFAT, and UDFS) while supporting 
standard disk and file operations. There are three types of Virtual Hard Disk and they have a minimum size of 3 MB:
- **Fixed**: The VHD image file is pre-allocated on the actual hard disk for the maximum size requested.
- **Expandable**: Also known as "dynamic", "dynamically expandable", and "sparse", the VHD image file uses only as much space on the actual hard disk as needed
to store the actual data the virtual disk currently contains. The maximum size of a dynamic virtual disk is 2,040 GB (2 TB).
- **Differencing**: This type uses a "Parent-Child" relationship. Simply put, it's just like a clean snapshot, when we create a first VHD, for example `parent.vhd`,
using type **Fixed** or **Expandable**, we want to restore the current virtual drive to its original state in the event of a failure, in this case, we need to create
another child VHD (also called Differencing VHD), for example `child.vhd`, any data write operations on `child.vhd` will not affect the `parent.vhd` and therefore, we
can able to recovery quickly or we can also create more VHDs based on the parent. The maximum size of a differencing virtual disk is 2,040 GB.

A VHD file is sometimes referred to as a Virtual Machine, but essentially, it remains a standalone file existing on the physical machine. With the simple 
description of a VHD above, we can easily imagine how an attacker could hide data inside a VHD. The attacker simply needs to create a virtual hard drive, mount it, 
create a partition for the VHD, format it with a specific file system, and then use it as normal. After successfully hiding the data, the attacker simply needs 
to detach the VHD.

# References
[1] About VHD - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/vstor/about-vhd

---
