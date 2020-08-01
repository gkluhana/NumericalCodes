%% Part 2
n=100;
dx = 1/n;

%Main Diagonal 
d0 = -3*ones(n,1);
%Off Diagonal 1
d1 = 6*ones(n-1,1);
%Off Diagonal 2
d2 = ones(n-2,1);
%Off Diagonal -1
d3 = -2*ones(n-1,1);
% Construct A
AG = diag(d0,0)+diag(d1,1)+diag(d2,2) + diag(d3,-1);

%Fill in Wrapped Around Values
AG(1,n)=-2;
AG(n,1) = 6;
AG(n,2) = 1;
AG(n-1,1) = 1

%Multiply with the dx factor
A = dx*AG;


%% Part 4
f =  @(t,y)A*y

%% Part 5 
%Create Initial Condition Vector
x = 0:dx:1-dx
y0 = initialCondition(x);

%Solve System using ODE45
[t,y] = ode45(f,[0 2],y0)

%Plot Solution
plot(x,y(length(t),:))

%% Part 6
%Exact solution is u = u0(x-at) = u0(x-2)
exact = initialCondition(x-2);
hold on
plot(x,exact)

%Label Graphs
legend('ode5','exact')
