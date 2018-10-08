# 소개
* 이 Project는 Webcam으로부터 입력받은 영상에서 손가락 개수를 세는 Project이다. 
* Deep Learning의 기법 중 하나인 CNN(Convolution Neural Network)을 이용하였다. 
* 각 Step 마다 사용된 Application을 아래에 소개하겠다.


### 1. Capture_Finger_Shape
 - Visual Studio 2010 / 
 - Cam으로부터 640 x 480 영상을 찍는다.
 - 영상의 R , G , B 값을 보고 피부를 선택한다.
 - 피부색과 유사한 Object를 골라내기 위해서 가장 큰 Contour(손이라고 판단)만 남기고 삭제한다.
 - 원래 크기의 절반인 320 x 240의 JPG 파일로 저장해 준다.
 - Space Bar를 누르면, 아래와 같은 Image가 Capture되어 저장된다.(손가락 2개 Image를 Caputre한 예)
   ***
   ![손가락 2개](./Images/Capture_Finger_Shape_Test_0.jpg)   ![손가락 2개](./Images/Capture_Finger_Shape_Test_1.jpg)


### 2. Generate_Training_Images
 - Python 3.5.
 - Data Augmentation
 - JPG File을 읽어서 회전 / 크기변경 / 위치 이동 등을 하여 학습 Data의 양을 증가시킨다.
 - 1번 Step에서 Capture한 손가락 Image를 손가락 개수 별로 각 폴더에 넣어둔다.
 - 실행하여 원하는 손가락 개수 폴더를 선택하고, 원하는 동작(회전 / 크기변경 / 위치 이동)을 선택하고 각 동작별 Parameter를 입력하면 해당하는 Data Augmentation 동작을 수행한다.
 - Training Data양이 많지 않을 경우에 유용하다.
 - 저의 경우에는 1번 Step에서 각 손가락 개수별로 20에서 30장 정도 Capture하고, 2번 Step에서 각 손가락 개수별로 8-9000장 준비하였다.
 - 출력은 320 x 240 JPG 


### 3. TransformImgData
 - Visual Studio 2010. 
 - 2번 Step에서 작성된 320 x 240 JPG 파일을 읽어서 각 Pixel을 0x00 or 0xFF 으로 변환하여 Binary File을 만들어준다.
 - Image File을 Tensorflow가 학습할 수 있는 형태로 변환하는 과정임.
 - 2번 Step에서 작성된 Image File들의 Path와 출력할 Binary File들이 저장될 Path를 지정해 줘야 한다.
 <pre><code>
 /* 변환할 Image File의 위치 */
 sprintf(tmp,"C:\\Deep\\GetFingerCount\\Generate_Training_Images\\%d\\*.jpg" , i);
 /* 출력할 Image File의 위치 */
 sprintf(OpenFileName, "C:\\Deep\\GetFingerCount\\Generate_Training_Images\\%d\\%s", i , fd.name );
 </code></pre>
 - 변환된 Binary File은 0부터 증가하는 Index번호로 File Name이 정해진다.


### 4. ChooseTrainingTestDataSet
 - Visual Studio 2010. 
 - Step 3에서 변환된 Binary File을 읽어서 N개 단위(Batch Size)로 묶어서 하나의 Binary File로 만든다.
 - 묶음으로 만들때는 Random하게 선택해서 만들도록 한다.
 - Batch Size는 아래 상수
 <pre><code>
 const int	ONE_BATCH_SIZE = 100;
 </code></pre>
 - Step 3에서 만든 Binary File의 개수를 직접 아래 배열에 입력해 준다.(자동으로 하도록 하고 싶었지만, 귀차니즘....)
 <pre><code>
 int		NUM_OF_IMAGE[] = { 10900 , 9300 ,  8800, 9200 , 7800 , 7900 }; // 순서대로 0부터 5개 손가락 개수 Test Image 수
 </code></pre>
 - Step 3에서 작성한 Data File의 Path는 아래와 같이 지정한다. 각자 Path에 맞게 수정하자.
 <pre><code>
 _stprintf(SrcPath , _T("C:\\Deep\\GetFingerCount\\TransformImgData\\TransformImgData\\TransformImgData\\%d\\%05d.bin"), Folder , FileNumber );
 </code></pre> 
 - Training Data와 Answer도 함께 만든다.
 - Training Data File Naming : TrainingDataSet_xxxx.dat
 - Answer File Naming : CorrectAns_xxxx.dat
 



