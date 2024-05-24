#include <WiFi.h>

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>


String message_latitude, message_longitude;
const int alertPin = 2;
String url1;
#define GPS_BAUDRATE 9600

const char* ssid = "Deven123";
const char* password = "deven3112";

#define BOTtoken "6810691424:AAFo73k71MJ1obnk7DAA9BqB9LnkPX_cGKQ"
String CHAT_ID = "1316357329";
String ALLOWED_CHAT_IDS = "1316357329";
TinyGPSPlus gps;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 500;
unsigned long lastTimeBotRan;

String userChatID = "";
bool ledState = LOW;
bool isAddingChatID = false;
bool isRemovingChatID = false;
// url1 = "https://www.google.com/maps/search/?api=1&query="+message_latitude+","message_longitude;

const int TRIG_PIN   = 26; // Arduino pin connected to Ultrasonic Sensor's TRIG pin
const int ECHO_PIN   = 25; // Arduino pin connected to Ultrasonic Sensor's ECHO pin
const int BUZZER_PIN = 5; // Arduino pin connected to Piezo Buzzer's pin
const int DISTANCE_THRESHOLD = 50; // centimeters

// variables will change:
float duration_us, distance_cm;


void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (ALLOWED_CHAT_IDS.indexOf(chat_id) == -1) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      bot.sendMessage(chat_id, "Wait...", "");
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control the system:\n\n";
      welcome += "/getlocation - Location Command \n";
      welcome += "/Caretakers - Check Allowed Chat IDs\n";
      welcome += "/Add_Caretaker - Add Chat ID\n";
      welcome += "/Remove_Caretaker - Remove Chat ID\n";
      bot.sendMessage(chat_id, welcome, "");
    } else if (text == "/getlocation") {
        bot.sendMessage(chat_id, "Wait...", "");
        Serial.println(Serial2.available());
        Serial.println(gps.encode(Serial2.read()));
        Serial.println(gps.location.isValid());
        if (Serial2.available() > 0) {
          if (gps.encode(Serial2.read())) {
            if (gps.location.isValid()) {
              bot.sendMessage(chat_id, "https://www.google.com/maps/search/?api=1&query=" + message_latitude + "," + message_longitude, "");
            } 
          }
                else{
        bot.sendMessage(chat_id, "GPS Not Found.", "");
      }
        }
    } else if (text == "/Users") {
      bot.sendMessage(chat_id, "Wait...", "");
      String message = "Allowed Chat IDs:\n";
      for (size_t i = 0; i < ALLOWED_CHAT_IDS.length(); i++) {
        if (ALLOWED_CHAT_IDS[i] == ',') {
          message += '\n';
        } else {
          message += ALLOWED_CHAT_IDS[i];
        }
      }
      bot.sendMessage(chat_id, message, "");
    } 
    else if (alertPin == HIGH) {
        bot.sendMessage(chat_id, "Alert...", "");
        bot.sendMessage(chat_id, "https://www.google.com/maps/search/?api=1&query=" + message_latitude + "," + message_longitude, "");
    }
    else if (text == "/User_add") {
      bot.sendMessage(chat_id, "Wait...", "");
      if (chat_id == CHAT_ID) {
        bot.sendMessage(chat_id, "Please provide the chat ID you want to add to the allowed list:", "");
        isAddingChatID = true;
      } else {
        bot.sendMessage(chat_id, "Unauthorized access. Only the bot owner can grant user control.", "");
      }
    } else if (isAddingChatID && chat_id == CHAT_ID) {
      if (userChatID == "") {
        userChatID = text;
        bot.sendMessage(chat_id, "Please enter the chat ID again to confirm:", "");
      } else if (userChatID == text) {
        ALLOWED_CHAT_IDS += "," + text;
        bot.sendMessage(chat_id, "User chat ID added to allowed list.", "");
        userChatID = "";
        isAddingChatID = false;
      }
    } else if (text == "/User_remove") {
      bot.sendMessage(chat_id, "Wait...", "");
      if (chat_id == CHAT_ID) {
        bot.sendMessage(chat_id, "Please provide the chat ID you want to remove from the allowed list:", "");
        isRemovingChatID = true;
      } else {
        bot.sendMessage(chat_id, "Unauthorized access. Only the bot owner can remove users.", "");
      }
    } else if (isRemovingChatID && chat_id == CHAT_ID) {
      int userIndex = ALLOWED_CHAT_IDS.indexOf(text);
      if (userIndex != -1) {
        ALLOWED_CHAT_IDS.remove(userIndex, text.length() + 1);  // +1 to remove comma as well
        bot.sendMessage(chat_id, "User chat ID removed from allowed list.", "");
      } else {
        bot.sendMessage(chat_id, "The provided chat ID was not found in the allowed list.", "");
      }
      isRemovingChatID = false;
    } else {
      bot.sendMessage(chat_id, "Wait...", "");
      bot.sendMessage(chat_id, "Invalid Input", "");
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control the system:\n\n";
      welcome += "/A - Command A\n";
      welcome += "/B - Command B\n";
      welcome += "/Users - Check Allowed Chat IDs\n";
      welcome += "/User_add - Add Chat ID\n";
      welcome += "/User_remove - Remove Chat ID\n";
      bot.sendMessage(chat_id, welcome, "");
    }
  }
}


void setup() {
  Serial.begin(9600);

  Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, 16, 17);
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());









  // WiFi.begin(ssid, password);
  // client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.println(WiFi.localIP());
  // Wire.setClock(400000);
  // Wire.begin();
  // delay(250);
  // Wire.beginTransmission(0x68);
  // Wire.write(0x6B);
  // Wire.write(0x00);
  // Wire.endTransmission();
  pinMode(TRIG_PIN, OUTPUT);   // set arduino pin to output mode
  pinMode(ECHO_PIN, INPUT);    // set arduino pin to input mode
  pinMode(BUZZER_PIN, OUTPUT); // set arduino pin to output mode

}

void loop() {

    if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages > 0) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  } 
    digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if(distance_cm < DISTANCE_THRESHOLD)
    digitalWrite(BUZZER_PIN, HIGH); // turn on Piezo Buzzer
  else
    digitalWrite(BUZZER_PIN, LOW);  // turn off Piezo Buzzer

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  delay(100);
  while (Serial2.available() > 0) {
          if (gps.encode(Serial2.read())) {
            if (gps.location.isValid()) {
              message_latitude = "Latitude: " + String(gps.location.lat(), 6) + "\n";
              message_longitude = "Longitude: " + String(gps.location.lng(), 6);
            } 
          }
  }
  url1 = "https://www.google.com/maps/search/?api=1&query=" + message_latitude + "," + message_longitude;

}
