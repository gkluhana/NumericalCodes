close all, clear

%Sample a smooth function
n= 50
dx = 1/n
x = 0:dx:1-dx
y = sin(pi*x).^100
plot(x,y,'o')

figure
hold on

%Interp1
N=500;
dX = 1/N
xq=0:dX:1-dX
yq = interp1(x,y,xq)
plot(xq,yq,'o')

%"Exact" Solution
X = 0:dX:1-dX
ye = sin(pi*X).^100
plot(X,ye)

legend('interp1','exact')

figure

%FFT interpolation
N=500
yf = interpft(y,N)
dyf= dx*length(x)/N;
xf = 0:dyf:1-dyf
yf = yf(1:length(xf));
plot(xf,yf,'x')

hold on
%"Exact" Solution
n= 1000
dx = 1/n
x = 0:dx:1-dx
ye = sin(pi*x).^100
plot(x,ye)


legend('interpft','exact')


