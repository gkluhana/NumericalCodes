%Lorenz Attractor

[t,y]=rk4(@lorenz,[0 100],[1 2 1],0.01);               %
% [T,y2] = ode45(@lorenz,[0 100],[1 2.0001 1]);       %


plot3(y(1,1),y(1,2),y(1,3));
hold on
for i=2:size(y,1)
          plot3(y(1:i,1),y(1:i,2),y(1:i,3),'r-');
%           plot3(y2(1:i,1),y2(1:i,2),y2(1:i,3),'b-');
          drawnow
end

%Van Der Pol

[t,y]=ode15s(@vanDerPol,[0 20],[0 2] ); 
plot(t,y(:,1));