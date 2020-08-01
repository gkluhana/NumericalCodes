%speed for making DFT matrix
n=10100

%Construct DFT Matrix by filling out entries - vectorized
tic 
    i= 0:1:n-1;
    j= 0:1:n-1;
    A= exp((2*pi*1i/n)*i'*j);
toc

%Construct DFT Matrix by filling fft routine 
tic
    A = fft(eye(n,n));  
toc
