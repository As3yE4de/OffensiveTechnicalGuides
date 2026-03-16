# Hide data inside hidden volume on Windows - Technical concepts
A volume, also known as a drive, is a collection of addressable sectors that an Operating System (OS) or application can use for data storage. The sectors in a volume need not be consecutive on a physical storage device; instead, they need to only give the impression that they are, users simply need to open File Explorer and store their data in a volume, they don't need to know the structure of the volume at physical level. Taking advantage of this, the attacker only needs to find a way to make a volume hidden from the view of the average user but still be able to bypass the OS's file system.




# References
[1] File System Forensic Analysis - Brian Carrier

[2]  Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

---
