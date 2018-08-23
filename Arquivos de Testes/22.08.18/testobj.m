pontos = importdata('OBJETO.TXT');
pontos1 = pontos(72:300, 1:2);
pontos2 = pontos(301:523, 1:2);
pontos3 = pontos(524:758, 1:2);

theta1 = pontos1(:,2);
rho1 = pontos1(:,1);
polar(deg2rad(theta1), rho1);

hold on;

theta2 = pontos2(:,2);
rho2 = pontos2(:,1);
% theta2 = theta2 - 16;
polar(deg2rad(theta2), rho2);

theta3 = pontos3(:,2);
rho3 = pontos3(:,1);
% theta3 = theta3 - 32;
polar(deg2rad(theta3), rho3);