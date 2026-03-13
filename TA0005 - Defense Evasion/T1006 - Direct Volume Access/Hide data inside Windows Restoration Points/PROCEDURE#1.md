# Hide data inside Windows Restoration Points - Procedure #1
As we have known, to create a restoration point, we have to make sure VSS service is running. To check if VSS is running, we use the following command:<br>
`C:\> powershell -Command "Get-Service vss"`<br>
<img width="928" height="212" alt="image" src="https://github.com/user-attachments/assets/973f66fe-824a-4856-9ccb-df67b825e716" />

To enable this service, we use:<br>
`C:\> powershell -Command "Start-Service vss`<br>
<img width="827" height="161" alt="image" src="https://github.com/user-attachments/assets/b6208c80-fbc2-45d2-b1b2-3e361098847b" />

System Restore is disabled by default, to enable this feature on volume E:, for example, we use the following command:<br>
`C:\> powershell -Command "Enable-ComputerRestore -Drive 'C:\','F:\'"`<br><br>
**Note:** Remember to enable System Restore for the system drive (C:) first to avoid *Include System Drive in the list of drives* error if we want to create restoration points on volumes not C: drive.<br><br>
<img width="981" height="53" alt="image" src="https://github.com/user-attachments/assets/153ac563-0717-424a-9429-bdd0d0513f73" />

After enabling System Restore on E: drive, just hide our data in this volume and then use the following command to create a restoration point:<br>
`C:\> powershell -Command "Checkpoint-Computer -Description 'test' -RestorePointType MODIFY_SETTINGS"`<br>
<img width="1121" height="228" alt="image" src="https://github.com/user-attachments/assets/23a72c84-2c1e-470c-9532-b0ffef883d03" />

To confirm our restoration point, use the following command:<br>
`E:\> vssadmin list shadows /for=E:`<br>
<img width="1102" height="243" alt="image" src="https://github.com/user-attachments/assets/6f6d701d-0330-40fc-a604-d6816a7da044" />

Now, we can delete our data on volume E:, whenever we want to access our data, just create a directory symbolic link to the Shadow Copy Volume. Have a look at the below image:<br>
<img width="1136" height="525" alt="image" src="https://github.com/user-attachments/assets/3dd32c89-40eb-4fa0-901b-9722b85c7574" />

To access our text file, just navigate to the directory symbolic link and use `type` command normally. If we want to run our binary `calc.exe`, use the 
following command:<br>
`E:\> wmic process call create <Shadow Copy Volume>\calc.exe`<br><br>
**NOTE:** Replace the question mark ( ? ) to the period ( . )<br><br> 
<img width="1032" height="540" alt="image" src="https://github.com/user-attachments/assets/804af64d-c5dd-4be0-9fad-d4aad3e7d84c" />

# Tested environment
- Windows 10 x64

---
