# Hide data inside VHD on Windows - Procedure #1
In this procedure, we are going to hide our data inside a VHD file. First, to create a virtual disk, we use the following command to create a VHD with the maximum 
size is 50 MB:
```
C:\> diskpart
DISKPART> create vdisk file="C:\testVHD\MyVhd.vhd" maximum=50 type=fixed
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside VHD on Windows/Screenshots/Screenshot 2026-03-20 124132.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/dbdbb223b36c90b5ab46ca57abf2d2a5c3f83487/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20VHD%20on%20Windows/Screenshots/Screenshot%202026-03-20%20124132.png)

Next, we attach this VHD file and create a primary partition for it:
```
DISKPART> select vdisk file="C:\testVhd\MyVhd.vhd"
DISKPART> attach vdisk
DISKPART> create partition primary
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside VHD on Windows/Screenshots/Screenshot 2026-03-20 124550.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/93f146c879153a69cf2867382a0feb7522116005/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20VHD%20on%20Windows/Screenshots/Screenshot%202026-03-20%20124550.png)

Next, we start formatting this partition with a file system NTFS, label "MySecret" and the drive letter will be Z:
```
DISKPART> format fs=ntfs label="MySecret" quick
DISKPART> assign letter Z
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside VHD on Windows/Screenshots/Screenshot 2026-03-20 124952.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/5ec6681bd99888f6c03e5825cfc1c37501a2a32b/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20VHD%20on%20Windows/Screenshots/Screenshot%202026-03-20%20124952.png)

At this time, we have our own file system, we can use it as normal, hide our data inside this VHD. After hiding data, we only need to detach this VHD by using the 
following command:
```
DISKPART> select vdisk file="C:\testVhd\MyVhd.vhd"
DISKPART> detach vdisk
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside VHD on Windows/Screenshots/Screenshot 2026-03-20 125350.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/b6a41029d547a4812c2e6c594ea741874e6f86e4/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20VHD%20on%20Windows/Screenshots/Screenshot%202026-03-20%20125350.png)

As our VHD is stored at `C:\testVhd` directory, we can use another technique to hide our `MyVhd.vhd` but we don't discuss about this in this procedure.

# Tested environment
- Windows 10 x64

---
