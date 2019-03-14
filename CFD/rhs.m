function rhs = rhs(t, u , dummy, A , dx,a)

rhs  = (a/dx).*A*u;
