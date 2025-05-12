#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 
 
#define SDA 14                    //Define SDA pins 
#define SCL 13                    //Define SCL pins 
 
/* 
 * note:If lcd1602 uses PCF8574T, IIC's address is 0x27, 
 *      or lcd1602 uses PCF8574AT, IIC's address is 0x3F. 
*/ 

#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_wifi.h>

// Wi-Fi credentials
const char* ssid = "SpectrumSetup-FE";
const char* password = "Ns03@Jm01_Rp05@Jm44";

// IP of the machine running Mosquitto (on your local network)
const char* mqtt_server = "34.71.227.204";  // Replace with your actual local IP
unsigned int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

LiquidCrystal_I2C lcd(0x27,16,2);  
void setup() { 
  Wire.begin(SDA, SCL);           // attach the IIC pin 
  if (!i2CAddrTest(0x27)) { 
    lcd = LiquidCrystal_I2C(0x3F, 16, 2); 
  } 
  lcd.init();                     // LCD driver initialization 
  lcd.backlight();                // Open the backlight 

  lcd.setCursor(0,0);             
  lcd.print("Local Play:  1");   
  lcd.setCursor(0,1);             
  lcd.print("Remote Play: 2");    

  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT Broker
  delay(5000);
  mqttClient.setServer(mqtt_server, 1883);  // Mosquitto default port
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Connect to MQTT broker (you can use a unique client ID)
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      mqttClient.subscribe("esp32/tictac");  // Subscribe to topic

    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }

} 

void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "1") { 
    lcd.clear();
    lcd.setCursor(13,0);
    lcd.print("1");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.setCursor(13,0);
    lcd.print("1");
    delay(1000);
    lcd.clear();
    local();
  }

  else if (message == "2") {
    lcd.clear();
    lcd.setCursor(13,1);
    lcd.print("2");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.setCursor(13,1);
    lcd.print("2");
    delay(1000);
    lcd.clear();
    remote();
  } 
  else if (message == "3") {
    Serial.println("Restarting ESP32...");
    ESP.restart(); // Restart the ESP32
  }
  else if (message == "4") {
    Serial.println("Stopping program...");
    mqttClient.disconnect(); // esp disconnect to mqtt
  }
  else {
    Serial.println("Invalid input received");
  }
}

void loop() {
  mqttClient.loop();
}

void local() { //note to self: when in different function, does not register callback function inputs
  int game = 0;
  int botX = 0;
  int botO = 0;
  int tie = 0;

  while (game < 100) {
    char tic[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    int xO = 1; // 1 for X, 0 for O
    int moves = 0;
    bool gameOver = false;

    while (!gameOver && moves < 9) {
      int move;
      do {
        move = random(9);
      } while (tic[move] != ' ');

      tic[move] = xO ? 'X' : 'O';
      moves++;

      // Check for win
      if ((tic[0] == tic[1] && tic[1] == tic[2] && tic[0] != ' ') ||
          (tic[3] == tic[4] && tic[4] == tic[5] && tic[3] != ' ') ||
          (tic[6] == tic[7] && tic[7] == tic[8] && tic[6] != ' ') ||
          (tic[0] == tic[3] && tic[3] == tic[6] && tic[0] != ' ') ||
          (tic[1] == tic[4] && tic[4] == tic[7] && tic[1] != ' ') ||
          (tic[2] == tic[5] && tic[5] == tic[8] && tic[2] != ' ') ||
          (tic[0] == tic[4] && tic[4] == tic[8] && tic[0] != ' ') ||
          (tic[2] == tic[4] && tic[4] == tic[6] && tic[2] != ' ')) {
        gameOver = true;
        if (xO == 1) botX++;
        else if (xO == 0) botO++;
        game++;
      }

      if (moves == 9 && gameOver == false) 
      {
        gameOver = true;
        tie++;
        game++;
      }
      if(xO == 1) // Switch turn
      {
        xO = 0;
      }
      else if (xO == 0)
      {
        xO = 1;
      }
    }


    // Display result on LCD
    lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
    lcd.print("Bot v Bot");     // The print content is displayed on the LCD 
    lcd.print(" G:");
    lcd.print(game);
    lcd.setCursor(0, 1);
    lcd.print("X:");
    lcd.print(botX);
    lcd.print(" O:");
    lcd.print(botO);
    lcd.print(" T:");
    lcd.print(tie);
    delay(5000); // Delay to view result
  } 
  while(game == 100)
  {
    delay(1);
  }
}

void remote() {

}

bool i2CAddrTest(uint8_t addr) { 
  Wire.beginTransmission(addr); 
  if (Wire.endTransmission() == 0) { 
    return true; 
  } 
  return false; 
}