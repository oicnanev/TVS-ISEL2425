### Exercise 1

**b) Suppose AMD wants to support virtual address spaces of 512 PB. What changes would need to be made to Figure 5.18? How many different "Page-Map Level-5" tables could then exist for a single process? Read sections 1.1.3 and 5.3.1 and indicate the ranges of canonical addresses for the proposed extension.
**

----

![Figure 5.18](./img/5_18.jpeg)

![section 1.1.3](./img/section_1_1_3.jpeg)

![section 5.3.1](./img/section_5_3_1.jpeg)

----

Se quisessemos estender o suporte a 512PB, teriamos de usar 59 bits. $2^{59}$ = 512PB

Uma das formas, seria cria um *Page-Map Level-6* (PML6) com apenas 2 bits, que poderia apontar para 4 *Page-Map Level-5* (PML5) distintas, o que quer dizer que cada processo poderia ter até 4 PML5.

Ficariamos assim com:

- 12 bits de offset
-  9 bits de Page-Table Index
-  9 bits de Page-Directory Index
-  9 bits de Page-Directory pointer Index
-  9 bits de Page-Map Level-4
-  9 bits de Page-Map Level-5
-  2 bits de Page-Map Level-6
-  5 bits de extensão do sinal do bit 58:
    * bit 58 a **1** - **endereços para kernel mode**
        + do 0xF800 0000 0000 0000 ao 0xFFFF FFFF FFFF FFFF
    * bit 58 a **0** - **endereços para user mode**
        + do 0x0000 0000 0000 0000 ao 0x07FF FFFF FFFF FFFF

