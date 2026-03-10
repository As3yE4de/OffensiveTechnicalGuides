# Hide data in ADS on Windows
## Description
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
## Procedure
Let's see how we can hide and retrieve a simple text file named `secret.txt` inside ADS. To hide this file inside another file, we use the following command:<br>
`C:\> type secret.txt > cover.txt:MYSECRET`<br>
and use `dir` command with `/r` switch to confirm our created ADS.<br>
<img width="817" height="221" alt="image" src="https://github.com/user-attachments/assets/4cc0a962-586c-43b4-8fe3-e78890e5c725" />

To read our ADS, we use the following command:<br>
`C:\> more < cover.txt:MYSECRET`<br>
<img width="888" height="236" alt="image" src="https://github.com/user-attachments/assets/001db82e-9e07-4337-808d-ee73145a0090" />

To retrieve our secret, we can simply redirect this output to a file:<br>
`C:\> more < cover.txt:MYSECRET > retrieved.txt`<br>
<img width="772" height="266" alt="image" src="https://github.com/user-attachments/assets/e1b316ab-c8a5-4826-a743-5c86d29863f7" />

What about the binary code? Just use the `type` command as we have used before.<br>
<img width="847" height="247" alt="image" src="https://github.com/user-attachments/assets/89808e9b-3970-4b83-b434-34310c88caf6" />

To run this binary, there are two ways for us: create a symbolic link (required Administrator) and use `wmic` command. To create a symbolic link to our binary, we use 
the following command:<br>
`C:\> mklink run.exe C:\testADS\cover.txt:MYBIN`<br>Then, use `C:\> start run.exe` to run the binary.<br>
<img width="1336" height="476" alt="image" src="https://github.com/user-attachments/assets/d8a8b530-b1e9-4f3c-b907-e751c5104b86" />

To run the binary using `wmic`, we use the following command:<br>
`C:\> wmic process call create C:\testADS\cover.txt:MYBIN`<br>
<img width="962" height="350" alt="image" src="https://github.com/user-attachments/assets/172e2140-6ec2-4f23-afa1-1e0bcb971949" />

To retrieve our malicous binary, we use the following simple Powershell script:<br>
`C:\> powershell -Command "Get-Content .\cover.txt -Stream MYBIN -Encoding Byte -ReadCount 0 | Set-Content .\retrieved_calc.exe -Encoding Byte"`<br>
<img width="1637" height="436" alt="image" src="https://github.com/user-attachments/assets/fd6fb059-88e3-43ef-8bb0-ff90fae08552" />

In the below example, we will create a stealth ADS. To do this, we must create the cover file using the following command:<br>
`C:\> echo "This is a cover file" > \\?\C:\testADS\CON`<br>
<img width="856" height="256" alt="image" src="https://github.com/user-attachments/assets/ff178856-b9e1-4ffe-baa5-6a1a4203bbb4" />

Using this reserved name, we cannot open it normally such as using `type CON` or click on this file in File Explorer. We must use Win32 File Namespaces, just have 
a look at the below image:<br>
<img width="837" height="72" alt="image" src="https://github.com/user-attachments/assets/ee900198-bb7f-45a5-ac02-377c479708c8" />

Even after creating ADS for this `CON`, using `dir /r` cannot display our ADS.
<img width="987" height="281" alt="image" src="https://github.com/user-attachments/assets/14d61681-a6dc-4219-8adf-a162412420dc" />

Just remember that if we use reserved names, we must use Win32 File Namespaces.

So how to remove our ADS? To remove ADS, we can use `Streams.exe` from Microsoft Sysinternals, to display all ADS, we use the following command:<br>
`C:\> streams.exe -nobanner -s C:\testADS`<br>
<img width="967" height="161" alt="image" src="https://github.com/user-attachments/assets/dbf9a485-c3a1-4147-9007-bc7a10f83caf" />

If we look closely, this tool cannot display the ADS of a file with reserved name, in this case, we can use `lads.exe` tool.<br>
<img width="977" height="267" alt="image" src="https://github.com/user-attachments/assets/d8fbc586-d963-432e-bae2-8eed6ae66e30" />

To delete all ADSs, we use the following command:<br>
`C:\> .\streams.exe -nobanner -d C:\testADS\cover.txt`<br>For `CON` file, use the Win32 File Namespaces.<br>
<img width="980" height="317" alt="image" src="https://github.com/user-attachments/assets/368bdf10-8a43-4b0f-9a14-471a8d891fc8" />

<img width="1077" height="492" alt="image" src="https://github.com/user-attachments/assets/dd4fd3eb-35da-4788-a666-9806e7a5ff2d" />



## Tested environment
- Windows 10 x64
## References
