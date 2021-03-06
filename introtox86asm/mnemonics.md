# INSTURCTIONs

`NOP` No Operation. No registers no values nothing. Just there to pad bytes or delay time. Bad guys use it to make simple exploits more reliable. But thats another class.

NOP actually takes a register and re-registers it on itself :\)

`PUSH` ==&gt; push word, double word ort quadword onto the STACK. can either be emmediuate\(a numeric constant\) of the value in a register.

fror our purposes, it will; always be a DWORD\(for this course\) the push instruction aautomatically decrements the stack pointter, esp, by 4. Why decrements? bcs its stack, its FILO.

`POP` ==&gt; Pop a value from the Stack

take a Dword off the stack, put it ina a register and increment esp by 4 \( reverse of Push\)

`CALL` ==&gt; Call's job is to transfer control to a different function in a way that control can later be resumed where it left off.

First it pushes the address of the next instruction onto the stack. for use by RET\(return from procedure\) for when the procedure is done.

Because idea behind a function is , do this and I will keep executing from the next line. So call also pushes the address of the next instruction set, so that once the called function is completed, the program keeps continuing from where it left off. Its a reminder to where to return to after executing the function.

Then it changes eip to the address given in the instruction.

DEstionation address can be specified in a multiple ways.

* Absolute address ==&gt; address 0x0030434
* Relative address\(relative the the end of the instruction\) ==&gt; some adress that hex 50 bytes ta ta ta.

`RET`==&gt; Return from Procedure.

Two forms :

1\) pop the top of the stack into the eip

in this form, the instructuon ius just written as ret.

typically used by cdecl functiuons

2\) pop the top of the stacj into eip and add a constatn number of bytes to esp.

in this form , the instruction is written as ret 0x8 of 0x20 etc. typically used bty stdcall fucntions

`MOV` ==&gt; Move.

Can move:

register to register

memory to register, register to memory, immediate to reguster immediate to memory.

immediate is hardcoded value.

> NEVER MEMORY TO MEMORY
>
> Memory addresses are given in r/m32 from.

`ADD` ==&gt; a = a+ b for example add eax,ebx means eax = eax + ebx. so it summs them and writes on what is on the left side

`SUB` ==&gt; subtraction. sub eax, \[ebx\*2\]

`LEA` ==&gt; Load Effective Address. ==&gt; frequently used with pointer arithmetic sometimes for just arithmetic in general. uses the r/m32 form but **is the exception to the rule**

Lea makes the calcualtion and appoints to the destination without disturbing the memory. Does the calc on the spot.

that the \[\] syntax means dereference\("value at"\)

example: ebx = 0X2, edx =0x1000 --- lea eax,\[edx+ebx_2\] \*eax = 0X1004, not the value at 0x1004!!_

`JMP` ==&gt; Jump. Changes the eip unconditionally of the given address. Main forms of the address.

short relative : 1 byte displacement from the end of the instruction.

`01151015 jmp 1155000h` doesnt have the number `1155000h` anywhere in it, it is really `jmp 0x0E` btyes forward.

near relative: 4 bytes displacement from the curent eip

absolute: hardcoded address in instruction. like `01151013 jmp 01151023`

absolute indirect : indirect address calculated with r/m32

`jmp -2` is infinite loop for short relative jmp :\)

Conditional Jumps: `je, jge, jne, jle`

`JCC` ==&gt; Jump if Condition is met.

`JNE` == `JNZ` jump if not equal means jump if not zero. What does it mean ? jne a b for example. the operation is a-b. so flag is either 0 or 

so 0 means equal. if not equal, its not zero. So it makes zero flag check

`TEST` ==&gt; computes the bit-wise logical AND of first operant\(source 1 operand\) and teh second operand\(source 2 operand\) and sets the SF, ZFand PF staus flags accordingly.

like CMP, sets the flags and does not save the results

`AND` ==&gt; destination operand can be r/m32 or register. source operand can be r/m32 or register or immediate!

`and al, bl`

`and eax, 0x42`

`OR`==&gt; destination operand can be r/m32 or register. source operand can be r/m32 or register or immediate!

`XOR` ==&gt; A xor B = If A , then A ; else then B.

there is a trick with XOR, when we xor something with itself, the result will always be 0. so ZF will be set. `xor eax eax` sets the Zero Flag in that sense

thus, it is commonly used to zero a register by XORing it with itself because it is faster than `mov`.

`NOT` ==&gt; single source/destination operand can be r/m32. One's complement negation.

`not al` or `not [al+bl]`.

example :

```text
NOT 0x00110011b(al-0x33)
result 0x11001100b((al-0xCC))
```

`SHL` ==&gt; Shift Logical Left. Can actually be used with the C and C++ &lt;&lt; operator.

first operand\(source and destination\) operand is an r/m32

second operand is either a cl\(lowest byte of ecx\) or or a 1 byte immediate\(hardcoded\).

the 2nd operand is the number of places to shift

> it multiplies the register by 2 for each placeteh value is shifted. More efficient than a multiply insturction.

bits shifted off to the left hand side are `shifted into` \(set\) the carry flag.\(CF\)

`SHR` ==&gt; Shift Logical Right

Same thing with SHL, but this time each bit registers divide by power of that bit.

like `shr eax, 5 ==> divide eax with 2**5(32) and write it on eax.`

Still much effective than division and faster. becasue just shifting bits around.

when shifting bits to right so much so that it is now outbound of 8 bits, Carry flag\(CF\) is set to 1 to show that it is not actually 0 but it is carried.

`IMUL` ==&gt; signed Multiplier. 3 forms:

1\) imul r/m32 ==&gt; edx:eax=eax\*r/m32

for example : `imul ecx` take single parameter.

what does this mean : multiply ecx with eax and write the return to edx:eax

2\) imul reg, r/m32 ==&gt; reg=reg\*r/m32

for example : `imux eax,ecx`

so eax times ecx and puts it back into eax.

if there is a chance for overlow, we make some modifications so it fits into the first form.

3\) imul reg,r/m32, immediate ==&gt; reg=r/m32\*immadiate

Multiplication and division have some possible side effects. For example, multiplication of two 32 bits may exceed the bounds of 32 bits of data.

Easiliy.

Thus, it is saved to edx:eax, so that it fits onto 64 bits.

note that `edx:eax` means concatenation. When eax is saved onto edx:eax it means the result of the operation is outbound of 32 bit limits!.

in 32 bit architecture, in this kind of a scenario, half of the result it written on edx and half of it written on eax and compiler figures out to how to output it.

`DIV` ==&gt; Unsigned Divide.

2 Forms:

1\) divide ax\(16 bit of eax\) by r/m8 , al=quotient, ah=remainder

2\)edx:eax by r/m32, eax=quotient,edx=remainder

if the divisor is 0, a `divide by 0 exception` is raised.

`REP STOS` ==&gt; Repeat Store String

One of a family of "rep" operations hich reapeat a single instruction multiple times\(i.e. stos is also a standalone instruction\)

Rep is not a standalone instructionm its more of a prefix to stos.

> all rep operations use `ecx` register as counter to determine how mant times to loop through the instruction. Each time it execures, it decrements ecx. once ecx=0, it continues to the next instruction.

either moves one byte at a time or one dword at a time.

either fill btye at \[edi\] with al or fill dword at \[edi\] with ax.

moves teh edi register forward one byte or one dword at at time, so that the repeated store operation is `storing into consecutive locations`.

thus, there are 4 pieces which must happen before the actual `rep stos` occurs

1\) set `edi` to start the destination 2\) \(e\)ax to the value to store 3\) ecx to the number of times to store

`NEG`==&gt; 2s Compliment negation. take a positive number make it negative for 2s.

so takes a value and outputs the negative value.

`MUL` ==&gt; unsigned multiplication.

`DEC` ==&gt; Decrement a register \(substract one\)

`LEAVE`==&gt; tearing stackframe\(mov esp, ebp pop ebp ret\)

