# Transparent MIPS Architecture Simulator

Seeking to demonstrate the operation of the MIPS architecture, this simulator was built in order to perform a series of
instructions belonging to the architecture's ISA. Running directly in the terminal, the instructions are executed one by one,
always showing the impact they have on the structure of the machine. After the execution of each instruction, the state
of the machine is always updated, making it possible to visualize the contents of the registers, of the different levels of the memory
cache, page table, main memory, etc. In addition, it also allows checking the hit rate of caches and
of TLB.

This software was developed as a project of the discipline of Computer Architecture and Organization II, being thought of
in order to provide a transparent view of how the architecture works. It will be seen that it constitutes a didactic software,
proving to be interesting especially for students who are learning about Computer Architecture and Organization.

# Instructions for use:

To install:

```console
$ make
```

To use, just enter the commands you want in the "instructions.dsk" file, and then run:

```console
$./main
```

# Screenshots
 At startup:
![Startup](https://user-images.githubusercontent.com/50810498/185251508-d9ec82db-e024-4d1a-8924-a72745ead000.png)

Components and their content:
![Components and their content](https://user-images.githubusercontent.com/50810498/185251717-2053a572-9929-49f4-ae26-161b65b42881.png)

After some commands have been executed:
![after some commands have been executed](https://user-images.githubusercontent.com/50810498/185251826-5399e292-7555-47d9-8ed1-7e58efd41f52.png)

