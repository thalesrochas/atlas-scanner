pontos = importdata('TESTEDELAY1000.TXT');
pontos1 = pontos(84:336, 1:2);
pontos2 = pontos(337:500, 1:2);

theta1 = pontos1(:,2);
rho1 = pontos1(:,1);
polar(deg2rad(theta1), rho1);

hold on;

theta2 = pontos2(:,2);
rho2 = pontos2(:,1);
% theta2 = theta2 - 16;
polar(deg2rad(theta2), rho2);