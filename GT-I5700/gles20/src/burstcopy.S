        .file   "burstcopy.s"
        .text
        .align  2
        .global burst8copy
        .type   burst8copy, %function
burst8copy:
        mov     ip, sp
        stmfd   sp!, {fp, ip, lr, pc}
        sub     fp, ip, #4
        stmfd sp!,{r0-r10}
loop1:
        ldmia r0!,{r3-r10}
        stmia r1!,{r3-r10}

        subs r2,r2,#1
        bne loop1
        
        ldmfd sp!,{r0-r10}
        sub     sp, fp, #12
        ldmfd   sp, {fp, sp, pc}


        .global burst8copy_one_dst
burst8copy_one_dst:
        mov     ip, sp
        stmfd   sp!, {fp, ip, lr, pc}
        sub     fp, ip, #4
        stmfd sp!,{r0-r10}
loop2:
        ldmia r0!,{r3-r10}
        str r3,[r1,#0]
        str r4,[r1,#0]
        str r5,[r1,#0]
        str r6,[r1,#0]
        str r7,[r1,#0]
        str r8,[r1,#0]
        str r9,[r1,#0]
        str r10,[r1,#0]

        subs r2,r2,#1
        bne loop2
        
        ldmfd sp!,{r0-r10}
        sub     sp, fp, #12
        ldmfd   sp, {fp, sp, pc}
        
