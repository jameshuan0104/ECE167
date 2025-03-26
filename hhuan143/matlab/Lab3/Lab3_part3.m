close all;

% Load CSV Data (Assumes it has three columns: X, Y, Z)
raw_data = readmatrix('gyro_10sec.csv');  % Replace with your actual CSV filename
raw_data_min = readmatrix('gyro_10min.csv');

X_raw = raw_data(:,1);  % First column: X
Y_raw = raw_data(:,2);  % Second column: Y
Z_raw = raw_data(:,3);  % Third column: Z

X_raw_min = raw_data_min(:,1);  % First column: X
Y_raw_min = raw_data_min(:,2);  % Second column: Y
Z_raw_min = raw_data_min(:,3);  % Third column: Z

% Compute the mean for each axis
X_avg = mean(X_raw);
Y_avg = mean(Y_raw);
Z_avg = mean(Z_raw);

X_avg_min = mean(X_raw_min);
Y_avg_min = mean(Y_raw_min);
Z_avg_min = mean(Z_raw_min);

% Display results
fprintf('Average X: %.2f\n', X_avg);
fprintf('Average Y: %.2f\n', Y_avg);
fprintf('Average Z: %.2f\n', Z_avg);

fprintf('Average X: %.2f\n', X_avg_min);
fprintf('Average Y: %.2f\n', Y_avg_min);
fprintf('Average Z: %.2f\n', Z_avg_min);

angle_x = cumtrapz((X_raw_min - -14) / 17000) * 0.02 * 180;
angle_y = cumtrapz((Y_raw_min - -17.5) / 17000) * 0.02 * 180;
angle_z = cumtrapz((Z_raw_min - 5.7) / 17000) * 0.02 * 180;

% Create time vector
time = (0:length(X_raw_min)-1) * 0.02;  % Time in seconds

% Plot results
figure;
plot(time, angle_x, 'r', 'LineWidth', 1.5); hold on;
plot(time, angle_y, 'g', 'LineWidth', 1.5);
plot(time, angle_z, 'b', 'LineWidth', 1.5);
hold off;

% Formatting the plot
xlabel('Time (s)');
ylabel('Angle (Degrees)');
title('Integrated Gyroscope Data - Angle Over Time');
legend('Angle X', 'Angle Y', 'Angle Z');
grid on;

% For gyro degree calibration
raw_data_x = readmatrix('gyro_x.csv');
X_raw = raw_data_x(:,1); 

raw_data_y = readmatrix('gyro_y.csv');
Y_raw = raw_data_y(:,2); 

raw_data_z = readmatrix('gyro_z.csv');
Z_raw = raw_data_z(:,3); 

% Length of the data
num_samples_X = length(X_raw);
num_samples_Y = length(Y_raw);
num_samples_Z = length(Z_raw);

% Sampling interval (in seconds)
sampling_interval = 0.02;  % 20 ms between each sample

% Create the time vector (starting from 0 and incrementing by sampling_interval)
time_x = (0:num_samples_X-1) * sampling_interval;  % Time vector starting from 0
time_y = (0:num_samples_Y-1) * sampling_interval;  % Time vector starting from 0
time_z = (0:num_samples_Z-1) * sampling_interval;  % Time vector starting from 0

angles_x = cumtrapz(time_x, X_raw);
angles_y = cumtrapz(time_y, Y_raw);
angles_z = cumtrapz(time_z, Z_raw);

figure;
plot(time_z, angles_z);
xlabel('Time (seconds)');
ylabel('Sensor Value');
title('Gyro Z');
grid on;

figure;
plot(time_x, angles_x);
xlabel('Time (seconds)');
ylabel('Sensor Value');
title('Gyro X');
grid on;

figure;
plot(time_y, angles_y);
xlabel('Time (seconds)');
ylabel('Sensor Value');
title('Gyro Y');
grid on;