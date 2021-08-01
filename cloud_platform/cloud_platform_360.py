# Untitled - By: 13173 - 周五 7月 23 2021

import sensor, image, time, math
from pyb import Pin,UART,Timer
import gc
#系统初始化
gc.collect()

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(30)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
#用户函数

#定时回调
def tick(t):
    #print("x=",cx,",y=",cy)
   # uart1.write("cx="+str(cx)+",cy="+str(cy)+"\n")
    gc.collect()
    #uart1.write("cx")
def tof_dist():         #测距
    #uart3 = UART(3, 115200)
    #time.sleep(0.5)
    b=uart3.read()
    #uart3.deinit()
    if b!= None:
        #a =hex(b[3])+hex(b[4])
        #return print(a)
        #a = b[4]*100+b[5]
       # print("tof=",b[3]*255+b[4])
        c =b[3]*255+b[4]
        uart1.write(str(c))
        print("tof=",c)
        #a = a.replace('0x','')
        #a = '0x'+a
        #print("a:",int(a))
        #return print(hex(b[3])+hex(b[4]))
        #return print(b)

#初始化外设
uart1 = UART(1, 115200)
uart3 = UART(3, 115200)
tim2 = Timer(3, freq=1,callback=tick)     # 使用定时器创建一个定时器对象-在1Hz触发
# 打开补光灯 (低电)
pin6 = Pin('P6', Pin.IN, Pin.PULL_UP)
pin6 = Pin('P6', Pin.OUT_PP, Pin.PULL_NONE)
pin6.value(pin6.value(1))
time.sleep(2)
pin6.value(pin6.value(0))
img = sensor.snapshot().lens_corr(1.8)
tim1 = Timer(4,freq=50)
chx = tim1.channel(1,Timer.PWM,pin=Pin("P7"))
chy = tim1.channel(2,Timer.PWM,pin=Pin("P8"))
chx.pulse_width_percent(0) #配置通道占空比
chy.pulse_width_percent(0) #配置通道占空比

f_x = (2.8 / 3.984) * 160 # 默认值
f_y = (2.8 / 2.952) * 120 # 默认值
c_x = 160 * 0.5 # 默认值(image.w * 0.5)
c_y = 120 * 0.5 # 默认值(image.h * 0.5)
chx_zk = 0
chy_zk = 0
while(True):
    img = sensor.snapshot()
    for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # 默认为TAG36H11

       img.draw_rectangle(tag.rect(), color = (255, 0, 0))
       #img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
       print_args = (tag.x_translation(), tag.y_translation(), tag.z_translation(), tag.id())
       #print("Tx:%.2f,Ty:%.2f,Tz:%.2f,Id:%d,"% print_args)
       #uart1.write(str("%.2f,%.2f,%.2f,%d,"% print_args))
       #time.sleep_ms(1000)
       #x_zf=tag.x_translation()*100+500
       #if(x_zf>1000):
       #     x_zf=1000
       #elif(x_zf<100):
       #     x_zf =100
       #uart_printf = (x_zf,abs(tag.z_translation())*100,tag.id())
       #uart1.write("%d%d%d"% uart_printf)
       #print("Tx:%d,Tz:%d,Id:%d"% uart_printf)
       print("x:",tag.x_translation(),"y:",tag.y_translation())
       chx_zk = tag.x_translation()#-1 * 2
       chy_zk = tag.y_translation()#+13 * 0.5
       if(chx_zk<-0.5):
           chx_zk = 8
           chx.pulse_width_percent(int(chx_zk)) #配置通道占空比
           print("chx=",chx_zk)
       elif(chx_zk>0.5):
           chx_zk = 3
           chx.pulse_width_percent(int(chx_zk)) #配置通道占空比
           print("chx=",chx_zk)
       else:
           chx.pulse_width_percent(0) #配置通道占空比
           print("atag!")
       if(chy_zk<-1):
           chy_zk = 3
           chy.pulse_width_percent(int(chy_zk) ) #配置通道占空比
           print("chy=",chy_zk)
       elif(chy_zk>1):
           chy_zk =8
           chy.pulse_width_percent(int(chy_zk) ) #配置通道占空比
           print("chy=",chy_zk)
       else:
           chy.pulse_width_percent(0) #配置通道占空比
           print("atag!")
    if(chx_zk == 0):
        chx.pulse_width_percent(0) #配置通道占空比
        print("notag")
    if(chy_zk == 0):
       chy.pulse_width_percent(0) #配置通道占空比
       print("notag")
    #time.sleep_ms(50)
    chy_zk = 0
    chx_zk = 0

    #tof_dist()
