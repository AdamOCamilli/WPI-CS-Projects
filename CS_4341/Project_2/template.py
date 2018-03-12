from keras.models import Sequential
from keras.layers import Dense, Activation
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

# Model Template
model = Sequential() # declare model
model.add(Dense(10, input_shape=(28*28, ), kernel_initializer='he_normal')) # first layer
model.add(Activation('relu'))

# first hidden layer
hiddenLayer1 = Dense(10, kernel_initializer='random_uniform')
model.add(hiddenLayer1) # hidden layer
model.add(Activation('relu'))

# second hidden layer
hiddenLayer2 = Dense(10, kernel_initializer='random_uniform')
model.add(hiddenLayer2) # hidden layer
model.add(Activation('relu'))

# third hidden layer
hiddenLayer3 = Dense(10, kernel_initializer='random_uniform')
model.add(hiddenLayer3) # hidden layer
model.add(Activation('relu'))

# final output layer
model.add(Dense(10, kernel_initializer='he_normal')) # last layer
model.add(Activation('softmax'))


# Compile Model
model.compile(optimizer='sgd',
              loss='categorical_crossentropy', 
              metrics=['accuracy'])

# Train Model
history = model.fit(trainingData, trainingLabel,
                    validation_data = (validData, validLabel),
                    epochs=25,
                    batch_size=512)


# Report Results

print(history.history)
testResults = model.predict(testData)


results = []
#TODO: Make this it's own layer
for result in list(testResults): #loop through list of arrays to get max value of array
    #Use np.where to get tuple of location of max value in result tuple, then grab first number.
    results.append(np.where(result==result.max())[0][0])

#convert labels back to real numbers
resultLabels = []
for oldLabel in listTestLabel:
    resultLabels.append(np.where(oldLabel == oldLabel.max())[0][0])

#write to csv
resultArray = np.array([resultLabels,results]).transpose()
np.savetxt('results.csv',resultArray,fmt='%d',delimiter=',',header='actual,predicted',comments='')

count = 0
for data,resultLabel,result in zip(testData,resultLabels,results):
    if not resultLabel == result:
        newData = data / 255
        newData = newData.reshape(28,28)
        imgplot = plt.imshow(newData)
        plt.savefig('one/' + str(count) + '-' + str(result) + '.png')
        count += 1