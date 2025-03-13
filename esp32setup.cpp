#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>

#define EEPROM_SIZE 512
#define AP_SSID "ESP32_Provisioning"
#define AP_PASSWORD "12345678"
#define DNS_PORT 53

// Pin definitions for LEDs
const int ledPin1 = 4;
const int ledPin2 = 17;
const int ledPin3 = 18;

DNSServer dnsServer;
WebServer server(80);

void handleRoot() {
      Serial.println("Captive portal is now open.");
  String html = R"rawliteral(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ScentVision</title>
</head>
<body>
    <header>
        <img id="logo">
        <h3 id="title">ScentVision</h2>
        <h1>Connect<span id="ed">ed</span></h1>
        <p id="instruction1">your device to wifi to get started</p>
        <p id="instruction2">to your preferred network</p>
        <p id="instruction3">by inputting your info</p>
    </header>

    <div id="search-for-networks-link">
        <a href="#" onclick="searchForNetworks()">Search for available networks</a>
    </div>

    <div id="networks-list-container">
        <h4 id="loading-networks-text">Loading networks...</h4>
        <div id="networks"></div>
        </div>
    </div>

    <div id="form-div">
        <form>
            <label>SSID</label>
            <br>
            <input type="text" id="ssid" name="ssid">
            <br>
            <label>Password</label>
            <br>
            <input type="text" id="password" name="pass">
            <br>
        </form>
        <div id="connect-button-div">
            <button id="connect-button" type="submit" onclick="byeForm(event)">Connect</button>
        </div>
    </div>

    <div id="establishing-network-div">
        <h3 id="connecting-to-text">Connecting to...</h3>
        <h3 id="connected-to-text">Congrats! <br> Your device is now connected to</h3>
        <h2 id="network-ssid">Spectrum-Wifi-32</h2>
        <div id="done-button-div" style="display: none;">
            <button id="done-button" onclick="finishConnection()">Done</button>
        </div>
    </div>

    <div id="loading-bar-element">
        <div id="loading-bar-container">
            <div id="loading-bar-animation"></div>
        </div>
    </div>

    <script>
function searchForNetworks() {
    const search = document.getElementById("search-for-networks-link");
    const networkList = document.getElementById("networks-list-container");
    const loadingNetworksText = document.getElementById("loading-networks-text");
    const loadingBarElement = document.getElementById("loading-bar-element");
    const networksContainer = document.getElementById("networks");

    // Hide instruction 1 and show instruction 2
    document.getElementById("instruction1").style.display = "none";
    document.getElementById("instruction2").style.display = "block";

    // Show loading elements
    search.style.display = "none";
    networkList.style.display = "block";
    loadingNetworksText.style.display = "block";
    loadingBarElement.style.display = "block";

    // Fetch networks from the ESP32
    fetch('/scan')
        .then(response => response.json()) // Parse response as JSON
        .then(networks => {
            // Hide loading elements
            loadingNetworksText.style.display = "none";
            loadingBarElement.style.display = "none";

            // Clear the networks container
            networksContainer.innerHTML = "";

            // Parse and display the networks
            networks.forEach(network => {
                // Create a div for each network
                const networkDiv = document.createElement("div");
                networkDiv.classList.add("network");
                networkDiv.textContent = `${network.ssid} (${network.rssi} dBm)`; // Display SSID and RSSI

                // Add the network div to the container
                networksContainer.appendChild(networkDiv);

                // Attach a click event listener to the network div
                networkDiv.addEventListener("click", () => {
                    const ssid = network.ssid; // Use the SSID directly
                    document.getElementById("ssid").value = ssid;

                    // Move to the next section
                    document.getElementById("instruction2").style.display = "none";
                    document.getElementById("instruction3").style.display = "block";
                    document.getElementById("form-div").style.display = "block";
                    networkList.style.display = "none";
                });
            });
        })
        .catch(error => {
            console.error("Error fetching networks: ", error);
            alert("An error occurred while fetching networks. Please try again.");
        });
}



        function byeForm(event){
            event.preventDefault();
            const ssid = document.getElementById("ssid").value;
            const password = document.getElementById("password").value;

             document.getElementById("form-div").style.display = "none";
             document.getElementById("establishing-network-div").style.display = "block";
             document.getElementById("connecting-to-text").style.display = "block";
             document.getElementById("network-ssid").textContent = ssid;
             document.getElementById("loading-bar-element").style.display = "block";
             
            fetch('/connect', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(password)}`
            })
            .then(response => response.text())
            .then(data => {
              console.log("ESP32 Response: ", data);
                
                if(data.includes("success")){
                    setTimeout(() => {
                      document.getElementById("loading-bar-element").style.display = "none";
                      document.getElementById("connecting-to-text").style.display = "none";
                      document.getElementById("connected-to-text").style.display = "block";
                      document.getElementById("instruction3").style.display = "none";
                      document.getElementById("ed").style.display = "inline";
                      document.getElementById("done-button-div").style.display = "block";
                    }, 5000);

                } else {
                    alert("Failed to connect. Please try again");
                    document.getElementById("establishing-network-div").style.display ="none";
                    document.getElementById("search-for-networks-link").style.display = "block"
                }
            })
            .catch(error => {
                console.log("Error connecting to network:", error);
                alert("An error occurred. Please try again.");
            });
        }
    </script>

     <style>
        * {
            font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;
            color: #fff;
        }

        body {
            height: 100vh;
            text-align: center;
            background: rgb(19, 7, 46);
            background: linear-gradient(180deg, rgba(19,7,46,1) 28%, rgba(47,23,96,1) 54%, rgba(140,82,255,1) 90%);
        }

        #logo { 
            width: 50px;
            position: absolute;
            top: 20px;
            left: 20px;
        }

        #title {
            width: 40px;
            position: absolute;
            top: 40px;
            top: 10px;
            left: 75px;
            font-weight: 800;
        }

        header {
            margin-top: 150px;
        }

        h1 {
            font-size: 3em;
            line-height: 0.01em;
        }

        #ed {
            color: #5cb85c;
            display: none;
        }

        p {
            font-size: 1.2em;
        }

        #instruction2, #instruction3 {
            display: none;
        }

        #search-for-networks-link {
            height: 10em;
            margin: 100px auto;
            font-size: 1.2em;
            font-weight: 800;
        }

        #networks-list-container {
            margin: 100px auto 0px auto;
            display: none;
        }

        .network {
            display: block;
            padding: 10px;
            margin: 10px auto;
            border: 2px solid white;
            border-radius: 10px;
            width: 80%;
            text-align: center;
            color: white;
            background-color: rgba(255, 255, 255, 0.1); /* Light background */
            cursor: pointer;
            transition: background-color 0.3s ease, color 0.3s ease;
        }

        .network:hover {
            background-color: rgba(255, 255, 255, 0.3);
            color: black;
        }



        #form-div {
            height: 10em;
            margin: 100px auto;
            width: 75%;
            box-sizing: border-box;
            display: none;
        }

        form {
            text-align: left;
        }

        label {
            font-size: 18px;
        }
        
        input {
            width: 100%;
            margin: 5px 0;
            padding: 5px 10px;
            border-radius: 2em;
            border: none;
            height: 2em;
            color: #000;
            font-size: 16px;
        }

        /* should add something for the focus */
        input:focus {
            outline: none;
        } 

        #connect-button-div, #done-button-div {
            display: flex;
            margin: 80px auto;
            justify-content: center;
        }

        #connect-button, #done-button {
            background-color: #5cb85c;
            border: none;
            border-radius: 2em;
            height: 2.5em;
            width: 8em;
            font-size: 1.2em;
            color: #000;
            font-weight: 900;
            text-align: center;
        }

        #establishing-network-div {
            margin: 80px auto 10px;
            width: 80%;
            box-sizing: border-box;
            display: none;
        }

        h3 {
            font-weight: 100;
        }

        h2 {
            font-size: 2em;
        }

        #connecting-to-text {
            display: none;
        }

        #connected-to-text {
            display: none;
        }

        #loading-bar-element {
            margin: 10px auto;
            display: none;
        }

        #loading-bar-container {
            width: 80%;
            height: 20px;
            background-color: transparent;
            border: 2px solid #5cb85c;
            border-radius: 10px;
            margin: 10px auto;
            overflow: hidden;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }

        #loading-bar-animation {
            width: 0;
            height: 100%;
            background-color: #5cb85c;
            animation: loading-animation 3s linear infinite;
            border-radius: 10px;
        }

        @keyframes loading-animation {
            0% { width: 0%; }
            50% { width: 50%;}
            100% { width: 100%; }
        }

        

     </style>    
</body>
</html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Generate network list in JSON format
String generateNetworkList() {
    String json = "[";
    int n = WiFi.scanNetworks(false, true);
    for (int i = 0; i < n; ++i) {
        if (i > 0) json += ",";
        json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
    }
    json += "]";
    return json;
}

// Handle LED control requests
void handleLEDControl() {
    if (server.hasArg("led") && server.hasArg("state")) {
        int led = server.arg("led").toInt();
        bool state = server.arg("state").toInt();
        int ledPin = getLedPin(led);
        if (ledPin != -1) {
            digitalWrite(ledPin, state ? HIGH : LOW);
            server.send(200, "text/plain", "LED state changed");
        } else {
            server.send(400, "text/plain", "Invalid LED number");
        }
    } else {
        server.send(400, "text/plain", "Missing parameters");
    }
}

// Helper function to map LED number to pin
int getLedPin(int led) {
    switch (led) {
        case 1: return ledPin1;
        case 2: return ledPin2;
        case 3: return ledPin3;
        default: return -1;
    }
}

// Handle serial commands for Unity handshake
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        message.trim();

        if (message == "dis u?") {
            Serial.println("ya"); // Respond to Unity's handshake
        } else {
            // Handle LED control commands in "led:state" format
            int led = message.charAt(0) - '0';  // Extract LED number
            int state = message.charAt(2) - '0'; // Extract state (1 or 0)
            int ledPin = getLedPin(led);
            if (ledPin != -1) {
                digitalWrite(ledPin, state ? HIGH : LOW);
                Serial.println("LED state changed via Serial");
            } else {
                Serial.println("Invalid LED number");
            }
        }
    }
}

void startCaptivePortal() {
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    IPAddress ip = WiFi.softAPIP();
    Serial.println("Access Point started with IP: " + ip.toString());

    dnsServer.start(DNS_PORT, "*", ip);

    server.on("/", handleRoot);
    server.on("/scan", []() {
        Serial.println("Client requested /scan");
        String networks = generateNetworkList();
        server.send(200, "application/json", networks);
    });

    server.on("/connect", []() {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");

        Serial.println("Received Wi-Fi credentials:");
        Serial.println("SSID: " + ssid);
        Serial.println("Password: " + pass);

        EEPROM.writeString(0, ssid);
        EEPROM.writeString(100, pass);
        EEPROM.commit();

        WiFi.begin(ssid.c_str(), pass.c_str());

        if (!MDNS.begin("esp32-led")) { // Set hostname to "esp32-led"
            Serial.println("Error starting mDNS");
           } else {
              Serial.println("mDNS responder started");
              Serial.println("Hostname: esp32-led.local");
           }

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 40) {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to Wi-Fi!");
            Serial.println("IP Address: " + WiFi.localIP().toString());
            server.send(200, "text/plain", "success");
        } else {
            Serial.println("\nFailed to connect to Wi-Fi.");
            server.send(200, "text/plain", "failed");
        }
    });

    server.on("/led", handleLEDControl);

    server.onNotFound([]() {
        String hostHeader = server.hostHeader();
        if (hostHeader != WiFi.softAPIP().toString()) {
            server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/", true);
            server.send(302, "text/plain", "Redirecting...");
        } else {
            server.send(404, "text/plain", "Not found");
        }
    });

    server.begin();
    Serial.println("Captive portal started.");
}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);

    startCaptivePortal();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    handleSerialCommands(); // Continuously listen for Unity handshake and commands
}