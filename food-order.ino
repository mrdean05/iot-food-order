
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0
#include <ArduinoJson.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

int number, pizzaCost, totalPizza;
int number1, drinkCost, totalDrink;
String pizzaType, pizzaSize;
String drinkType, drinkSize;


LiquidCrystal_I2C lcd( 0x27, 20, 4);
const byte NumRows = 3;
const byte NumCols = 3;

char keymap[NumRows][NumCols] = {{'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
};

byte rowPins [NumRows] = {23, 19, 18};
byte colPins [NumCols] = {5, 17, 16};
Keypad myKd = Keypad (makeKeymap (keymap), rowPins, colPins, NumRows, NumCols);
char keypressed;

const char* ssid = "mr_dean";
const char* password = "m.anifest";

const char* awsEndpoint = "a2y7dpna4hsvgm-ats.iot.us-east-1.amazonaws.com";

// Update the two certificate strings below. Paste in the text of your AWS
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end
// of each line:

// xxxxxxxxxx-certificate.pem.crt
const char* certificate_pem_crt = \

                                  "-----BEGIN CERTIFICATE-----\n" \
                                  "MIIDWTCCAkGgAwIBAgIUfYvmvNJ9B7+Y40nSMAl6x0mx5OowDQYJKoZIhvcNAQEL\n" \
                                  "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
                                  "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMDIxNTE0NDAw\n" \
                                  "OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
                                  "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN4kOlE9hX5O3ScQzTmb\n" \
                                  "7giNO8rk4fTXyBVqCDRN0YPXU9j8uSixxm5vVBHdH77t4ZJTEqWh5hpMtiG+0hx8\n" \
                                  "k+n9YqLtBrtvX0UNWrBgySn57e+V1j4fcR1qkjqiHipnpNtACi9/M+Jk1ZMcSWgP\n" \
                                  "8zM6z/R+bjWBXgFL+WctS76/uJ9x/1PQH0hIGkzV/Qet9Ka8NE1yb+GuRpvLTG0d\n" \
                                  "U8W50e3EhtITl6akqOrLQOSuQiGofAa+Lu72x3aiRPMX4VvHhL1KMup28qBwah1w\n" \
                                  "pC03Hg5hrgdY9Q77WA/cqqxZDZF8HAXfomgOvpyd1y/+Mlqmz1jAnnAQXJxBK20n\n" \
                                  "MT8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUtbPRPmrXSwGmC8BjqPxiTSqN0dowHQYD\n" \
                                  "VR0OBBYEFBsfV/vEDq9jxYPMv1VGW/GqRBKFMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
                                  "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBHLSC3Wsd/SWuYwMUL3qt7hlbL\n" \
                                  "nLlbEdvF7HY/2KzM9bAndzUs9Z4Fcg2ISlJcK7ccSm3UdTBLJldogQXvimaI/4OI\n" \
                                  "3YKveTTUpXE1+9yCzqIefcKACeR9CR/SLCy6VI1Vyplqijxdr8KjxMqIwC3LOhLr\n" \
                                  "TZ9gvtRey4VGEPWfYxnK3CDYh/ISPhOXwoW96hXmQBQEV9bZhi7CMOuK5GwuAj+c\n" \
                                  "SKm0/YOgUVp7NUO5q5NfpVJAautmdhcr/GR2U3SO/kR+lTX8u7Uh9Ll3uvd5S+V/\n" \
                                  "Ueac3LM0e7pyvpYmtugOpMMQjNUj/McdGtYJGo3QarsjW0Oe8Yqoj+yD+ELp\n" \
                                  "-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

                              "-----BEGIN RSA PRIVATE KEY-----\n" \
                              "MIIEpAIBAAKCAQEA3iQ6UT2Ffk7dJxDNOZvuCI07yuTh9NfIFWoINE3Rg9dT2Py5\n" \
                              "KLHGbm9UEd0fvu3hklMSpaHmGky2Ib7SHHyT6f1iou0Gu29fRQ1asGDJKfnt75XW\n" \
                              "Ph9xHWqSOqIeKmek20AKL38z4mTVkxxJaA/zMzrP9H5uNYFeAUv5Zy1Lvr+4n3H/\n" \
                              "U9AfSEgaTNX9B630prw0TXJv4a5Gm8tMbR1TxbnR7cSG0hOXpqSo6stA5K5CIah8\n" \
                              "Br4u7vbHdqJE8xfhW8eEvUoy6nbyoHBqHXCkLTceDmGuB1j1DvtYD9yqrFkNkXwc\n" \
                              "Bd+iaA6+nJ3XL/4yWqbPWMCecBBcnEErbScxPwIDAQABAoIBAETHDLNIjANeYYSN\n" \
                              "5CyZBarqJ4AbO4Z2JMHUkXO4QE1EpvZA0Qcze4+HsozRXNndhwCp7dUQelDcPWvX\n" \
                              "S/B79cP1qsts/a+cvO+BQfnoDYCMnEmYp7uWOox0CZBTz2krq9QiUMUB6kuHEsU4\n" \
                              "ddXdUlwjMm2tk2vHgRZ/sllqodANsmF8wBo9Cp2KJL2uM0a0ES4EeKHsV9xTkGat\n" \
                              "Kpxz5302AqLwRo5UUJUK0YeoAaA8d/viE0W47QtGxmGiXGKqQnwpclMm6Qtn96OR\n" \
                              "h7+hcxsivKy0ZMiKxuQeJ30f5c5tLRnCSJv6ULDvfc7ZcQQKesJ46XSqAsNjEque\n" \
                              "kR/E6nkCgYEA9JiibYJ7qy45vqrFUfm7F6SNRPPGI0sBwaM646a18G3NHtngpcuv\n" \
                              "TXKOlYAU9Ioq2h0Z2pKxk+974jXXz6xp24vhwMjmdkSxTY1NmYRxR7Tbt80eQHmR\n" \
                              "yH4+bS9CnO/5Sd2jpCq8MEZcubhHexQYPvSKrCqJZsNc+bOLFCnvuwMCgYEA6H+W\n" \
                              "FOpIi6L/dFNdU39nZxtewXJdG8rZoGidt0q37p1iZm77XVO4MLN+mIOiqd0N6B7f\n" \
                              "0ME/JltEUjdOeefvUs4LNHKjpjYngols7q+Q2DejAAS3XeXLuSBZqAgv8A7Smu1S\n" \
                              "OYcNB9XEdHekxcptfdoo9OVs9UTx/QXKBy9bnhUCgYEAsL6+2H0qWhcMGRATh9n6\n" \
                              "bOo2PvYcXhJsUX1fJ3OnEvZ0+28bWxLlOQSnrbYg7lgNARF4Ri4sbabnER4xNVr0\n" \
                              "H3ZkCkfh6OJYcJ7DzUQcC2HQR2/MBXETfBplh8ZXz0nFDLBA+Hb9B7FAfd+An5p0\n" \
                              "rsFMl34hTYWsYK9xt39aQPsCgYEA0CpcAAH8Tjn0qvxxFO9/b4vL/TweLWIkFEFW\n" \
                              "5OObZ6O2lKxNlzP8OG6drIQ/wgLQDsKF9mzv07HnrA0k0HVfjC47TgxOheT63x+Y\n" \
                              "TuUWtOCrqrqOSx4Sl4BlhdM+yJsc6eYOtr86ZYd6TCVLfGRQQFxyS0KXmcrPN5JV\n" \
                              "XrWCPDkCgYAJ5dUf5UULjNsW9yfUfNVCMNufukJZNk/PP8xKgPErrbL9Sn9e6vSU\n" \
                              "gbSpB/LF7Fn/1UAv5rVgF0k+5KpQd/kEhqQLbFtyqIxvx8+VuZE4mraM97NX+/if\n" \
                              "HVyuYNMsHkRsPAbJxu+u2MIXBuxZAXzy9pEATRR93gZwNWX48zYqfg==\n" \
                              "-----END RSA PRIVATE KEY-----\n";

/* root CA can be downloaded in:
  https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
*/
const char* rootCA = \
                     "-----BEGIN CERTIFICATE-----\n" \
                     "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
                     "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
                     "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
                     "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
                     "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
                     "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
                     "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
                     "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
                     "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
                     "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
                     "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
                     "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
                     "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
                     "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
                     "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
                     "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
                     "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
                     "rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
                     "-----END CERTIFICATE-----\n";

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);



void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("ORDER YOUR FOOD");

  delay(50); Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());


  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  lcd.setCursor(2, 0);
  lcd.print("ORDER YOUR FOOD");
  lcd.setCursor(0, 1);
  lcd.print("1. PIZZA");
  lcd.setCursor(0, 2);
  lcd.print("2. DRINKS");

  keypressed = myKd.getKey ();
  if (keypressed == '1') {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("PIZZA MENU LIST");
    lcd.setCursor(0, 1);
    lcd.print("1. ALL SEASON PIZZA");
    lcd.setCursor(0, 2);
    lcd.print("2. CHICKEN PIZZA");
    lcd.setCursor(0, 3);
    lcd.print("3. BEEF PIZZA");

    int flag = 0;

    while (flag == 0) {

      keypressed = myKd.getKey ();

      if (keypressed == '1') {
        pizzaType = "ALL SEASON PIZZA";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("ALL SEASON PIZZA");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 45GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 55GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 65GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            pizzaCost = 45;
            pizzaSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ALL SEASON PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("ALL SEASON PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            pizzaCost = 55;
            pizzaSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ALL SEASON PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("ALL SEASON PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            pizzaCost = 65;
            pizzaSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ALL SEASON PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("ALL SEASON PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;

              }
            }

          }

        }

      }



      if (keypressed == '2') {
        pizzaType = "CHICKEN PIZZA";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("CHICKEN PIZZA");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 45GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 55GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 65GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            pizzaCost = 45;
            pizzaSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("ALL SEASON PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            pizzaCost = 55;
            pizzaSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("CHICKEN PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            pizzaCost = 65;
            pizzaSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("CHICKEN PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }

        }

      }



      if (keypressed == '3') {
        pizzaType = "BEEF PIZZA";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("BEEF PIZZA");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 45GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 55GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 65GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            pizzaCost = 45;
            pizzaSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("ALL SEASON PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            pizzaCost = 55;
            pizzaSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("CHICKEN PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            pizzaCost = 65;
            pizzaSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CHICKEN PIZZA");
            lcd.setCursor(0, 1);
            lcd.print("NUMBER OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number = pressedkey.toInt();
                totalPizza = pizzaCost * number;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("CHICKEN PIZZA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalPizza);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;

              }
            }

          }

        }

      }




    }

    
     pubSubCheckConnect();
    String msg;
    StaticJsonDocument<500> doc;
    doc["Table Number"] = "1";
    doc["Order Type"] = "Pizza";
    doc["Pizza Type"] = pizzaType;
    doc["Pizza Size"] =  pizzaSize;
    doc["Pizza Cost"] =  pizzaCost;
    doc["Num of Order"] = number;
    doc["Total Cost"] = totalPizza;
    serializeJsonPretty(doc, msg);
    boolean rc = pubSubClient.publish("pizza/order", msg.c_str());
    Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(msg);
    lcd.clear();

  }



  if (keypressed == '2') {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("DRINK LIST");
    lcd.setCursor(0, 1);
    lcd.print("1. COKE");
    lcd.setCursor(0, 2);
    lcd.print("2. FANTA");
    lcd.setCursor(0, 3);
    lcd.print("3. SPRITE");

    int flag = 0;

    while (flag == 0) {

      keypressed = myKd.getKey ();

      if (keypressed == '1') {
        drinkType = "COKE";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("COKE");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 10GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 15GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 20GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            drinkCost = 10;
            drinkSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COKE");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("COKE");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            drinkCost = 15;
            drinkSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COKE");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("COKE");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            drinkCost = 20;
            drinkSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COKE");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("COKE");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;

              }
            }

          }

        }

      }



      if (keypressed == '2') {
        drinkType = "FANTA";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("FANTA");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 10GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 15GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 20GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            drinkCost = 10;
            drinkSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("COKE");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            drinkCost = 15;
            drinkSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("FANTA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            drinkCost = 20;
            drinkSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("FANTA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }

        }

      }



      if (keypressed == '3') {
        drinkType = "SPRITE";
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("SPRITE");
        lcd.setCursor(0, 1);
        lcd.print("1. SMALL - 10GH ");
        lcd.setCursor(0, 2);
        lcd.print("2. MEDIUM - 15GH ");
        lcd.setCursor(0, 3);
        lcd.print("3. BIG - 20GH");

        int flag1 = 0;

        while (flag1 == 0) {

          keypressed = myKd.getKey ();
          if (keypressed == '1') {
            drinkCost = 10;
            drinkSize = "SMALL";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("COKE");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }

          }


          if (keypressed == '2') {
            drinkCost = 15;
            drinkSize = "MEDIUM";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("FANTA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;
              }
            }
          }


          if (keypressed == '3') {
            drinkCost = 20;
            drinkSize = "BIG";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FANTA");
            lcd.setCursor(0, 1);
            lcd.print("number OF ORDER: ");

            int flag2 = 0;
            while (flag2 == 0) {

              keypressed = myKd.getKey ();

              if (keypressed) {

                String pressedkey = String(keypressed);
                number1 = pressedkey.toInt();
                totalDrink = drinkCost * number1;
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("FANTA");
                lcd.setCursor(0, 1);
                lcd.print("TOTAL COST: ");
                lcd.print(totalDrink);
                delay (3000);
                flag2 = 1;
                flag1 = 1;
                flag = 1;

              }
            }

          }

        }

      }




    }

    
    pubSubCheckConnect();
    String msg;
    StaticJsonDocument<500> doc;
    doc["Table Number"] = "1";
    doc["Order Type"] = "Drinks";
    doc["Drink Type"] = drinkType;
    doc["Drink Size"] =  drinkSize;
    doc["Drink Cost"] =  drinkCost;
    doc["Num of Order"] = number1;
    doc["Total Cost"] = totalDrink;
    serializeJsonPretty(doc, msg);
    boolean rc = pubSubClient.publish("pizza/order", msg.c_str());
    Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(msg);
    lcd.clear();

  }


}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
