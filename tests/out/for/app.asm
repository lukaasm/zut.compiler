MOV #0,0
MOV #0,0
MOV R1,#0
MOV R2,2
SUB R1,R2
JNL 12
MOV R1,#0
MOV R2,2
ADD R1,R2
MOV #3,R1
MOV #0,#3
JMP 2
MOV #1,10
MOV R1,#1
MOV R2,0
SUB R1,R2
JNG 23
MOV R1,#1
MOV R2,1
SUB R1,R2
MOV #4,R1
MOV #1,#4
JMP 13
MOV #1,0
MOV #2,6
MOV #0,0
MOV R1,#0
MOV R2,#2
SUB R1,R2
JNL 41
MOV R1,#1
MOV R2,1
ADD R1,R2
MOV #5,R1
MOV #1,#5
MOV R1,#0
MOV R2,1
ADD R1,R2
MOV #6,R1
MOV #0,#6
JMP 26
