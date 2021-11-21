
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "DreamLand"; // MK WIFI
const char *password ="emlaymaymedungxaiwifichuanhaem69212169"; //Pass WIFI
//const char *host = "192.168.1.4";  // Dia chi IP may

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  // Ket noi duoc neu thanh cong
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  HTTPClient http;    


  //---gui du lieu len server php--------//
  String data = "load=1";// Chuoi up len 
  http.begin("http://192.168.1.4:80/doan/max_min.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(data);   //Send the request
  //---gui du lieu len server php--------//

  //---nhan phan hoi tu server php--------//
  Serial.println(httpCode);   //Print HTTP return code
  Serial.print("load: ");
  String payload = http.getString();    //Get the response payload
  Serial.println(payload);    //Print request response payload
  //---nhan phan hoi tu server php--------//

  
  http.end();  //Close connection
  
  delay(1000);  //Post Data at every 2 seconds
}
//=======================================================================
