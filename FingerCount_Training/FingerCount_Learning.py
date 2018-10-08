"""
Finger Counter Learning
2018.08.17 Yi Chang Sic 
"""

import tensorflow as tf
import numpy as np


tf.set_random_seed(777)



# Hyper Parameter
LEARNING_RATE = 0.001
TOTAL_ITER_COUNT = 10
BATCH_SIZE = 100
TOTAL_TRAINING_DATA_NUM = 53800






"""
"""
def GetTrainData(Index , TrainOrTest , TestFileIndex = 0):  
    
    if TrainOrTest:
        fileName = '.\Learning_Data_BIN\Training\TrainingDataSet_%04d.dat' % Index
    else:        
        fileName = '.\Learning_Data_BIN\Test\TrainingDataSet_%04d.dat' % TestFileIndex
        
    TrainDataFile = open(fileName,'rb')
    Data = TrainDataFile.read()

    ret = np.frombuffer(Data , dtype = np.uint8);
    ret = ret.astype(np.float32 , copy=False)
    ret = np.reshape(ret, (-1,320,240,1))

    return ret





"""
"""
def GetCorrectAns(Index , TrainOrTest , TestFileIndex = 0):
    OneHot = [1., 0., 0., 0., 0., 0.,
              0., 1., 0., 0., 0., 0.,
              0., 0., 1., 0., 0., 0.,
              0., 0., 0., 1., 0., 0.,
              0., 0., 0., 0., 1., 0.,
              0., 0., 0., 0., 0., 1.]
    
    if TrainOrTest:
        fileName = '.\Learning_Data_BIN\Training\CorrectAns_%04d.dat' % Index
    else:
        fileName = '.\Learning_Data_BIN\Test\CorrectAns_%04d.dat' % TestFileIndex
        
    AnswerFile = open(fileName,'r')
    Data = AnswerFile.read()
    tmp = []
    
    for label in Data:
        if label != '\n':
            tmp = tmp + (OneHot[int(label)*6 : (int(label)+1)*6])
    
    ret = np.asarray(tmp , dtype = np.float32);
    ret = np.reshape(ret, (-1,6))
    
    return ret  #np.ndarray.tolist(ret)






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
config = tf.ConfigProto()
config.gpu_options.allow_growth = True
sess = tf.Session(config = config)

sess.run(tf.global_variables_initializer())


# Train
print("Learning Start !")
for iter in range(TOTAL_ITER_COUNT):
    avg_cost = 0
    total_batch = int(TOTAL_TRAINING_DATA_NUM / BATCH_SIZE)
     
    for i in range(total_batch):
        Train_Data = GetTrainData(i , True)        
        Label = GetCorrectAns(i , True)        
               
        for k in range( int(BATCH_SIZE / 10) ):
            a = np.split(Label , 10)
            b = np.asarray(a)
            
            d = np.split(Train_Data , 10)
            e = np.asarray(d)
            
            c,_ = sess.run([cost,optimizer] , feed_dict= {X:e[k] , Y:b[k] , Keep_Prob : 0.7} )
            
        avg_cost = c / total_batch
        
        print(avg_cost , i , iter)
    
    # Save Trained Model
    SAVER_DIR = './SaveModel/Iter_%03d/TrainedModel.ckpt' % iter
    saver = tf.train.Saver()
    ckpt = tf.train.get_checkpoint_state(SAVER_DIR)
    saver.save(sess, SAVER_DIR, write_meta_graph=False)
   

print("Learning Completed !")



sess.close()
print("End")
exit()