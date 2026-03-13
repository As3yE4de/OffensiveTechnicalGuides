# Set hidden attribute for files and folders on Windows - Technical concepts
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
# References
[1] Understanding Windows File Attributes - https://www.2brightsparks.com/resources/articles/understanding-file-attributes.html

[2] File Attribute Constants (WinNT.h) - Win32 apps | Microsoft Learn - https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants

---
