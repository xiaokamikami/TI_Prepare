
import sensor, image, lcd, time
from Maix import GPIO
import KPU as kpu
from fpioa_manager import fm    # 导入库
color_R = (255, 0, 0)
color_G = (0, 255, 0)
color_B = (0, 0, 255)


class_IDs = ['RED', 'BLUE']
#class_IDs = ['mask', 'unmask']




def drawConfidenceText(image, rol, classid, value):
    text = ""
    _confidence = int(value * 100)

    if classid == 1:
        text = 'red: ' + str(_confidence) + '%'
    elif classid ==2:
        text = 'blue: ' + str(_confidence) + '%'

    image.draw_string(rol[0], rol[1], text, color=color_R, scale=2.5)




lcd.init()
sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
sensor.set_hmirror(1)    #设置摄像头镜像
sensor.set_vflip(1)      #设置摄像头翻转
sensor.run(1)


task = kpu.load(0x300000)

#anchor = (0.4929, 0.8741, 0.9821, 1.5277, 2.1476, 2.1129, 2.8415, 3.3253, 4.2735, 3.4473)
anchor = (2.1136, 2.0357, 2.2907, 2.4346, 2.4789, 2.2351, 2.771, 2.6935, 3.256, 3.2596)
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

            if classID == 4 and confidence > 0.5:           #图像符合
                _ = img.draw_rectangle(itemROL, color_R, tickness=5)
                if totalRes == 1:
                drawConfidenceText(img, (0, 0), 1, confidence)

            else:
                _ = img.draw_rectangle(itemROL, color=color_G, tickness=5)
                if totalRes == 1:

                    drawConfidenceText(img, (0, 0), 0, confidence)
            time.sleep_ms(100)

    else:

        _ = lcd.display(img)

_ = kpu.deinit(task)
