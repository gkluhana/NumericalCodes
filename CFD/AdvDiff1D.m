%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% 1D ADVECTION_DIFFUSION  %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialize
% Physical Space:             u_t  + au_x = (d(x)u_x)_x
% Fourier Space :  \hat{u}_t -aik \hat{u} = - d \hat{u}
clear all, close all, clc
n  = 100;
L  = 1;
dx = 1/n;
d  = 0 ;
a  = 1 ;
tf = 1;

%%Initial Condition
x = linspace(0,1,n);
u0 = init(x,n)
% plot(x,u0)


%% Using MATLAB Function

%Choose scheme 
% A = upwind(n,dx);
% A = centered(n,dx);
A = upwind3(n,dx);

%% ODE45
tspan = [0:0.05:tf]
[t u_m]  = ode45('rhs',tspan,u0, [],A,dx,a);
% waterfall(x,t,u_m);
map = [0 0 0];
colormap(map);  
hold on
plot(x,u_m(size(t,1),:))

%% Spectral Stuff
hold on;
D       = dftmatrix(n,n);
lambda    = inv(D)* A * D;
z_0     = inv(D)*u0';
z_t     = z_0.*exp(diag(lambda)*tf);
u_s     = D*z_t;
plot(x,u_s)

%% Component wise spectral ?!



%% Exact Time Integration


%Exact solution
u_e = init(x-a*tf)
plot(x,u_e)

legend('upwind','centered','upwind3','exact')

