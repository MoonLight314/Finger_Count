# 소개
* 이 Project는 Webcam으로부터 입력받은 영상에서 손가락 개수를 세는 Project이다. 
* Deep Learning의 기법 중 하나인 CNN(Convolution Neural Network)을 이용하였다. 
* 각 Step 마다 사용된 Application을 아래에 소개하겠다.


### Capture_Finger_Shape
 - Visual Studio 2010. 
 - Cam으로부터 640 x 480 영상을 찍는다.
 - 영상의 R , G , B 값을 보고 피부를 선택한다.
 - 피부색과 유사한 Object를 골라내기 위해서 가장 큰 Contour(손이라고 판단)만 남기고 삭제한다.
 - 원래 크기의 절반인 320 x 240의 JPG 파일로 저장해 준다.


### Generate_Training_Images
 - Python 3.5.
 - Data Augmentation
 - JPG File을 읽어서 회전 / 크기변경 / 위치 이동 등을 하여 학습 Data의 양을 증가시킨다.
 - 출력은 640 x 480 JPG 


### TransformImgData
 - Visual Studio 2010. 
 - 640 x 480 JPG 파일을 읽어서 각 Pixel을 0x00 or 0xFF 으로 변환하여 Binary File을 만들어준다.
 - 이때 Size를 축소할 수 있다.
 - 변환된 Binary File의 크기는 축소된 크기에 따라 가변


### ChooseTrainingTestDataSet
 - Visual Studio 2010. 
 - 변환된 Binary File을 읽어서 N개 단위로 묶어서 하나의 Binary File로 만든다.
 - 이때 정답도 함께 만든다.
