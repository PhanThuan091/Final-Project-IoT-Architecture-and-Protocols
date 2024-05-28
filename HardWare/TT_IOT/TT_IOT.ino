#include <LCD_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define API_KEY "AIzaSyBUq1d6LTjhXJkb9GPj5sqI2EVLwCeP2yc"
#define DATABASE_URL "nkn-ptt-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "ttktandiot@hcmute.com"
#define USER_PASSWORD "123ntt_pkn"
//khoi tao dht
#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);

//Coop1
FirebaseData fbdo;
FirebaseData fbdo1;
FirebaseData fbdo2;
FirebaseData fbdofanCoop1;
FirebaseData fbdoledCoop1;
FirebaseData fbdofeedCoop1;
//Coop2
FirebaseData fbdo3;
FirebaseData fbdo4;
FirebaseData fbdo5;
FirebaseData fbdofanCoop2;
FirebaseData fbdoledCoop2;
FirebaseData fbdofeedCoop2;
//xac thuc nguoi dung
FirebaseAuth auth;
//cau hinh ket noi
FirebaseConfig config;


const char *ssid = "IoT Lab";
const char *password = "IoT@123456";
String path = "/";

unsigned long t1 =0;

//khai bao chan su dung
//Coop1
const int coop1led1 = 26;
const int coop1led2 = 25;
const int coop1feed = 33;
const int coop1FanOnbutton = 39;
const int coop1FanOffbutton = 34;
const int coop1Feedbutton = 36;
const int pot_res1 = 27;
const int fanCoop1 = 12;
//Coop2
const int coop2led1 = 15;
const int coop2led2 = 2;
const int coop2feed = 23;
const int coop2FanOnbutton = 18;
const int coop2FanOffbutton = 5;
const int coop2Feedbutton = 19;
const int pot_res2 = 32;
const int fanCoop2 = 13;
//Get data from firebase
//coop1
String FanCoop1FromFirebase = "0";
String LedCoop1FromFirebase = "0";
String FeedCoop1FromFirebase = "0";
//coop2
String FanCoop2FromFirebase = "0";
String LedCoop2FromFirebase = "0";
String FeedCoop2FromFirebase = "0";
// Bien luu trang thai cac nut
int  coop1FanOnState = 0;
int  coop1FanOffState = 0;
int  coop1FeedState = 0;

int  coop2FanOnState = 0;
int  coop2FanOffState = 0;
int  coop2FeedState = 0;

String lastFanState = "-1"; // Lưu trạng thái cuối cùng của quạt

//lcd
LCD_I2C lcd(0x27, 16, 2);

void setup(){
  Serial.begin(115200);
  
  // initialize lcd
  lcd.begin();
  // turn on lcd backlight                      
  lcd.backlight();

  pinMode(coop1led1, OUTPUT);
  pinMode(coop1led2, OUTPUT);
  pinMode(coop1feed, OUTPUT);
  pinMode(fanCoop1, OUTPUT);

  pinMode(coop2led1, OUTPUT);
  pinMode(coop2led2, OUTPUT);
  pinMode(coop2feed, OUTPUT);
  pinMode(fanCoop2, OUTPUT);

  pinMode(coop1FanOnbutton, INPUT);
  pinMode(coop1FanOffbutton, INPUT);
  pinMode(coop1Feedbutton, INPUT);

  pinMode(coop2FanOnbutton, INPUT);
  pinMode(coop2FanOffbutton, INPUT);
  pinMode(coop2Feedbutton, INPUT);

  Serial.print(" ");
  Serial.print("Connecting to wifi ...");
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("");
  Serial.print("Wifi connected!!");

  dht.begin();

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  

  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);  

}

void readStateButton()
{
  coop1FanOnState = digitalRead(coop1FanOnbutton);
  coop1FanOffState = digitalRead(coop1FanOffbutton);
  coop1FeedState = digitalRead(coop1Feedbutton);

  coop2FanOnState = digitalRead(coop2FanOnbutton);
  coop2FanOffState = digitalRead(coop2FanOffbutton);
  coop2FeedState = digitalRead(coop2Feedbutton);

  //Coop1 Fan & Feed
  if(coop1FanOnState == HIGH)
  {
    digitalWrite(fanCoop1, HIGH);
    Firebase.RTDB.setFloat(&fbdofanCoop1,path + "chuong1/Quat", 1);
  }

  if(coop1FanOffState == HIGH)
  {
    digitalWrite(fanCoop1, LOW);
    Firebase.RTDB.setString(&fbdofanCoop1,path + "chuong1/Quat", 0);
  }

  if(coop1FeedState == HIGH)
  {
    digitalWrite(coop1feed, HIGH);
    Firebase.RTDB.setString(&fbdofeedCoop1,path + "chuong1/Cho_an", 1);
    delay(10000);
    digitalWrite(coop1feed, LOW);
    Firebase.RTDB.setString(&fbdofeedCoop1,path + "chuong1/Cho_an", 0);
  }

  //Coop2 Fan & lamp
  if(coop2FanOnState == HIGH)
  {
    digitalWrite(fanCoop2, HIGH);
    Firebase.RTDB.setString(&fbdofanCoop2,path + "chuong2/Quat", 1);
  }

  if(coop2FanOffState == HIGH)
  {
    digitalWrite(fanCoop2, LOW);
    Firebase.RTDB.setString(&fbdofanCoop2,path + "chuong2/Quat", 0);
  }

  if(coop2FeedState == HIGH)
  {
    digitalWrite(coop2feed, HIGH);
    Firebase.RTDB.setString(&fbdofeedCoop2,path + "chuong2/Cho_an", 1);
    delay(10000);
    digitalWrite(coop2feed, LOW);
    Firebase.RTDB.setString(&fbdofeedCoop2,path + "chuong2/Cho_an", 0);
  }
}


void loop()
{
  readStateButton();


  if (Firebase.ready() && (millis() - t1 > 2500 || t1 == 0))
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int val = analogRead(35);
    int percentage = map(val, 0, 4095, 0, 100);
    //Coop1
    Firebase.RTDB.setString(&fbdo, path + "chuong1/Doam",h);
    Firebase.RTDB.setString(&fbdo1, path + "chuong1/Nhietdo",t);
    Firebase.RTDB.setString(&fbdo2, path + "chuong1/Khoi",percentage);
  
    //Coop2
    Firebase.RTDB.setString(&fbdo3, path + "chuong2/Doam",h);
    Firebase.RTDB.setString(&fbdo4, path + "chuong2/Nhietdo",t);
    Firebase.RTDB.setString(&fbdo5, path + "chuong2/Khoi",percentage);

    //Get data from Firebase
    //Coop 1
    //Fan
    if (Firebase.RTDB.getString(&fbdofanCoop1, "chuong1/Quat")) {
    String FanCoop1FromFirebaseString = fbdofanCoop1.stringData();
    Serial.println("Quat1 từ Firebase String: " + String(FanCoop1FromFirebaseString));
      if(FanCoop1FromFirebaseString =="1")
      {
        digitalWrite(fanCoop1, HIGH);
      }else{digitalWrite(fanCoop1, LOW); }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }

    //Lamp
    if (Firebase.RTDB.getString(&fbdoledCoop1, "chuong1/Den")) {
    LedCoop1FromFirebase = fbdoledCoop1.stringData();
    Serial.println("Den1 từ Firebase: " + String(LedCoop1FromFirebase));
      if(LedCoop1FromFirebase == "0.5")
      {
        digitalWrite(coop1led1, HIGH);
        digitalWrite(coop1led2, LOW);
      }else if(LedCoop1FromFirebase == "1")
      {
        digitalWrite(coop1led1, LOW);
        digitalWrite(coop1led2, HIGH);
      }
      else
      {
        digitalWrite(coop1led1, LOW);
        digitalWrite(coop1led2, LOW);
      }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }

    //Feed
    if (Firebase.RTDB.getString(&fbdofeedCoop1, "chuong1/Cho_an")) {
    FeedCoop1FromFirebase = fbdofeedCoop1.stringData();
    Serial.println("Feed1 từ Firebase: " + String(FeedCoop1FromFirebase));
      if(FeedCoop1FromFirebase == "1")
      {
        digitalWrite(coop1feed, HIGH);
      }else{digitalWrite(coop1feed, LOW); }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }


    //Coop2
    // Fan
    if (Firebase.RTDB.getString(&fbdofanCoop2, "chuong2/Quat")) {
    FanCoop2FromFirebase = fbdofanCoop2.stringData();
    // String FanCoop2FromFirebaseString = fbdofanCoop2.stringData().c_str();
    Serial.println("Quat2 từ Firebase: " + String(FanCoop2FromFirebase));
      if(FanCoop2FromFirebase == "1" )
      {
        digitalWrite(fanCoop2, HIGH);
      }else{digitalWrite(fanCoop2, LOW); }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }
    //Lamp
    if (Firebase.RTDB.getString(&fbdoledCoop2, "chuong2/Den")) {
    LedCoop2FromFirebase = fbdoledCoop2.stringData();
    Serial.println("Den2 từ Firebase: " + String(LedCoop2FromFirebase));
      if(LedCoop2FromFirebase == "0.5" )
      {
        digitalWrite(coop2led1, HIGH);
        digitalWrite(coop2led2, LOW);
      }else if(LedCoop2FromFirebase == "1" )
      {
        digitalWrite(coop2led1, LOW);
        digitalWrite(coop2led2, HIGH);
      }
      else
      {
        digitalWrite(coop2led1, LOW);
        digitalWrite(coop2led2, LOW);
      }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }

    //Feed
    if (Firebase.RTDB.getString(&fbdofeedCoop2, "chuong2/Cho_an")) {
    FeedCoop2FromFirebase = fbdofeedCoop2.stringData();
    Serial.println("Feed2 từ Firebase: " + String(FeedCoop2FromFirebase));
      if(FeedCoop2FromFirebase == "1" )
      {
        digitalWrite(coop2feed, HIGH);
      }else{digitalWrite(coop2feed, LOW); }

    } else {
      Serial.println("Lỗi khi đọc giá trị từ Firebase.");
    }
    
    
    //LCD
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Gas:");
    lcd.print(percentage);
    lcd.print("%");

    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(t);
    lcd.setCursor(8, 1);
    lcd.print("Hum:");
    lcd.print(h);

    //Kiem tra
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" %");
    Serial.print("Gas: ");
    Serial.print(percentage);
    Serial.println(" %");
    
  }
    
}



  // int valLedCoop1 = analogRead(pot_res1);
  // int perLedCoop1 = map(valLedCoop1, 0, 4095, 0, 100);

  // int valLedCoop2 = analogRead(pot_res2);
  // int perLedCoop2 = map(valLedCoop2, 0, 4095, 0, 100);
  // Serial.print("bien tro: ");
  // Serial.println(percentage);

  //Bien tro Coop1
  // if(perLedCoop1 >70)
  // {
  //   digitalWrite(coop1led1, LOW);
  //   digitalWrite(coop1led2, HIGH);
  //   Firebase.RTDB.setFloat(&fbdoledCoop1,path + "chuong1/Den", 1);
  // }
  // else if (perLedCoop1<20)
  // {
  //   digitalWrite(coop1led1, LOW);
  //   digitalWrite(coop1led2, LOW);
  //   Firebase.RTDB.setFloat(&fbdoledCoop1,path + "chuong1/Den", 0);
  // }
  // else
  // {
  //   digitalWrite(coop1led1, HIGH);
  //   digitalWrite(coop1led2, LOW);    
  //   Firebase.RTDB.setFloat(&fbdoledCoop1,path + "chuong1/Den", 0.5);
  // }
  // //Bien tro Coop2
  // if(perLedCoop2 >70)
  // {
  //   digitalWrite(coop2led1, LOW);
  //   digitalWrite(coop2led2, HIGH);
  //   Firebase.RTDB.setFloat(&fbdoledCoop2,path + "chuong2/Den", 1);
  // }
  // else if (perLedCoop2<20)
  // {
  //   digitalWrite(coop2led1, LOW);
  //   digitalWrite(coop2led2, LOW);
  //   Firebase.RTDB.setFloat(&fbdoledCoop2,path + "chuong2/Den", 0);
  // }
  // else
  // {
  //   digitalWrite(coop2led1, HIGH);
  //   digitalWrite(coop2led2, LOW);    
  //   Firebase.RTDB.setFloat(&fbdoledCoop2,path + "chuong2/Den", 0.5);
  // }

// const char *ssid = "Ktx p514";
// const char *password = "homektx514";

