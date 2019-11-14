clear
clc
% close all

pontos = importdata('3D1.TXT');
% pontos = pontos(1:17000, :);

rho = pontos(:,1);
theta = deg2rad(pontos(:,2) - 15);
phi = deg2rad(pontos(:,3));

x = rho .* sin(phi) .* cos(theta);
y = rho .* sin(phi) .* sin(theta);
z = rho .* cos(phi);

plot3(x, y, z, '.');