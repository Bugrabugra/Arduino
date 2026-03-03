# Seeed Xiao ESP32-C3 Pinout Diagram
## OLED Display with 4 Buttons (SSD1315)

### Connections:

```
Seeed Xiao ESP32-C3          OLED Module (4-Button SSD1315)
┌─────────────────┐         ┌─────────────────┐
│                 │         │                 │
│ 3V3 ────────┬───┤         ├─── VCC          │
│             │   │         │                 │
│ GND ────────┼───┤         ├─── GND          │
│             │   │         │                 │
│ D5  ────────┼───┤         ├─── BTN1 (UP)    │
│             │   │         │                 │
│ D6  ────────┼───┤         ├─── BTN2 (DOWN)  │
│             │   │         │                 │
│ D7  ────────┼───┤         ├─── BTN3 (BACK)  │
│             │   │         │                 │
│ D8  ────────┼───┤         ├─── BTN4 (ENTER) │
│             │   │         │                 │
│ SDA (D4) ───┼───┤         ├─── SDA          │
│             │   │         │                 │
│ SCL (D3) ───┘   │         ├─── SCL          │
│                 │         │                 │
└─────────────────┘         └─────────────────┘
```

### Detailed Pin Mapping:

| Xiao ESP32-C3 Pin | Connected To | Function |
|-------------------|--------------|----------|
| 3V3               | VCC          | Power (3.3V) |
| GND               | GND          | Ground |
| D5 (GPIO5)        | BTN1         | UP Button |
| D6 (GPIO6)        | BTN2         | DOWN Button |
| D7 (GPIO7)        | BTN3         | BACK Button |
| D8 (GPIO8)        | BTN4         | ENTER Button |
| D4 (SDA/GPIO4)    | SDA          | I2C Data |
| D3 (SCL/GPIO3)    | SCL          | I2C Clock |

### Seeed Xiao ESP32-C3 Pin Layout:

```
     USB-C
  ┌───────────┐
  │           │
  │  [ ] [ ]  │ ← D0, D1
  │  [ ] [ ]  │ ← D2, D3 (SCL)
  │  [ ] [ ]  │ ← D4 (SDA), D5 (BTN1)
  │  [ ] [ ]  │ ← D6 (BTN2), D7 (BTN3)
  │  [ ] [ ]  │ ← D8 (BTN4), D9
  │  [ ] [ ]  │ ← D10, 3V3
  │  [ ] [ ]  │ ← GND, 5V
  │           │
  └───────────┘
```

### Important Notes:
1. **Power**: The OLED module works with 3.3V from Xiao
2. **I2C Address**: 0x3C (default for SSD1315)
3. **Buttons**: Use internal pull-up resistors (INPUT_PULLUP)
4. **Wiring**: Keep wires short for stable I2C communication

### Button Functions:
- **BTN1 (D5)**: Navigate UP in menus
- **BTN2 (D6)**: Navigate DOWN in menus  
- **BTN3 (D7)**: Go BACK to previous menu
- **BTN4 (D8)**: ENTER/Select menu item

### Color Coding (if using colored wires):
- Red: 3V3 Power
- Black: GND
- Yellow: SDA (I2C Data)
- Green: SCL (I2C Clock)
- Blue: D5 (UP)
- Orange: D6 (DOWN)
- Purple: D7 (BACK)
- Gray: D8 (ENTER)
