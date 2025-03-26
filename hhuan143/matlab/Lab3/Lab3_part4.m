close all
% Step 1: Generate Random Tumble Data
npts = 1000; % Number of sample points
[A_noise, H_noise, A_dist, B_dist] = CreateTumbleData(npts);

X_raw = A_noise(:,1);
Y_raw = A_noise(:,2);
Z_raw = A_noise(:,3);

% Step 2: Calibrate the Data
kstep = 10;  % Number of iterations for calibration
plotFlag = 1; % Set to 1 to visualize calibration process
[Atilde, Btilde] = CalibrateEllipsoidData3D(X_raw, Y_raw, Z_raw, kstep, plotFlag);

% Step 3: Correct the Data
[X_corr, Y_corr, Z_corr] = CorrectEllipsoidData3D(X_raw, Y_raw, Z_raw, Atilde, Btilde);



