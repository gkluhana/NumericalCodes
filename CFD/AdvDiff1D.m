%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% 1D ADVECTION_DIFFUSION  %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialize
% Physical Space:             u_t  + au_x = (d(x)u_x)_x
% Fourier Space :  \hat{u}_t -aik \hat{u} = - d \hat{u}

close, clear

n  = 2000;
L  = 1;
dx = 1/n;
d  = 0 ;
a  = 1 ;
tf = 1;

%%Initial Condition
x = linspace(0,1,n);
u0 = init(x,n);
%  plot(x,u0);
leg = cell(0);

%% Using MATLAB Function

%Choose scheme 
 A = upwind(n,dx);
%  A = centered(n,dx);
% A = upwind3(n,dx);
 B = centered2(n,dx);
%{
%% ODE45
tspan = [0:0.05:2];
rhs = @(t,u)a*A*u;
[t u_m]  = ode45(rhs,tspan,u0);
%   waterfall(x,t,u_m);
%  map = [0 0 0];
%  colormap(map);  
% 
plot(x,u_m(length(t),:));
leg{end+1} = ['ode45 with FD']
% hold off
%Animate plots 
% for i = 1:length(t)
%     plot(x,u_m(i,:))
%     drawnow
%     pause(0.5)
% end

hold on
%}
%% Exponential Time differencing stuff

%%DFT MATRIX using FFT
lambda  = fft(full(A(1,:)+0.0001*B(1,:))); 
z_0     = ifft(u0);
z_t     = exp((lambda)*tf).*z_0;
u_s     = real(fft(z_t));
plot(x,u_s)
leg{end+1} = 'ETD with FD'
hold on
%{
%% Use myeuler
dt =0.001;

rhs = @(t,u)a*A*u;
[t,u_e] = myeuler(rhs,[0 tf],u0,dt);
plot(x,u_e(:,length(t)))
leg{end+1} = 'myeuler with FD'

%%Animate plots 
% for i = 1:length(t)
%     plot(x,u_e(:,i))
% %     ylim([-0.5,1.5])
%     drawnow
%     pause(0.5)
% end

%% Spectral Discretization in Space
ik = 1i*(2*pi)*[0:n/2-1 0 -n/2+1:-1]';
rhs = @(t,u_hat)a*ik.*u_hat
u0_hat = fft(u0);
[t,u_hat] = ode45(rhs, [0 tf],u0_hat);
u_pseudo = real(ifft(u_hat(length(t),:)));
plot(x,u_pseudo,'o')
leg{end+1}= 'ode45 with Spectral'

%% Spectral Discretization in Space and ETD
ik = 1i*(2*pi)*[0:n/2-1 0 -n/2+1:-1]';
u0_hat = fft(u0);
u_hat = exp(ik)*u0_hat;
u_psuedo = real(ifft(u_hat));
plot(x,u_pseudo,'x');
leg{end+1}='ETD with Spectral'

%% Spectral Discretization in Space with myeuler
ik = 1i*(2*pi)*[0:n/2-1 0 -n/2+1:-1]';
dt =0.0001;
rhs = @(t,u_hat)a*ik.*u_hat;
u0_hat = fft(u0);
[t,u_hat] = myeuler(rhs, [0 tf],u0_hat',dt);
u_pseudo = real(ifft(u_hat(:,length(t))));
plot(x,u_pseudo)
leg{end+1}= 'myeuler with Spectral'

%% Exact Time Integration

%Exact solution
u_e = init(x-a*tf);
plot(x,u_e,'.-');
leg{end+1}='exact'
%% Legend
legend(leg)

%% Convergence Of Methods
%}

%% Spectral Discretization in Space and ETD
ik = 1i*(2*pi)*[0:n/2-1 0 -n/2+1:-1];
k2 =(2*pi* [0:n/2 -n/2+1:-1]).^2;
u0_hat = fft(u0);
u_hat = exp(tf*(a*ik-0.0001*k2)).*u0_hat;
u_psuedo = real(ifft(u_hat));
plot(x,u_psuedo);
leg{end+1}='ETD with Spectral';

%%


