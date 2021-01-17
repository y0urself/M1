#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Polygon


# List: [3929 µs, 3929 µs, 3930 µs, 3933 µs, 3934 µs, 3936 µs, 3936 µs, 3936 µs, 3937 µs, 3938 µs, 3939 µs, 3939 µs, 3940 µs, 3940 µs, 3940 µs, 3941 µs, 3941 µs, 3941 µs, 3942 µs, 3942 µs, 3942 µs, 3942 µs, 3942 µs, 3942 µs, 3943 µs, 3943 µs, 3944 µs, 3944 µs, 3944 µs, 3944 µs, 3945 µs, 3945 µs, 3945 µs, 3945 µs, 3946 µs, 3946 µs, 3946 µs, 3946 µs, 3947 µs, 3947 µs, 3947 µs, 3948 µs, 3948 µs, 3949 µs, 3949 µs, 3949 µs, 3949 µs, 3949 µs, 3949 µs, 3950 µs, 3950 µs, 3950 µs, 3950 µs, 3950 µs, 3951 µs, 3952 µs, 3952 µs, 3953 µs, 3953 µs, 3954 µs, 3954 µs, 3954 µs, 3955 µs, 3955 µs, 3955 µs, 3955 µs, 3956 µs, 3956 µs, 3957 µs, 3957 µs, 3958 µs, 3958 µs, 3958 µs, 3958 µs, 3958 µs, 3958 µs, 3958 µs, 3959 µs, 3960 µs, 3963 µs, 3963 µs, 3963 µs, 3964 µs, 3966 µs, 3968 µs, 3986 µs, 3987 µs, 4038 µs, 4057 µs, 4060 µs, 4065 µs, 4066 µs, 4071 µs, 4079 µs, 4109 µs, 4133 µs, 4558 µs, 4563 µs, 4817 µs, 4850 µs, 4890 µs, 4923 µs, 4930 µs, 5477 µs, 5497 µs, 5561 µs, 5589 µs, 5779 µs, 5825 µs, 5843 µs, 5953 µs, 6061 µs, 6149 µs, 6170 µs, 6216 µs, 6355 µs, 6356 µs, 6422 µs, 6463 µs, 7035 µs, 7165 µs, 7250 µs, 7256 µs, 7328 µs, 7374 µs, 7528 µs, 7643 µs, 7662 µs, 7677 µs, 7713 µs, 7736 µs, 7806 µs, 7917 µs, 8112 µs, 8166 µs, 8193 µs, 8210 µs, 8317 µs, 8472 µs, 8527 µs, 8659 µs, 8771 µs, 9048 µs, 10710 µs, 10785 µs, 11862 µs, 12337 µs, 12407 µs, 12523 µs, 14062 µs, 14516 µs, 14970 µs, 15058 µs, 15149 µs, 16008 µs, 17225 µs, 18018 µs, 19631 µs, 24321 µs, 31000 µs, 33982 µs, 39465 µs, 42564 µs, 42721 µs, 43218 µs, 43608 µs, 45232 µs, 45743 µs, 47754 µs, 49550 µs, 50568 µs, 51181 µs, 51222 µs, 54239 µs, 54806 µs, 54978 µs, 55687 µs, 60693 µs, 61611 µs, 61649 µs, 63503 µs, 64327 µs, 67212 µs, 71672 µs, 72340 µs, 73803 µs, 74335 µs, 80250 µs, 80681 µs, 81399 µs, 82196 µs, 84769 µs, 86556 µs, 87248 µs, 89791 µs, 90238 µs, 91101 µs, 95271 µs, 97946 µs, 243452 µs]

def isint(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False

# parsing the input
def parse(file):
  sum = 0
  sums = []
  times = []
  names = []
  num = 0;
  with open(file, 'r') as f:
    for line in f:
        l = line.split()
        if len(l) > 0:
          time = []
          print(str(len(l))) 
          name = l[0] + " " + l[1]
          for n in l:
            if isint(n):
              time.append(int(n))
              sum = sum + int(n)
          sums.append(sum)
          times.append(time)
          names.append(name)
          num = num + 1
  return sums, times, names, num

sum1, times1, names1, num1 = parse(sys.argv[2])

if sys.argv[1] == "bars1":

    plt.figure(figsize=(24,18))


    mar = 0
    for bla in range(len(own)):
        if bla is not 0 and bla % 10 == 0:
            mar = mar + 1
        i.append(bla + mar)
        i2.append(bla + mar + 0.5)
        i3.append(bla + mar + 0.25)
        if bla is not 0 and bla % 5 == 0 and bla % 10 is not 0:
            i4.append(bla - 0.25 + mar)
    width = 1
    
    p1 = plt.bar(i, own, .5, label="Zufall")
    p2 = plt.bar(i2, hrs, .5, label="Heuristische Werte")

    u = 0
    for rect in p1:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2.0, height + 1, var2[u], ha='center', va='bottom', rotation=90)
        u = u + 1

    plt.ylim(70,260)
    plt.yticks([75,100,125,150,175,200,225,250], yt, fontsize=20)

    #plt.xticks(i,  var2, rotation=90)
    plt.xticks(i4,  files, fontsize=20)
    plt.legend(loc=9, fontsize=20)
    plt.savefig("bar1")
    plt.show()
elif sys.argv[1] == "bars2":
    plt.figure(figsize=(24,18))

    mar = 0
    for bla in range(len(own)):
        if bla is not 0 and bla % 10 == 0:
            mar = mar + 1
        i.append(bla + mar)
        i2.append(bla + mar + 0.5)
        i3.append(bla + mar + 0.25)
        if bla is not 0 and bla % 5 == 0 and bla % 10 is not 0:
            i4.append(bla - 0.5 + mar)

    width = 1
        
    i2 = np.arange(0.5, len(own) + 0.5, 1)

    p1 = plt.bar(i, var, .9, bottom=1, label="Abweichungen zu den heuristischen Werten")

    u = 0
    for rect in p1:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2.0, height + 1.005, var2[u], ha='center', va='bottom', rotation=90)
        u = u + 1

    plt.ylim(1,1.1)

    plt.yticks(fontsize=20)

    plt.xticks(i4,  files, fontsize=20)
    plt.legend(loc=9, fontsize=20)
    plt.savefig("bar2")
    plt.show()
elif sys.argv[1] == "bars3":

    plt.figure(figsize=(24,18))
    i = []
    i2 = []
    i3 = []
    mar = 0
    for bla in range(len(own)):
        if bla is not 0 and bla % 10 == 0:
            mar = mar + 1
        i.append(bla + mar)
        i2.append(bla + mar + 0.5)
        i3.append(bla + mar + 0.25)
        if bla is not 0 and bla % 5 == 0 and bla % 10 is not 0:
            i4.append(bla - 0.5 + mar)

    width = 1
    
    # u = 0
    # for rect in p1:
    #     height = rect.get_height()
    #     plt.text(rect.get_x() + rect.get_width()/2.0, height + 1.005, var2[u], ha='center', va='bottom', rotation=90)
    #     u = u + 1

    p1 = plt.bar(i, own, .5, bottom=1, label="Zufall")
    p1 = plt.bar(i2, own_t, .5, bottom=1, label="Zufall optimiert")
    p1 = plt.bar(i3, hrs, 1, bottom=1, label="Heuristische Werte")

    plt.ylim(70,260)
    plt.yticks([75,100,125,150,175,200,225,250], yt, fontsize=20)
    plt.yticks(fontsize=20)

    plt.xticks(i4,  files, fontsize=20)
    plt.legend(loc=9, fontsize=20)
    plt.savefig("bar3")
    plt.show()
elif sys.argv[1] == "boxes":
    #fig, ax = plt.subplots()

    numBoxes = num1
    labels = names1
    fig, ax1 = plt.subplots(figsize=(14, 12))
    fig.canvas.set_window_title('')
    #plt.subplots_adjust(left=0.1, right=0.1, top=0.1, bottom=0.1)

    # data = []
    # for i in range(0, len(times1), numBoxes):
    #     data.append(times1[i:i + 10])

    print(times1)

    bp = plt.boxplot(times1, notch=0, sym='+', vert=1, whis=1.5)
    plt.setp(bp['boxes'], color='black')
    plt.setp(bp['whiskers'], color='black', linestyle='-')
    plt.setp(bp['fliers'], color='#A30000', marker='+')
    plt.setp(bp['medians'], color='#A30000')


    # Add a horizontal grid to the plot, but make it very light in color
    # so we can use it for reading data values but not be distracting
    ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey',
                alpha=0.5)

    # Hide these grid behind plot objects
    ax1.set_axisbelow(True)
    ax1.set_title('Round-Trip-Time of a 1450bytes UDP packet in different scenarios')
    #ax1.set_xlabel('Instanz I')
    ax1.set_ylabel('Time in µs')


    # Now fill the boxes with desired colors
    
    boxColors = ['darkkhaki', 'seagreen', 'royalblue']

    medians = list(range(numBoxes))
    for i in range(numBoxes):
        box = bp['boxes'][i]
        boxX = []
        boxY = []
        for j in range(5):
            boxX.append(box.get_xdata()[j])
            boxY.append(box.get_ydata()[j])
        boxCoords = list(zip(boxX, boxY))
        # Alternate between Dark Khaki and Royal Blue
        k = i % 3
        boxPolygon = Polygon(boxCoords, facecolor=boxColors[k])
        ax1.add_patch(boxPolygon)
        # Now draw the median lines back over what we just filled in
        med = bp['medians'][i]
        medianX = []
        medianY = []
        for j in range(2):
            medianX.append(med.get_xdata()[j])
            medianY.append(med.get_ydata()[j])
            plt.plot(medianX, medianY, 'k')
            medians[i] = medianY[0]
        # Finally, overplot the sample averages, with horizontal alignment
        # in the center of each box
        plt.plot([np.average(med.get_xdata())], [np.average(times1[i])], color='w', marker='*', markeredgecolor='#A30000')

    # Set the axes ranges and axes labels
    ax1.set_xlim(0.5, numBoxes + 0.5)
    top = 260
    bottom = 70
    #ax1.set_ylim(bottom, top)
    xtickNames = plt.setp(ax1, xticklabels=labels)
    plt.setp(xtickNames, rotation=90, fontsize=8)
    ax1.set_yscale('log')

    x = np.arange(1, numBoxes + 1, 1)

    #print(str(len(x)) + " y " + str(len(var2)))

    #plt.scatter(x, hrs2, label="Heuristische Werte")

    #plt.yscale('log');

    yt = ['500', '1000', '2000', '4000', '20000', '50000', '100000', '250000', '1000000', '3500000']
    ax1.set_yticks([500,1000,2000,4000,20000,50000,100000,250000,1000000,3500000])
    ytickNames = plt.setp(ax1, yticklabels=yt)
    plt.setp(ytickNames, rotation=0, fontsize=8)

    # Due to the Y-axis scale being different across samples, it can be
    # hard to compare differences in medians across the samples. Add upper
    # X-axis tick labels with the sample medians to aid in comparison
    # (just use two decimal places of precision)
    pos = np.arange(numBoxes) + 1
    upperLabels = [str(int(s)) for s in medians]
    weights = ['bold', 'semibold']
    #for tick, label in zip(range(numBoxes), ax1.get_xticklabels()):
    #    ax1.text(pos[tick], top - (top*0.015), upperLabels[tick],
    #            horizontalalignment='center', size='medium', weight=weights[k],
    #            color='#A30000')

    # Finally, add a basic legend
    #plt.legend(loc=9, fontsize=20)

    plt.savefig(sys.argv[3])
    
    plt.show()
