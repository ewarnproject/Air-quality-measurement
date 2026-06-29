CREATE DATABASE IF NOT EXISTS pollution_db;

USE pollution_db;

CREATE TABLE sensor_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    mq2 INT,
    mq3 INT,
    mq135 INT,
    aqi_status VARCHAR(20),
    temperature FLOAT NULL,
    humidity FLOAT NULL
);

CREATE TABLE users(
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE,
    email VARCHAR(100),
    password VARCHAR(255)
);

SELECT * FROM sensor_data;