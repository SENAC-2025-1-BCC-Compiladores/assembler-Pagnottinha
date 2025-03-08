; SOMA

.data
    A DB 15
    B DB 0xAF
    X DB ?

.code
.org 10
    LDA A ; ac = 5
    ADD B ; ac = 5 + 7 (C)
    STA X ; x = ac = C
    HLT