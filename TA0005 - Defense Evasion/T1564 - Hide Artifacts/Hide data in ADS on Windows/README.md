# Hide data in ADS on Windows - Technical concepts
ADS (Alternative Data Stream) which is only available on NTFS file system. When a file is created in this file system, it has differnet data streams, but one of them is 
unamed. This unamed data stream is also called the default stream that contains the real data that we expect to see. In addition to the default stream, there can be many 
non-primary data streams, also known as ADSs. ADSs store additional information for the file, contrary to the default stream, they are invisible to the normal users. 
The full name of a data stream is of the form below.<br>
`<filename>:<stream name>:$DATA`

As we have mentioned before, the default stream is unamed that means we can read the content of a file with two different ways:<br>
<img width="813" height="125" alt="image" src="https://github.com/user-attachments/assets/cf84b01d-a3dd-4561-8769-1ac7e1f77706" />

In the case of directories, there is no default data stream, but there is a default directory stream. Instead of the stream type `$DATA` in case of files, 
directories have the stream type `$INDEX_ALLOCATION` and the default stream named `$I30`. We can list the content of a directory with three different ways:<br>
<img width="753" height="597" alt="image" src="https://github.com/user-attachments/assets/70f2838b-5333-439d-86ac-8fcffaef7d83" />

As we have known, a file can contains different ADSs, so what are the remaining ADS beside the default stream? If we create a file on our local machine, there is only 
one data stream for this file - the default stream. But the ADSs can be created by the third party, let's take a look at the file `Streams.zip` which has been downloaded 
from Microsoft Edge:<br>
<img width="806" height="292" alt="image" src="https://github.com/user-attachments/assets/e495c041-65e0-455c-89b6-49163876d160" />

These ADSs of `Streams.zip` file hold additional information for this file, as we have mentioned before. Now, get back to the big issue. ADS files can be appended to existing files 
or a folder without affecting their size or functionality. In addition to this, ADS files are not visible to Windows Explorer (File Explorer) or to the `DIR` command in CMD 
(unless we use the `/R` switch). Because of this issue, an attacker can hide a malicious code or secret data inside ADS, retrieve and run the code without any problem. 
Once the malicious code is executed, it can run silently in the background and no process will appear in the process list of Windows task manager; the program used 
to open the original file (default stream) will appear in the process list but no sign of the malicious program will be visible (process hollowing). Furthermore, we 
can attach the malicious program to a legitimate process, which will make detection of the code much more difficult (process injection).

Is there any way to be invisible from `dir` command with `/r` switch? Yes, in this case, just rename the host file with the following reserved names:<br>
- CON
- PRN
- AUX
- NUL
- COM1 to COM9
- LPT1 to LPT9
# References
[1] Data Hiding Techniques in Windows OS: A Practical Approach to Investigation and Defense - Nihad Ahmad Hassan, Rami Hijazi, Helvi Salminen

[2] [MS-FSCC]: NTFS Streams | Microsoft Learn - https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-fscc/c54dec26-1551-4d3a-a0ea-4fa40f848eb3

[3] Naming Files, Paths, and Namespaces - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file

[4] Naming Files, Paths, and Namespaces - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file#win32-file-namespaces

---
