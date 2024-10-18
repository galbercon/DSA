#include <SPI.h>
#include <Ethernet.h>

// Nastavitve MAC in IP naslova
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x27 };
IPAddress ip(10, 8, 128, 43);

// Inicializacija strežnika
EthernetServer server(80);

// Definicija pina za LED
const int ledPin = 2;

void setup() {
  // Začetek Ethernet povezave
  Ethernet.begin(mac, ip);
  server.begin();

  // Nastavitev LED pina kot izhod
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("Strežnik je pripravljen");
}

void loop() {
  // Sprejemanje novih klientov
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Nov klient");
    boolean currentLineIsBlank = true;
    String request = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        
        if (c == '\n' && currentLineIsBlank) {
          // HTTP odgovor
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Vklopi/Izklopi LED</title>");
          client.println("<style>");
          client.println("body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; background-color: #f0f8ff; margin: 0; }");
          client.println("h1 { color: #4682b4; }");
          client.println("button { padding: 10px 20px; margin: 10px; background-color: #4682b4; color: white; border: none; cursor: pointer; }");
          client.println("button:hover { background-color: #5a9bd3; }");
          client.println(".container { text-align: center; }");
          client.println("</style></head>");
          client.println("<body>");
          client.println("<div class=\"container\">");
          client.println("<h1>Vklopi/Izklopi LED</h1>");
          client.println("<a href=\"/LED_ON\"><button>Vklopi LED</button></a>");
          client.println("<a href=\"/LED_OFF\"><button>Izklopi LED</button></a>");
          client.println("<a href=\"/SLOW_BLINK\"><button>Pocasno utripanje</button></a>");
          client.println("<a href=\"/FAST_BLINK\"><button>Hitro utripanje</button></a>");
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    // Branje HTTP glave
    if (request.indexOf("GET /LED_ON") != -1) {
      digitalWrite(ledPin, HIGH);  // Vklop LED
      Serial.println("LED vklopljena");
    }
    if (request.indexOf("GET /LED_OFF") != -1) {
      digitalWrite(ledPin, LOW);   // Izklop LED
      Serial.println("LED izklopljena");
    }
    if (request.indexOf("GET /SLOW_BLINK") != -1) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
      }
      Serial.println("Pocasno utripanje");
    }
    if (request.indexOf("GET /FAST_BLINK") != -1) {
      for (int i = 0; i < 20; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
      }
      Serial.println("Hitro utripanje");
    }
    delay(1);
    client.stop();
    Serial.println("Klient prekinil povezavo");
  }
}
