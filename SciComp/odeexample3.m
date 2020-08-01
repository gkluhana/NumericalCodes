r= 10;
f = @(t,y)[y(2); r*(1-y(1)^2)*y(2) - y(1)]; 
[t,y]=ode45(f,[0 3*r],[0.5 1] ); 
plot(t,y(:,1),'o--r')

size(t)