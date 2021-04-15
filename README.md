# PrioBoost
Simple kernel driver to set thread priority using some WinAPIS , since only small set of priorities are available to set from user mode , the idea is to use client / driver 
architeture , the user client send the <b> TheadId </b> and <b> Priority </b> , the kernel driver will look for the real thread object in kernel space with <b>PsLookupThreadByThreadId</b>
and change it with <b> KeSetBasePriorityThread</b> .
