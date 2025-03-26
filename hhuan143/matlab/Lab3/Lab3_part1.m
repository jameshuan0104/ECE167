%% Load Raw Data from EllipseXYData.m
% Load the simulated 2D ellipse data
run('EllipseXYData.m'); % Executes the script to load Xmeas, Ymeas, and R
x = Xmeas;              % Raw x-axis measurements
y = Ymeas;              % Raw y-axis measurements
R = 1;                  % Known radius (defined in EllipseXYData.m)

%% Calibrate Data Using Iterative Least Squares
kstep = 10;             % Number of calibration iterations
plotFlag = 1;           % 1 = show plots, 0 = suppress plots
[Atilde, Btilde] = CalibrateEllipseData2D(x, y, kstep, plotFlag);

%% Apply Calibration to Correct Raw Data
[x_cal, y_cal] = CorrectEllipseData2D(x, y, Atilde, Btilde);

