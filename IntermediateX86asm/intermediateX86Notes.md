# Intermediate IntlX 86 Processor Architecture & Assembly.

## Part 2:

This course will cover the following :

> Memory Segmentation
> Paging
> Interrupts
> Debugging, I/O

In this lecture, we will dwell on IA-32, not 64 bit architecture yet.(that is for another one, wait for it :))

Also there wont be floating point assembly or registers since in reverse engineering or in malware analysis there is not much of a use for it.

This is what we are goint to learn :) 

![systemLevelRegistersandDataStructures](img/OIP.jpg)


## CPUID- CPU Feature Identification.

Different processors support different features. `CPUID` is how we know if the chip we are running on supports newer features, such as hardware virtualization, 64 bitmode(asdasdafa), Hyperthreading, thermal monitors etc.

CPUID does not have operands. Rather it __takes input__ as value preloaded into `eax` (and possible ecx). After it finishes , the outputs are stored to `eax, ebx, ecx and edx`


so if you want your code to be compatible, you need to check some features before implementing.

ID flag in EFLAGS(bit 21), this is the CPUID flag. If it set to 0, you set to 1 and if it stays at 1, then it has CPUID, if it returned back to 0, then you dont have CPUID.
But how do we read and write EFLAGS?

In order to manipulate the flags, we have 2 instructions. 

![EFLAGS](img/eflags.png)

there are FLAGS which are 16 bits and also there are E(xtended)FLAGS, which are 32 bits. make sure which one to push and pull. the problem occurs because all of them have the same opcode, which is `9C`. the instuction makes the difference.



`PUSHFD`: If you need to read the entire EFLAGS register, make usre you use PUSHFD not PUSHF. The difference is, PUSHFD uses Dword size flags so its not 16 bits but 32 bits.

so PUSHFD takes the flag and puses it to the stack, just like anything else.

opcode for PUSHFD is `9C`



`POPFD`:  There are some flags which will not be transferred from the stack to EFLAGS unless you are in ring 0. these are security purpose flags. we are generally operating on ring 3.   If you need to set the entiure EFLAGS register, make sure you use `POPFD` not just `POPF`, same 16 bit issue.

opcode for POPFD is `9D`

![flags](img/flags.gif)


###  Information Returned by CPUID Instruction

![return](img/cpuidreturn.png)


CPUIDs are also returning processor manufacturer ID strings. for example,
```
AMDisbetter

CyrixInstead

GenuineIntel

sis sis sis
```

This is what I got running the cpuid.c file in the source files

![cpuid](img/CPUID.png)

here `0x16` is actually what is stored in my `eax`. so nothing big.

according to CPUID return value, 0x16 gives this kind of information:

```
Skylake-based processors (proc base & max freq; Bus ref. freq)	0x16	0x8000 0008
```

Infortmation provided about the processor :

EAX: VirtualPhysical Address size
bits 7-0: Physical Address bits
bits 8-15: bVirtual address bits
bits 31-16: reserved


EBX: Reserved=0
ECX:  Reserved=0
EDC:  Reserved=0



>So, rings and modes:

**`Real mode`** is , when you restart the processor it enter the mode called `real mode` anb basically its like compatibility mode. For example when you run DOS now, it runs on this real mode. No virtual memory, no privilege rings, 16 bit mode.

thats it actually.

Most of the OSs run in protected mode. 

**`Protected Mode`**

this mode is the native state of te processor. Among the capabilirties of protected mode is the ability to directly execuce `Real-address mode`. 8086 software in a protected, multi taskin environment. This feature is called **virtual-8086 mode** althouh it is not actuually a processor mode. 

Virtual-8086 is just for backwards compatibility,.

Protected mode adds support for virtual memory and privile rings.

But when cpu is restarting is restarts in real mode and goes into protected mode. so there is a bootstrapping around 16 bit real mode.



**`System Management Mode(SMM) `**

This mode provides an operating system or executive with a transferred mechanism for implementing platform-spesific functions such as power management and system security. in this mode, you have reach all of the memory, hardware support. OS and hypervisor cannot reach to this level. 
The processor enters SMM when the external SMM interups pin (SMI#) is activated or an SMI is received from the advanced progammable interrupt controller (APIC)

THUS, SMM has become a popular target for advanced rootkit discussiuons since access to SMM is locked by BIOS, so that neither `ring 0` nor `VMX` hypervisors can access it! Thus, if VMX us more privileged than ring 0 ('ring -1'), SMM is more privileged than VMX('ring -2') because a hypervisor cant even read SMM memory.!

this is sick! once a kit is hooked to this level, it blocks that memory part and is hidden inside!

`https://github.com/jussihi/SMM-Rootkit` check this for SMM rootkit example




### Privilege Rings

- x86 rings are enforced by hardware.  
- you often hear that normal programs execute in ring 3(user space) and the privleged code executes in ring 0(kernel space).
- in order to understand rings, we need to understand a capability called segmentation


![rings](img/rings.png)


### Paravirtualized XEN!

Requires a modified guest os. 
what is paravirtualization?

Paravirtualization (PV) is an efficient and lightweight virtualization technique introduced by the Xen Project team, later adopted by other virtualization solutions. PV does not require virtualization extensions from the host CPU and thus enables virtualization on hardware architectures that do not support Hardware-assisted virtualization. However, PV guests and control domains require kernel support and drivers that in the past required special kernel builds, but are now part of the Linux kernel as well as other operating systems.

Paravirtualization implements the following functionality

Disk and Network drivers
Interrupts and timers
Emulated Motherboard and Legacy Boot
Privileged Instructions


so instad of guest OS is touching to the hardware, it touches an API and that API controls these requests. 


## SEGMENTATION

Segmentation provides a mechanism dividing the processor's addressable memory space ( called linead address space) into smaller protected address spaces called (segments)

When we talk about segmentation and segment registers, we are talking about their interactions with the `Linear Address Space` which is actually on the virtual memory **but maps to phsical memory 1 to 1 so it is safe to say that it is pyhical memory __at least until we start to talk about paging.__**