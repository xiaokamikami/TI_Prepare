#M1W
import sensor, image, lcd, time
from Maix import GPIO
import KPU as kpu
from fpioa_manager import fm    # 导入库
import gc
import ujson
from machine import UART
import machine
from board import board_info

#系统初始化
gc.collect()


#引脚定义
LED_G = 12
LED_R = 13
LED_B = 14
LED_Compensation = 16



fm.register(LED_G, fm.fpioa.GPIO0)
fm.register(LED_R, fm.fpioa.GPIO1)
fm.register(LED_B, fm.fpioa.GPIO2)

fm.register(LED_Compensation, fm.fpioa.GPIO3)


led_b=GPIO(GPIO.GPIO0, GPIO.OUT)
led_g=GPIO(GPIO.GPIO1, GPIO.OUT)
led_r=GPIO(GPIO.GPIO2, GPIO.OUT)

led_c=GPIO(GPIO.GPIO3, GPIO.OUT)

led_g.value(1)
led_b.value(1)
led_r.value(1)
led_c.value(1)

fm.register(15,fm.fpioa.UART2_TX)
fm.register(17,fm.fpioa.UART2_RX)
uart_stm32 = machine.UART(UART.UART2, 115200, 8, 1, 0, timeout=100, read_buf_len=128)

color_G = (255, 0, 0)
color_B = (0, 255, 0)
color_R = (0, 0, 255)
color_Blk = (255,255,255)
color_Wit = (10,10,10)

#class_IDs = ['Black', 'Blue', 'Red', 'Green', 'White']
class_IDs = ['Black', 'Blue', 'Red', 'White','Green']
#class_IDs = ['mask', 'unmask']




def drawConfidenceText(image, rol, classid, value):
    text = ""
    _confidence = int(value * 100)

    if classid == 0:
        text = 'Black: ' + str(_confidence) + '%'
        led_g.value(1)
        led_r.value(1)
        led_b.value(1)
    elif classid ==1:
        text = 'Blue: ' + str(_confidence) + '%'
        led_b.value(0)
        led_r.value(1)
        led_g.value(1)
    elif classid ==2:
        text = 'Red: ' + str(_confidence) + '%'
        led_r.value(0)
        led_g.value(1)
        led_b.value(1)
    elif classid ==4:
        text = 'Green: ' + str(_confidence) + '%'
        led_g.value(0)
        led_r.value(1)
        led_b.value(1)
    elif classid ==3:
        text = 'White: ' + str(_confidence) + '%'
        led_g.value(0)
        led_r.value(0)
        led_b.value(0)


    image.draw_string(rol[0], rol[1], text, color=color_G, scale=2.5)




lcd.init()
sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_hmirror(0)    #设置摄像头镜像
sensor.set_vflip(0)      #设置摄像头翻转
#sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
#sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
sensor.run(1)


task = kpu.load(0x300000)

anchor = (0.8863, 1.5684, 1.5746, 2.7734, 1.5938, 0.8922, 3.0614, 1.7475, 3.0793, 3.5948)
#anchor = (1.3316, 2.3774, 1.4857, 2.6176, 1.6022, 2.8221, 1.8719, 3.375, 2.6094, 1.5146)
_ = kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)
img_lcd = image.Image()

ball_dict = {}

while (True):

    img = sensor.snapshot()
    code = kpu.run_yolo2(task, img)
    if code:
        totalRes = len(code)

        for item in code:
            confidence = float(item.value())
            itemROL = item.rect()
            classID = int(item.classid())
            print_args = (item.x(), item.y(), classID)
            #print("x:",print_args[0],"y:",print_args[1],"id:",print_args[2])
            if classID == 0 and confidence > 0.5:           #图像符合
                _ = img.draw_rectangle(itemROL, color_Blk, tickness=5)        #打框
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), classID, confidence)          #显示名称
            elif classID == 1 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_B, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), classID, confidence)

            elif classID == 2 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_G, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), classID, confidence)

            elif classID == 3 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_R, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), classID, confidence)

            elif classID == 4 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_Wit, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), classID, confidence)
            _ = lcd.display(img)

            ball_dict["x"]   = print_args[0]
            ball_dict["y"]   = print_args[1]
            ball_dict["co"]  = print_args[2]
            if(confidence > 0.5):

            #ball_dict["va"] = int(confidence*100)
                encoded = ujson.dumps(ball_dict)
                uart_stm32.write(encoded+"\n")
                time.sleep_ms(50)
                print(encoded)
            # else:
            #     _ = img.draw_rectangle(itemROL, color=color_G, tickness=5)
            #     if totalRes == 1:

            #         drawConfidenceText(img, (0, 0), 0, confidence)


    else:
        led_g.value(1)
        led_r.value(1)
        led_b.value(1)
        _ = lcd.display(img)
    gc.collect()


uart_A.deinit()
del uart_A
_ = kpu.deinit(task)
