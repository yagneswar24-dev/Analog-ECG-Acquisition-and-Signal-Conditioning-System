# Analog ECG Acquisition and Signal Conditioning System

## Overview

This project implements a low-cost Electrocardiogram (ECG) acquisition and signal conditioning system capable of extracting and displaying cardiac biopotential signals in the presence of significant environmental noise.

The system combines analog front-end amplification, active filtering, and digital signal verification using an Arduino-based measurement setup. The project was developed as part of the **ECE 212P Analog Circuits Laboratory**.

## Project Objective

ECG signals are typically very weak (approximately 1 mV) and highly susceptible to interference from:

* Power line noise (50 Hz mains)
* Electrode motion artifacts
* Baseline drift
* High-frequency and relatively higher amplitude environmental noise

The objective of this project was to design a complete signal-conditioning chain capable of:

1. Acquiring low-amplitude ECG signals.
2. Amplifying the differential biopotential signal.
3. Rejecting common-mode interference.
4. Filtering unwanted frequency components.
5. Verifying signal quality through digital measurements.

---

## System Architecture

ECG Electrodes

↓

Instrumentation Amplifier (INA)

↓

Right Leg Drive (RLD)

↓

50 Hz Notch Filter

↓

Oscilloscope / Arduino Measurement

---

## Hardware Components

### Analog Front End

* TL074 Quad Op-Amps
* 1% Metal Film Resistors
* Ceramic and Film Capacitors
* ECG Electrodes and Snap Leads

### Power Supply

* NE555 Timer
* 1N4148 Diodes
* Charge Pump Capacitors
* Battery Supply

### Measurement and Verification

* Arduino Nano (ATmega328P)
* Oscilloscope
* Function Generator

---

## Key Design Blocks

### 1. Isolated Power Supply

A 555 timer operating in astable mode drives a diode-capacitor charge pump network to generate a negative supply rail from a battery source.

Benefits:

* Electrical isolation
* Dual-supply operation
* Improved signal swing for op-amp stages

### 2. Instrumentation Amplifier

A three-op-amp instrumentation amplifier topology was implemented using the AD620.

Functions:

* Differential amplification of ECG signals
* High input impedance
* Improved common-mode rejection

### 3. Right Leg Drive (RLD)

An active Right Leg Drive circuit senses common-mode interference and feeds an inverted correction signal back to the body.

Benefits:

* Reduction of 50 Hz mains pickup
* Improved signal-to-noise ratio
* Enhanced Common Mode Rejection Ratio (CMRR)

### 4. Active Filter Bank

#### Twin-T Notch Filter

* Center Frequency: 50 Hz
* Suppresses power-line interference

---

## Signal Verification Procedure

To validate the signal-conditioning chain:

1. Test pulses were generated using an Arduino Nano.
2. The generated waveform was passed through the analog filtering stages.
3. The filtered output was measured again using the Arduino ADC.
4. Oscilloscope measurements were used to compare:

   * Input waveform
   * Filtered waveform
   * Noise reduction effectiveness
5. Frequency-response and attenuation characteristics of the filters were verified experimentally.

---

## Results

The system successfully demonstrated:

* ECG signal amplification from millivolt-level inputs.
* Significant reduction of 50 Hz mains interference.
* Baseline stabilization through high-pass filtering.
* Removal of high-frequency noise through low-pass filtering.
* Accurate recovery of filtered signals measured using the Arduino.

The final output displayed a significantly cleaner waveform compared to the raw input signal.

---

## Applications

* Biomedical signal acquisition
* Educational ECG demonstrations
* Analog signal conditioning studies
* Biomedical instrumentation laboratories

---

## References

1. ECG Signal Processing Fundamentals
2. TL074 Operational Amplifier Datasheet
3. NE555 Timer Datasheet
4. Biomedical Instrumentation Textbooks
5. Analog Filter Design References
