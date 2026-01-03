<<<<<<< HEAD
# ECE 355 – Microprocessor-Based Systems
All assignments for ECE355, Fall 2025 at UVic. 
=======
# ECE355 Project: PWM Signal Generation and Monitoring System
For this project we developed a system to **measure PWM signals from both a Function Generator and 555 timer**, and **display both the signal frequency and resistance from a potentiometer**. The initial frequency displayed is from the function generator, and can be switched back and forth between the 555 timer when the USER button is pressed. The value from the potentiometer is retrieved with ADC, then sent to the optocoupler with DAC. The 555 timer is then driven by the optocoupler and sent back to the STM32F0. This setup allows the potentiometer to control the frequency and duty cycle of the 555 timer, similarly to how a function generator operates. Finally, the measured signal frequency and potentiometer resistance are displayed on the OLED display mounted on the PBMCUSLK board, which is driven by the STM32F0’s SPI. 
<div align = "center">
  <div>
    <img style="width:50%;height:auto" width="771" height="552" alt="image" src="https://github.com/user-attachments/assets/2f95ffa6-116d-4cc8-baa4-7a503f53f1fe">
    <p>Overall system</p>
  </div>
  <div>
    <img style = "width:50%;height:auto" width="901" height="635" alt="image" src="https://github.com/user-attachments/assets/465abbbd-8466-4dde-9269-5afcdb3d01d2" />
    <p>555 timer - Optocoupler circuit</p>
  </div>
</div>
>>>>>>> 03f575b2f1c90ac825270ea9e32db7382fb5923c
