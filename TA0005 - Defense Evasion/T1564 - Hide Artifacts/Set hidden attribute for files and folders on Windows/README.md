# Set hidden attribute for files and folders on Windows
## Description
By exploiting the built-in file attributes of the file system, an attacker can easily make a file or even a folder invisible to the average user.
File attributes are also known as metadata, are pieces of information which determine the behavior of a file or a folder in how they exist on the file system on Windows. 
There are many attributes which are defined by Microsoft but three of them can be abused to hide data from normal users:

| Attributes                                                  | Description                                                                           |
| ----------------------------------------------------------- | ------------------------------------------------------------------------------------- |
| **FILE_ATTRIBUTE_HIDDEN**<br>2 (0x00000002)                 | The file or directory is hidden. It is not included in an ordinary directory listing. |
| **FILE_ATTRIBUTE_SYSTEM**<br>4 (0x00000004)                 | A file or directory that the operating system uses a part of, or uses exclusively.    |
| **FILE_ATTRIBUTE_NOT_CONTENT_INDEXED**<br>8192 (0x00002000) | The file or directory is not to be indexed by the content indexing service.           |

We can understand these three attributes simply as follows:
- *Hidden attribute*<br>When this attribute is assigned to a file or a folder, it will be invisible to File Explorer (without enabling the feature to view hidden items).
- *System attribute*<br>When this attribute is assigned to a file or folder, the system pays more attention to it than usual because any file or folder with this
attribute is considered part of the system and is protected by the OS. Normal user cannot see it unless they disable *Hide protected operating system files*. The System attribute is often accompanied by the Hidden attribute.
- *Not Content Indexed attribute*<br>When this attribute is assigned to a file, its content will not be searched when users use search functionality in File Explorer and as
a result, this file will not appeared in File Explorer.
## Procedure
First, let's take a look at the attributes of `secret.txt` file using the following command:
```
C:\> attrib.exe secret.txt
```
<img width="692" height="257" alt="image" src="https://github.com/user-attachments/assets/722370ed-3dcf-4e53-b32c-82e7873fc2d8" />

Also, we use File Explorer to search its content:
<img width="692" height="131" alt="image" src="https://github.com/user-attachments/assets/430f0541-f8da-40b7-a28b-169dd60cb7d9" />

Currently, this file doesn't have any special attributes except for Archive (which is enabled by default when it's created), so File Explorer can easily list 
this file for users, and even search for keywords within the file's content because it doesn't have the Not Content Indexed attribute. Now, we are going to set 
Hidden attribute for this file and observe its behavior.
```
C:\> attrib.exe +H secret.txt
```
<img width="1045" height="470" alt="Picture1" src="https://github.com/user-attachments/assets/4dde54ed-26d8-4200-993b-f69421c06ee4" />

Once the Hidden attribute is set, the file immediately becomes invisible to File Explorer. However, the Hidden attribute alone is not enough, 
as users can still find it by its content.
```
C:\> findstr /SI "password" *.txt
```
<img width="665" height="198" alt="image" src="https://github.com/user-attachments/assets/08a5d8fb-3b8b-4e29-aa0c-1acadb73529c" />

Now, let's set three attributes Hidden, System, and Not Content Indexed for this file with the following command:
```
C:\> attrib.exe +H +S +I secret.txt
```
<img width="692" height="131" alt="image" src="https://github.com/user-attachments/assets/fcd22c9a-5918-4726-9bf1-bacd034e0ec6" />

Observing in File Explorer.

<img width="693" height="112" alt="image" src="https://github.com/user-attachments/assets/217e6f41-822c-4d31-8039-aaec700f0af6" />

With the two images above, the secret.txt file has become invisible to File Explorer and its content search functionality. As we have mentioned before, a normal user 
can see `secret.txt` file by checking *Hidden Items* in File Explorer. Let's look at the below image:<br>
<img width="692" height="70" alt="image" src="https://github.com/user-attachments/assets/6a8d97aa-ef9f-4f00-9223-ffc50daa042e" />

Of course, the `secret.txt` file has not appeared yet because of the System attribute. So, in order to see this file completely, we need to disable *Hide protected operating system 
files* in File Explorer.<br>
<img width="415" height="117" alt="image" src="https://github.com/user-attachments/assets/865d306f-0d68-4333-9e4f-aae48b475817" />

At this point, the `secret.txt` file will appear even if it has the System, Hidden, and Not Content Indexed attributes.
<img width="692" height="217" alt="image" src="https://github.com/user-attachments/assets/abe4ef25-27f8-4440-8772-3b911c9932cf" />

## Tested environment
- Windows 10 x64
## References
[1] https://www.2brightsparks.com/resources/articles/understanding-file-attributes.html

[2] https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants

