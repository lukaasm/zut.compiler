SUB 1,0
JZ 3
JMP 3
3:3:ADD 0,1
JZ 9
ADD 0,0
JZ 9
MOV #1,1
JMP 10
MOV #1,0
SUB #1,0
JZ 12
12:MOV #0,0
MOV R1,#0
MOV R2,0
SUB R1,R2
JNL 18
MOV #0,2
18:MOV R1,#0
MOV R2,0
SUB R1,R2
JG 36
MOV R1,#0
MOV R2,0
SUB R1,R2
JNG 27
JMP 29
ADD 0,1
JZ 31
MOV #2,1
JMP 32
MOV #2,0
ADD #2,0
JZ 36
MOV #3,1
JMP 37
MOV #3,0
SUB #3,0
JZ 41
MOV #0,123
JMP 42
MOV #0,321