# Untitled - By: 13173 - 周三 9月 8 2021

import sensor,image,lcd,time

#------常用初始化   ↓-------
lcd.init()
sensor.reset()                      #复位摄像头
sensor.set_pixformat(sensor.RGB565) # 设置像素格式 RGB565
sensor.set_framesize(sensor.QVGA)   # 设置帧尺寸 QVGA (320x240)
sensor.skip_frames()                #等待曝光稳定
sensor.set_auto_exposure(False, 1)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(True)     #自动白平衡开启(True)或者关闭(False)
sensor.set_auto_exposure(False,1)
#------常用初始化   ↑-------
'''对于 RGB565 图像，每个元组需要有六个值(l_lo，l_hi，a_lo，a_hi，b_lo，b_hi)
分别是 LAB中 L，A 和 B 通道的最小值和最大值。
L的取值范围为0-100，a/b 的取值范围为-128到127。'''

#红色阈值[0],绿色阈值[1],蓝色阈值[2]
red = (100, 29, -22, 6, 127, -128)
green = (0, 80, -70, -10, -0, 30)
blue = (0, 30, 0, 64, -128, -20)

while True:
    img=sensor.snapshot()
    img.lens_corr(1.8)
    #blobs = img.find_blobs([rgb_thresholds[0]])
    #if blobs:
        #for b in blobs:
            #tmp=img.draw_rectangle(b[0:4])  #在图像上绘制一个矩形。
            #tmp=img.draw_cross(b[5], b[6])  #画十字交叉
            #c=img.get_pixel(b[5], b[6])#    返回(x, y)位置的RGB888像素元组
    #blobs = img.find_blobs([red,green,blue])
    #if blobs:
        #for b in blobs:
            #tmp = img.draw_rectangle(b[0:4])
            #c = img.get_pixel(b[5],b[6])
            #print(b[8])
    for c in img.find_circles(threshold = 3500, x_margin = 10, y_margin = 10, r_margin = 10,
             r_min = 2, r_max = 100, r_step = 2):
         area = (c.x()-c.r(), c.y()-c.r(), 2*c.r(), 2*c.r())
         #area为识别到的圆的区域，即圆的外接矩形框
         statistics = img.get_statistics(roi=area)#像素颜色统计
         print(statistics)
         #(0,100,0,120,0,120)是红色的阈值，所以当区域内的众数（也就是最多的颜色），范围在这个阈值内，就说明是红色的圆。
         #l_mode()，a_mode()，b_mode()是L通道，A通道，B通道的众数。
         if 29<statistics.l_mode()<100 and -22<statistics.a_mode()<6 and -128<statistics.b_mode()<127:#if the circle is red
             img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))#识别到的红色圆形用红色的圆框出来
         else:
             img.draw_rectangle(area, color = (255, 255, 255))
             #将非红色的圆用白色的矩形框出来
    lcd.display(img)


