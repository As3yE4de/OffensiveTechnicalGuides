# Hide data inside hidden volume on Windows - Procedure #2
In this procedure, we will hide data inside a hidden volume by changing partition type/ID. Let's assume we want hide `secret.txt` file which is in volume E:. First, we need to get the volume GUID of volume E:
```
C:\> mountvol
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 191945.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/86c05fe8ea5884a7d39c3573e5fc349c9eb72118/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20191945.png)

Now, let's check whether this volume is MBR or GPT by using command:
```
C:\> powershell -Command "Get-Partition | Select-Object PartitionNumber, DriveLetter, MbrType, GptType"
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 190546.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/65315fc03919fb4c67852d3ab1e60a83c22713e9/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20190546.png)

The volume E: is GPT, also we detect a volume F: is MBR, we will try to hide data inside a MBR partition later. Let's have a look at partitions in volume E first:
```
C:\> diskpart
DISKPART> select volume E
DISKPART> detail partition
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 191520.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/766460ff53bceac9c30a2bee070fcd750d26afdb/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20191520.png)

As we have seen, the value of the `Type` in this partition indicates that this is a GPT partition, the `Hidden` entry has a value `No` which means this partition is visible to users, also, the `Attrib` entry is all zeroes which means there is no special attributes asigned to it yet. To make this partition hidden, we have to change the value of `Attrib` by using the following command (in DISKPART prompt):
```
DISKPART> gpt attributes=0x4000000000000000
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 200343.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/ebf8a76d9357930cd5b627138f26ad75e458e818/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20200343.png)

After running the command above, drive E will no longer appear in File Explorer. However, if we try to access files using the volume GUID, the operation will fail because when a GPT partition is assigned the hidden attribute, the operating system will not create a GUID volume for that partition. But there is one more "tunel", NT namespace, using prefix `\\?\GLOBALROOT`. Let's change the value of `Attrib` back to normal and run the following command to get the NT namespace of volume E:
```
C:\> fltmc volumes
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 201942.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/eb894da721b97f6288d461a7de4e0154fca5b25a/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20201942.png)

Ok now, let's change this volume to hidden. After changing, we are able to access this volume by creating a symbolic link to directory 
`\\?\GLOBALROOT\Device\HarddiskVolume4\` and read our `secret.txt` normally.
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 202423.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/7f74866c1e1819b8f0f9f71d3c721bca719e4f9f/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20202423.png)

What about the volume F: which is MBR partition? To change volume F: to hidden, we use the following command:
```
C:\> diskpart
DISKPART> select volume F
DISKPART> detail partition
DISKPART> set id=17
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 202759.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/0055b9b0db103658e5f81fcf6c881b67c7a24647/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20202759.png)



# Tested environment
- Windows 10 x64

---
