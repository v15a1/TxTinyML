![Javascript](https://img.shields.io/badge/-c++-black?logo=c%2B%2B&style=for-the-badge)

<p align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
<!--     <img src="https://github.com/othneildrew/Best-README-Template/raw/master/images/logo.png" alt="Logo" width="80" height="80"> -->
  </a>

  <h1 align="center">TxTinyML</h1>

  <p align="center">
    A library for transferring TinyML models from MCU to Central devices
    <br />
    <a href="#Installation"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/v15a1/TxTinyML">View Repo</a>
    ·
    <a href="https://github.com/v15a1/TxTinyML/issues">Report Bug</a>
    ·
    <a href="https://github.com/v15a1/TxTinyML/issues">Request Feature</a>
  </p>
</p>

### Description

TxTinyML allows the MCUs that run TinyML inference using ML models represented as C/C++ arrays to be transferred opportunistically in ad-hoc networks. The library is composed of a rule based system paired with a fuzzy logic inferrer for transferring the models to a central device implementing the client protocol. 

### How to install? `General use`

**Step 1:** Make a clone of the project using Git or download at Download on the button "Download as zip."

**Step 2:** Clone or unzip the files into some folder

**Step 3:** Compile and link it to your code / Copy the file to `../Arduino/Libraries`

### Tutorial

**Step 1:** Create an instance of `Proximity` globally
```cpp
#import <Arduino.h>

Proximity *proximity = new Proximity(); //<- Dynamically allocate Proximity

void setup() {}
void loop() {}
```
**Step 2:** Read the Received Signal Strength Indicator (RSSI) and enqueue the RSSI value into the instance
```cpp
double rssi = getRSSI(); // Reads RSSI values
proximity -> enqueue(rssi);
```

**Step 3:** Get Decision from the decision mechanism for transferring a model.
```cpp
proximity->prepare(2048); // Prepare proximity to the expected
```

**Step 4:** Get Decision from the decision mechanism for transferring a model.
```cpp
bool isTransferring = proximity -> getDecision(battery);
```
