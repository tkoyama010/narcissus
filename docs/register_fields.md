register field
---

```

    31              23              15              7               0
    +---------------+---------------+---------------+---------------+
ER0 |               E0              |     R0H       |      R0L      |
    +---------------+---------------+---------------+---------------+
ER1 |               E1              |     R1H       |      R1L      |
    +---------------+---------------+---------------+---------------+
ER2 |               E2              |     R2H       |      R2L      |
    +---------------+---------------+---------------+---------------+
ER3 |               E3              |     R3H       |      R3L      |
    +---------------+---------------+---------------+---------------+
ER4 |               E4              |     R4H       |      R4L      |
    +---------------+---------------+---------------+---------------+
ER5 |               E5              |     R5H       |      R5L      |
    +---------------+---------------+---------------+---------------+
ER6 |               E6              |     R6H       |      R6L      |
    +---------------+---------------+---------------+---------------+
ER7 |               E7            (S|P)   R7H       |      R7L      |
    +---------------+---------------+---------------+---------------+

                    23                                              0
                    +---------------+---------------+---------------+
PC                  |                                               |
                    +---------------+---------------+---------------+

                                                    7               0
                                                    +---------------+
CCR                                                 |               |
                                                    +---------------+


+---------------+-------+
|register filed | 32bit | 
+---------------+-------+
| 000           | ER0   |
+---------------+-------+
| 001           | ER1   |
+---------------+-------+
| 010           | ER2   |
+---------------+-------+
| 011           | ER3   |
+---------------+-------+
| 100           | ER4   |
+---------------+-------+
| 101           | ER5   |
+---------------+-------+
| 110           | ER6   |
+---------------+-------+
| 111           | ER7   |
+---------------+-------+

+---------------+-------+-------+
|register filed | 16bit |  8bit | 
+---------------+-------+-------+
| 0000          |  R0   |  R0H  |
+---------------+-------+-------+
| 0001          |  R1   |  R1H  |
+---------------+-------+-------+
| 0010          |  R2   |  R2H  |
+---------------+-------+-------+
| 0011          |  R3   |  R3H  |
+---------------+-------+-------+
| 0100          |  R4   |  R4H  |
+---------------+-------+-------+
| 0101          |  R5   |  R5H  |
+---------------+-------+-------+
| 0110          |  R6   |  R6H  |
+---------------+-------+-------+
| 0111          |  R7   |  R7H  |
+---------------+-------+-------+
| 1000          |  L0   |  R0L  |
+---------------+-------+-------+
| 1001          |  L1   |  R1L  |
+---------------+-------+-------+
| 1010          |  L2   |  R2L  |
+---------------+-------+-------+
| 1011          |  L3   |  R3L  |
+---------------+-------+-------+
| 1100          |  L4   |  R4L  |
+---------------+-------+-------+
| 1101          |  L5   |  R5L  |
+---------------+-------+-------+
| 1110          |  L6   |  R6L  |
+---------------+-------+-------+
| 1111          |  L7   |  R7L  |
+---------------+-------+-------+

```
