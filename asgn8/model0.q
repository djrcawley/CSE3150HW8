B=Poisson(0.25)
A=FIFO()
C=ServerExp(0.25)
X=Exit
B->A
A->C
C->X
