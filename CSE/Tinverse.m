function T = Tinverse(n)

%Can this be vectorized? Sum Matrix

d = n:-1:1
T = diag(d,0);

for i=1:n
    for j=1:n
        if i > j
            T(i,j) = n - (i - 1);
        else
            if i < j
            T(i,j) = n - (j - 1);
            end
            %do nothing
        end
    end
end
