/*
接线说明:无

程序说明:通过esp32s3建立一个简单的网页服务器,在页面上显示引脚电平状态,输出GPIO0的电平(不按下的时候电平为1,按下电平为0),
当未按下BOOT时,两颗LED灯会亮,而按下BOOT键后两颗LED灯会熄灭



注意事项:无

函数示例:无

作者:灵首

时间:2023_3_10

*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>

//对开发板上的灯进行宏定义
#define LED_A 10
#define LED_B 11
#define pin_scanned 0    //GPIO0是接的BOOT按键


WebServer esp32s3_webServe(80);//实例化一个网页服务的对象
WiFiMulti wifi_multi;  //建立WiFiMulti 的对象,对象名称是 wifi_multi


/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }
}



/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("LINGSOU123","12345678");
  wifi_multi.addAP("LINGSOU12","12345678");
  wifi_multi.addAP("LINGSOU1","12345678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}



/*
# brief 处理网站根目录"/"的访问请求
# param 无
# retval  无
*/
void handleRoot(){
  int pinStateValue;    
  String pinState;

  pinStateValue = digitalRead(pin_scanned);   //获取GPIO0的电平状态
  
  //对GPIO0不同的电平状态进行不同的操作,当其为"1"时网页显示"GPIO0 state is HIGH",同时灯亮;
  //当其为"0"时网页显示"GPIO0 state is LOW",同时灯灭;
  if (pinStateValue == 1){
    pinState = "GPIO0 state is HIGH";
    digitalWrite(LED_A,1);
    digitalWrite(LED_B,1);
  }
  else{
    pinState = "GPIO0 state is LOW";
    digitalWrite(LED_A,0);
    digitalWrite(LED_B,0);
  }

  esp32s3_webServe.send(200,"text/plain",pinState);
  //200是状态码;"text/plain"是描述将以纯文本的形式显示一段字符串 pinState

}

/*
# brief 错误处理,当访问出错时会出现的页面情况
# param 无
# retval  无
*/
void handleNotFound(){
  esp32s3_webServe.send(404,"text/plain","404 NotFound");//404:状态码(404是不正常的),"text/plain":发送的内容格式,"404 NotFound":发送的内容
}


/*
# brief esp32s3建立网页服务初始化函数,提供一个网页根目录处理函数和一个错误处理函数
# param 无
# retval  无
*/
void esp32s3_webServe_init(void){
  esp32s3_webServe.begin();
  esp32s3_webServe.on("/",handleRoot);
  esp32s3_webServe.onNotFound(handleNotFound);
  Serial.print("\n HTTp esp32s3_webServe started");
}


void setup() {
  Serial.begin(9600);

  pinMode(LED_A,OUTPUT);
  pinMode(LED_B,OUTPUT);//设置灯的引脚的工作状态

  wifi_multi_init();//储存多个WiFi
  wifi_multi_con();//自动连接WiFi

  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());//输出连接信息(连接的WIFI名称及开发板的IP地址)

  esp32s3_webServe_init();//初始化网页服务,开启服务器工作

}


void loop() {
  esp32s3_webServe.handleClient();//循环运行,使能运行处理HTTP访问请求,尽量不要在其中加上延时函数,避免服务器不工作
}
