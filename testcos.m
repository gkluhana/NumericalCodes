delta = 0.5
f = 2
t = 0:0.001:100
x = 0.5/2 * (1+ 0.25 * cos(2*pi*f.*t))
figure
plot(x,t)