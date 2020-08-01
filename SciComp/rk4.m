function [t,y] = rk4(f,Tspan, y0,dt)

y(1,:) = y0;
n = floor(Tspan(2) - Tspan(1)) / dt;
t = Tspan(1):dt:Tspan(2)-dt;
for i = 1:length(t)-1
    k1= dt*f(t(i),y(i,:));
    k2= dt*f(t(i)+0.5*dt, y(i,:)+0.5*k1);
    k3= dt*f(t(i)+0.5*dt, y(i,:)+0.5*k2);
    k4= dt*f(t(i)+ dt, y(i,:)+k3);
    y(i+1,:) = y(i,:)+(1/6)*(k1+2*k2+2*k3+k4); 
end
