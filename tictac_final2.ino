#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_wifi.h>

#define SDA 14
#define SCL 13

// Wi-Fi and MQTT
const char* ssid = "iPhone (3)";
const char* password = "7ehs17k64dx6j";
const char* mqtt_server = "104.198.150.251";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);
volatile int receivedMove = -1;

// Board state
char tic[9];
bool used[9];
int xO = 1;  // 1 = X, 0 = O
int moves = 0;

void setup() {
  Wire.begin(SDA, SCL);
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client")) {
      mqttClient.subscribe("esp32/tictac");
      mqttClient.subscribe("player/move");
      mqttClient.subscribe("player/rematch");
    } else {
      Serial.print("MQTT connect failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }

  showMenu();
}

void loop() {
  mqttClient.loop();
}

void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PvB: 1 PvP: 2");
  lcd.setCursor(0, 1);
  lcd.print("Auto: 3");

  while (true) {
    mqttClient.loop();
    if (receivedMove > 0) {
      int mode = receivedMove;
      receivedMove = -1;
      if (mode == 1) {
        pvb();
      } else if (mode == 2) {
        pvp();
      } else if (mode == 3) {
        // implement autoP()
      }
      break;
    }
    delay(100);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("MQTT Message: " + message);

  if (String(topic) == "esp32/tictac" && (message == "1" || message == "2" || message == "3")) {
    receivedMove = message.toInt();
  } else if (String(topic) == "player/move" && message.toInt() >= 1 && message.toInt() <= 9) {
    receivedMove = message.toInt();
  } else if (String(topic) == "player/rematch" && (message == "Y" || message == "y" || message == "N" || message == "n")) {
    receivedMove = (message == "Y" || message == "y") ? 100 : 200;
  }
}

void pvp() {
  resetGame();
  lcd.clear();
  lcd.print("PvP Started!");
  delay(2000);

  while (true) {
    mqttClient.loop();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(xO == 1 ? "X's Turn" : "O's Turn");
    lcd.setCursor(0, 1);
    lcd.print("Pick 1-9");

    mqttClient.publish("counterPlayer", xO == 1 ? "1" : "0");

    while (receivedMove == -1) {
      mqttClient.loop();
      delay(100);
    }

    int move = receivedMove - 1;

    if (move < 0 || move > 8 || used[move]) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Move!");
      delay(2000);
      receivedMove = -1;
      continue;
    }

    used[move] = true;
    tic[move] = (xO == 1) ? 'X' : 'O';
    moves++;
    mqttClient.publish("move", String(move + 1).c_str());
    receivedMove = -1;

    // Debug: print board
    Serial.print("Board state: ");
    for (int i = 0; i < 9; i++) Serial.print(tic[i]);
    Serial.println();

    if (checkWin()) {
      Serial.println("Win detected");
      const char* result = (xO == 1) ? "X wins" : "O wins";
      mqttClient.publish("move", result);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(result);
      delay(3000);
      bool rematch = handleRematch();
      if (rematch) {
        pvp();
      } else {
        showMenu();
      }
      return;
    }

    if (moves == 9) {
      mqttClient.publish("move", "Draw");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("It's a DRAW!");
      delay(3000);
      bool rematch = handleRematch();
      if (rematch) {
        pvp();
      } else {
        showMenu();
      }
      return;
    }

    xO = 1 - xO;
  }
}

void resetGame() {
  for (int i = 0; i < 9; i++) {
    tic[i] = ' ';
    used[i] = false;
  }
  xO = 1;
  moves = 0;

  Serial.println("Board reset:");
  for (int i = 0; i < 9; i++) Serial.print(tic[i]);
  Serial.println();
}

bool checkWin() {
  int wins[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},
    {0,3,6}, {1,4,7}, {2,5,8},
    {0,4,8}, {2,4,6}
  };

  for (int i = 0; i < 8; i++) {
    Serial.print("Checking combo: ");
    Serial.print(wins[i][0]); Serial.print(tic[wins[i][0]]);
    Serial.print(", ");
    Serial.print(wins[i][1]); Serial.print(tic[wins[i][1]]);
    Serial.print(", ");
    Serial.print(wins[i][2]); Serial.println(tic[wins[i][2]]);

    if (tic[wins[i][0]] == tic[wins[i][1]] &&
        tic[wins[i][1]] == tic[wins[i][2]] &&
        tic[wins[i][0]] != ' ') {
      return true;
    }
  }
  return false;
}

bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

bool handleRematch() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Play again?");
  lcd.setCursor(0, 1);
  lcd.print("Y or N via TUI");

  receivedMove = -1;
  while (receivedMove != 100 && receivedMove != 200) {
    mqttClient.loop();
    delay(100);
  }

  return (receivedMove == 100);
}

void pvb() {
  resetGame();
  lcd.clear();
  lcd.print("PvB Started!");
  delay(2000);

  while (true) {
    mqttClient.loop();

    if (xO == 1) {  // Human turn
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your Turn (X)");
      lcd.setCursor(0, 1);
      lcd.print("Pick 1-9");
      mqttClient.publish("counterPlayer", "1");

      while (receivedMove == -1) {
        mqttClient.loop();
        delay(100);
      }
    } else {  // Bot turn
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bot Thinking...");
      mqttClient.publish("counterPlayer", "0");

      while (receivedMove == -1) {
        mqttClient.loop();
        delay(100);
      }
    }

    int move = receivedMove - 1;
    receivedMove = -1;

    if (move < 0 || move > 8 || used[move]) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Move!");
      delay(2000);
      continue;
    }

    used[move] = true;
    tic[move] = (xO == 1) ? 'X' : 'O';
    moves++;
    mqttClient.publish("move", String(move + 1).c_str());

    if (checkWin()) {
      const char* result = (xO == 1) ? "X wins" : "O wins";
      mqttClient.publish("move", result);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(result);
      delay(3000);
      bool rematch = handleRematch();
      if (rematch) {
        pvb();
      } else {
        showMenu();
      }
      return;
    }

    if (moves == 9) {
      mqttClient.publish("move", "Draw");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("It's a DRAW!");
      delay(3000);
      bool rematch = handleRematch();
      if (rematch) {
        pvb();
      } else {
        showMenu();
      }
      return;
    }

    xO = 1 - xO;
  }
}

