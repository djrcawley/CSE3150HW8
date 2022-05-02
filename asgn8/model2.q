B=Poisson(0.25)
A=FIFO()
C=ServerNormal(5,1)
X=Exit
D=Dispatch([(A,0.8),(X,0.2)])
B->A
A->C
C->D

