# Cache covert channel
## ACM winterschool 2022

## Directory Structure
The code base contains two directory, Makefile and some other required files and described as follows:

**1.** The **lib** directory contains the standard helper functions and data structures. This directory contains two files, namely **util.h** and **util.c**. All commonly used functions and data structures are declared in the **util.h**, and the corresponding definition is provided in the **util.c**. 

**2. covert-channel** contain the base code for **sender** and **receiver** in files **sender.c** and **receiver.c**, respectively. 

**3. Examples** directory contains two example programs. One is demonstrating the use of library function for mapping a file into memory, and the other one is to convert a string into binary and vice-versa.

**4. Makefile** is used for the compilation of the codebase.

**5.** The **share_mem.txt** file is for establishing a shared memory region between two processes.  For creating a covert channel between **sender** and the **receiver**, a shared memory region is necessary on which we can create a **FLUSH+RELOAD** based **Cache Covert Channel**.

## To Build System
Use **make** command to compile the assignment. After successful compilation, all the binaries are generated in the root directory of the codebase.

**1. Compile code:** To compile for sender and receiver, use the **make** command from the root directory of the codebase.
> **make**

**2. Clean the build:**
> **make clean**

## Generated Binary Code
After the compilation, binary files (**sender** and **receiver**) are placed in the root directory of the codebase.

## Run

**1. Receiver:** Run the receiver first and wait for it to calibrate. 
> **./receiver**

**2. Sender:** Once the receiver is ready(you will get a prompt saying Start sender now), run the sender in a different terminal and send a message.
> **./sender**
