function A = centered2(n,dx);
e1 = ones(n,1);

A = spdiags([-e1 2*e1 -e1], [-1:1],n,n);
A(1,n) = -1;
A(n,1) = -1;

A = 1/(dx^2) *A;

