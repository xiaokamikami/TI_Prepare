
import cv2
import time
import os
 
cap = cv2.VideoCapture(1)
cap.set(3,320)
cap.set(4,240)
 
def snapShotCt():
    # camera_idx��������ѡ������ͷ�����Ϊ0��ʹ����������ͷ������ʼǱ�������ͷ����1�������ľ����л�����ͷ��
    # ret, frame = cap.read()  # cao.read()��������ֵ����һ���洢һ��boolֵ����ʾ����ɹ���񡣵ڶ����ǵ�ǰ��ȡ��ͼƬ֡��
    count = 100
    while True:
 
        # ������ͷ��ȡͼƬ
        success, img = cap.read()
        
        cv2.imwrite("save_pic/" + str(count) + '.jpg', img)
        cv2.imshow('image', img)
        time.sleep(0.5)  # ����һ�� ��ͨ���������������������֡��
        count += 1
        # ret, frame = cap.read()  # ��һ��֡ͼƬ
        # ���ֻ����������waitkey�������԰󶨰�����֤������շţ�ͨ��q���˳�����
        #      k = cv2.waitKey(1)
        #      if k == '27':
        #         break
        # ���ߵõ�800���������˳�����������Ը���ʵ������޸���������ʵ�ʲ��Ժ�800�ŵ�Ч���ǱȽ������
        if count >= 100:
            break
 
 
# �ر�����ͷ���ͷ���Դ
snapShotCt()
cap.realease()
cv2.destroyAllWindows()
# camera.release()
# cv2.destroyAllWindows()
