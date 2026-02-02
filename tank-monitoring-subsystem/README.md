

## Voltage Divider
Il voltage divider serve solo sul filo Echo (l'output del sensore), perché Trig va bene a 3.3V (l'ESP32 lo interpreta correttamente come HIGH), mentre Echo esce a 5V e va ridotto.

Schema testuale (il più comune e raccomandato)
HC-SR04 Echo ───┬─────────── ESP32 GPIO (es. 22 o 23)
                │
               R1 = 1 kΩ (1000 Ω)
                │
                └─────────── GND (terra comune)
               R2 = 2 kΩ (2000 Ω)   ← o 2.2 kΩ va benissimo
R1 (serie): tra Echo del sensore e il GPIO dell'ESP32
R2 (a massa): tra il GPIO dell'ESP32 e GND
Con 1kΩ + 2kΩ ottieni circa 3.3V quando Echo è a 5V (formula: Vout = 5V × R2 / (R1 + R2) = 5 × 2000 / 3000 ≈ 3.33V → perfetto e sicuro).