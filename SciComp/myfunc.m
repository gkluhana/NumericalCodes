function f= myfunc(i)
switch i
% f = @(t,y)[y/t];
% 
    case 1
    f = @(t,y)[sin(t^2)*y];
    case 2
    f = @(t,y)[sign(sin(t^2))*y];
    case 3    
    f = @(t,y)[sin(t^2)-y];
    case 4
    f = @(t,y)[sign(sin(t^2))-y];
    case 5
    f = @(t,y)[3*cos(t)-abs(y).^2.*y];
    case 6
    f = @(t,y)[3*cos(t)];
    case 7
    f = @(t,y)[3*cos(t)-y.^3];   
    case 8
    f = @(t,y)[3*cos(t)+0.5*log(1-abs(y)).*y];
end

