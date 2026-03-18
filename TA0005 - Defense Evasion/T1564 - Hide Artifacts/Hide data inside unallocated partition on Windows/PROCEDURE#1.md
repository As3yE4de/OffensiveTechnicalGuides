# Hide data inside unallocated partition on Windows - Procedure #1
Let's assume we want to hide our malicious binary `calc.exe` inside unallocated partitions. First, let's check whether there is any unallocated partitions on disks 
by using the following command:
```
C:\> diskpart
DISKPART> list disk
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 191213.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/1840a1f47f3562a532c634d9042161170046ad09/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20191213.png)

As a result, there are 301 MB on `Disk 0` which are free. Now, we start gathering information about the size and start offset of each partition on `Disk 0` by using the 
following command:
```
C:\> powershell -Command "Get-Partition -DiskNumber 0 | Select-Object PartitionNumber, DriveLetter, Size, Offset"
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 193831.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/d1554d105b829e99729874c6cd3b359bac475c88/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20193831.png)

After calculating and comparing the start offset of each partition on this physical disk using the formulas: *StartOffset<sub>N</sub> + Size<sub>N</sub> = 
StartOffset<sub>N+1</sub>*, we detected a gap between Partition 3 and Partition 4 whose size is 315,465,216 bytes (300.85107421875 MB):
- `Partition 3`:
  - Start Offset of Partition 3 (in bytes): 122683392
  - Start Offset of the next Partition (in bytes): 122683392 + 63226143232 = 63348826624
- `Partition 4`:
  - Start Offset of Partition 4 (in bytes): 63664291840

This gap is definitely the unallocated partition we're looking for. Before hide data inside this gap, there are two more information we need to gather. 
Let's begin with the number of bytes per sector on this physical disk, we use the following command:
```
C:\> wmic diskdrive get BytesPerSector, DeviceID
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 195647.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/6bf061d8d2cc4ee0317a92c4a1b73471f8e61719/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20195647.png)

Next, let's calculate the sector offset of the unallocated partition.
- `Unallocated Partition`:
  - Start sector offset (in sectors): 63348826624 / 512 = 123728177
  - Unallocated sectors: 315465216 / 512 = 616143 sectors

Now that we have all the necessary information, the next step is to double-check to ensure we're in the correct unallocated partition. First, let's double-check the start 
sector of this unallocated partition by using command:
```
C:\> dd if=\\.\PhysicalDrive0 bs=512 skip=123728177 count=1 | powershell -Command "$input | Format-Hex"
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 201317.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/bb1c9c990291455be4eb21a3847d8a199f424de0/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20201317.png)

All zeroes. Let's double-check the previous sector to make sure it is the end of Partition 3.
```
C:\> dd if=\\.\PhysicalDrive0 bs=512 skip=123728176 count=1 | powershell -Command "$input | Format-Hex"
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 201528.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/83ab287f40fccdc3d6194efa90b6c2712601b98b/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20201528.png)

Perfect! Now, as our malicious binary `calc.exe` is 27648 bytes (54 sectors). We use the following command to hide it inside this partition:
```
C:\> dd if=C:\testUnallocatedPartition\calc.exe of=\\.\PhysicalDrive0 bs=512 seek=123728177 count=54
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside unallocated partition on Windows/Screenshots/Screenshot 2026-03-18 202013.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/5ec083d3cbf811b5ada3408f7d915d574891324d/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20unallocated%20partition%20on%20Windows/Screenshots/Screenshot%202026-03-18%20202013.png)

# Tested environment
- Windows 10 x64

---
