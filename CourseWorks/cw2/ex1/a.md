### Exercise 1

**a) Consider Figures 5-18 through 5-23. What changes would need to be made to these figures if the architectural limit for physical memory were to be increased to 64 PB?**

![4Kbyte Page Translation - Long Mode 5-Level Paging](./img/5_18.jpeg)

![4Kbyte PML5E, PML4E, PDPE and PDE - Long Mode](./img/PML5E_PML4E_PDPE_PDE.jpeg)

![4Kbyte PTE - Long Mode](./img/5_23.jpeg)

----

Pela imagem 5-18, temos que:

- 12 bits para Physical-Page Offset
- 9 bits para Page-Table Offset
- 9 bits para Page-Directory Offset
- 9 bits para Page-Directory Pointer Offset
- 9 bits para Page-Map Level-4 Offset
- 9 bits para Page-Map Level-5 Offset

12 + 9 + 9 + 9 + 9 + 9 = 57 bits

#### $2^{57}$

- 1 = KB
- 2 = MB
- 3 = GB
- 4 = TB
- **5 = PB**
- 6 = EB

$2^7$ = 128

Logo, $2^{57}$ = 128Pbyte

O que quer dizer que para aumentar para uma capacidade de
64Pbyte não precisamos de fazer nada.

Se, por outro lado, quisessemos diminuir a capacidade para 64PByte,
bastava que em vez dos 9 bits do Page-Map Level-5 Offset
tivessemos apenas 8.

12 + 9 + 9 + 9 + 9 + 8 = 56 bits

$2^{56}$ = 64PByte

O problema que daqui surge, é que com 8 bits no Page-Map Level 5 Offset apenas
conseguiriamos apontar para metade ($2^8$ = 256) dos endereços do Page-Map
Level 4 que continuaria com 9 bits ($2^9$ = 512)
