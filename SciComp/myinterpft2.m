close all, clear

%Sample a non-smooth function
n= 400
dx = 1/n
x = 0:dx:1-dx
y = triangularPulse(0.45,0.55,x)
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
ye = triangularPulse(0.45,0.55,X)
plot(X,ye)

legend('interp1','exact')

figure

%FFT interpolation
N=1000
yf = interpft(y,1000)
dyf= dx*length(x)/N;
xf = 0:dyf:1-dyf
yf = yf(1:length(xf));
plot(xf,yf,'x')

hold on
%"Exact" Solution
plot(X,ye)


legend('interpft','exact')


