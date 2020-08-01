function [t y] = myeuler(f,Tspan,y0,dt)
t = Tspan(1):dt:Tspan(2);
y(:,1) = y0;
for i = 1:length(t)-1
    y(:,i+1) =y(:,i) + dt*f(t(i),y(:,i)) ;
end


