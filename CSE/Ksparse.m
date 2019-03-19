function Ksp = Ksparse(n)
%Ksparse for n = 8
vsp = sparse([2 -1 zeros(1,n-2)]);    %row
Ksp = toeplitz(vsp);                %shift circularly 
bsp = Ksp(:,2);                     %extract second column
usp = Ksp\bsp;                      %solve Ku = bsp
uuu = full(usp);                    %answer should have 1 on second element

