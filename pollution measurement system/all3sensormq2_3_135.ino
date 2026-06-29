#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- WiFi ----------------
const char* ssid = "SERVER-ISDR";
const char* password = "ISDR@430";

// Python Flask server IP
const char* serverURL = "http://192.168.31.117:5000/data";

// ---------------- MQ2 ----------------
#define MQ2_AO 34
#define MQ2_DO 25

// ---------------- MQ3 ----------------
#define MQ3_AO 35
#define MQ3_DO 26

// ---------------- MQ135 ----------------
#define MQ135_AO 32
#define MQ135_DO 27

// ---------------- Buzzer ----------------
#define BUZZER 13

// ---------------- dht11 ----------------
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- Thresholds ----------------
#define MQ2_THRESHOLD 4000
#define MQ3_THRESHOLD 4000
#define MQ135_THRESHOLD 4000

// ---------------- Variables ----------------
int mq2_analog, mq3_analog, mq135_analog;
int mq2_digital, mq3_digital, mq135_digital;
float temperature, humidity;

void setup()
{
  Serial.begin(115200);

  pinMode(MQ2_DO, INPUT);
  pinMode(MQ3_DO, INPUT);
  pinMode(MQ135_DO, INPUT);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  analogReadResolution(12);
  dht.begin();

  // OLED
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");                    
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 20);
  display.println("START");
  display.display();

  delay(2000);

  // WiFi Connection
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Read Sensors
  mq2_analog = analogRead(MQ2_AO);
  mq3_analog = analogRead(MQ3_AO);
  mq135_analog = analogRead(MQ135_AO);

  mq2_digital = digitalRead(MQ2_DO);
  mq3_digital = digitalRead(MQ3_DO);
  mq135_digital = digitalRead(MQ135_DO);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Alert Status
  bool alert = false;

  if (mq2_analog > MQ2_THRESHOLD ||
      mq3_analog > MQ3_THRESHOLD ||
      mq135_analog > MQ135_THRESHOLD)
  {
    alert = true;
    digitalWrite(BUZZER, HIGH);
  }
  else
  {
    digitalWrite(BUZZER, LOW);
  }

  String status = alert ? "ALERT" : "NORMAL";

  // Serial Monitor
  Serial.println("========================");

  Serial.print("MQ2: ");
  Serial.println(mq2_analog);

  Serial.print("MQ3: ");
  Serial.println(mq3_analog);

  Serial.print("MQ135: ");
  Serial.println(mq135_analog);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("STATUS: ");
  Serial.println(status);

  // Digital detection messages
  if (mq2_digital == LOW)
    Serial.println("MQ2 Gas Detected");

  if (mq3_digital == LOW)
    Serial.println("MQ3 Alcohol Detected");

  if (mq135_digital == LOW)
    Serial.println("MQ135 Poor Air Quality");

  // OLED
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("AIR MONITOR");

  display.setCursor(0, 15);
  display.print("MQ2: ");
  display.println(mq2_analog);

  display.setCursor(0, 28);
  display.print("MQ3: ");
  display.println(mq3_analog);

  display.setCursor(0, 41);
  display.print("MQ135: ");
  display.println(mq135_analog);

  display.setCursor(80, 15);
  display.print("Temp:");
  display.print(temperature);

  display.setCursor(80, 28);
  display.print("Humidity:");
  display.print(humidity);

  display.setCursor(0, 55);
  display.print(status);

  display.display();

  // Send Data to Python Flask
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(serverURL);

    http.addHeader(
      "Content-Type",
      "application/x-www-form-urlencoded"
    );

    String postData =
      "mq2=" + String(mq2_analog) +
      "&mq3=" + String(mq3_analog) +
      "&mq135=" + String(mq135_analog) +
      "&temp=" + String(temperature) +
      "&hum=" + String(humidity) +
      "&status=" + status;

    int httpCode = http.POST(postData);

    Serial.print("HTTP Response: ");
    Serial.println(httpCode);

    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }

  delay(300000); 
}
