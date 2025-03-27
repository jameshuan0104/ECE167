Ro=eye(3);
angleX = deg2rad(-180 + (360 * rand));
angleY = deg2rad(-90 + (180 * rand));
angleZ = deg2rad(-180 + (360 * rand));
rotX=[1 0 0; 0 cos(angleX) -sin(angleX); 0 sin(angleX) cos(angleX)];
rotY=[cos(angleY) 0 sin(angleY); 0 1 0; -sin(angleY) 0 cos(angleY)];
rotZ=[cos(angleZ) -sin(angleZ) 0; sin(angleZ) cos(angleZ) 0; 0 0 1];
Ro=rotX*rotY*rotZ;

accelInertial=[0;0;-1];
accelReading=Ro*[0;0;-1];
magInertial=[1;0;0];
magReading=Ro*[1;0;0];

R = DCMfromTriad(magReading,accelReading,magInertial,accelInertial);
function euler_angles = dcmToEuler(DCM)
    % dcmToEuler Converts a Direction Cosine Matrix (DCM) to Euler angles (ZYX convention)
    % Input:
    %   DCM - 3x3 Direction Cosine Matrix
    % Output:
    %   euler_angles - 1x3 vector containing [roll, pitch, yaw] in radians

    % Ensure DCM is valid (orthogonality check)
    if size(DCM,1) ~= 3 || size(DCM,2) ~= 3
        error('Input must be a 3x3 matrix');
    end

    % Extract rotation angles using the ZYX convention
    if(DCM(3,1) > 1)
    pitch = asin(-DCM(3,1)); % theta (Y-axis rotation)

    % Check for singularity (Gimbal Lock condition)
    if abs(pitch - pi/2) < 1e-6
        roll = 0;
        yaw = atan2(DCM(1,2), DCM(2,2));
    elseif abs(pitch + pi/2) < 1e-6
        roll = 0;
        yaw = atan2(-DCM(1,2), -DCM(2,2));
    else
        roll = atan2(DCM(3,2), DCM(3,3));  % phi (X-axis rotation)
        yaw = atan2(DCM(2,1), DCM(1,1));   % psi (Z-axis rotation)
    end

    % Return Euler angles as a row vector [roll, pitch, yaw]
    euler_angles = [roll, pitch, yaw];
end

angles = dcmToEuler(R);