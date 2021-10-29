from skimage import data_dir, io, transform, color
import numpy as np
import cv2
import os
import PIL.Image as img 
import glob
from PIL import Image
from tifffile.tifffile import main
# def convert_gray(f):
#     rgb = io.imread(f)  # 依次读取rgb图片
#     # gray = color.rgb2gray(rgb)  # 将rgb图片转换成灰度图 dst = transform.RandomResizedCrop((360, 360))  # 将灰度图片大小转换为256*256
      
#     dst = rgb[0:360,120:480]
#     dst = transform.resize(rgb, (224, 224))  # 将图片大小转换为224*224

#     return dst

# str = "./Blod_color/black/*.jpg"
# coll = io.ImageCollection(str, load_func=convert_gray)
# num = 0
# for i in range(len(coll)):
#     io.imsave('./Blod_color/huancun/huancun' + np.str(i) + '.jpg', coll[i])  # 循环保存图片
#     num = i
#↑*****图片剪裁*****↑#

#↓*****图片增量*****↓#
def zl():
    output_path = 'E:\\K210\\model\\Blod_color\\output'  # 输出文件夹名称

    img_list = []
    img_list.extend(glob.glob('Blod_color/blod/*.jpg'))  # 所有jpg图片的路径

    print(img_list)  # 打印查看是否遍历所有图片

    for img_path in img_list:
        img_name = os.path.splitext(img_path)[0]  # 获取不加后缀名的文件名
        print(img_name)  # 打印查看文件名
        im = Image.open(img_path)
        im = im.convert("RGB")  # 把PNG格式转换成的四通道转成RGB的三通道
        im_rotate = im.rotate(180, expand=1)  # 逆时针旋转90度,expand=1表示原图直接旋转
        # 判断输出文件夹是否已存在，不存在则创建。
        folder = os.path.exists(output_path)
        if not folder:
            os.makedirs(output_path)
        # 把旋转后的图片存入输出文件夹
        im_rotate.save('E:\\K210\\model\\Black\\'+img_path)
    # im_rotate.save( os.path.join(output_path ,img_path))

    print('OK')
zl()