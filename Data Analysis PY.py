import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load your sensor data
df = pd.read_csv('feeds.csv')

# Data Science: Find anomalies using Standard Deviation
temp_mean = df['field1'].mean()
temp_std = df['field1'].std()
df['is_anomaly'] = df['field1'] > (temp_mean + 2 * temp_std)

# Plotting the results for the report
plt.figure(figsize=(10,5))
plt.plot(df['created_at'], df['field1'], label='Temp (C)')
plt.scatter(df[df['is_anomaly'] == True]['created_at'], 
            df[df['is_anomaly'] == True]['field1'], color='red', label='Failure Warning')
plt.title('Appliance Health Monitoring - Anomaly Detection')
plt.xlabel('Time')
plt.ylabel('Temperature')
plt.legend()
plt.show()