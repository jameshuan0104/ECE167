close all;

% Load CSV Data (Assumes it has three columns: X, Y, Z)
raw_data = readmatrix('mag_raw.csv');  % Replace with your actual CSV filename (change between acc/mag)
calibrated_data = readmatrix('mag_calibrated.csv');

X_raw = raw_data(:,1);  % First column: X
Y_raw = raw_data(:,2);  % Second column: Y
Z_raw = raw_data(:,3);  % Third column: Z

X_calibrated = calibrated_data(:,1);  % First column: X
Y_calibrated = calibrated_data(:,2);  % Second column: Y
Z_calibrated = calibrated_data(:,3);  % Third column: Z

% Create figure
figure;

% ----- Subplot 1: Raw Data -----
subplot(1,2,1); % 1 row, 2 columns, first plot
scatter3(X_raw, Y_raw, Z_raw, 10, 'r', 'filled');  
hold on;
[Xs, Ys, Zs] = sphere(30);  % Generate sphere coordinates
surf(Xs, Ys, Zs, 'FaceAlpha', 0.1, 'EdgeColor', 'none', 'FaceColor', 'k'); % Reference sphere
hold off;
xlabel('X-axis');
ylabel('Y-axis');
zlabel('Z-axis');
title('Raw Magnetometer Data');
grid on;
axis equal;
view(3);

% ----- Subplot 2: Calibrated Data -----
subplot(1,2,2); % 1 row, 2 columns, second plot
scatter3(X_calibrated, Y_calibrated, Z_calibrated, 10, 'b', 'filled');  
hold on;
surf(Xs, Ys, Zs, 'FaceAlpha', 0.1, 'EdgeColor', 'none', 'FaceColor', 'k'); % Reference sphere
hold off;
xlabel('X-axis');
ylabel('Y-axis');
zlabel('Z-axis');
title('Calibrated Magnetometer Data');
grid on;
axis equal;
view(3);

% Display
sgtitle('Raw vs Calibrated Magnetometer Data'); % Super title for both plots