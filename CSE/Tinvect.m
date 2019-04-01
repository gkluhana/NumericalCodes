function [T,Ti] = Tinvect(n)

e1  = ones(n,1);
U   = spdiags([e1 -e1], [0, 1] , n ,n) 
Ui  = triu(ones(n,n))                       %Sum Matrix
I   = U*Ui                                  %should be identity
T   = U'*U                                  %T = U' * U
Ti  = Ui*Ui'                                %Tinverse