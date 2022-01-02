# Gazpar Sensor for HomeAssitant

![foo](/img/gazpar.jpg "title")

ESP32 + Attiny85 sensor for HomeAssistant to track Gaz consumption. Uses the data port located on the french Gazpar energy meter.

## Background

The [Gazpar](https://particuliers.engie.fr/economies-energie/conseils-economies-energie/conseils-gazpar/gazpar-compteur-gaz-communiquant.html) meter provided by [GRDF](https://www.grdf.fr/) has a 2 pin connector on its left side, which sends an electrical pulse for every 10 liters of gaz consumed. This project's goal is to leverage this port to integrate Gaz consumtion into Home Assistant, with the help of an MQTT broker.

This setup will be located in my utility closet, ouside of my appartement, without an AC outlet. So, we will need battery power, and thus a low-power design. More on that in the next chapter !

## Electronics

To lower the power consumtion, we won't be running the ESP32 all the time, as its normal run mode draws around 80mA. We use 18650 lipo cells, with a capacity in the 2800mAh-4000mAh range, this would mean a battery run time of 2-3 days, which is not acceptable.

To solve this, we use an Attiny85, which draws a much more reasonable 2mA during normal operations, and wake up the ESP32 every couple hours to transmit the data.

TODO: after a couple runs, report back the battery life here !

## ESP32

The ESP32 runs a very very simple routine: 

 - Boot
 - Connect to WiFi
 - Connect to MQTT
 - Wait for the retained value from HASS
 - Fetch the new number of pulses from the Attiny
 - Adds the old value and the new value
 - Publish the new value to the MQTT broker
 - Go to sleep

## Attiny85

TODO
