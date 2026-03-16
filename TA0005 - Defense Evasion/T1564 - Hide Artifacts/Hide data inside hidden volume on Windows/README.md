# Hide data inside hidden volume on Windows - Technical concepts
A volume, also known as a drive, is a collection of addressable sectors that an Operating System (OS) or application can use for data storage. The sectors in a volume need not be consecutive on a physical storage device; instead, they need to only give the impression that they are, users simply need to open File Explorer and store their data in a volume, they don't need to know the structure of the volume at physical level. Taking advantage of this, the attacker only needs to find a way to make a volume hidden from the view of the average user but still be able to bypass the OS's file system.

When we create a volume (maybe via Disk Management) and assign a letter to it, we actually created a mount point for an individual file system. Simply put, a mount point is a gateway (an entry point) that allows us to access directories or files within a file system. Even if we don't assign a letter for a volume (no mount point created), we are still able to access that volume via Volume GUID path, it's like we're hiding the gateway and crawling into the file system through a tunnel. Volume GUID paths are strings of form `\\?\Volume{GUID}\` where GUID is a globally unique identifier (GUID) that identifies the volume.



# References
[1] File System Forensic Analysis - Brian Carrier

[2]  Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

[3] https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-volume

---
