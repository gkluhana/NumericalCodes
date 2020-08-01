for i=1:8
f =myfunc(i)
[t,y]=ode113(f,[0 5],[pi+exp(1) pi-exp(1)] ); 
plot(t,y(:,1));
fprintf('Solved function %d in %d time steps \n',i,length(t))
hold on
end