from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.utils import to_categorical
from sklearn.neighbors import KNeighborsClassifier
import random
import numpy as np
import matplotlib.pyplot as plt


#Fixes input for model
images = np.load('images.npy')
imageList = []
for image in images:
    imageList.append(np.reshape(np.array(image),(1,784)).flatten())
fixedImages = np.array(imageList)

#fixes labels for model
labels = np.load('labels.npy')
fixedLabels = to_categorical(labels)

#stratify data
data = [[],[],[],[],[],[],[],[],[],[]]
listTrain = []
listValid = []
listTest = []
listTrainLabel = []
listValidLabel = []
listTestLabel = []
for i in range(0,len(fixedImages)): #sort images by their label
    data[labels[i]].append([fixedImages[i],fixedLabels[i]])
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

''' KNN '''

# Vary for each experiment
k = 2

model = KNeighborsClassifier(k)
model.fit(trainingData,trainingLabel)

testResults = model.predict(testData)


knn_results = []
#TODO: Make this it's own layer
for result in list(testResults): #loop through list of arrays to get max value of array
    #Use np.where to get tuple of location of max value in result tuple, then grab first number.
    knn_results.append(np.where(result==result.max())[0][0])

#convert labels back to real numbers
resultLabels = []
for oldLabel in listTestLabel:
    resultLabels.append(np.where(oldLabel == oldLabel.max())[0][0])

#write to csv
resultArray = np.array([resultLabels,knn_results]).transpose()
result_csv = 'knn_results' + '(' + str(k) + ').csv'
np.savetxt(result_csv,resultArray,fmt='%d',delimiter=',',header='actual,predicted',comments='')

count = 0
for data,resultLabel,result in zip(testData,resultLabels,knn_results):
    if not resultLabel == result:
        newData = data / 255
        newData = newData.reshape(28,28)
        imgplot = plt.imshow(newData)
        plt.savefig('knn/' + 'k(' + str(k) + ')_' + str(count) + '-' + str(result) + '.png')
        count += 1