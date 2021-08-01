# Untitled - By: 13173 - 周五 7月 30 2021
import sensor, image, time, math
from pyb import Pin,UART,Timer
import gc
from pyb import Servo
#系统初始化
gc.collect()

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(30)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
#用户函数
pan_servo=Servo(1)
tilt_servo=Servo(2)
pan_servo.calibration(500,2500,1500)
tilt_servo.calibration(500,2500,1500)


#定时回调垃圾清理
def tick(t):
    gc.collect()
def tof_dist():         #测距
    b=uart3.read()
    if b!= None:
        c =b[3]*255+b[4]
        uart1.write(str(c))
        print("tof=",c)


#初始化外设
uart1 = UART(1, 115200)
uart3 = UART(3, 115200)
tim2 = Timer(3, freq=1,callback=tick)     # 使用定时器创建一个定时器对象-在1Hz触发
# 打开补光灯 (低电)
pin6 = Pin('P6', Pin.IN, Pin.PULL_UP)
pin6 = Pin('P6', Pin.OUT_PP, Pin.PULL_NONE)
pin6.value(pin6.value(1))
#time.sleep(2)
pin6.value(pin6.value(0))
img = sensor.snapshot().lens_corr(1.8)


f_x = (2.8 / 3.984) * 160 # 默认值
f_y = (2.8 / 2.952) * 120 # 默认值
c_x = 160 * 0.5 # 默认值(image.w * 0.5)
c_y = 120 * 0.5 # 默认值(image.h * 0.5)
pan_ch_live = 50    #跟随值记录
tilt_ch_live = 50

tilt_servo.angle(50, 100)
pan_servo.angle(50, 100)
while(True):
    img = sensor.snapshot()
    for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # 默认为TAG36H11
       if tag:
           img.draw_rectangle(tag.rect(), color = (255, 0, 0))
           #img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
           pan_error = (tag.cx()-img.width()/2)/1.5
           tilt_error = (tag.cy()-img.height()/2)/1.5
           pan_ch=  (-pan_error-40)/2+80
           tilt_ch= (tilt_error+40)/2+40
           print("pan_ch",pan_ch)
           print("tilt_ch",tilt_ch)
           if(52<pan_ch<58):
                pass
           else:
               if(58 <pan_ch):
                   pan_ch_live+=1
                   if(pan_ch_live>90):
                      pan_ch_live = 90
                   pan_servo.angle(pan_ch_live, 100)
               elif(52 >pan_ch):
                   pan_ch_live-=1
                   pan_servo.angle(pan_ch_live, 100)
           if(52<tilt_ch<58):
                pass
           else:
               if(58 <tilt_ch):
                   tilt_ch_live+=1
                   tilt_servo.angle(tilt_ch_live, 100)
               elif(52 >tilt_ch):
                   tilt_ch_live-=1
                   tilt_servo.angle(tilt_ch_live, 100)
           print("pan",pan_ch_live)
           print("tilt",tilt_ch_live)
           #tilt_servo.angle(tilt_ch, 100)

    ##time.sleep_ms(50)

    #tof_dist()
