# Hide data inside hidden volume on Windows - Procedure #1
In this procedure, we will hide data inside a hidden volume by removing drive letter. Let's assume we want to make NTFS E: volume hidden, first, we use `mountvol` 
to get the Volume GUID of this drive:
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 122711.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/dd50f99606a428736941b83bac5a73c404de4678/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20122711.png)

Now, we run `diskpart` command like the following:
```
C:\> diskpart
DISKPART> list volume
DISKPART> select volume E
DISKPART> remove
DISKPART> exit
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 123535.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/483e616b149609d4ea4f166f42b76b9b7d155f72/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20123535.png)

If we open File Explorer, we will no longer see drive E. As we have gather the volume GUID of E: volume before, we can easily access this file system to read 
`secret.txt` by using the following command:
```
C:\> type \\?\Volume{GUID}\secret.txt
```
![TA0005 - Defense Evasion/T1564 - Hide Artifacts/Hide data inside hidden volume on Windows/Screenshots/Screenshot 2026-03-17 123933.png](
https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/a555c7edf31767084880abbc572329cc72fbd518/TA0005%20-%20Defense%20Evasion/T1564%20-%20Hide%20Artifacts/Hide%20data%20inside%20hidden%20volume%20on%20Windows/Screenshots/Screenshot%202026-03-17%20123933.png)

# Tested environment
- Windows 10 x64

---
