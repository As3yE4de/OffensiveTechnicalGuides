# Set hidden attribute for files and folders on Windows - Procedure #1
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
# Tested environment
- Windows 10 x64

---
