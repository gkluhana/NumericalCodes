%%BS and KS equatons

tic
n       = 2^8;
L       = 32*pi
dx      = (L-0)/n;
x       = (0:dx:L-dx)';
T       = 10;
u0      = initialCondition(x);
u0_hat  = fft(u0);
dt      = 10e-2;
Tspan   = 0:dt:T
% u_t = -0.5*(u^2)_x - u_{xx} - u_{xxxx}

ik = (1i*2*pi*[0:n/2-1 0 -n/2+1:-1])';
k2 = (2*pi*[0:n/2 -n/2+1:-1]').^2;
k4 = k2.^2;
f = @(t,u) (-1/2*ik.*(fft((ifft(u).^2))) + k2.*u - k4.*u)
[t,u_hat] = ode45(f, Tspan, u0_hat,Options);
toc
for i = 1:length(t)
    u  = real(ifft(u_hat(i,:)));
    plot(x,u)
    ylim([-1,1])
    drawnow
end
