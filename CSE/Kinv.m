function Ki = Kinv(n)
Ki = zeros(n,n);
for i = 1:n
    for j = 1:i
        Ki(i,j) = (n-i+1)*j;
    end
end

Ki = Ki + tril(Ki,-1)';
Ki = 1/(n+1) * Ki;

