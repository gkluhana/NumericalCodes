function c = mybisection(f,I) 
%f is the function handle,
% I is the interval
c = (I(1)+I(2))/2;

while abs(f(c)) > 1e-6 
    a = I(1);
    b = I(2);
    if f(a)*f(c) > 0 % same sign
        I = [c b];
    else
        I = [a c];
    end
    c = (I(1)+I(2))/2
end