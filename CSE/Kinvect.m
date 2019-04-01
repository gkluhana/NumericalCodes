function Ki = Kinvect(n)
a = 1:n;
J = repmat(a',1,n);
I = repmat(a,n,1);
Ti = (n +1) - max(I,J) ;
Ki = Ti - 1/(n+1) * (n-I+1).*(n-J+1); %Using Ti-Ki formula, check TiMinusKi
