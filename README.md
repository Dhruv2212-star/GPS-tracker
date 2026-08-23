Ai ussage- None 
Project description- a small pocket sized instrument that uses GPS module paired with Xiao RP2040 module as the MCU to track your coordinates every min that you can put on websites like google maps or GPS visualizer, it uses a small Li-Po battery to save some space and also prevent and hazard related to fires from damage. it also has a charging USB-c port to charge the battery and charging and protection based on TP4056. there are 3 LEDs that provide data on GPS logging, data logging and power. there is also a push button to provide the user control for stopping, starting or making event loggs. 

--------------------------------------------------------------------------------------------------------------------------------------------
# Lets get into the project details and the function of each block and the components present in them! 
 ## block 1  
 <img width="331" height="435" alt="image" src="https://github.com/user-attachments/assets/350d00db-0ed1-4dab-b370-ef7a3c7362b7" />  

 1. USB-c: (J1) for charging the battery or powering the device  
 2. F1- polyfuse: protects the board from excessive input current  
 3. D1- Protects the board from input voltage spikes  
 4. R1- CC1 pulldown   
 5. R2- CC2- pulldown  
## block 2
<img width="527" height="515" alt="image" src="https://github.com/user-attachments/assets/c2e28e6a-966d-4d74-911f-3aafb62eb35c" />

1. TP4056 (U1): main ic for charging the battery  
2. R3- sets the charging current for battery  
3. C1- stabalises the input current from the charger's side  
4. C2- stabalizes the battery input current of the charger  
5. D2- indicates battery is charging  
6. D3- indicates that the charging is complete  
7. R4- limits D2 current to prevent it from burnout  
8. R5- Limits D3 current to prevent it from burnout  
## Block 3 
<img width="520" height="353" alt="image" src="https://github.com/user-attachments/assets/fb5faebf-b1c8-4367-84d2-9d8a14f2404b" /> 

1. AP2112K-3.3 (U2): converts battery voltage-> 3.3v  
2. C2- LDO input  
3. C3- filter 3.3v  
## Block 4 
<img width="384" height="161" alt="image" src="https://github.com/user-attachments/assets/27bb769f-ec81-42b1-aa98-af2b2906575c" /> 

1. Rp4020 module (u3)- the MCU for the project  
2. C5- decoupling capacitor for U3   
## Block 5
<img width="415" height="422" alt="image" src="https://github.com/user-attachments/assets/e0b7d08b-fe8b-49bc-8e88-969a60c8014c" />  

1. MAX-M10S (U4): the main GPS module that recives the satalite rays 
2. C6- decoupling for U4  
3. C7additional capacitor for filtering supply  
4. J3- antena  
## Block 6  
<img width="720" height="272" alt="image" src="https://github.com/user-attachments/assets/45a093a9-f55b-48af-a1fb-f133e271b353" />  

1. SD card (J4): the storage for the cordinates that the GPS module records  
2. C8: servers as the purpose of high frequency decoupling
3. C9: servers as the purpose of 3.3v supply filtering
## Block 7 
<img width="400" height="298" alt="image" src="https://github.com/user-attachments/assets/152c7bc1-5753-4a4f-9ed5-9f8747eb73c3" /> 

1. D4, indicates the GPS status  
2. D5 indicates the GPS data logging    
3. D6 Indicates board power
4. R6: protect D3 from overcurrent  
5. R7: protect D4 from overcurrent
6. R8: protect D5 from overcurrent
7. SW1: For User control  

<img width="683" height="545" alt="png" src="https://github.com/user-attachments/assets/63d5485b-5c42-443f-88c0-e3177899e244" />

# Final product 
<img width="845" height="416" alt="image" src="https://github.com/user-attachments/assets/8393a0a1-ffaf-44f6-8706-67533fa1fc0f" />
