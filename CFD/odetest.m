n=200
dx= 1/n;
e1  = ones(n,1);
a= 1
A   = spdiags([e1 -e1], [-1 0], n ,n);
% A(1,n) = 1;
% A(n,1) = 1;
tspan  = [0:0.05:1];
x = linspace(0,1,n);
% u0 = exp(-(x-0.5).^2/(2*0.01))'
u0 = sin(pi*x).^100
plot(x,u0)

[t y]  = ode45('rhs',tspan,u0, [],A,dx,a);
waterfall(x,t,y);
map = [0 0 0];
colormap(map);