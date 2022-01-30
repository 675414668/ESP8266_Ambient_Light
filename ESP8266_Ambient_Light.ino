#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET

#include <Blinker.h>
#include <Adafruit_NeoPixel.h>

char auth[] = "************";//设备号
char ssid[] = "************";//无线账号
char pswd[] = "************";//无线密码

int LED_OPEN=0;//氛围灯标志位
int Control_OPEN=0;//氛围灯颜色控制标志位

int count=0;//计数器
int send_data=0;//发送命令标志位
uint8_t colorR1 ;//用于颜色存储
uint8_t colorG1 ;
uint8_t colorB1 ;
uint8_t colorR2 ;//用于颜色存储2
uint8_t colorG2 ;
uint8_t colorB2 ;
uint8_t colorR_Set= map(  0, 0, 255, 0, 255);//颜色默认值
uint8_t colorG_Set= map(255, 0, 255, 0, 255);//颜色默认值
uint8_t colorB_Set= map(242, 0, 255, 0, 255);;//颜色默认值

#define LED1 16
#define LED2 14
#define PIN  10
#define NUMPIXELS      57
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define RGB_1 "RGBKey"

// 新建组件对象
BlinkerButton Button1("LED_Button");
BlinkerButton Button2("Control_Button");
BlinkerRGB RGB1(RGB_1);

//APP端颜色设置回调函数
void rgb1_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("R value: ", r_value);
    BLINKER_LOG("G value: ", g_value);
    BLINKER_LOG("B value: ", b_value);
    BLINKER_LOG("Rrightness value: ", bright_value);

    uint8_t colorR = map(r_value, 0, 255, 0, bright_value);
    uint8_t colorG = map(g_value, 0, 255, 0, bright_value);
    uint8_t colorB = map(b_value, 0, 255, 0, bright_value);
    
    colorR2=colorR1 ;//存储
    colorG2=colorG1 ;
    colorB2=colorB1 ;
    
    colorR1=colorR ;//存储
    colorG1=colorG ;
    colorB1=colorB ;
    Button2.print("on");
    Control_OPEN=1;//氛围灯颜色调节
    send_data=1;//运行标志位为1
}

// 按下按键1即会执行该函数
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED1, !digitalRead(LED1));

    if(digitalRead(LED1)==0)
    {
      Button1.print("off");
      LED_OPEN=0;//关闭
     }
     else
     {
       Button1.print("on");
       LED_OPEN=1;//开启
      }
      send_data=1;//运行标志位为1
}
// 按下按键2即会执行该函数
void button2_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED2, !digitalRead(LED2));

    if(digitalRead(LED2)==0)
    {
      Button2.print("off");
      Control_OPEN=0;//关闭
     }
     else
     {
       Button2.print("on");
       Control_OPEN=1;//开启
      }
      send_data=1;//运行标志位为1
}
// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

}

void setup() {
    // 初始化串口
    Serial.begin(115200);

    #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif

    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED2, LOW);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    pixels.begin();
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);//按键1
    Button2.attach(button2_callback);//按键2
    RGB1.attach(rgb1_callback);
}

void loop() {
    Blinker.run();
    count++;//led计数器
    if(send_data==1)//运行标志位为1
    {
        if(LED_OPEN==1 )//开启
        {
          if(Control_OPEN==1)//控制开启
          {
            int r1=colorR2,g1=colorG2,b1=colorB2;
            
             for(int n = 0; n < NUMPIXELS; n++){
               pixels.setPixelColor(n, pixels.Color(colorR1,colorG1,colorB1));
               pixels.show();
              } 
          }
          else
          {
            int r=0,g=0,b=0;
            
              for(int i = 0; i < NUMPIXELS; i++){
                 pixels.setPixelColor(i, pixels.Color(colorR_Set,colorG_Set,colorB_Set));
                 pixels.show();
                }
          }   
        }
        else if (LED_OPEN==0)//关闭
        {
             uint8_t colorR = map(0, 0, 255, 0, 0);
             uint8_t colorG = map(0, 0, 255, 0, 0);
             uint8_t colorB = map(0, 0, 255, 0, 0);

             for(int i = 0; i < NUMPIXELS; i++){
                pixels.setPixelColor(i, pixels.Color(colorR,colorG,colorB));
                pixels.show();
                }
        }
         send_data=0;//运行标志位清0     
    }
    if(count==100)
    {
       digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
       count=0;
    }
}
