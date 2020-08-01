%Code by Nick Trefethen
N=8;
h=2*pi/N; x = h*(1:N)';
v = exp(sin(x)); vprime = cos(x).*v; vprimeprime = v.*(cos(x).^2-sin(x));
% plot(x,v,x,vprime,x,vprimeprime);
% legend('function','first derivative','second derivative')
% plot(x,v,'o')

%Compute the derivatives of v using FFT 
v_hat = fft(v);
ik = 1i*[0:N/2-1 0 -N/2+1:-1]';
w_hat = ik.*v_hat;
w = real(ifft(w_hat));

%Second Derivative 
K = [0:N/2 -N/2+1:-1]'
W_hat = -(K).^2.*v_hat;
W = real(ifft(W_hat));

%Two successive first derivatives
W_hatnew = ik.*w_hat;
W_new = real(ifft(W_hatnew));


%Compute Errors
error = norm(w-vprime,inf);
error2 = norm(W-vprimeprime,inf);
error3 = norm(W_new-vprimeprime,inf);

%Plot Numerical Solution
plot(x,w,'o',x,W,'o',x,W_new,'x')
hold on
% figure
% plot(x,vprimeprime,x,W,'o')

%Plot 'Exact' Solution
N=200;
h=2*pi/N; x = h*(1:N)';
v = exp(sin(x)); vprime = cos(x).*v; vprimeprime = v.*(cos(x).^2-sin(x));
plot(x,v,x,vprime,x,vprimeprime) 
legend('w','W','W_new','v','vprime','vprimeprime')