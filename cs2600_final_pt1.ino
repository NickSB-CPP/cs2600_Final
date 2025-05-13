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
const char* mqtt_server = "34.27.138.43";  // Replace with your actual local IP
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
  lcd.print("PvB: 1 PvP: 2");   
  lcd.setCursor(0,1);             
  lcd.print("Auto: 3");    

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
      mqttClient.subscribe("move");
      mqttClient.subscribe("counterPlayer");
      mqttClient.subscribe("counterPlayer2");

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
    lcd.setCursor(5,0);
    lcd.print("1");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.setCursor(5,0);
    lcd.print("1");
    delay(1000);
    lcd.clear();
    pvb();
  }

  else if (message == "2") {
    lcd.clear();
    lcd.setCursor(12,0);
    lcd.print("2");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.setCursor(12,0);
    lcd.print("2");
    delay(1000);
    lcd.clear();
    pvp(topic, payload, length);
  } 
  else if (message == "3") {
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("3");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.setCursor(6,1);
    lcd.print("3");
    delay(1000);
    lcd.clear();
    autoP();
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

void pvb() { //note to self: when in different function, does not register callback function inputs

}

void pvp(char* topic, byte* payload, unsigned int length) {
  int game = 0;
  bool xDub = false;
  bool oDub = false;
  bool tie = false;
  bool one = false; 
  bool two = false;
  bool three = false;
  bool four = false;
  bool five = false;
  bool six = false;
  bool seven = false;
  bool eight = false;
  bool nine = false;

  char tic[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  int xO = 1; // 1 for X, 0 for O
  if (xO = 1)
  {
  lcd.clear();
  lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
  lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
  lcd.setCursor(0,1);
  lcd.print("Pick Space 1-9");
  mqttClient.publish("counterPlayer", "1");
  delay(3000);
  }
  else if (xO = 0)
  {
  lcd.clear();
  lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
  lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
  lcd.setCursor(0,1);
  lcd.print("Pick Space 1-9");
  mqttClient.publish("counterPlayer", "0");
  delay(3000);
  }

  int moves = 0;
  bool gameOver = false;

    while (!gameOver && moves < 9) {
      int move;
      bool choice;
      do {
        //move = int val
          choice = true;
          Serial.print("Message arrived [");
          Serial.print(topic);
          Serial.print("]: ");

          String message = "";
          for (int i = 0; i < length; i++) {
            message += (char)payload[i];
          }
          Serial.println(message);

          if (message == "1") { 
            if(one == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (one == false)
            {
            move = 1;
            one = true;
            mqttClient.publish("move", "1");
            delay(3000); // Publish every 5 seconds
            }
          }

          else if (message == "2") {
            if(two == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (two == false)
            {
            move = 2;
            two = true;
            mqttClient.publish("move", "2");
            delay(3000); // Publish every 5 seconds
            }
          } 
          else if (message == "3") {
            if(three == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (three == false)
            {
            move = 3;
            three = true;
            mqttClient.publish("move", "3");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "4") {
            if(four == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (four == false)
            {
            move = 4;
            four = true;
            mqttClient.publish("move", "4");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "5") {
            if(five == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (five == false)
            {
            move = 5;
            five = true;
            mqttClient.publish("move", "5");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "6") {
            if(six == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (six == false)
            {
            move = 6;
            six = true;
            mqttClient.publish("move", "6");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "7") {
            if(seven == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (seven == false)
            {
            move = 7;
            seven = true;
            mqttClient.publish("move", "7");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "8") {
            if(eight == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (eight == false)
            {
            move = 8;
            eight = true;
            mqttClient.publish("move", "8");
            delay(3000); // Publish every 5 seconds
            }
          }
          else if (message == "9") {
            if(nine == true)
            {
              choice = false; 
              if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("# already in use");
              }
            }
            else if (nine == false)
            {
            move = 9;
            nine = true;
            mqttClient.publish("move", "9");
            delay(3000); // Publish every 5 seconds
            }
          }
          else
          {
            Serial.println("Invalid input received");
            choice == false;
            if (xO == 1)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: X's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("Invalid");
              }
              else if (xO == 0)
              {
                lcd.clear();
                lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
                lcd.print("PVP: O's Turn");     // The print content is displayed on the LCD 
                lcd.setCursor(0,1);
                lcd.print("Invalid");
              }
          }
      } while (tic[move - 1] != ' ' && choice == false);

      if (xO == 1)
      {
        tic[move-1] = 'X';
      }
      else if (xO == 0)
      {
        tic[move-1] = 'O';
      }
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
        if (xO == 1) xDub = true;
        else if (xO == 0) oDub = true;
        game++;
      }

      if (moves == 9 && gameOver == false) 
      {
        gameOver = true;
        tie = true;
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
    if (xDub == true)
    {
      lcd.clear();
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
      lcd.print("X WON! Congrats!");     // The print content is displayed on the LCD 
      lcd.setCursor(0,1);
      lcd.print("Returning to menu...");
      delay(7000);
      setup();
    }
    else if (oDub == true)
    {
      lcd.clear();
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
      lcd.print("O WON! Congrats!");     // The print content is displayed on the LCD 
      lcd.setCursor(0,1);
      lcd.print("Returning to menu...");
      delay(7000);
      setup();
    }
    else if (tie == true)
    {
      lcd.clear();
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0 
      lcd.print("DRAW!!");     // The print content is displayed on the LCD 
      lcd.setCursor(0,1);
      lcd.print("Returning to menu...");
      delay(7000);
      setup();
    }
}
void autoP() {

}

bool i2CAddrTest(uint8_t addr) { 
  Wire.beginTransmission(addr); 
  if (Wire.endTransmission() == 0) { 
    return true; 
  } 
  return false; 
}

