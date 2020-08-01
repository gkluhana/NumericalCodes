function A = idftmatrix(n,m)
% i= 0:1:n-1;
% j= 0:1:m-1;
% 
% 
%A= exp((-2*pi*1i/n)*i'*j);
%A = (1/n)*A

A = ifft(eye(n,n))