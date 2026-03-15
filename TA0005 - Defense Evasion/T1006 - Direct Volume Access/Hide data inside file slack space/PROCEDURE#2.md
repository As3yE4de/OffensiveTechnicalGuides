# Hide data inside file slack space - Procedure #2
In this procedure, we will use `DiskEnum` tool to automate the process of finding file slack space. Let's assume we want find the slack space of `test1.txt` and 
`test2.txt` files on E: volume.
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Screenshots/Screenshot 2026-03-15 182945.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/698059ecc7afcb26c4e226cffcd9a0217c25042a/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Screenshots/Screenshot%202026-03-15%20182945.png)

Next, use `DiskEnum` tool with the following command:
```
C:\> .\DiskEnum.exe --slack=E:\
```
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Screenshots/Screenshot 2026-03-15 183235.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/7c90d942484712ee762199b0b89ced4c3f223be7/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Screenshots/Screenshot%202026-03-15%20183235.png)

According to the above result, we have total 8 free sectors (3 free sectors in `file1.txt`, 5 free sectors in `file2.txt`). To confirm, we use the following command to check slack space in `file1.txt`:
```
C:\> dd if=\\.\E: bs=512 skip=11341 count=1 | powershell -Command "$input | Format-Hex"
```
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Screenshots/Screenshot 2026-03-15 183605.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/48772ce9c8540394cb0373ec3e8937450a207442/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Screenshots/Screenshot%202026-03-15%20183605.png)

Also, we can double-check the previous sector whose offset is `11340` to make sure.
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Screenshots/Screenshot 2026-03-15 183646.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/48772ce9c8540394cb0373ec3e8937450a207442/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Screenshots/Screenshot%202026-03-15%20183646.png)

Now, we use the following command to hide our `secret.txt` in `file1.txt`'s slack space:
```
C:\> dd if=C:\testSlackSpace\secret.txt of=\\.\E: bs=512 seek=11341 count=1
```
and confirm our success with the following command:
```
C:\> dd if=\\.\E: bs=512 skip=11341 count=1 | powershell -Command "$input | Format-Hex"
```
![TA0005 - Defense Evasion/T1006 - Direct Volume Access/Hide data inside file slack space/Screenshots/Screenshot 2026-03-15 184147.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/fd59a1031781424dcc2ac82192d64e62988546b8/TA0005%20-%20Defense%20Evasion/T1006%20-%20Direct%20Volume%20Access/Hide%20data%20inside%20file%20slack%20space/Screenshots/Screenshot%202026-03-15%20184147.png)

# Tested environment
- Windows 10 x64

---
