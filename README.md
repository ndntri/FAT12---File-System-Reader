# FAT12 File System Reader
![floppy-disk](https://github.com/user-attachments/assets/28268b7c-66a4-46e5-a371-4a203c9109fc)

- [FAT12 File System Reader](#fat12-file-system-reader)
  - [1. Introduction](#1-introduction)
  - [2. Features and Functionality](#2-features-and-functionality)
  - [3. Document](#3-document)
  - [4. Installation and Run](#4-installation-and-run)
  - [5. Notes](#5-notes)
  - [6. Authors](#6-authors)
  - [7. License](#7-license)


## 1. Introduction
- This application is designed to explore and comprehend the intricacies of FAT12 file systems.
- By delving into its inner workings, you’ll gain insights into how files and directories are organized on a FAT12 drive.
- Notably, you won’t need **an actual floppy disk**. Instead, you can utilize **a virtual floppy disk** (using the provided **floppy.img** file).
- Additionally, this project incorporates the **CMSIS** standard, allowing you to familiarize yourself with this essential framework.

## 2. Features and Functionality
2.1. Read information from the FAT table. (File: floppy.img)<br>
2.2. Display lists of files and directories.<br>
   ![Screenshot 2024-09-10 112312](https://github.com/user-attachments/assets/1c9b0a30-3e6c-423b-b436-d591f314538a)

   ![Screenshot 2024-09-10 112533](https://github.com/user-attachments/assets/766a3a7e-8d5c-4fb6-9c53-b04adc882128)

2.3. Extract content from FAT12 files.<br>
   ![Screenshot 2024-09-10 112701](https://github.com/user-attachments/assets/4249392a-ca12-4372-a917-68af4c1238e0)
## 3. Document
3.1. [fat12_description.pdf](https://github.com/user-attachments/files/16823361/fat12_description.pdf)<br>
3.2. [FAT12_overview.pdf](https://github.com/user-attachments/files/16823365/FAT12_overview.pdf)<br>
3.3. [floppy.zip](https://github.com/user-attachments/files/16823371/floppy.zip)

## 4. Installation and Run
4.1. Install Dev-C++.<br>
4.2. Clone the project repository( or you can download project):<br>
```   git clone https://github.com/ndntri/FAT12---File-System-Reader.git ```<br>
4.3. Run the file:  ```mock1.dev```<br>
4.4. Compile and Run (F11)

## 5. Notes
 - You must have the **_file floppy.img_** in the same directory as the source code you’ve cloned (the downloaded directory)

## 6. Authors
 - Nguyen Dang Nhu Tri

## 7. License
 - The project is distributed under the MIT License. Refer to the LICENSE file for more details.
