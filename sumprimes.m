n = 2000000
B =  ones(1,n);
B(1) = 0;
s = 0
primes = []
tic
for i = 2:n
    if B(i) == 1
       primes = [primes,i];
       j=i;
       s = s+ i;
       while j*i <= n
          B(j*i) = 0;
          j = j+1;
       end
    end
end
toc
s
