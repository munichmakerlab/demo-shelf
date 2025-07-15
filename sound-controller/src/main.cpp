#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>

// ========================================
// DEBUG CONFIGURATION
// ========================================
#define DEBUG false  // Set to false to disable all serial output

#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_BEGIN(x) Serial.begin(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_BEGIN(x)
#endif

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
#define DFPLAYER_VOLUME 25      // Volume level (0-30)
#define DFPLAYER_START_SOUND 1  // Start sound track number (1-based index)
#define SOUND_TRACK_COUNT 6     // Number of available tracks
#define DEBOUNCE_DELAY 50       // Button debounce delay in ms
#define DOUBLE_CLICK_DELAY 300  // Maximum time between clicks for double-click (ms)
#define I2C_SLAVE_ADDRESS 0x08  // I2C slave address

// ========================================
// GLOBAL OBJECTS & VARIABLES
// ========================================
SoftwareSerial softSerial(DFPLAYER_RX, DFPLAYER_TX);
DFRobotDFPlayerMini speaker;

// Button handling
uint8_t lastButtonState = LOW; 
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
  DEBUG_BEGIN(9600);
  
  // Project header
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("         🎵 SOUND CONTROLLER 🎵        ");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("Arduino Nano MP3 Player & Controller");
  DEBUG_PRINTLN("- DFPlayer Mini Integration");
  DEBUG_PRINTLN("- Single/Double Click Control");
  DEBUG_PRINTLN("- Optional I2C Remote Control");
  DEBUG_PRINT("- ");
  DEBUG_PRINT(SOUND_TRACK_COUNT);
  DEBUG_PRINTLN(" Track Support");
  DEBUG_PRINT("- I2C Address: 0x");
  DEBUG_PRINTLN(I2C_SLAVE_ADDRESS);
  DEBUG_PRINT("- DFPlayer Volume: ");
  DEBUG_PRINTLN(DFPLAYER_VOLUME);
  DEBUG_PRINT("- Trigger Pin: ");
  DEBUG_PRINTLN(INPUT_PIN_PLAY_RANDOM_TRACK);
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Wait for external sound controller to startup (delay 5 seconds)..."); 
  delay(5000);
  DEBUG_PRINTLN("[INFO] === SoundController Starting ===");

  initializeDFPlayer();
  initializeInputs();
  initializeI2C();

  DEBUG_PRINTLN("[INFO] Setup complete. Ready for input!");
}

// ========================================
// MAIN LOOP
// ========================================
void loop() {
  handleButtonInput();
  
  // Check for double-click timeout
  if (waitingForSecondClick && (millis() - lastClickTime) > DOUBLE_CLICK_DELAY) {
    // Single click detected - play random track
    DEBUG_PRINTLN("[DEBUG] Single click - playing random track...");
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
  DEBUG_PRINTLN("[INFO] Initializing DFPlayer Mini...");
  
  softSerial.begin(DFPLAYER_BAUD_RATE);
  
  if (!speaker.begin(softSerial, true, false)) {
    DEBUG_PRINTLN("[ERROR] DFPlayer Mini not found!");
    DEBUG_PRINTLN("[ERROR] Check connections and restart.");
    while (true) {
      delay(1000); // Stay in error state
    }
  }
  
  speaker.volume(DFPLAYER_VOLUME);
  delay(100); // Give DFPlayer time to process
  DEBUG_PRINTLN("[INFO] DFPlayer Mini ready.");

  // Play the start sound
  speaker.play(DFPLAYER_START_SOUND);
}

void initializeInputs() {
  DEBUG_PRINTLN("[INFO] Setting up input pins...");
  
  pinMode(INPUT_PIN_PLAY_RANDOM_TRACK, INPUT_PULLUP);
  pinMode(INPUT_PIN_1, INPUT_PULLUP);
  pinMode(INPUT_PIN_2, INPUT_PULLUP);
  pinMode(INPUT_PIN_3, INPUT_PULLUP);
  
  DEBUG_PRINTLN("[INFO] Input pins configured.");
}

void initializeI2C() {
  DEBUG_PRINTLN("[INFO] Initializing I2C interface...");
  
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  DEBUG_PRINT("[INFO] I2C slave ready at address: 0x");
  DEBUG_PRINTLN(I2C_SLAVE_ADDRESS);
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
      DEBUG_PRINTLN("[DEBUG] First click detected, waiting for second...");
      
    } else if (clickCount == 2 && waitingForSecondClick && 
               (currentTime - lastClickTime) <= DOUBLE_CLICK_DELAY) {
      // Double click detected
      DEBUG_PRINTLN("[DEBUG] Double click - stopping current track...");
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
  uint8_t trackNumber = random(2, SOUND_TRACK_COUNT + 1);

  DEBUG_PRINT("[INFO] Playing track: ");
  DEBUG_PRINTLN(trackNumber);
  speaker.play(trackNumber);
}

void stopCurrentTrack() {
  DEBUG_PRINTLN("[INFO] Stopping current track...");
  speaker.stop();
}


// ========================================
// I2C COMMAND HANDLER
// ========================================
void receiveEvent(int bytes) {
  // Safety check: ensure data is available
  if (!Wire.available()) {
    DEBUG_PRINTLN("[WARN] I2C: No data received");
    return;
  }

  uint8_t command = Wire.read();
  DEBUG_PRINT("[DEBUG] I2C command received: ");
  DEBUG_PRINTLN(command);
  
  switch (command) {
    case 1: // Play random track
      DEBUG_PRINTLN("[INFO] I2C: Playing random track");
      playRandomTrack();
      break;
      
    case 2: // Stop playback
      DEBUG_PRINTLN("[INFO] I2C: Stopping playback");
      speaker.stop();
      break;
      
    case 3: // Pause playback
      DEBUG_PRINTLN("[INFO] I2C: Pausing playback");
      speaker.pause();
      break;
      
    case 4: // Resume playback
      DEBUG_PRINTLN("[INFO] I2C: Resuming playback");
      speaker.start();
      break;
      
    case 5: // Volume up
      DEBUG_PRINTLN("[INFO] I2C: Volume up");
      speaker.volumeUp();
      break;
      
    case 6: // Volume down
      DEBUG_PRINTLN("[INFO] I2C: Volume down");
      speaker.volumeDown();
      break;
      
    default:
      // Handle specific track commands (10-14)
      if (command >= 10 && command <= (9 + SOUND_TRACK_COUNT)) {
        uint8_t track = command - 9;
        DEBUG_PRINT("[INFO] I2C: Playing specific track ");
        DEBUG_PRINTLN(track);
        speaker.play(track);
      } else {
        DEBUG_PRINT("[WARN] I2C: Unknown command ");
        DEBUG_PRINTLN(command);
      }
      break;
  }
  
  // Clear any remaining bytes in buffer
  while (Wire.available()) {
    Wire.read();
  }
}
