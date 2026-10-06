# SkyChain

SkyChain is a compact, wearable smart keychain designed to clip onto your belt, fit in your pocket, or go anywhere with you. Whenever you spot a plane flying overhead or a vessel nearby and wonder where it's heading, SkyChain gives you real-time flight telemetry directly on your wrist or belt.

---

##  Overview & How It Works

SkyChain fetches live overhead tracking data via the **OpenSky Network API** to give you real-time details about aircraft above you:

- **Altitude & Flight Height**
- **Ground Speed**
- **Flight Origin & Destination**

Every time SkyChain detects a nearby aircraft or tracks a target overhead, it triggers an acoustic haptic alert via an onboard buzzer to let you know new telemetry is ready to view.

---

##  Core Features

- **Real-Time Aircraft Tracking**: Connects via OpenSky API for live location, speed, height, and route metrics.
- **Haptic & Audio Alerts**: Transistor-driven piezo buzzer triggers instant audio feedback upon detection.
- **Visual Display**: Integrated high-contrast OLED screen for displaying clear flight stats.
- **Autonomous Battery Management**: Onboard USB-C charging and LiPo battery regulation for untethered daily wear.
- **Compact Keychain Form Factor**: Portable, lightweight hardware layout designed for EDC (Everyday Carry).

---

##  Custom PCB Design

Designing and routing the SkyChain printed circuit board has been one of the most exciting and complex hardware challenges yet:

- **Microcontroller Core**: ESP32 module managing Wi-Fi requests, display driving, and alert logic.
- **Power System**: TP4056 lithium battery charger paired with an AMS1117 3.3V low-dropout regulator and USB-C power delivery.
- **Transistor Switching**: S8050 NPN transistor driver circuit dedicated to buffering high-decibel alert pulses to the active buzzer.
- **Optimized Passives**: 0805 SMD passive array for impedance matching, USB CC pull-downs, and power decoupling.

---

*Built for Hack Club / Hack Life — excited to complete the hardware assembly and unlock the 3D printer reward!*
