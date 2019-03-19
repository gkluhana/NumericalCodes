function [S,U] = sumdifference(n)
U = eye(n) - diag(ones(n-1,1),1);
S = triu(ones(n));