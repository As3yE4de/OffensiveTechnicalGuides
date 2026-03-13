# Hide data inside file slack space - Procedure #1
Let's assume we want to hide `secret.txt` in file slack spaces of `file1.txt` and `file2.txt`.
<img width="827" height="328" alt="image" src="https://github.com/user-attachments/assets/3dca7d2d-f67e-47df-b647-b7918c9a0018" />

The first informations we need to gather are bytes per sector and bytes per cluster of volume E:, so we use the following command:
```
C:\> fsutil fsinfo ntfsinfo E:
```
<img width="883" height="370" alt="image" src="https://github.com/user-attachments/assets/5ca605e5-c5f4-403c-b296-34b1cca995a1" />

We have:
- Bytes per sector: 512 bytes
- Bytes per cluster: 4096 bytes
- The number of sector in a cluster: 4096 / 512 = 8 sectors

Let's view the LCNs in `file1.txt` and `file2.txt` by using command:
```
C:\> fsutil file queryExtents E:\file1.txt
C:\> fsutil file queryExtents E:\file2.txt
```
<img width="767" height="102" alt="image" src="https://github.com/user-attachments/assets/8093401f-c6cc-4a4a-b5bb-4acaaf0c2a3f" />

The start LCN of `file1.txt ` is 0x25 (37) and the start LCN of `file2.txt` is 0x58f (1423). Now we need the sector offset of these LCNs by multiplying them by the number of sector in a cluster which is 8:
- `file1.txt`: 37 * 8 = 296
- `file2.txt`: 1423 * 8 = 11384

We should double-check before proceeding further by using command:
```
C:\> dd if=\\.\E: bs=512 skip=37 count=1 | powershell -Command "$input | Format-Hex"
C:\> dd if=\\.\E: bs=512 skip=1423 count=1 | powershell -Command "$input | Format-Hex"
```
<img width="1160" height="753" alt="image" src="https://github.com/user-attachments/assets/d8678b74-669a-4272-a460-6673d15f15ca" />

<img width="1108" height="741" alt="image" src="https://github.com/user-attachments/assets/f489da4a-fb47-4710-b02e-64d72de45d3c" />

Then, we have to find the last LCN of those files, as we have seen, `file1.txt` only has 1 extent that means this extent is also the last extent and the number of allocated clusters for this file is only one that means the start LCN is also the last LCN. Look at `file2.txt`, similar to `file1.txt`, it has 1 extent and the number of allocated clusters is 2, to find the sector offset of the last LCN (the second cluster) of `file2.txt`, we use `(1423 + 2 - 1) * 8` = `11392`. So, we have the following information:
- The sector offset of last LCN in `file1.txt`: 296
- The sector offset of last LCN in `file2.txt`: 11392

Next, we have to find the number of used sectors and the number of free sectors in the last LCN of those files by using the following formulas:
- `file1.txt`:
  - (ceil)Used sectors = (Real size of file % bytes per cluster) / bytes per sector = (3926 % 4096) / 512 = 7.66796875 = 8 sectors
  - Free sectors = The number of sector in a cluster - Used sectors = 8 - 8 = 0 sector
- `file2.txt`:
  - (ceil)Used sectors = (Real size of file % bytes per cluster) / bytes per sector = (4832 % 4096) / 512 = 1.4375 = 2 sectors
  - Free sectors = The number of sector in a cluster - Used sectors = 8 - 2 = 6 sectors

Let's confirm our calculation with `dd` tool and use the skip values `303` for `file1.txt` and `11393` for `file2.txt`:
```
C:\> dd if=\\.\E: bs=512 skip=303 count=1 | powershell -Command "$input | Format-Hex"
C:\> dd if=\\.\E: bs=512 skip=11393 count=1 | powershell -Command "$input | Format-Hex"
```
<img width="1112" height="696" alt="image" src="https://github.com/user-attachments/assets/b511d6c0-0dab-4ae0-b0eb-eb519df11e89" />

<img width="1108" height="640" alt="image" src="https://github.com/user-attachments/assets/b971fdac-04ab-427f-a75f-d9b83d422262" />

From the above results, we are only able to hide data inside the slack space of `file2.txt`:
- The sector offset of the first sector in the slack is `11394`.
- The number of sectors we can hide is 6 sectors.

Now, let's hide our `secret.txt` which is 41 bytes (1 sector) in `file2.txt`'s slack space, by using command:
```
C:\> dd if=C:\testSlackSpace\secret.txt of=\\.\E: bs=512 seek=11394 count=1
```
<img width="1122" height="731" alt="image" src="https://github.com/user-attachments/assets/e9d9f55e-d3fd-4735-ae10-11918e58b73a" />

# Tested environment:
- Windows 10 x64

---
