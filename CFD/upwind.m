function A = upwind(n,dx)
e1  = ones(n,1);

A   = spdiags([e1 -e1], [-1 0], n ,n);
A(1,n)=1
A = 1/dx * A