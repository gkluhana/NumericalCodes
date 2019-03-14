%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% 1D ADVECTION_DIFFUSION  %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Physical Space:             u_t  + au_x = (d(x)u_x)_x
% Fourier Space :  \hat{u}_t -aik \hat{u} = - d \hat{u}


n  = 1000;
L  = 1;
dx = 1/n;
d  = 0 ;
a  = 1 ;
%%Initial Condition

x  = 0:dx:L
u0 = sin(pi*x).^100
plot(x,u0)

%Using MATLAB Function
tspan = [0 1]
odefun = @(t,u)-a*j*k*
ode45(
