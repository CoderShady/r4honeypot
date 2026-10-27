#include "WiFiS3.h"
#include "Arduino_LED_Matrix.h"

const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

ArduinoLEDMatrix matrix;
byte frame[8][12];

WiFiServer telnetDecoy(23);
WiFiServer sshDecoy(2222);
WiFiServer httpDecoy(80);

float phase1 = 0.0;
float phase2 = 0.0;
unsigned long alertTimer = 0;
bool isAlertActive = false;
int totalBreachAttempts = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  matrix.begin();
  randomSeed(analogRead(A0));

  Serial.println("\n=======================================================");
  Serial.println("    UNO R4 WiFi - HARDWARE CANARY & NETWORK TRIPWIRE    ");
  Serial.println("=======================================================");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("[CRITICAL] ESP32-S3 WiFi module not detected!");
    while (true);
  }

  Serial.print("[*] Connecting to subnet: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int dot = 0;
  while (WiFi.status() != WL_CONNECTED) {
    memset(frame, 0, sizeof(frame));
    frame[3][dot % 12] = 1;
    frame[4][dot % 12] = 1;
    matrix.renderBitmap(frame, 8, 12);
    dot++;
    delay(200);
    Serial.print(".");
  }

  while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
    delay(100);
  }

  telnetDecoy.begin();
  sshDecoy.begin();
  httpDecoy.begin();

  Serial.println("\n[+] CANARY NODE ARMED AND LISTENING");
  Serial.print("[+] Trap IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("[+] Armed Ports     : TCP 23 (Telnet), TCP 2222 (SSH), TCP 80 (HTTP)");
  Serial.println("-------------------------------------------------------");
  Serial.println("Waiting for unauthorized recon or lateral movement...\n");
}

void loop() {
  checkDecoyPort(telnetDecoy, 23, "TELNET-IOT-TRAP");
  checkDecoyPort(sshDecoy, 2222, "SSH-BANNER-TRAP");
  checkDecoyPort(httpDecoy, 80, "HTTP-ADMIN-TRAP");

  if (isAlertActive && (millis() - alertTimer > 8000)) {
    isAlertActive = false;
  }

  renderGenerativeWave();
  matrix.renderBitmap(frame, 8, 12);
  delay(55);
}

void checkDecoyPort(WiFiServer &server, int port, const char* serviceName) {
  WiFiClient client = server.available();
  if (client) {
    isAlertActive = true;
    alertTimer = millis();
    if (totalBreachAttempts < 12) totalBreachAttempts++;

    IPAddress attackerIP = client.remoteIP();

    Serial.println("🚨 [INTRUSION ALERT] TRIPWIRE TRIGGERED 🚨");
    Serial.print("  -> Timestamp (ms) : "); Serial.println(millis());
    Serial.print("  -> Service Hit    : "); Serial.print(serviceName);
    Serial.print(" (Port "); Serial.print(port); Serial.println(")");
    Serial.print("  -> Source IP      : "); Serial.println(attackerIP);

    String payload = "";
    unsigned long timeout = millis();
    while (client.connected() && (millis() - timeout < 250)) {
      while (client.available()) {
        char c = client.read();
        if (payload.length() < 160) payload += c;
      }
    }

    if (payload.length() > 0) {
      payload.trim();
      Serial.println("  -> Captured Probe :");
      Serial.println("     " + payload);
    } else {
      Serial.println("  -> Captured Probe : <SYN/Connect Port Scan - No Payload>");
    }

    if (port == 23) {
      client.println("BusyBox v1.31.1 (IoT-Gateway) built-in shell (ash)");
      client.print("iot-node-04 login: ");
    } else if (port == 2222) {
      client.println("SSH-2.0-OpenSSH_8.2p1 Ubuntu-4ubuntu0.5");
    } else if (port == 80) {
      client.println("HTTP/1.1 401 Unauthorized");
      client.println("WWW-Authenticate: Basic realm=\"IoT Core Admin Panel\"");
      client.println("Content-Type: application/json");
      client.println("Connection: close\r\n");
      client.println("{\"error\":\"unauthorized\",\"node\":\"R4-SEC-GATEWAY\"}");
    }

    delay(50);
    client.stop();
    Serial.println("-------------------------------------------------------\n");
  }
}

void renderGenerativeWave() {
  memset(frame, 0, sizeof(frame));

  for (int i = 0; i < totalBreachAttempts && i < 12; i++) {
    frame[0][i] = 1;
  }

  float speed1 = isAlertActive ? 0.65 : (0.18 + (random(-3, 4) * 0.01));
  float speed2 = isAlertActive ? 0.95 : (0.09 + (random(-2, 3) * 0.01));
  float amp    = isAlertActive ? 2.8  : 1.8;

  for (int col = 0; col < 12; col++) {
    float w1 = sin(phase1 + (col * 0.48));
    float w2 = cos(phase2 - (col * 0.32));
    float combined = (w1 * 0.65) + (w2 * 0.35);

    int jitter = isAlertActive ? random(-1, 2) : 0;

    int y = 4 + round(combined * amp) + jitter;

    int minRow = (totalBreachAttempts > 0) ? 1 : 0;
    if (y < minRow) y = minRow;
    if (y > 7) y = 7;

    frame[y][col] = 1;

    if (!isAlertActive && (y + 1 <= 7)) {
      frame[y + 1][col] = 1;
    } else if (isAlertActive && (col % 2 == 0)) {
      frame[7][col] = 1;
    }
  }

  phase1 += speed1;
  phase2 += speed2;
  if (phase1 > 628.0) phase1 -= 628.0;
  if (phase2 > 628.0) phase2 -= 628.0;
}
