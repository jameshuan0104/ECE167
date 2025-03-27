% Step 1: Collect Accelerometer Data
clear;
clear clc;
close all;



% % Define filenames
% file1 = 'AccelMagTumble.csv';
% file2 = 'AccelMagTumble2Min.csv';
% 
% % Read both CSV files into tables
% data1 = readtable(file1, 'PreserveVariableNames', true);
% data2 = readtable(file2, 'PreserveVariableNames', true);
% 
% % Combine the two tables (vertically stack rows)
% combinedData = [data1; data2];  % or combinedData = vertcat(data1, data2);
% 
% writetable(combinedData, 'AccelMagTumble3min.csv');
% 

%read from file
filename = 'test1.csv';  
data = readtable(filename, 'PreserveVariableNames', true);

% Remove the last row
data(end, :) = [];  %because of way teleplot converts to csv

ARaw = [data.Accel_z, data.Accel_y, data.Accel_x];

BRaw = [data.Mag_z, data.Mag_y, data.Mag_x];





% maxRows = 10000;
% ARaw = ARaw(1:min(end, maxRows), :);
% BRaw = BRaw(1:min(end, maxRows), :);

% norm_ARaw = vecnorm(ARaw,2,2);
% norm_BRaw = vecnorm(BRaw,2,2);
% 
% ANorm = ARaw ./ norm_ARaw;  
% BNorm = BRaw ./ norm_BRaw;

inertialAccel = [-1;0;0];
inertialMag = [41237; 23223; 5192];

rmishat = eye(3);



misAlignmentMatrix = AlignPrimarySecondary(ARaw',BRaw',inertialAccel,inertialMag);


[z,y,x] = dcm2angle(misAlignmentMatrix);


disp(rad2deg(z));
disp(rad2deg(y));
disp(rad2deg(x));

