function A = centered(n,dx)
e1  = ones(n,1);

A   = spdiags([e1 -e1], [-1 1], n ,n);
A(1,n)=1;
A(n,1)=-1;
A = 1/(2*dx) * A;