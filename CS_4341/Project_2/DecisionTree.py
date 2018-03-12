from sklearn import tree
from keras.utils import to_categorical
import random
import numpy as np
import matplotlib.pyplot as plt


#Fixes input for model
images = np.load('images.npy')
imageList = []
for image in images:
    imageList.append(np.reshape(np.array(image),(1,784)).flatten())
fixedImages = np.array(imageList)

#load labels for model
labels = np.load('labels.npy')

#stratify data
data = [[],[],[],[],[],[],[],[],[],[]]
listTrain = []
listValid = []
listTest = []
listTrainLabel = []
listValidLabel = []
listTestLabel = []
for i in range(0,len(fixedImages)): #sort images by their label
    data[labels[i]].append([fixedImages[i],labels[i]])
for labelSet in data:
    #Shuffle data, then get certain percentage of dataset
    randomStratified = labelSet
    random.shuffle(randomStratified)
    #Separate pairs of image and label
    listTrain += [pair[0] for pair in randomStratified[0:int(len(randomStratified) * 0.6)]]
    listValid += [pair[0] for pair in randomStratified[int(len(randomStratified)*0.6):int(len(randomStratified)*0.75)]]
    listTest += [pair[0] for pair in randomStratified[int(len(randomStratified)*0.75):int(len(randomStratified))]]
    listTrainLabel += [pair[1] for pair in randomStratified[0:int(len(randomStratified) * 0.6)]]
    listValidLabel += [pair[1] for pair in randomStratified[int(len(randomStratified) * 0.6):int(len(randomStratified) * 0.75)]]
    listTestLabel += [pair[1] for pair in randomStratified[int(len(randomStratified) * 0.75):int(len(randomStratified))]]
#convert data to arrays:
trainingData = np.array(listTrain)
validData = np.array(listValid)
testData = np.array(listTest)
trainingLabel = np.array(listTrainLabel)
validLabel = np.array(listValidLabel)
testLabel = np.array(listTestLabel)


#create classifier and fit it to training set and training labels
classifier = tree.DecisionTreeClassifier()
classifier = classifier.fit(trainingData,trainingLabel)

#test model on test data
testResults = []
for test in testData:
    prediction = classifier.predict(test.reshape(1,-1)) #reshape here to 'convert' to a 2D array with one row
    testResults.append(prediction)

#write to csv
resultArray = np.array([testLabel,testResults]).transpose()
np.savetxt('results.csv',resultArray,fmt='%d',delimiter=',',header='actual,predicted',comments='')

count = 0
for data,resultLabel,result in zip(testData,testLabel,testResults):
    if not resultLabel == result:
        newData = data / 255
        newData = newData.reshape(28,28)
        imgplot = plt.imshow(newData)
        plt.savefig('tree/' + str(count) + '-' + str(result) + '.png')
        count += 1