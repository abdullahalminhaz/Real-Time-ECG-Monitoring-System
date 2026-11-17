# Real-Time ECG Monitoring System

<p align="center">
  <strong>A Low-Cost Embedded System for Real-Time Electrocardiogram Acquisition and Heart Rate Estimation</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino%20Nano-00979D?logo=arduino" alt="Arduino Nano"/>
  <img src="https://img.shields.io/badge/Sensor-AD8232-blue" alt="AD8232"/>
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus" alt="C++"/>
  <img src="https://img.shields.io/badge/Domain-Biomedical%20Engineering-purple" alt="Biomedical Engineering"/>
  <img src="https://img.shields.io/badge/Status-Experimental-orange" alt="Experimental"/>
</p>

---

## Overview

The **Real-Time ECG Monitoring System** is a low-cost embedded biomedical instrumentation project developed to acquire, process, and visualize electrocardiogram (ECG) signals in real time.

The system integrates an **AD8232 ECG sensor module** with an **Arduino Nano (ATmega328P)** to capture cardiac electrical activity through surface electrodes.

Digital signal processing techniques are implemented to improve signal quality, suppress unwanted noise, and support adaptive QRS complex detection. The system estimates heart rate in Beats Per Minute (BPM) by measuring the time interval between consecutive detected R-peaks.

This project demonstrates the practical application of embedded systems, biomedical instrumentation, and digital signal processing in physiological signal monitoring.

## Key Features

- Real-time ECG signal acquisition.
- Target sampling frequency of 250 Hz.
- 10-bit analog-to-digital conversion.
- Moving average filtering.
- Digital high-pass filtering.
- Digital low-pass filtering.
- Adaptive threshold-based R-peak detection.
- Slope-based peak identification.
- Refractory period for false peak suppression.
- RR interval measurement.
- Rolling-average BPM estimation.
- Electrode lead-off detection.
- Basic signal quality monitoring.
- Real-time serial data visualization.

## System Architecture

The system follows a sequential signal acquisition and processing architecture.

```text
       ECG Electrodes
             |
             v
        AD8232 Module
             |
             v
        Arduino Nano
             |
             v
     Raw ECG Acquisition
             |
             v
    Moving Average Filter
             |
             v
      High-Pass Filter
             |
             v
       Low-Pass Filter
             |
             v
     Adaptive QRS Detection
             |
             v
       RR Interval Analysis
             |
             v
       BPM Calculation
             |
             v
        Serial Output
```

## Hardware Requirements

| Component | Description |
|---|---|
| Arduino Nano | ATmega328P microcontroller |
| AD8232 ECG Module | Analog ECG signal acquisition |
| ECG Electrodes | Surface cardiac signal sensing |
| Jumper Wires | Hardware interconnections |
| USB Cable | Programming and serial communication |

## Circuit Connections

The following pin configuration is used in the implementation.

| AD8232 / Component | Arduino Nano |
|---|---|
| OUT | A0 |
| LO+ | D10 |
| LO- | D11 |
| LED | D13 |

**Note:** Ensure the electrode connections and power supply follow the AD8232 module manufacturer's specifications.

## Signal Processing Methodology

The acquired ECG signal passes through multiple processing stages before heartbeat detection.

### 1. ECG Signal Acquisition

The AD8232 module acquires the electrical activity of the heart and provides an analog output to the Arduino Nano.

The Arduino samples the analog signal through pin A0.

### 2. Moving Average Filter

A moving average filter is implemented to reduce high-frequency fluctuations in the acquired signal.

The mathematical expression is:

\[
y[n]=\frac{1}{N}\sum_{k=0}^{N-1}x[n-k]
\]

Where:

- \(x[n]\) represents the input signal.
- \(y[n]\) represents the filtered output.
- \(N\) represents the number of samples.

The implementation uses a window size of 8 samples.

### 3. High-Pass Filter

A digital high-pass filter suppresses slow baseline variations associated with breathing and electrode movement.

The implementation uses a recursive filtering approach with a coefficient of 0.995.

### 4. Low-Pass Filter

A low-pass filter reduces high-frequency noise and smooths the processed ECG signal.

An exponential filtering approach is implemented with a coefficient of 0.45.

### 5. Adaptive QRS Detection

The system identifies potential R-peaks using an adaptive threshold and slope-based peak detection algorithm.

The detection process includes:

- Adaptive peak estimation.
- Dynamic threshold calculation.
- Rising and falling slope analysis.
- Refractory period enforcement.
- Minimum and maximum RR interval constraints.

A refractory period of 300 ms is used to prevent multiple detections within a short interval.

### 6. RR Interval Measurement

The RR interval represents the time between two consecutive detected R-peaks.

It is calculated using:

\[
RR=t_{R_n}-t_{R_{n-1}}
\]

The implementation accepts RR intervals between 400 ms and 1800 ms for BPM estimation.

### 7. Heart Rate Calculation

The heart rate is calculated using:

\[
BPM=\frac{60000}{RR}
\]

Where RR is expressed in milliseconds.

A rolling buffer of five RR intervals is maintained to smooth the estimated heart rate.

## Experimental Configuration

| Parameter | Value |
|---|---:|
| Microcontroller | Arduino Nano |
| Processor | ATmega328P |
| ECG Sensor | AD8232 |
| Target Sampling Rate | 250 Hz |
| Sampling Interval | 4 ms |
| ADC Resolution | 10-bit |
| Serial Baud Rate | 115200 |
| Moving Average Window | 8 samples |
| High-Pass Filter Coefficient | 0.995 |
| Low-Pass Filter Coefficient | 0.45 |
| Refractory Period | 300 ms |
| Minimum RR Interval | 400 ms |
| Maximum RR Interval | 1800 ms |
| RR Averaging Buffer | 5 intervals |

## Experimental Results

The developed prototype demonstrated real-time ECG signal acquisition, digital filtering, heartbeat detection, and BPM estimation.

The experimental demonstration reported an observed heart rate of approximately:

**65 BPM**

The system successfully demonstrated the integration of low-cost embedded hardware with basic digital ECG signal processing.

The reported value represents an experimental observation, not a clinically validated accuracy measurement.

## Software Requirements

- Arduino IDE
- Arduino Nano board support
- USB serial communication
- Arduino Serial Monitor / Serial Plotter

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/abdullahalminhaz/Real-Time-ECG-Monitoring-System.git
```

### 2. Open the Project

Navigate to the `src` directory and open `ECG_Monitor.ino` using Arduino IDE.

### 3. Hardware Setup

Connect the AD8232 ECG module to the Arduino Nano according to the pin configuration described above.

### 4. Upload the Firmware

- Connect Arduino Nano to your computer.
- Select Arduino Nano from the Board menu.
- Select the appropriate processor and COM port.
- Upload the firmware.

### 5. Monitor ECG Signals

Open the Serial Monitor or Serial Plotter and configure the baud rate to:

```text
115200
```

Observe the processed ECG signal and calculated BPM output.

## Repository Structure

```text
Real-Time-ECG-Monitoring-System/
│
├── README.md
├── LICENSE
│
├── src/
│   └── ECG_Monitor.ino
│
├── docs/
│   ├── system_architecture.png
│   └── presentation.pdf
│
├── hardware/
│   └── circuit_connections.png
│
└── results/
    ├── ecg_waveform.png
    └── experimental_results.png
```

## Applications

- Biomedical instrumentation education.
- Embedded ECG signal acquisition.
- Biomedical signal processing.
- Real-time heart rate estimation.
- Low-cost physiological monitoring prototypes.
- Engineering laboratory demonstrations.

## Limitations and Future Improvements

Although the prototype demonstrates real-time ECG acquisition and basic heart rate estimation, several improvements can be explored:

- More robust QRS detection algorithms.
- Improved motion artifact suppression.
- Quantitative performance evaluation against a reference ECG device.
- ECG data storage and logging.
- Wireless ECG data transmission.
- Graphical user interface development.
- Validated heart rate variability analysis.
- IoT-based remote physiological monitoring.

## Contributors

**Md. Rajesh Chowdhury**

**Abdullah Al Minhaz**

Department of Electrical and Electronic Engineering  
Jamalpur Science & Technology University (JSTU)  
Bangladesh.

## Acknowledgement

This project was developed as part of a Biomedical Engineering Laboratory assessment at Jamalpur Science & Technology University.

We sincerely acknowledge our project supervisor, **Md. Amimul Ihsan**, for his academic guidance, technical support, and valuable suggestions throughout the development of this project.

## Disclaimer

This project is intended exclusively for educational and research purposes. It is an experimental prototype and is not a certified medical device. The measurements should not be used for clinical diagnosis, treatment decisions, or emergency medical assessment.

## License

License information will be added following agreement between the project contributors.

---

<p align="center">
  <strong>Real-Time ECG Monitoring System</strong><br/>
  Biomedical Engineering | Embedded Systems | Digital Signal Processing
</p>
