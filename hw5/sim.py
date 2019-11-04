#!/usr/bin/env python
import matplotlib #make sure matplotlib is installed
import matplotlib.pyplot as plt
import numpy as np

myfile = open("out.dat", 'r')#output of the terminal in dat

line = myfile.readline()
nTime = 5000
cnt = 1
data = np.zeros((8, nTime, 4))
while line:
    print("Line {}: {}".format(cnt, line.strip()))
    line = myfile.readline()
    # conver to the list
    list = line.split(",")
    li = []
    for i in list:
        try:
            li.append(float(i))
        except:
            break
    try:
        data[int(li[0]),cnt//8,0] = li[1]
        data[int(li[0]),cnt//8,1] = li[2]
        data[int(li[0]),cnt//8,2] = li[3]
        data[int(li[0]),cnt//8,3] = li[4]
    except:
        break

    cnt += 1


plt.ion()#interactive session on
fig = plt.figure()
ax = fig.add_subplot(111)
x=data[:,0,1]#2D diagram with x
y=data[:,0,3]#and z
ax.set_autoscale_on(True) # enable autoscale
ax.autoscale_view(True,True,True)
# ax.set_xlim([np.min(data[:,:,1]),np.max(data[:,:,1])])#scale to the limits of x
# ax.set_ylim([np.min(data[:,:,3]),np.max(data[:,:,3])])#scale to the limits of y
line1, = ax.plot(x, y, 'x')
text = []

for i in range (2000):
    print(i)
    for txt in fig.texts:
        txt.remove()
    x = data[:, i, 1]
    y = data[:, i, 3]
    line1.set_ydata(y)
    line1.set_xdata(x)
        #Delete previous annotations
    for ann in text:
        ann.remove()
    text = []
    text.append(plt.text(x[0], y[0], "buzzy"))
    for j in range(1,8,1):
        if int(data[j, i, 0])==0:
            text.append(plt.text(x[j], y[j], str(j)+"crashed"))
        elif (int(data[j, i, 0])==2):
            text.append(plt.text(x[j], y[j], str(j)+"landed"))
        else:
            text.append(plt.text(x[j], y[j], str(j)))

    ax.relim()        # Recalculate limits
    ax.autoscale_view(True,True,True) #Autoscale
    fig.canvas.draw()
    fig.canvas.flush_events()
 
plt.show()