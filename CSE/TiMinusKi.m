function R = TiMinusKi(n)
a = 1:n;
J = repmat(a',1,n);
I = repmat(a,n,1);
R = 1/(n+1) * (n-I+1).*(n-J+1);
