function d2 = MahalDist(dx, K)
%Mahalonobis distance
%Note:
%   dx = x2 - x1
%   K  = K1 + K2

%old matlab syntax (slower)
% d2 = dx' * inv(K) * dx;

%new matlab syntax (faster)
d2 = dx' * (K \ dx);
end