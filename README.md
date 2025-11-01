Simple, 2 channel, IIC module to start a deployment charge, ignite a second stage or light a high-current LED string!  Program it the way YOU want, it's your rocket.  Use two for up to  four channels, just jumper the address high for 0x23 and low for 0x21.  You can leave PWRON off for startup and during preflight operations for safety, and the PNP FET architechture means no inadvertant firings from grounding.  BATTMON can be used as a double check to ensure a firing battery IS connected.  The continuity circuits will show ONLY that there's a way to ground, no indication of high resistance or dead  short.  TST1 puts 3v3 into your igniter through a 15k resistor AND a diode.  So, if Sn1 reads HIGH, there's no path to ground and continuity fails. Ch1 HIGH connects the 3S LiPo plus side to the channel 1 circuit.  TST2, Sn2 and Ch2 operate in a similar fashon. 

OSHPark linl for PCB:
https://oshpark.com/shared_projects/tfre3jx1

The recommend battery for this unit is the Radical R/C, RRC 3S 240mAh Lipo Super High Discharge 20C
https://www.radicalrc.com/item/RRC-3S-240mAh-Lipo-Super-High-Discharge-20C-108260

