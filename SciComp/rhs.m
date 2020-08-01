function u_new = rhs(t,u,ik,k2,k4)
    u_new = zeros(length(u),1)
    u_new = (-1/2*ik.*(fft((ifft(u).^2))) + k2.*u - k4.*u)
end
