# Hide data inside hidden volume on Windows - Procedure #2
In this procedure, we will hide data inside a hidden volume by changing partition type/ID. Let's assume we want hide `secret.txt` file which is in volume E:. First, 
let's check whether this volume is MBR or GPT by using command:
```
C:\> powershell -Command "Get-Partition | Select-Object PartitionNumber, DriveLetter, MbrType, GptType"
```
