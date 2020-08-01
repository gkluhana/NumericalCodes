relerror = [];
Tf = 1;
i = 1;
for i = 1:10
%for i = 2:2:16
n  = 100*i;
L  = 1;
dx = 1/n;
d  = 0 ;
a  = 1 ;
tf = Tf;

%%Initial Condition
x = linspace(0,1,n);
u0=init(x,n);
%  plot(x,u0);
leg = cell(0)

%% Using MATLAB Function

%Choose scheme
%A = upwind(n,dx);
 A = centered(n,dx);
%  A = upwind3(n,dx);
 
%% Exact Integration in Time
lambda  = fft(full(A(1,:)));
z_0     = ifft(u0);
z_t     = exp((lambda)*tf).*z_0;
u_s     = real(fft(z_t));
%plot(x,u_s)
%% Exact solution
u_exact = init(x-a*tf);

%% Relative Error
error = abs(u_exact - u_s);

relerror =[relerror, norm(u_exact - u_s,1)/norm(u_s,1)];
plot(x,error)
hold on
end
hold off
for i = 1:length(relerror)-1;
	order(i) = relerror(i)/relerror(i+1);
end


