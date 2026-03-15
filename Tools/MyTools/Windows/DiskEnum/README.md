# README
## Usage
**NOTE:** Please run as Administrator.

To print usage, simply type the tool's name and hit Enter.

`.\DiskEnum.exe <COMMAND(S)>`

**COMMANDS**

`--disk`  List physical disks.

`--disk-partition`  List all partitions of disks.

`--disk-volume`  List all volumes of disks.

`--volume-extent=<DRIVE_LETTER>`  List all extents of a specified volume. The drive letter must be a 
single character (for example: .\DiskEnum.exe --disk-volume=C).

`--volume-simple`  List all dynamic simple volumes (only NTFS).

`--volume-spanned`  List all dynamic spanned volumes (only NTFS).

`--volume-striped`  List all dynamic striped volumes (only NTFS).

`--volume-mirrored`  List all dynamic mirrored volumes (only NTFS).

`--slack=<PATH>`  Find slack space of files. The given path can be a file path or a directory path.

## Examples
To enumerate physical disks on the machine, use the following command:

`.\DiskEnum.exe --disk`

<img width="1129" height="497" alt="image" src="https://github.com/user-attachments/assets/678933c0-7fdc-4299-bbb2-32ab214ad150" />

To enumerate all partitions on each disks, use the following command:

`.\DiskEnum.exe --disk-partition`

<img width="722" height="701" alt="image" src="https://github.com/user-attachments/assets/eb6510c0-0da6-4890-82cf-c3b597781922" />

To enumerate all volumes on each disks, use the following command:

`.\DiskEnum.exe --disk-volume`

<img width="859" height="704" alt="image" src="https://github.com/user-attachments/assets/7bf9cc45-0e34-4aaa-9baf-c6a1c6f89af2" />

To enumerate all extents of a specified volume, for example: C volume, use the following command:

`.\DiskEnum.exe --volume-extent=C`

<img width="689" height="135" alt="image" src="https://github.com/user-attachments/assets/8b5a8bc6-0161-4e2b-a7f2-57e98c838584" />

*Four below examples, at the current time, they work on NTFS file system*

To enumerate all simple volumes on dynamic disks, use the following command:

`.\DiskEnum.exe --volume-simple`

<img width="717" height="284" alt="image" src="https://github.com/user-attachments/assets/1e31a1dc-e18e-4e3a-b77b-ccae6572db53" />

To enumerate all spanned volumes on dynamic disks, use the following command:

`.\DiskEnum.exe --volume-spanned`

<img width="734" height="485" alt="image" src="https://github.com/user-attachments/assets/aeaf6c29-50b2-4254-816a-10bfcf850d0b" />

To enumerate all striped volumes on dynamic disks, use the following command:

`.\DiskEnum.exe --volume-striped`

<img width="767" height="495" alt="image" src="https://github.com/user-attachments/assets/a622755b-71c1-46db-89be-7563888917d5" />

To enumerate all mirrored volumes on dynamic disks, use the following command:

`.\DiskEnum.exe --volume-mirrored`

<img width="765" height="484" alt="image" src="https://github.com/user-attachments/assets/1ae03b8e-4c3a-4d13-9f09-f3494546c441" />

*The `--slack` command works best with dd.exe. Remember to raw write as sector, not byte.*

To find slack spaces of a specified file, for example: C:\Users\User\Downloads\test.txt, use the following command:

`.\DiskEnum.exe --slack=C:\Users\User\Downloads\test.txt`

<img width="725" height="131" alt="image" src="https://github.com/user-attachments/assets/d8098c06-dd16-41b5-9e43-b69e40f30ee7" />

Or to find slack spaces of all files in a specified directory, just supply the path to the directory, it will scann recursively.

`.\DiskEnum.exe --slack=C:\Users\User\Test`

<img width="766" height="343" alt="image" src="https://github.com/user-attachments/assets/0bdd346b-fda6-4d88-968e-878d19ad4d68" />

## Tested environment
- Windows 10 x64

---
