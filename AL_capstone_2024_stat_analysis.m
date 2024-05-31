lat_actual = [18.009988,18.009882,18.009926,18.009964,18.009855,18.010001,18.010001,18.009855];
lon_actual = [-76.775228,-76.775222,-76.775218,-76.775207,-76.775248,-76.775204,-76.775248,-76.775204];
lat_gps = [18.00981522,18.00982857,18.00984955,18.00982094,18.00980759,18.00990237,18.00988997,18.0098163];
lon_gps = [-76.7753067,-76.77541351,-76.77533722,-76.77528381,-76.77547455,-76.77535134,-76.77438656,-76.77534256];

p_lat = polyfit(lat_actual, lat_gps, 1); 
lat_pred = polyval(p_lat, lat_actual); 

p_lon = polyfit(lon_actual, lon_gps, 1); 
lon_pred = polyval(p_lon, lon_actual); 


SSres_lat = sum((lat_gps - lat_pred).^2); 
SStot_lat = sum((lat_gps - mean(lat_gps)).^2); 
R_squared_lat = 1 - SSres_lat/SStot_lat;


SSres_lon = sum((lon_gps - lon_pred).^2); 
SStot_lon = sum((lon_gps - mean(lon_gps)).^2); 
R_squared_lon = 1 - SSres_lon/SStot_lon;

RMSE_lat = sqrt(mean((lat_gps - lat_pred).^2));
RMSE_lon = sqrt(mean((lon_gps - lon_pred).^2));

MAE_lat = mean(abs(lat_gps - lat_pred));
MAE_lon = mean(abs(lon_gps - lon_pred));

MAPE_lat = mean(abs((lat_gps - lat_pred) ./ lat_actual)) * 100;
MAPE_lon = mean(abs((lon_gps - lon_pred) ./ lon_actual)) * 100;


figure;
subplot(2, 1, 1);
plot(lat_actual, lat_gps, 'o', lat_actual, lat_pred, '-');
xlabel('Ground Latitude');
ylabel('GPS Module Latitude');
legend('Actual', 'Predicted');

subplot(2, 1, 2);
plot(lon_actual, lon_gps, 'o', lon_actual, lon_pred, '-');
xlabel('Ground Longitude');
ylabel('GPS Module Longitude');
legend('Actual', 'Predicted');

% Display the coefficient of determination (R-squared) for latitude and longitude
disp(['Coefficient of Determination (R-squared) for Latitude: ', num2str(R_squared_lat)]);
disp(['Coefficient of Determination (R-squared) for Longitude: ', num2str(R_squared_lon)]);
disp(['RMSE for Latitude: ', num2str(RMSE_lat)]);
disp(['RMSE for Longitude: ', num2str(RMSE_lon)]);
disp(['MAE for Latitude: ', num2str(MAE_lat)]);
disp(['MAE for Longitude: ', num2str(MAE_lon)]);
disp(['MAPE for Latitude: ', num2str(MAPE_lat), '%']);
disp(['MAPE for Longitude: ', num2str(MAPE_lon), '%']);