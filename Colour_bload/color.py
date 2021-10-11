
import sensor, image, lcd, time
from Maix import GPIO
import KPU as kpu
from fpioa_manager import fm    # 导入库
import gc
#系统初始化
gc.collect()

color_R = (255, 0, 0)
color_G = (0, 255, 0)
color_B = (0, 0, 255)
color_Blk = (255,255,255)
color_Wit = (10,10,10)

class_IDs = ['Black', 'Blue', 'Green', 'Red', 'White']
#class_IDs = ['mask', 'unmask']




def drawConfidenceText(image, rol, classid, value):
    text = ""
    _confidence = int(value * 100)

    if classid == 1:
        text = 'Black: ' + str(_confidence) + '%'
    elif classid ==2:
        text = 'Blue: ' + str(_confidence) + '%'
    elif classid ==3:
        text = 'Green: ' + str(_confidence) + '%'
    elif classid ==4:
        text = 'Red: ' + str(_confidence) + '%'
    elif classid ==5:
        text = 'White: ' + str(_confidence) + '%'
    image.draw_string(rol[0], rol[1], text, color=color_G, scale=2.5)




lcd.init()
sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_hmirror(1)    #设置摄像头镜像
sensor.set_vflip(1)      #设置摄像头翻转
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
sensor.run(1)


task = kpu.load(0x300000)

#anchor = (0.4929, 0.8741, 0.9821, 1.5277, 2.1476, 2.1129, 2.8415, 3.3253, 4.2735, 3.4473)
anchor = (1.4191, 2.4646, 1.6486, 2.8296, 1.9241, 3.2515, 2.5446, 1.4539, 2.9777, 1.683)
_ = kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)
img_lcd = image.Image()


while (True):

    img = sensor.snapshot()
    code = kpu.run_yolo2(task, img)
    if code:
        totalRes = len(code)

        for item in code:
            confidence = float(item.value())
            itemROL = item.rect()
            classID = int(item.classid())
            if classID == 0 and confidence > 0.5:           #图像符合
                _ = img.draw_rectangle(itemROL, color_Blk, tickness=5)        #打框
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), 1, confidence)          #显示名称

            elif classID == 1 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_B, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), 2, confidence)

            elif classID == 2 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_G, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), 3, confidence)

            elif classID == 3 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_R, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), 4, confidence)

            elif classID == 4 and confidence > 0.5:
                _ = img.draw_rectangle(itemROL, color_Wit, tickness=5)
                if totalRes == 1:
                    drawConfidenceText(img, (0, 0), 5, confidence)

            # else:
            #     _ = img.draw_rectangle(itemROL, color=color_G, tickness=5)
            #     if totalRes == 1:

            #         drawConfidenceText(img, (0, 0), 0, confidence)
            time.sleep_ms(100)

    else:

        _ = lcd.display(img)

_ = kpu.deinit(task)
