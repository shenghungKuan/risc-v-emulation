# RISC-V emulator
This is an emulator in C for a subset of the RISC-V Instruction Set Architecture (ISA).  
It also includes an implementation of a processor cache simulator for the following cache types:
* A direct mapped cache with a block size on 1 word
* A direct mapped cache with a block size of 4 words
* A 4-way set associative cache with a block size of 1 word and LRU slot replacement
* A 4-way set associative cache with a block size of 4 words and LRU slot replacement
## Usage
```
cd <project directory>
make
./main [-a | -dm <cache size> <block size> | -sa <cache size> <block size>] <prog> [<arg1> ...]
```
* -a: instruction analysis
* -dm: direct mapped cache with (cache size) blocks of size of (block size) words
* -sa: set associative cache with (cache size) blocks of size of (block size) words
* prog: program to be executed, including quadratic, midpoint, max3, to_upper, get_bitseq, get_bitseq_signed, swap, sort, and fib_rec
* arg: integer arguments to be passed in
## Sample Usage
[quadratic]
```
./main quadratic 2 4 6 8
C: 36
Asm: 36
Emu: 36
```

[midpoint]
```
./main midpoint 0 4
C: 2
Asm: 2
Emu: 2
```

[max3]
```
./main max3 2 10 8
C: 10
Asm: 10
Emu: 10
```

[to_upper]
```
./main to_upper FooBar
C: FOOBAR
Asm: FOOBAR
Emu: FOOBAR
```

[get_bitseq_1]
```
./main get_bitseq 94116 12 15
C: 6
Asm: 6
Emu: 6
```

[get_bitseq_2]
```
./main get_bitseq 94117 4 7
C: 10
Asm: 10
Emu: 10
```

[get_bitseq_signed_1]
```
./main get_bitseq_signed 94117 12 15
C: 6
Asm: 6
Emu: 6
```

[get_bitseq_signed_2]
```
./main get_bitseq_signed 94117 4 7
C: -6
Asm: -6
Emu: -6
```


[fib_rec]
```
./main fib_rec 10
C: 55
Asm: 55
Emu: 55
```

[swap_1]
```
./main swap 0 1 22 3]
C: 33 22
Asm: 33 22
Emu: 33 22
```

[swap_2]
```
./main swap 4 3 11 22 33 55 66 77
C: 11 22 33 66 55 77
Asm: 11 22 33 66 55 77
Emu: 11 22 33 66 55 77
```


[sort]
```
./main sort 6 4 1 2 5 3
C: 1 2 3 4 5 6
Asm: 1 2 3 4 5 6
Emu: 1 2 3 4 5 6
```

[quadratic_analysis]
```
./main -a quadratic 2 4 6 8
C: 36
Asm: 36
Emu: 36
=== Analysis
Instructions Executed  = 6
R-type + I-type        = 5 (83.33%)
Loads                  = 0 (0.00%)
Stores                 = 0 (0.00%)
Jumps/JAL/JALR         = 1 (16.67%)
Conditional branches   = 0 (0.00%)
  Branches taken       = 0 (0.00%)
  Branches not taken   = 0 (0.00%)
```

[sort_analysis]
```
./main -a sort 80 70 60 50 40 30 20 10
C: 10 20 30 40 50 60 70 80
Asm: 10 20 30 40 50 60 70 80
Emu: 10 20 30 40 50 60 70 80
=== Analysis
Instructions Executed  = 910
R-type + I-type        = 353 (38.79%)
Loads                  = 225 (24.73%)
Stores                 = 169 (18.57%)
Jumps/JAL/JALR         = 92 (10.11%)
Conditional branches   = 71 (7.80%)
  Branches taken       = 8 (11.27%)
  Branches not taken   = 63 (88.73%)
```

[sort_dm_32_1]
```
./main -dm 32 1 sort 80 70 60 50 40 30 20 10 8 7 6 5 4 3 2 1
C: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Asm: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Emu: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
=== Cache (I)
Type          = direct mapped
Size          = 32 slots
Block size    = 1 words
Ways          = 1
References    = 3802
Hits          = 1381 (36.32% hit ratio)
Misses        = 2421 (63.68% miss ratio)
Misses (cold) = 31
Misses (hot)  = 2390
% Used        = 96.88%
```

[sort_dm_32_4]
```
./main -dm 32 4 ort 80 70 60 50 40 30 20 10 8 7 6 5 4 3 2 1
C: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Asm: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Emu: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
=== Cache (I)
Type          = direct mapped
Size          = 32 slots
Block size    = 4 words
Ways          = 1
References    = 3802
Hits          = 3077 (80.93% hit ratio)
Misses        = 725 (19.07% miss ratio)
Misses (cold) = 8
Misses (hot)  = 717
% Used        = 100.00%
```

[sort_sa_32_1_4]
```
./main -sa 32 1 4 sort 80 70 60 50 40 30 20 10 8 7 6 5 4 3 2 1
C: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Asm: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Emu: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
=== Cache (I)
Type          = set associative
Size          = 32 slots
Block size    = 1 words
Ways          = 4
References    = 3802
Hits          = 2500 (65.75% hit ratio)
Misses        = 1302 (34.25% miss ratio)
Misses (cold) = 32
Misses (hot)  = 1270
% Used        = 100.00%
```

[sort_sa_32_4_4]
```
./main -sa 32 4 4 sort 80 70 60 50 40 30 20 10 8 7 6 5 4 3 2 1
C: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Asm: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
Emu: 1 2 3 4 5 6 7 8 10 20 30 40 50 60 70 80
=== Cache (I)
Type          = set associative
Size          = 32 slots
Block size    = 4 words
Ways          = 4
References    = 3802
Hits          = 3181 (83.67% hit ratio)
Misses        = 621 (16.33% miss ratio)
Misses (cold) = 8
Misses (hot)  = 613
% Used        = 100.00%
```
