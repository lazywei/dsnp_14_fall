To use the SelfCheck program
============================
1. Download the "SelfCheck" program.

2. Download the "MustExist.txt" and "MustRemove.txt" files. Please note that

   for different homework, there are these two files with different contents,

   but with the same file names.

3. Put the your .tgz file, SelfCheck, MustExist.txt and MustRemove.txt in the

   same directory.

4. Type: ./SelfCheck bxxxxxxxx_hw1.1.tgz 

   For example, ./SelfCheck b77503057_hw1.1.tgz


What does the program checks
============================
1. The program will check your student ID which is part of your .tgz file.

   The student ID must follow the following rule:

   - The first character must be lower case
 
     and followed by eight digit numbers.

2. The program will check the existence of the files which is specified in

   MustExist.txt.

   If there are some lost files, the program will show error message and

   the lost files.

3. The program will check the absence of the files which is specified in

   MustRemove.txt.

   If there are some redundant files, the program will show error message

   and the redundant file names.

4. Otherwise, it will show "Succeed in self checking."
