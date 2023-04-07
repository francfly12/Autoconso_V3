# Autoconso_V3
Display and Control Home power consumption with a raspberry Pico, M5stack &amp; Can bus 

The aim is to create a newer version of home power supply indicating and control.
This is very much inspired by EmonCMS work. It improves a current version which is currently running.
# Purpose :
- measure home electrical power consumption and PV production.
- Display on a remote colour display power consumption, PV solar production, Excess production.
- Control excess electricity production by dumping energy on a water heater.
# New Harware :
Raspberry pico selected now for its capability, reliability and price.
- Use of a can bus for communication between main unit(sensors) and display unit, and also with power heater manager.
- Use of physical RJ45 connectors and wiring, as they are convenient and reliable.
- No use of WIfi, not adapted to the task in some locations with thick walls. Althought WIFI could be later adopted through a wifi Pico.
I am developping a special IC given here.
# Software
Developped using Arduino IDE, but still a work in progress.
Already provides readings of Volts, Amps, power.
Will be enhanced and commented in english later.
Avoids use of libraries.
# Workflow
- Develop main unit hardware, CI, 3D printed case.
- Develop and test software for main unit.
- Develop Display unit harware and software.
- Modify water power heater from V2 to V3. (new can bus connectivity, same functions).
