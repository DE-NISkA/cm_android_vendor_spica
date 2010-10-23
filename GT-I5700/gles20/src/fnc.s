        stmfd        sp!,{r2 - r6, lr}

        CMP      r0,#0xc
        BNE      |L1.47|
        LDMIA    r1!, {r3-r5}
        stmia    r2!, {r3-r5}
        
        MOV      r0,#1
        ldmfd        sp!, {r2 - r6, pc}

|L1.47|:        
        CMP      r0,#0x10
        BNE      |L1.48|
        LDMIA    r1!, {r3-r6}
        stmia    r2!, {r3-r6}
        
        MOV      r0,#1
        ldmfd        sp!, {r2 - r6, pc}
    
|L1.48|:                    
        CMP      r0,#0x8
        BNE      |L1.49|
        LDMIA    r1!, {r3-r4}
        stmia    r2!, {r3-r4}

        MOV      r0,#1
        ldmfd        sp!, {r2 - r6, pc}

|L1.49|:                    
        CMP      r0,#0x4
        BNE      |L1.100|
        LDR      r3,[r1]
        STR      r3,[r2]

        MOV      r0,#1
        ldmfd        sp!, {r2 - r6, pc}

|L1.100|:
        CMP      r0,#0x0
        BEQ      |L1.101|
        LDR      r3,[r1], #4 
        STR      r3,[r2], #4
        SUB      r0,r0,#4
        B        |L1.100|
|L1.101|:
        MOV         r0,#1
        ldmfd        sp!, {r2 - r6, pc}
