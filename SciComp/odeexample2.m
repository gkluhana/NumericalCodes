opts = odeset('RelTol',1e-3,'AbsTol',[1e-4 1e-4 1e-5]);
[T,Y] = ode23(@rigid, [0 1 2], [0 1 1], opts);
plot(T,Y(:,1), 'o--r',T,Y(:,2),'s--b',T,Y(:,3),'d--g')
xlabel('t'); ylabel('y');title('RelTol=1e − 3');