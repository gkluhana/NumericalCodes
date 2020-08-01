%% INITIAL CONDITION
function y = initialCondition(x)
%  y = sin(3*2*pi*x);
 y = cos(x/16).*(1 + sin(x/16))