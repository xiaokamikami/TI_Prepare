
import cv2
import time
import os
 
cap = cv2.VideoCapture(1)
cap.set(3,320)
cap.set(4,240)
 
def snapShotCt():
    # camera_idx的作用是选择摄像头。如果为0则使用内置摄像头，比如笔记本的摄像头，用1或其他的就是切换摄像头。
    # ret, frame = cap.read()  # cao.read()返回两个值，第一个存储一个bool值，表示拍摄成功与否。第二个是当前截取的图片帧。
    count = 100
    while True:
 
        # 从摄像头读取图片
        success, img = cap.read()
        
        cv2.imwrite("save_pic/" + str(count) + '.jpg', img)
        cv2.imshow('image', img)
        time.sleep(0.5)  # 休眠一秒 可通过这个设置拍摄间隔，类似帧。
        count += 1
        # ret, frame = cap.read()  # 下一个帧图片
        # 保持画面的连续。waitkey方法可以绑定按键保证画面的收放，通过q键退出摄像
        #      k = cv2.waitKey(1)
        #      if k == '27':
        #         break
        # 或者得到800个样本后退出摄像，这里可以根据实际情况修改数据量，实际测试后800张的效果是比较理想的
        if count >= 100:
            break
 
 
# 关闭摄像头，释放资源
snapShotCt()
cap.realease()
cv2.destroyAllWindows()
# camera.release()
# cv2.destroyAllWindows()
