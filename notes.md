# Intel X86 Processors 


> only 14 assembkly instructions make up to 90 percent of the code!

 in intel terminology : a byte is a char(8) , a word is a short(16), int/long is double word(32), double long(64) 

 ![refresher](img/refresher.png)

 Intel is little endian. 0x12345678, least significant bit is registered first, then more significant and more significant.

![big/smallendian](img/endian.png)

Endians are only meaningful in byte level not in bit level!


Register Conventions!

`EAX` ==> stores function return value
`EBX` ==> Base pointer to the data section 
`ECX`==>counter for string and loop ops
`EDX` ==> I/O pointer 
`ESI`==>source pointer for string operations
`EDI`==>Destination pointer for string operations
`ESP`==>stack pointer
`EBP`==>Stack frame base pointer
`EIP`==>pointer to next instruction to execute(instruction pointer)


There are also other type of registers.

`EAX, EDX , ECX` == > Caller-save registers 

I am a function and I will call another function. And when I call I am sure I am going to destroy the current regusters(eax edx ecx)
so we save the copy of the registers before calling the function so we do not lose the register  


`EBP, EBX, ESI, EDI` ==> Callee-save registers  

If I call a functiion, it wont change and modify the info in the register of ebp, ebx etc. 


this `E` at the head of registers mean Extended. Since these are written originally for 16 bit , AX became Extended AX with 32 bits. etc.


`EFLAGS`

EFLAGS register holds many single bit flags. Will only ask you to remember the following for now :

Zero Flag(ZF)- Set if the resuklt of some instruction is zer, cleared otherwise
Sign Flah(SF) - Set equal to the most significant bit of the result.

## INSTURCTIONs

`NOP` No Operation. No registers no values nothing.  Just there to pad bytes or delay time.  Bad guys use it to make simple exploits more reliable. But thats another class.

NOP actually takes a register and re-registers it on itself :)


`PUSH` ==> push word, double word ort quadword onto the STACK.  can either be emmediuate(a numeric constant) of the value in a register.

fror our purposes, it will; always be a DWORD(for this course)
the push instruction aautomatically decrements the stack pointter, esp, by 4. Why decrements? bcs its stack, its FILO.

`POP`  ==> Pop a value from the Stack

take a Dword off the stack, put it ina a register and increment esp by 4 ( reverse of Push)


`CALL` ==> Call's job is to transfer control to a different function in a way that control can later be resumed where it left off.

First it pushes the address of the next instruction onto the stack.  for use by RET(return from procedure) for when the procedure is done.

Because idea behind a function is , do this and I will keep executing from the next line. So call also pushes the address of the next instruction set, so that once the called function is completed, the program keeps continuing from where it left off. Its a reminder to where to return to after executing the function.

Then it changes eip to the address given in the instruction.

DEstionation address can be specified in a multiple ways. 

- Absolute address  ==> address 0x0030434 

- Relative address(relative the the end of the instruction) ==> some adress that hex 50 bytes ta ta ta. 

`RET`==> Return from Procedure.

Two forms :

1) pop the top of the stack into the eip

in this form, the instructuon ius just written as ret.

typically used by cdecl functiuons

2) pop the top of the stacj into eip and add a constatn number of bytes to esp. 

in this form , the instruction is written as ret 0x8 of 0x20 etc. typically used bty stdcall fucntions



`MOV` ==> Move.

Can move:

register to register

memory to register, register to memory, immediate to reguster immediate to memory.

> NEVER MEMORY TO MEMORY

>Memory addresses are given in r/m32 from. 




## THE STACK

stack is the conceptiual area of main memory which is designated by OS when program is started. Stack is LIFO/FILO data structure where data is pushed on to top of the stack and popped off the top

Stack grows over lower memory addresses. Adding something to the stack means the top of the stack is now at a lower memory address.

ESP points to the top the stack. The lowest address which is being used(since it is LIFO)

Stack keeps track of which functions were called before the current one. It holds LOCAL VARIABLES and is used to pass arguments to the next function to be called



Register in time 1 : 

eax : 0x00000003
esp : 0x0012FF8C

So when we call lets say , `posh eax`, we expect the eax to be written 4 bytes after(decrement) the last esp register in the stack.

Register after push eax:

eax: 0x00000003
esp:0x0012FF88    ==> here, we jumped 4 bytes from FF8C to FF88. (where FF8C>FF88)

![push eax](img/push%20eax.png)


to reverse it , we use `POP` 

in order to reverse the action above, we `pop eax`and voila, esp jumped by 4 bytes up ( incremented) and top shelf of the stack is emptied.

![pop eax](img/pop%20eax.png)

> Everything numerically under esp is considered to be undefined. Although there is a data over there, it is not of our business. 

> once we call pop eax, the the value at the top of the stack (eax) is taken to the register and that memory spot is not undefined.


### Calling Convention

calling convention is about how to pass parameters and how to get parameters back.

how code calls a subroutine is compiler-dependent and configurable. But there are few conventions. We will deal with cdecl and stdcall conventions.

### cdecl

C Declaration. The most common calling convention. Function parameters pushed (using push)  onto stack RIGHT TO LEFT. Saves the old stace frame pointer and sets up a new stack frame.  

eax or edx:eax returns the result for primitive data types.

Most significant 32 bits go into edx, least significant bits go into eax

Also values are always big endian in registers, in memory they are little endian.

so for example :  `printf(%d\n, myVariable)`  ===> from right to left: push my variable to the stack then ppusth the pointer and trhen call the function.

function here, saves the old frame pointer, saves the address to the stack.

`Caller` is responsible for cleaning up the stack.  ==> 

so we have 2 parameters and 1 function call. push push and call. so whatever regiester is calling the function is also responsible to clean up the stack



### stdcall


Microsoft C++ code e.g. Win32API
parameters pushed onto stack right to left
saves the old stack frame pointer and sets up a new stack frame pointer

HERE, CALLEE is responsibnle for cleaning ip any stack parameters it takes not the CALLER!
