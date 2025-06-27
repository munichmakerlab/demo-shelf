#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>

// ========================================
// PIN DEFINITIONS
// ========================================
#define INPUT_PIN_PLAY_RANDOM_TRACK 5
#define INPUT_PIN_1 4
#define INPUT_PIN_2 3
#define INPUT_PIN_3 2

#define DFPLAYER_RX 10          
#define DFPLAYER_TX 11          

// ========================================
// CONFIGURATION
// ========================================
#define DFPLAYER_BAUD_RATE 9600 
#define DFPLAYER_VOLUME 5       // Volume level (0-30)
#define SOUND_TRACK_COUNT 5     // Number of available tracks
#define DEBOUNCE_DELAY 50       // Button debounce delay in ms
#define DOUBLE_CLICK_DELAY 300  // Maximum time between clicks for double-click (ms)
#define I2C_SLAVE_ADDRESS 0x08  // I2C slave address

// ========================================
// GLOBAL OBJECTS & VARIABLES
// ========================================
SoftwareSerial softSerial(DFPLAYER_RX, DFPLAYER_TX);
DFRobotDFPlayerMini speaker;

// Button handling
uint8_t lastButtonState = HIGH; 
uint32_t lastDebounceTime = 0;  // Changed from uint64_t to uint32_t
uint32_t lastClickTime = 0;     // Time of last button click
uint8_t clickCount = 0;         // Number of clicks detected
bool waitingForSecondClick = false; // Flag for double-click detection

// ========================================
// FUNCTION DECLARATIONS
// ========================================
void receiveEvent(int bytes);
void initializeDFPlayer();
void initializeInputs();
void initializeI2C();
void handleButtonInput();
void playRandomTrack();
void stopCurrentTrack();

// ========================================
// SETUP FUNCTION
// ========================================
void setup() {
  Serial.begin(9600);
  
  // Project header
  Serial.println("");
  Serial.println("========================================");
  Serial.println("         🎵 SOUND CONTROLLER 🎵        ");
  Serial.println("========================================");
  Serial.println("Arduino Nano MP3 Player & Controller");
  Serial.println("- DFPlayer Mini Integration");
  Serial.println("- Single/Double Click Control");
  Serial.println("- Optional I2C Remote Control");
  Serial.print("- ");
  Serial.print(SOUND_TRACK_COUNT);
  Serial.println(" Track Support");
  Serial.print("- I2C Address: 0x");
  Serial.println(I2C_SLAVE_ADDRESS, HEX);
  Serial.print("- DFPlayer Volume: ");
  Serial.println(DFPLAYER_VOLUME);
  Serial.print("- Trigger Pin: ");
  Serial.println(INPUT_PIN_PLAY_RANDOM_TRACK);
  Serial.println("========================================");
  Serial.println("");
  
  Serial.println("[INFO] === SoundController Starting ===");

  initializeDFPlayer();
  initializeInputs();
  initializeI2C();

  Serial.println("[INFO] Setup complete. Ready for input!");
}

// ========================================
// MAIN LOOP
// ========================================
void loop() {
  handleButtonInput();
  
  // Check for double-click timeout
  if (waitingForSecondClick && (millis() - lastClickTime) > DOUBLE_CLICK_DELAY) {
    // Single click detected - play random track
    Serial.println("[DEBUG] Single click - playing random track...");
    playRandomTrack();
    waitingForSecondClick = false;
    clickCount = 0;
  }
  
  delay(10); // Small delay to prevent excessive CPU usage
}

// ========================================
// INITIALIZATION FUNCTIONS
// ========================================
void initializeDFPlayer() {
  Serial.println("[INFO] Initializing DFPlayer Mini...");
  
  softSerial.begin(DFPLAYER_BAUD_RATE);
  
  if (!speaker.begin(softSerial, true, false)) {
    Serial.println("[ERROR] DFPlayer Mini not found!");
    Serial.println("[ERROR] Check connections and restart.");
    while (true) {
      delay(1000); // Stay in error state
    }
  }
  
  speaker.volume(DFPLAYER_VOLUME);
  delay(100); // Give DFPlayer time to process
  Serial.println("[INFO] DFPlayer Mini ready.");
}

void initializeInputs() {
  Serial.println("[INFO] Setting up input pins...");
  
  pinMode(INPUT_PIN_PLAY_RANDOM_TRACK, INPUT_PULLUP);
  pinMode(INPUT_PIN_1, INPUT_PULLUP);
  pinMode(INPUT_PIN_2, INPUT_PULLUP);
  pinMode(INPUT_PIN_3, INPUT_PULLUP);
  
  Serial.println("[INFO] Input pins configured.");
}

void initializeI2C() {
  Serial.println("[INFO] Initializing I2C interface...");
  
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  Serial.print("[INFO] I2C slave ready at address: 0x");
  Serial.println(I2C_SLAVE_ADDRESS, HEX);
}

// ========================================
// BUTTON HANDLING
// ========================================
void handleButtonInput() {
  uint8_t currentButtonState = digitalRead(INPUT_PIN_PLAY_RANDOM_TRACK);
  uint32_t currentTime = millis();

  // Detect button press with debouncing
  if (currentButtonState != lastButtonState && 
      currentButtonState == LOW && 
      (currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
    
    clickCount++;
    
    if (clickCount == 1) {
      // First click detected
      lastClickTime = currentTime;
      waitingForSecondClick = true;
      Serial.println("[DEBUG] First click detected, waiting for second...");
      
    } else if (clickCount == 2 && waitingForSecondClick && 
               (currentTime - lastClickTime) <= DOUBLE_CLICK_DELAY) {
      // Double click detected
      Serial.println("[DEBUG] Double click - stopping current track...");
      stopCurrentTrack();
      waitingForSecondClick = false;
      clickCount = 0;
    }
  }

  // Update button state tracking
  if (currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    lastDebounceTime = currentTime;
  }
}

void playRandomTrack() {
  uint8_t trackNumber = random(1, SOUND_TRACK_COUNT + 1);
  Serial.print("[INFO] Playing track: ");
  Serial.println(trackNumber);
  speaker.play(trackNumber);
}

void stopCurrentTrack() {
  Serial.println("[INFO] Stopping current track...");
  speaker.stop();
}


// ========================================
// I2C COMMAND HANDLER
// ========================================
void receiveEvent(int bytes) {
  // Safety check: ensure data is available
  if (!Wire.available()) {
    Serial.println("[WARN] I2C: No data received");
    return;
  }

  uint8_t command = Wire.read();
  Serial.print("[DEBUG] I2C command received: ");
  Serial.println(command);
  
  switch (command) {
    case 1: // Play random track
      Serial.println("[INFO] I2C: Playing random track");
      playRandomTrack();
      break;
      
    case 2: // Stop playback
      Serial.println("[INFO] I2C: Stopping playback");
      speaker.stop();
      break;
      
    case 3: // Pause playback
      Serial.println("[INFO] I2C: Pausing playback");
      speaker.pause();
      break;
      
    case 4: // Resume playback
      Serial.println("[INFO] I2C: Resuming playback");
      speaker.start();
      break;
      
    case 5: // Volume up
      Serial.println("[INFO] I2C: Volume up");
      speaker.volumeUp();
      break;
      
    case 6: // Volume down
      Serial.println("[INFO] I2C: Volume down");
      speaker.volumeDown();
      break;
      
    default:
      // Handle specific track commands (10-14)
      if (command >= 10 && command <= (9 + SOUND_TRACK_COUNT)) {
        uint8_t track = command - 9;
        Serial.print("[INFO] I2C: Playing specific track ");
        Serial.println(track);
        speaker.play(track);
      } else {
        Serial.print("[WARN] I2C: Unknown command ");
        Serial.println(command);
      }
      break;
  }
  
  // Clear any remaining bytes in buffer
  while (Wire.available()) {
    Wire.read();
  }
}
