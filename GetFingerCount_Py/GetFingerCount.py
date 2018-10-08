"""
Get Finger Count
2018.08.24 Yi Chang Sic 
"""

import cv2
import tensorflow as tf
import numpy as np
import time
import os.path


# Hyper Parameter
LEARNING_RATE = 0.001




def GetTrainData(fileName):  
    #fileName = './Test/2_00000.bin'
        
    TrainDataFile = open(fileName,'rb')
    Data = TrainDataFile.read()

    ret = np.frombuffer(Data , dtype = np.uint8);
    ret = ret.astype(np.float32 , copy=False)
    ret = np.reshape(ret, (1,320,240,1))

    return ret




def main():
    tf.set_random_seed(777)
    
        # Training Data Placeholder
    X = tf.placeholder(tf.float32, [None,320,240,1], 'X_Training_Image')    # Training_Image
    Y = tf.placeholder(tf.float32, [None,6] , 'Label')   # Correct Answer    
    Keep_Prob = tf.placeholder(tf.float32)
    
    # L1 Image In Shape = (None,320,240,1)
    W1 = tf.Variable(tf.random_normal([3,3,1,16], stddev = 0.01))
    # Conv => (None,320,240,16)
    # Pool => (None,160,120,16)
    L1 = tf.nn.conv2d(X , W1, strides=[1,1,1,1], padding = 'SAME')
    L1 = tf.nn.relu(L1)
    L1 = tf.nn.max_pool(L1 , ksize=[1,2,2,1] , strides=[1,2,2,1] , padding='SAME')
    L1 = tf.nn.dropout(L1,Keep_Prob)
    
    # L2 Image In Shape = (None, 160, 120, 16)
    W2 = tf.Variable(tf.random_normal([3,3,16,32] , stddev = 0.01))
    # Conv => (None,160,120,16)
    # Pool => (None,80,60,16)
    L2 = tf.nn.conv2d(L1 , W2 , strides=[1,1,1,1] , padding='SAME')
    L2 = tf.nn.relu(L2)
    L2 = tf.nn.max_pool(L2 , ksize = [1,2,2,1] , strides=[1,2,2,1] , padding='SAME')
    L2 = tf.nn.dropout(L2,Keep_Prob)
    
    # L3 Image In Shape = (None, 80, 60, 32)
    W3 = tf.Variable(tf.random_normal([3,3,32,64] , stddev = 0.01))
    # Conv => (None,80,60,32)
    # Pool => (None,40,30,32)
    L3 = tf.nn.conv2d(L2 , W3 , strides=[1,1,1,1] , padding='SAME')
    L3 = tf.nn.relu(L3)
    L3 = tf.nn.max_pool(L3 , ksize = [1,2,2,1] , strides=[1,2,2,1] , padding='SAME')
    L3 = tf.nn.dropout(L3,Keep_Prob)   
        
    # L4 Image In Shape = (None, 40, 30, 64)
    W4 = tf.Variable(tf.random_normal([3,3,64,128] , stddev = 0.01))
    # Conv => (None,40,30,32)
    # Pool => (None,20,15,32)
    L4 = tf.nn.conv2d(L3 , W4 , strides=[1,1,1,1] , padding='SAME')
    L4 = tf.nn.relu(L4)
    L4 = tf.nn.max_pool(L4 , ksize = [1,2,2,1] , strides=[1,2,2,1] , padding='SAME')
    L4 = tf.nn.dropout(L4,Keep_Prob)
    
    
    
    
    
    L4_Flat = tf.reshape( L4 , [-1 , 20*15*128] )
    
    
    
    # Fully Connected 80*60*64 Input   =>   6 Outputs
    W5 = tf.get_variable("W5" , shape=[20*15*128 , 6] , initializer=tf.contrib.layers.xavier_initializer())
    Bias = tf.Variable(tf.random_normal([6]))
    Logits = tf.matmul(L4_Flat , W5) + Bias    
    
    # Cost / Optimizer
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(logits = Logits , labels = Y))
    optimizer = tf.train.AdamOptimizer(learning_rate=LEARNING_RATE).minimize(cost)
    
    # Initialize
    sess = tf.Session()
    
    #sess = tf.Session()
    sess.run(tf.global_variables_initializer())

    # Restore trained data if exist
    SAVER_DIR = './SaveModel'
    saver = tf.train.Saver()
    ckpt = tf.train.get_checkpoint_state(SAVER_DIR)
    
    if ckpt and ckpt.model_checkpoint_path:
        saver.restore(sess, ckpt.model_checkpoint_path)        

        while True:
            if os.path.exists("C:/Deep/GetFingerCount/data.bin"):
                # Figure Out How Many Finger In Image
                Ans = sess.run( tf.argmax(Logits,1) , feed_dict={X:GetTrainData("C:/Deep/GetFingerCount/data.bin") , Keep_Prob : 1.0} )
                print("Prediction : ", Ans)
                os.remove("C:/Deep/GetFingerCount/data.bin", dir_fd=None)
                
                if not os.path.exists("C:/Deep/GetFingerCount/result.dat"):
                    Answer = open("C:/Deep/GetFingerCount/result.dat",'w')
                    Answer.write(str(Ans[0]))
                    Answer.close()
            
            time.sleep(0.1)                                
            
            
    else:
        print("No Train Model File")
        
    sess.close()
    exit() 




"""
"""
if __name__ == "__main__":
    # execute only if run as a script
    main()
