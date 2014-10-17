
R2SEG SEGMENT BYTE PUBLIC USE16 'CODE'
      ASSUME  CS:R2SEG, DS:NOTHING




        PUBLIC outp
outp            PROC  FAR
        push  bp
        mov   bp, sp
        mov   dx, [bp+8]
        mov   al, [bp+6]
        out   dx, al
        pop   bp
        ret   4
outp            ENDP




        PUBLIC outpw
outpw           PROC  FAR
        push  bp
        mov   bp, sp
        mov   dx, [bp+8]
        mov   ax, [bp+6]
        out   dx, ax
        pop   bp
        ret   4
outpw           ENDP




        PUBLIC inp
inp             PROC  FAR
        push  bp
        mov   bp, sp
        mov   dx, [bp+6]
        in    al, dx
        sub   ah, ah
        pop   bp
        ret   2
inp             ENDP




        PUBLIC inpw
inpw            PROC  FAR
        push  bp
        mov   bp, sp
        mov   dx, [bp+6]
        in    ax, dx
        pop   bp
        ret   2
inpw            ENDP

R2SEG   ENDS
   END
