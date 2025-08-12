Overview
This repository contains the code and documentation for my undergraduate dissertation project: a simplified EMG-based control system for robotic arms. The goal was to explore how prosthetic control systems can be made more affordable by reducing complexity while maintaining usability.

Project Aim
To design and implement a low-cost control method for robotic/prosthetic arms by processing EMG signals and translating them into servo motor movements, with lockout and cooldown logic for more reliable control.

Features
- EMG Signal Processing – Read and process muscle activity data from EMG sensors.
- Servo Motor Control – Map EMG input to robotic arm movement using Arduino.
- Latch & Lockout Logic – Prevent false triggers and unintended movements.
- Multi-Sensor Integration – Incorporated EMG, pressure sensors, and experimental TENG sensors.
- Proof-of-Concept Arm – Demonstrated feasibility for low-cost prosthetic applications.

📂 Contents
– Sensor signal reading, lockout logic, and servo control scripts.
– Circuit diagrams, block diagrams, and design notes.
– Data from servo and sensor tests.

Hardware Used
- EMG sensor module (MyoWare)
- Arduino microcontroller
- Servo motors
- Pressure sensors (small and large)
- Experimental TENG sensor
- Custom wiring and breadboard setup

How It Works
- EMG signals are read and filtered via Arduino.
- Processed signals are fed into latch and lockout logic to determine valid control inputs.
- Servo motors respond according to mapped muscle movements.

Dissertation Context
This project was developed as part of my BEng Electronic Engineering degree at the University of Exeter. The research explores how accessible sensors and simplified control logic can lower the cost of prosthetic devices while still allowing intuitive, real-time control.

Future Improvements
- Implement feature extraction for improved EMG accuracy.
- Add gesture recognition for multi-function control.
- Integrate TENG sensors for hybrid control input.
- Miniaturise the system for wearable use.
