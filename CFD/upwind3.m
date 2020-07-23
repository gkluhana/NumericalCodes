function A=upwind3(n,dx)
e1  = ones(n,1);

A   = spdiags([-1/6*e1 e1 -1/2*e1 -1/3*e1], [-2 -1 0 1], n ,n);
A(n,1) = -2;
A(1,n)=1;
A(1,n-1) = -1/6;
A(n,1) = -1/3;

A = 1/(dx) * A;