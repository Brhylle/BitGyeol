#include <Servo.h>
#include <Wire.h>           // Use hardware I2C
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
 
// Servo objects
Servo myDoorServo;
Servo myGarageServoOne;
 
// Default I2C communication (Wire)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the LCD address (0x27 or 0x3F)
RTC_DS3231 rtc;  // Use default I2C for RTC
 
// Pin assignments
int LDR_PIN = A2;
 
int exteriorLights1 = 2;
int exteriorLights2 = 4;
int exteriorLights3 = 7;
int exteriorLights4 = 8;
 
int interiorLights1 = 3; // for living room
int garageLights1 = 6;   // for garage room
 
int doorServoPin = A0;   // Pin for door servo
int oneGaragePin = A1;   // Pin for garage servo
 
unsigned long previousMillis = 0; // Tracks the last time scene was updated
const unsigned long interval = 10000; // 10 seconds interval
int currentScene = 0; // Tracks the current scene
 
// Variables for scrolling text
String scrollingText = "";
int scrollIndex = 0;
unsigned long scrollMillis = 0;
const unsigned long scrollInterval = 300; // Scroll every 300ms
 
bool garageLightsState = false;  // Initial state of garage lights is OFF
bool externalLightsState = false;
bool internalLightsState = false;
bool doorState = false;
bool garageState = false;
bool isAutomaticMode = true; // Start in automatic mode
bool showStatus = false;              // Flag to indicate status display mode
unsigned long statusStartMillis = 0; // Timestamp for when status display started
String statusMessage = "";           // Message to show during status display
 
 
// Declared constants
const uint8_t doorClosedValue = 0;
const uint8_t doorOpenedValue = 180;
 
const uint8_t garageClosedValue = 90;
const uint8_t garageOpenedValue = 0;
 
int lightValue;
int dv = 250;
 
void setup() {
  Serial.begin(9600);   // Initialize serial communication
 
  // Initialize RTC with default I2C
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // Halt if RTC is not found
  }
  
  // Check if RTC lost power and set the time if necessary
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC to compile time
  }
 
  // Initialize LCD with default I2C
  lcd.begin(16, 2);  // Initialize 16x2 LCD
  lcd.backlight();
  lcd.clear();       // Clear LCD display initially
 
  // Lighting pin initialization
  pinMode(LDR_PIN, INPUT);
  pinMode(exteriorLights1, OUTPUT);
  pinMode(exteriorLights2, OUTPUT);
  pinMode(exteriorLights3, OUTPUT);
  pinMode(exteriorLights4, OUTPUT);
  pinMode(interiorLights1, OUTPUT);
  pinMode(garageLights1, OUTPUT);
 
  // Actuator pin initialization
  myDoorServo.attach(doorServoPin);
  myGarageServoOne.attach(oneGaragePin);
 
  Serial.println("Lights and servos initialized.");
 
  // Initialize servos and lights
  myDoorServo.write(doorClosedValue);
  myGarageServoOne.write(garageClosedValue);
  toggleExternalLights(false);
}
 
void loop() {
  watchForPhotons();
  watchForCommands();
 
  // Print current time to Serial Monitor
  DateTime now = rtc.now();
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  if (now.minute() < 10) {
    Serial.print("0");  // Add leading zero for minutes
  }
  Serial.print(now.minute());
  Serial.print(":");
  if (now.second() < 10) {
    Serial.print("0");  // Add leading zero for seconds
  }
  Serial.print(now.second());
  Serial.print(" Date: ");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.println(now.year(), DEC);
 
  updateLCD();
}
 
// Read LDR and control lights/actuators based on light level
void watchForPhotons() {
  if (!isAutomaticMode) return; // Skip logic if not in automatic mode
 
  lightValue = analogRead(LDR_PIN);
  Serial.println(lightValue);
  delay(dv);

  // Get the current time stored in RTC
  DateTime now = rtc.now();
  int currentHour = now.hour();

  // This is the configuration of day and night, when to turn the night mode, and daylight mode.
  bool isNightTime = (currentHour >= 18 || currentHour < 0); // 6:00 PM to 5:59 AM
  bool isMorningTime = (currentHour >= 0 && currentHour < 18); // 6:00 AM to 5:59 PM
  
  // Combine light value and time checks to control devices
  if (isNightTime && lightValue <= 149) {  // Night + low light
    toggleExternalLights(true);
    toggleInternalLights(true);
    toggleGarageLights(true);
    toggleDoor(false);
    toggleGarage(false);
  } else if (isMorningTime && lightValue > 150 && lightValue <= 1000) { // Morning + higher light
    toggleExternalLights(false);
    toggleDoor(true);
    toggleGarage(true);
    toggleInternalLights(false);
    toggleGarageLights(false);
  } else {
    // Optional: Handle transitional or unexpected cases
    Serial.println("Clouds or obstruction detected.");
  }
}
 
// Check for serial commands to control components
void watchForCommands() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    Serial.flush(); // Clear buffer to avoid input overlap
    
    // Command-independent controls
    if (command == 'n') {
      // Reset RTC to the default compile-time values
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      Serial.println("RTC reset to default (compile time).");
      return; // Skip further checks
    }

    if (command == 'b') {
      // Set custom time (input: HH:MM:SS via Serial)
      Serial.println("Enter time in format HH:MM:SS:");
      while (!Serial.available()); // Wait for input
      delay(10); // Ensure the input buffer is filled
      String timeInput = Serial.readStringUntil('\n'); // Remove extra whitespace
      timeInput.trim();
      if (timeInput.length() == 8 && timeInput.charAt(2) == ':' && timeInput.charAt(5) == ':') {
        int hour = timeInput.substring(0, 2).toInt();
        int minute = timeInput.substring(3, 5).toInt();
        int second = timeInput.substring(6, 8).toInt();
        DateTime now = rtc.now();
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
        Serial.println("Time updated successfully!");
      } else {
        Serial.println("Invalid time format. Use HH:MM:SS.");
      }
      return; // Skip further checks
    }

    if (command == 'v') {
      // Set custom date (input: YYYY/MM/DD via Serial)
      Serial.println("Enter date in format YYYY/MM/DD:");
      while (!Serial.available()); // Wait for input
      delay(10); // Ensure the input buffer is filled
      String dateInput = Serial.readStringUntil('\n'); // Remove extra whitespace
      dateInput.trim();
      if (dateInput.length() == 10 && dateInput.charAt(4) == '/' && dateInput.charAt(7) == '/') {
        int year = dateInput.substring(0, 4).toInt();
        int month = dateInput.substring(5, 7).toInt();
        int day = dateInput.substring(8, 10).toInt();
        DateTime now = rtc.now();
        rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));
        Serial.println("Date updated successfully!");
      } else {
        Serial.println("Invalid date format. Use YYYY/MM/DD.");
      }
      return; // Skip further checks
    }

    if (command == 'm') {
      isAutomaticMode = !isAutomaticMode; // Toggle between manual and automatic
      statusMessage = isAutomaticMode ? "Mode: Automatic" : "Mode: Manual";
      showStatus = true;               // Enter status display mode
      statusStartMillis = millis();    // Record the start time for the status display
      Serial.println(statusMessage);   // Log to Serial Monitor
    }

    if (isAutomaticMode) return; // Ignore other commands if in automatic mode

    // Mode-dependent controls
    if (command == 'q') toggleDoor(!doorState);
    if (command == 'w') toggleGarage(!garageState);
    if (command == 'e') toggleExternalLights(!externalLightsState);
    if (command == 'r') toggleInternalLights(!internalLightsState);
    if (command == 't') toggleGarageLights(!garageLightsState);
  }
}
 
// Update the LCD display with the current time and date
void updateLCD() {
  unsigned long currentMillis = millis();
 
  // If in status display mode, show the status message
  if (showStatus) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(statusMessage);
 
    // Check if 3 seconds have passed since status display started
    if (currentMillis - statusStartMillis >= 3000) {
      showStatus = false;  // Exit status display mode
      previousMillis = currentMillis; // Reset scene cycle timer
    }
    return; // Skip normal scene cycle
  }
 
  // Handle scene switching every 10 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    currentScene = (currentScene + 1) % 2; // Cycle through scenes
    scrollIndex = 0; // Reset scrolling index for new scene
  }
 
  // Handle text scrolling every 300ms
  if (currentMillis - scrollMillis >= scrollInterval) {
    scrollMillis = currentMillis;
 
    lcd.clear(); // Clear LCD display before updating
 
    if (currentScene == 2) {
      scrollingText = "Group 4 Presents:";
      lcd.setCursor(0, 0);
      lcd.print(scrollingText.substring(scrollIndex, scrollIndex + 16));
      lcd.setCursor(0, 1);
      lcd.print("BitGyeol");
    } else if (currentScene == 0) {

      DateTime now = rtc.now();
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      if (now.hour() < 10) lcd.print("0");
      lcd.print(now.hour());
      lcd.print(":");
      if (now.minute() < 10) lcd.print("0");
      lcd.print(now.minute());
      lcd.print(":");
      if (now.second() < 10) lcd.print("0");
      lcd.print(now.second());
 
      lcd.setCursor(0, 1);
      lcd.print("Date: ");
      lcd.print(now.year());
      lcd.print("/");
      lcd.print(now.month());
      lcd.print("/");
      lcd.print(now.day());
    } else if (currentScene == 1) {
      DateTime now = rtc.now();
      String greeting;

      // * Cycle of day greetings logic
      bool isMorning = now.hour() >= 5 && now.hour() < 12;
      bool isAfternoon = now.hour() >= 12 && now.hour() < 18;
      bool isEvening = now.hour() >= 18 || now.hour() < 5;

      // * Customized greetings logic
      bool isChristmasEve = now.month() == 12 && now.day() == 24;
      bool isChristmasDay = now.month() == 12 && now.day() == 25;
      bool isNewYearEve = now.month() == 12 && now.day() == 31;
      bool isNewYearDay = now.month() == 1 && now.day() == 1;

      // * Customized greetings text
      String defaultgreet = "Have a great experience!";
      String christmas = "Maligayang Pasko!";
      String newyears = "Manigong Bagong Taon!";
      String christmaseve = "Happy Christmas Eve!";
      String newyearseve = "Happy New Years Eve!";

      if (isMorning) {
        greeting = "Good Morning!";
      } else if (isAfternoon) {
        greeting = "Good Afternoon!";
      } else if (isEvening) {
        greeting = "Good Evening!";
      }
 
      if (isChristmasEve) { scrollingText = christmaseve; }
      else if (isChristmasDay) { scrollingText = christmas; }
      else if (isNewYearEve) { scrollingText = newyearseve; }
      else if (isNewYearDay) { scrollingText = newyears; }
      else { scrollingText = defaultgreet; } 

      lcd.setCursor(0, 0);
      lcd.print(greeting.substring(scrollIndex, scrollIndex + 16)); // Display the greeting
      lcd.setCursor(0, 1);
      lcd.print(scrollingText.substring(scrollIndex, scrollIndex + 16));
    }
 
    // Update scroll index for the next frame
    scrollIndex++;
    if (scrollIndex + 16 > scrollingText.length()) {
      scrollIndex = 0; // Reset scrolling index when text ends
    }
  }
}
 
// Functions to control components
void toggleGarageLights(bool isActive) {
  garageLightsState = isActive;
  digitalWrite(garageLights1, isActive ? HIGH : LOW);
}
 
void toggleInternalLights(bool isActive) {
  internalLightsState = isActive;
  digitalWrite(interiorLights1, isActive ? HIGH : LOW);
}
 
void toggleDoor(bool isActive) {
  doorState = isActive;
  myDoorServo.write(isActive ? doorOpenedValue : doorClosedValue);
  Serial.println(isActive ? "Door opened" : "Door closed");
}
 
void toggleGarage(bool isActive) {
  garageState = isActive;
  myGarageServoOne.write(isActive ? garageOpenedValue : garageClosedValue);
  Serial.println(isActive ? "Garage opened" : "Garage closed");
}
 
void toggleExternalLights(bool isActive) {
  externalLightsState = isActive;
  digitalWrite(exteriorLights1, isActive ? HIGH : LOW);
  digitalWrite(exteriorLights2, isActive ? HIGH : LOW);
  digitalWrite(exteriorLights3, isActive ? HIGH : LOW);
  digitalWrite(exteriorLights4, isActive ? HIGH : LOW);
}
