#!/usr/bin/env python

import PyQt5.QtWidgets
import PyQt5.QtGui
import sys

app = PyQt5.QtWidgets.QApplication(sys.argv)

image = PyQt5.QtGui.QImage(640, 480, PyQt5.QtGui.QImage.Format_RGB888)
painter = PyQt5.QtGui.QPainter(image)
font = PyQt5.QtGui.QFont()
metrics = PyQt5.QtGui.QFontMetrics(font)

for char in "abcdefghijklmnopqrstuvwxyz":
    print( metrics.boundingRect(char) )
    print( metrics.height() )

image.save('rien.png')

