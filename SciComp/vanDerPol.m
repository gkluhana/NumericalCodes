function dydt = vanDerPol(t,y)
mu=10
dydt = [y(2); mu*(1-y(1)^2)*y(2)-y(1)];