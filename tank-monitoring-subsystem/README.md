
## Voltage Divider
Per alimentare il sonar sono stati utilizzati la terra e il Vcc dell'arduino. Per non danneggiare l'ESP e quindi ottenere 3V è stato utilizzato un voltage divider. 

R1 (serie) 1kΩ: tra Echo del sensore e il GPIO dell'ESP32
R2 (a massa) 2kΩ: tra il GPIO dell'ESP32 e GND

Vout = 5V × R2 / (R1 + R2) = 5 × 2000 / 3000 ≈ 3.33V