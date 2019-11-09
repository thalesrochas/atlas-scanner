clear
clc
% close all

pontos = importdata('J1565.TXT');
pontos1 = pontos(195:483, 1:2);
pontos2 = pontos(484:770, 1:2);
pontos3 = pontos(790:797, 1:2);
% 15.6
% 15.65
% 15.7
figure;
plot(pontos(:, 2));

theta = pontos(:,2);
rho = pontos(:,1);
figure;
polar(deg2rad(theta), rho);

% theta1 = pontos1(:,2);
% rho1 = pontos1(:,1);
% figure;
% polar(deg2rad(theta1), rho1);
% 
% hold on;
% 
% theta2 = pontos2(:,2);
% rho2 = pontos2(:,1);
% polar(deg2rad(theta2), rho2);
% 
% theta3 = pontos3(:,2);
% rho3 = pontos3(:,1);
% polar(deg2rad(theta3), rho3);