# -*- coding: utf-8 -*- 

import os
from PIL import Image
from collections import namedtuple

#
Parameter = namedtuple("Parameter", "FolderName , TransformType , Prm1 , Prm2")


# 방향 
UP      = 0
DOWN    = 1
LEFT    = 2
RIGHT   = 3




"""
사용자의 입력을 받아서 넘긴다.
"""
def GetTransfromParam():
    FolderName = input("Please, input Folder Name : ")
    TransformType = input("1. Rotate\n2. Scale\n3. Move\n")
    Prm1 = input("Parameter #1 : ")
    Prm2 = input("Parameter #2 : ")
    
    TransformParameter = Parameter(str(FolderName) , int(TransformType) , int(Prm1) , int(Prm2))
    
    return TransformParameter






"""
Parameter로 넘어온 그림파일을 start_angle에서 end_angle까지 1도씩
회전시키며 Image를 생성한다.
"""
def Rotate(filename,start_angle , end_angle ):
    img = Image.open(filename)

    for angle in range(start_angle , end_angle+1 , 5) :
        rot_img = img.rotate(angle)
        
        # 원래 File 이름에 회전한 Image File Name을 구분하기 위해서
        # 회전한 각도를 추가하여 그림 파일 이름을 만든다.   
        s = os.path.splitext(filename)
        s = os.path.split(s[0])
        rot_img.save( s[1] + str(angle) + "_Rot.jpg" )
    




"""

"""
def Scale(filename,start_scale,end_scale):
    img = Image.open(filename)

    for ratio in range(start_scale , end_scale , 5) :       
        scale_img = img.resize( (int(img.width + img.width * ratio / 100) , int(img.height + img.height * ratio / 100)) )
        
        #축소
        if ratio < 0:            
            new_img = Image.new("RGB",(320,240),(0,0,0))
            new_img.paste(  scale_img , 
                            (int((new_img.width - scale_img.width)/2),
                             int((new_img.height - scale_img.height)/2))
                            )

            s = os.path.splitext(filename)
            s = os.path.split(s[0])
            new_img.save( s[1] + str(ratio) + "_Resize.jpg" )
        
        #확대
        else:
            area = ( int((scale_img.width - 320)/2), 
                     int((scale_img.height - 240)/2), 
                     320 + int((scale_img.width - 320)/2), 
                     240 + int((scale_img.height - 240)/2))
            
            crop_img = scale_img.crop( area )
            
            s = os.path.splitext(filename)
            s = os.path.split(s[0])
            crop_img.save( s[1] + str(ratio) + "_Resize.jpg" )
    





"""
"""
def Move(filename):# , offset , dir):
    img = Image.open(filename)
    print(img.getbbox())
    
    """
    Direction = {UP:"_Up_Move.jpg" , DOWN:"_Down_Move.jpg" , LEFT:"_Left_Move.jpg" , RIGHT:"_Right_Move.jpg"}

    for move_offset in range( 0 , offset):
        new_img = Image.new("RGB",(320,240),(0,0,0))
        
        if dir == DOWN:
            new_img.paste(  img , (0,move_offset))
        elif dir == UP:
            new_img.paste(  img , (0,-move_offset))
        elif dir == LEFT:
            new_img.paste(  img , (-move_offset , 0))
        elif dir == RIGHT:
            new_img.paste(  img , (move_offset , 0))

        s = os.path.splitext(filename)
        s = os.path.split(s[0])
        new_img.save( s[1] + str(move_offset) + Direction[dir] )
    """


def backspace(n):
    print('\r', end='')                     # use '\r' to go back





#현재 디렉토리
current_dir = os.getcwd()

# Parameter를 받는다.
TransformParameter = GetTransfromParam()

# 작업할 폴더의 위치를 찾는다.
current_dir += "\\" + TransformParameter.FolderName

# 작업할 폴더의 전체 File Name List를 얻어온다.
filenames = os.listdir(current_dir)

#for file in filenames:
for idx, file in enumerate(filenames):
    ext = os.path.splitext(file)[-1]
    
    backspace(len(str(int(100 * (idx+1) / len(filenames)) ) ) )
    print( int(100 * (idx+1) / len(filenames)) , end='')
    
    if ext == '.jpg' :
        if TransformParameter.TransformType == 1:
            Rotate( current_dir + "\\" + file , TransformParameter.Prm1 , TransformParameter.Prm2 )
        elif TransformParameter.TransformType == 2:
            Scale( current_dir + "\\" + file , TransformParameter.Prm1 , TransformParameter.Prm2 )
        elif TransformParameter.TransformType == 3:
            Move(current_dir + "\\" + file )#, TransformParameter.Prm1 , TransformParameter.Prm2 )
            
            
            
            
            