📍 GPS Location Retrieval using AT Commands (SIM7672G)

This project demonstrates how to retrieve GPS location data using the SIM7672G LTE/GNSS module through AT command-based communication. It focuses on enabling GNSS functionality, reading raw NMEA sentences, and extracting meaningful location information such as latitude, longitude, time, and fix status.

🚀 Overview

The SIM7672G module provides integrated GNSS capabilities that can be accessed via standard AT commands. In this implementation, the module is configured to:

Enable GNSS functionality
Request real-time GPS data
Output raw NMEA sentences
Parse and extract relevant location information

This approach is useful for embedded systems, IoT devices, and applications where a lightweight and direct method of GPS data acquisition is required.

⚙️ Features

AT command-based GNSS control
Retrieval of raw NMEA sentences (e.g., $GPGGA, $GPRMC)
Parsing of GPS data (latitude, longitude, UTC time, fix status)
Simple and efficient implementation suitable for microcontrollers
Compatible with UART-based communication
🧠 How It Works
Initialize communication with the SIM7672G module via UART
Send AT commands to enable GNSS functionality
Request GPS data output from the module
Receive raw NMEA sentences
Parse the NMEA strings to extract meaningful GPS information
📡 Example Data

The module outputs standard NMEA sentences such as:

$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
$GPRMC,123520,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

These are then processed to obtain:

Latitude & Longitude
Timestamp (UTC)
Fix validity
Speed and direction (optional)
