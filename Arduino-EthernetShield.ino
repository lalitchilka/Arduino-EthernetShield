#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 2, 94);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

//LED to turn on/off
//int RED = 8;
String HTTP_req;          // stores the HTTP request

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  HTTP_req = "";
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if( HTTP_req.length() < 120)
          HTTP_req += c;  // save the HTTP request 1 char at a time
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          Serial.println(HTTP_req);      
          if (HTTP_req.indexOf("ajaxrefresh") >= 0 ) {
            // read switch state and analog input
            ajaxRequest(client);
            break;
          }
          else {              
            client.println("<!DOCTYPE HTML>");
            client.println("<html lang=\"en\">");
            client.println("<script>window.setInterval(function(){");
            client.println("nocache = \"&nocache=\" + Math.random() * 10;");
            client.println("var request = new XMLHttpRequest();");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("document.getElementById(\"digital_data\")\
.innerHTML = this.responseText;");
            client.println("}}}}");
            client.println(
              "request.open(\"GET\", \"ajaxrefresh\" + nocache, true);");
            client.println("request.send(null);");
            client.println("}, 5000);");
            client.println("</script><link href=\"https://raw.githack.com/lalitchilka/CSSForArduino/master/one-page-wonder.min.css\" rel=\"stylesheet\"></head>");
            // output the value of each analog input pin
            client.println("<header class=\"masthead text-center text-white\">");
            client.println("<div class=\"container\">");
            client.print("<h1>Digital Values</h1>");
            client.println("<div id=\"digital_data\">Digital digital input values loading.....</div>");
            client.println("</header>");
            client.println("</div>");
            client.println("</html>");
            break;
         }
    }    
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        }
        }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    HTTP_req = "";
    Serial.println("client disconnected");
  }
  }

// send the state of the switch to the web browser
void ajaxRequest(EthernetClient client)
{
  for (int digitalChannel = 0; digitalChannel < 14; digitalChannel++) {
    int digitalReading = digitalRead(digitalChannel);
    client.print("digital input ");
    client.print(digitalChannel);
    client.print(" is ");
    client.print(digitalReading);
    client.println("<br />");
  }
}
