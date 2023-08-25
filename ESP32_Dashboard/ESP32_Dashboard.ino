#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// GPIO the servo is attached to
static const int servoPin1 = 13;
static const int servoPin2 = 12;
static const int ledPin1 = 14;
static const int ledPin2 = 27;
static const int ledPin3 = 26;
static const int ledPin4 = 25;

// Replace with your network credentials
const char* ssid     = "Room105"; //replace with your SSID
const char* password = "indonesiaraya"; //replace with your password

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString1  = String(0);
String valueString2  = String(0);
String valueSpeed = String(0);
String btnVal1 = String(0);
String btnVal2 = String(0);
String btnVal3 = String(0);
String btnVal4 = String(0);
int delayServo = 0;

int pos1 = 0;
int pos2 = 0;

int currPos1 = 0;
int currPos2 = 0;
int targetPos1 = 0;
int targetPos2 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  myservo1.attach(servoPin1);  // attaches the servo on the servoPin to the servo object
  myservo2.attach(servoPin2);
  myservo1.write(0);  // attaches the servo on the servoPin to the servo object
  myservo2.write(0);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println(" <title>ESP32 IOT DASHBOARD</title>");
            // CSS 
            client.println("<style> body{font-family: arial;} img{width: 350px; height: 45px; padding-top: 7px; padding-bottom: 5px;}");
            client.println(".slider{width: 300px; margin-top: 10px;}.btn-block{word-spacing: 2rem; padding-top: 30px;}");
            client.println(".btn{background-color: red; color: white; width: 47px; height: 33px;}");
            client.println("footer{font-family: arial; font-weight: bold; margin-top: 3rem;}</style>");
            //Javascript
            client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>");    
            //Header
            client.println("</head><body><center><img src='http://usecaseiot.com/png2.png' alt='Responsive image'><h3>ESP32 DASHBOARD</h3>");
            //Slider
            client.println("<p><span id='speed'>Speed</span>");          
            client.println("<input type='range' min='0' max='5' class='slider' id='speedSlider1' onchange='speed(this.value)' value=\""+valueSpeed+"\"/></p>");
            client.println("<p><span id='servoPos1'></span>&deg;");          
            client.println("<input type='range' min='0' max='180' class='slider' id='servoSlider1' onchange='servo1(this.value)' value=\""+valueString1+"\"/></p>");
            client.println("<p><span id='servoPos2'></span>&deg;");          
            client.println("<input type='range' min='0' max='180' class='slider' id='servoSlider2' onchange='servo2(this.value)' value=\""+valueString2+"\"/></p>");
            //Button
            client.println("<div class='btn-block'>");
            client.println("<button id= 'btn1' class='btn' onclick='button1(this.value)' value=\""+btnVal1+"\"/><span id='btnlabel1'>OFF</span></button>"); 
            client.println("<button id= 'btn2' class='btn' onclick='button2(this.value)' value=\""+btnVal2+"\"/><span id='btnlabel2'>OFF</span></button>"); 
            client.println("<button id= 'btn3' class='btn' onclick='button3(this.value)' value=\""+btnVal3+"\"/><span id='btnlabel3'>OFF</span></button>"); 
            client.println("<button id= 'btn4' class='btn' onclick='button4(this.value)' value=\""+btnVal4+"\"/><span id='btnlabel4'>OFF</span></button></div>");

            client.println("<footer>Copyright &copy;2021 Rizky Project</footer>");
            
            //Ajax Javascript
            client.println("<script>var slider1 = document.getElementById('servoSlider1');");
            client.println("var servoP1 = document.getElementById('servoPos1'); servoP1.innerHTML = slider1.value;");
            client.println("slider1.oninput = function() { slider1.value = this.value; servoP1.innerHTML = this.value; }");
            
            client.println("var slider2 = document.getElementById('servoSlider2');");
            client.println("var servoP2 = document.getElementById('servoPos2'); servoP2.innerHTML = slider2.value;");
            client.println("slider2.oninput = function() { slider2.value = this.value; servoP2.innerHTML = this.value; }");

            client.println("var btn1 = document.getElementById('btn1'); var btn2 = document.getElementById('btn2'); var btn3 = document.getElementById('btn3'); var btn4 = document.getElementById('btn4')");
            client.println("var btnlabel1 = document.getElementById('btnlabel1'); var btnlabel2 = document.getElementById('btnlabel2'); var btnlabel3 = document.getElementById('btnlabel3'); var btnlabel4 = document.getElementById('btnlabel4');");
            client.println("var toggled1 = false; var toggled2 = false; var toggled3 = false; var toggled4 = false; var state1 = 0; var state2 = 0;var state3 = 0;var state4 = 0;");

            client.println("$.ajaxSetup({timeout:1000});");

            client.println("function button1(){"); // Button 1
            client.println("if (toggled1 == false){");
            client.println("state1 = 0; btnlabel1.innerHTML = 'ON'; btn1.style.backgroundColor = 'green'; btn1.style.color = 'white'; toggled1 = true;}");
            client.println("else{state1 = 1; btnlabel1.innerHTML = 'OFF'; btn1.style.backgroundColor = 'red'; btn1.style.color = 'white'; toggled1 = false;}");
            client.println("$.get('?button1='+ state1 + '&'); {Connection: close};}");
            client.println("function button2(){"); // Button 2
            client.println("if (toggled2 == false){");
            client.println("state2 = 0; btnlabel2.innerHTML = 'ON'; btn2.style.backgroundColor = 'green'; btn2.style.color = 'white'; toggled2 = true;}");
            client.println("else{state2 = 1; btnlabel2.innerHTML = 'OFF'; btn2.style.backgroundColor = 'red'; btn2.style.color = 'white'; toggled2 = false;}");
            client.println("$.get('?button2='+ state2 + '&'); {Connection: close};}");
            client.println("function button3(){"); // Button 3
            client.println("if (toggled3 == false){");
            client.println("state3 = 0; btnlabel3.innerHTML = 'ON'; btn3.style.backgroundColor = 'green'; btn3.style.color = 'white'; toggled3 = true;}");
            client.println("else{state3 = 1; btnlabel3.innerHTML = 'OFF'; btn3.style.backgroundColor = 'red'; btn3.style.color = 'white'; toggled3 = false;}");
            client.println("$.get('?button3='+ state3 + '&'); {Connection: close};}");
            client.println("function button4(){"); // Button 4
            client.println("if (toggled4 == false){");
            client.println("state4 = 0; btnlabel4.innerHTML = 'ON'; btn4.style.backgroundColor = 'green'; btn4.style.color = 'white'; toggled4 = true;}");
            client.println("else{state4 = 1; btnlabel4.innerHTML = 'OFF'; btn4.style.backgroundColor = 'red'; btn4.style.color = 'white'; toggled4 = false;}");
            client.println("$.get('?button4='+ state4 + '&'); {Connection: close};}");

            client.println("function speed(pos){");
            client.println("$.get('/?speed=' + pos + '&'); {Connection: close};}");
            client.println("function servo1(pos){");
            client.println("$.get('/?value1=' + pos + '&'); {Connection: close};}");
            client.println("function servo2(pos){");
            client.println("$.get('/?value2=' + pos + '&'); {Connection: close};}</script>");
           
            client.println("</center></body></html>");     
            
            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?speed=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueSpeed = header.substring(pos1+1, pos2); 
            } 
            else if(header.indexOf("GET /?value1=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString1 = header.substring(pos1+1, pos2); 
            } 
            else if(header.indexOf("GET /?value2=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString2 = header.substring(pos1+1, pos2);
            }
            else if(header.indexOf("GET /?button1=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              btnVal1 = header.substring(pos1+1, pos2);
            }
             else if(header.indexOf("GET /?button2=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              btnVal2 = header.substring(pos1+1, pos2);
            }
             else if(header.indexOf("GET /?button3=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              btnVal3 = header.substring(pos1+1, pos2);
            }
            else if(header.indexOf("GET /?button4=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              btnVal4 = header.substring(pos1+1, pos2);
            }

              //Servo Speed Level
               Serial.print("Speed Level = ");
               Serial.println(valueSpeed);
               if(valueSpeed == "1"){
                  delayServo = 100;
               }
               else if(valueSpeed == "2"){
                  delayServo = 70;
               }
               else if(valueSpeed == "3"){
                  delayServo = 45;
               }
               else if(valueSpeed == "4"){
                  delayServo = 20;
               }
               else if(valueSpeed == "5"){
                  delayServo = 5;
               }
               else{
                  delayServo = 0;
               }
              
              //Rotate the servo
              targetPos1 = valueString1.toInt();
              if(currPos1 > targetPos1){
                while(currPos1 > targetPos1){
                  currPos1 -= 1;
                  myservo1.write(currPos1);
                  delay(delayServo);
                }
              }
              else if(currPos1 < targetPos1){
                while(currPos1 < targetPos1){
                  currPos1 += 1;
                  myservo1.write(currPos1);
                  delay(delayServo);
                }
              }
              Serial.print("Servo 1 = ");
              Serial.println(targetPos1);
              
              targetPos2 = valueString2.toInt();
              if(currPos2 > targetPos2){
                while(currPos2 > targetPos2){
                  currPos2 -= 1;
                  myservo2.write(currPos2);
                  delay(delayServo);
                }
              }
              else if(currPos2 < targetPos2){
                while(currPos2 < targetPos2){
                  currPos2 += 1;
                  myservo2.write(currPos2);
                  delay(delayServo);
                }
              }
              Serial.print("Servo 2 = ");
              Serial.println(targetPos2);   

              
              //LED Control
              digitalWrite(ledPin1, btnVal1.toInt()); //LED 1
              Serial.print("Channel 1 = ");
              Serial.println(btnVal1); 
               
              digitalWrite(ledPin2, btnVal2.toInt()); //LED 2
              Serial.print("Channel 2 = ");
              Serial.println(btnVal2); 
              
              digitalWrite(ledPin3, btnVal3.toInt()); //LED 3
              Serial.print("Channel 3 = ");
              Serial.println(btnVal3);   

              digitalWrite(ledPin4, btnVal4.toInt()); //LED 4
              Serial.print("Channel 4 = ");
              Serial.println(btnVal4);   
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
