function A = dftmatrix(n,m)
i= 0:1:n-1;
j= 0:1:m-1;


A= exp((2*pi*1i/n)*i'*j);
