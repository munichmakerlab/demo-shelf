# Arduino Nano DFPlayer Mini Sound Controller

## ✨ Features

- 🎶 MP3 playback with DFPlayer Mini
- 🎲 Random track playback via single button click
- ⏹️ Stop current track via double button click
- 🔘 **Standalone operation** - works without I2C
- 📡 **Optional I2C addon** for external control
- 🔊 Volume control and full playback management
- 🎯 Support for 5 audio tracksntroller

Arduino Nano based sound controller with DFPlayer Mini MP3 module. Works standalone with physical buttons, with **optional** I2C communication for external control.

## ✨ Features

- 🎶 MP3 playback with DFPlayer Mini
- 🎲 Random track playback via button press
- � **Standalone operation** - works without I2C
- 📡 **Optional I2C addon** for external control
- 🔊 Volume control and full playback management
- 🎯 Support for 5 audio tracks

## 🔧 Hardware

**Required Components:**
- 🔌 Arduino Nano
- 🎧 DFPlayer Mini MP3 module
- 💾 MicroSD card with MP3 files
- 🔘 1x button (for random playback)
- 🔊 Speaker/headphones

**Optional (for I2C):**
- 🔗 I2C master device (e.g., another Arduino, Raspberry Pi)
- 📏 I2C pullup resistors (usually built-in)

**Pin Configuration:**
```
Pin 5  → Control button (single/double click)
Pin 10 → DFPlayer RX (required)
Pin 11 → DFPlayer TX (required)
A4/A5  → I2C (SDA/SCL) - optional addon
```

## 🚀 Setup

1. **⚡ Hardware:** Connect DFPlayer Mini to pins 10/11, button to pin 5
2. **💿 SD Card:** Format as FAT32, add MP3 files named `001.mp3` to `005.mp3`
3. **💻 Software:** Install `DFRobotDFPlayerMini` library and upload code
4. **🎮 Basic Usage:** Single click = random track, double click = stop
5. **📡 I2C (Optional):** Connect SDA/SCL pins for external control

## 📡 I2C Commands (Optional Addon)

**⚠️ Note:** The device works perfectly **without I2C**. This is an optional feature for external control.

The device can act as I2C slave at address `0x08`:

| Command | Function | Description |
|---------|----------|-------------|
| `1` | 🎲 Play random track | Plays random track (1-5) |
| `2` | ⏹️ Stop playback | Stops current playback |
| `3` | ⏸️ Pause | Pauses current track |
| `4` | ▶️ Resume | Resumes paused track |
| `5` | 🔊 Volume up | Increases volume by 1 |
| `6` | 🔉 Volume down | Decreases volume by 1 |
| `10-14` | 🎯 Play specific tracks | `10`=Track1, `11`=Track2, etc. |

### I2C Technical Details

- **Slave Address:** `0x08` (8 decimal)
- **Data Format:** Single byte command
- **Pullup Resistors:** Usually built into master device
- **Speed:** Standard I2C (100kHz)
- **Pins:** A4 (SDA), A5 (SCL) on Arduino Nano

## 📖 Usage

### 🎮 Standalone Operation (Primary Use)

**Button Controls on Pin 5:**
- 🎲 **Single Click:** Play random track (1-5)
- ⏹️ **Double Click:** Stop current track
- ⏱️ **Timing:** Double-click must be within 300ms

**Example Usage:**
1. Single click → Plays random track
2. Double click → Stops playback
3. Single click again → Plays new random track

**No additional setup required!**

### 💬 I2C Control (Optional Addon)

**Basic Example:**
```cpp
#include <Wire.h>

void setup() {
  Wire.begin(); // Initialize as I2C master
}

void playRandomTrack() {
  Wire.beginTransmission(0x08);
  Wire.write(1);  // Command: Play random track
  Wire.endTransmission();
}
```

**Advanced Example:**
```cpp
// Play specific track
void playTrack(uint8_t trackNumber) {
  if (trackNumber >= 1 && trackNumber <= 5) {
    Wire.beginTransmission(0x08);
    Wire.write(9 + trackNumber);  // Commands 10-14
    Wire.endTransmission();
  }
}

// Volume control
void volumeUp() {
  Wire.beginTransmission(0x08);
  Wire.write(5);
  Wire.endTransmission();
}

void volumeDown() {
  Wire.beginTransmission(0x08);
  Wire.write(6);
  Wire.endTransmission();
}

// Playback control
void stopMusic() {
  Wire.beginTransmission(0x08);
  Wire.write(2);
  Wire.endTransmission();
}
```

## ⚙️ Configuration

```cpp
#define DFPLAYER_VOLUME 5       // Default volume (0-30)
#define SOUND_TRACK_COUNT 5     // Number of tracks on SD card
#define DEBOUNCE_DELAY 50       // Button debounce in milliseconds
#define DOUBLE_CLICK_DELAY 300  // Max time between clicks for double-click
#define I2C_SLAVE_ADDRESS 0x08  // I2C address (only if using I2C)
```

**🔍 Debugging:** Serial output available at 9600 baud for troubleshooting.

**💡 Tip:** Start with standalone operation first, then add I2C if needed!
