A = 25; B = 23;
dom = [0 20]; x = chebfun('x',dom);
tmax = 0.0156;
S = spinop(dom,[0 tmax]);
S.lin = @(u) - diff(u,3);
S.nonlin = @(u) -.5*diff(u.^2); % spin cannot parse "u.*diff(u)"
S.init = 3*A^2*sech(.5*A*(x-3)).^2 + 3*B^2*sech(.5*B*(x-4)).^2;

N = 800;   % numer of grid points
dt = 5e-6; % time-step
tic, u = spin(S,N,dt,'plot','off'); time_in_seconds = toc;
plot(S.init), hold on, plot(u), hold off
text(4.4,1300,'t = 0'), text(13.5,1300,'t = 0.0156')


% 
% S.init = 3*A^2*sech(.5*A*(x-3)).^2;
% u = spin(S,N,dt,'plot','off');
% plot(S.init), hold on, plot(u), hold off
% text(3.4,1300,'t = 0'), text(13.2,1300,'t = 0.0156')
