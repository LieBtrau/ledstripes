For lighting a landscape sown from plywood, a number of PCB's with LEDs on them are created.  These PCB's can be connected together to form a string.  A separate control PCB controls the brightness of the LEDs.  The user uses a TV remote control to set the LED color and intensity.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=-IzDMhllZa0' target='_blank'><img src='http://img.youtube.com/vi/-IzDMhllZa0/0.jpg' width='425' height=344 /></a>

[http://ledstripes.googlecode.com/files/1S.JPG](http://ledstripes.googlecode.com/files/1.JPG)
[http://ledstripes.googlecode.com/files/2S.JPG](http://ledstripes.googlecode.com/files/2.JPG)
[http://ledstripes.googlecode.com/files/3S.JPG](http://ledstripes.googlecode.com/files/3.JPG)
[http://ledstripes.googlecode.com/files/4S.JPG](http://ledstripes.googlecode.com/files/4.JPG)


[http://ledstripes.googlecode.com/files/5S.JPG](http://ledstripes.googlecode.com/files/5.JPG)
[http://ledstripes.googlecode.com/files/6S.JPG](http://ledstripes.googlecode.com/files/6.JPG)
[http://ledstripes.googlecode.com/files/7S.JPG](http://ledstripes.googlecode.com/files/7.JPG)
[http://ledstripes.googlecode.com/files/8S.JPG](http://ledstripes.googlecode.com/files/8.JPG)


[http://ledstripes.googlecode.com/files/9S.JPG](http://ledstripes.googlecode.com/files/9.JPG)
[http://ledstripes.googlecode.com/files/10S.JPG](http://ledstripes.googlecode.com/files/10.JPG)
[http://ledstripes.googlecode.com/files/11S.JPG](http://ledstripes.googlecode.com/files/11.JPG)
[http://ledstripes.googlecode.com/files/12S.JPG](http://ledstripes.googlecode.com/files/12.JPG)
[http://ledstripes.googlecode.com/files/13S.JPG](http://ledstripes.googlecode.com/files/13.JPG)



# Design #
## SquarePCB ##
Nine of these PCB's will be placed in the arch of the plywood.  Each PCB contains eight LEDs and some extra electronics for controlling the LED current.

![![](http://ledstripes.googlecode.com/files/SquarePCB_R200_small.png)](http://ledstripes.googlecode.com/files/SquarePCB_R200.png)

  * [Revision 0](https://code.google.com/p/ledstripes/source/detail?r=0) of this PCB couldn't be used, because it contained some errors.  The pin layout, described in the datasheet seems not to correspond to the LED-pin layout.
In this revision, the LED-current was limited by a series resistor.  There's a big difference of forward voltage of the LEDs, which caused unevenly distribution of light between the LEDs.

  * [Revision 1](https://code.google.com/p/ledstripes/source/detail?r=1) solves the above mentioned issues.

[Design data of R1.00](http://ledstripes.googlecode.com/files/SquarePCB_R100.PDF)

  * [Revision 2](https://code.google.com/p/ledstripes/source/detail?r=2) has only minor changes with respect to [revision 1](https://code.google.com/p/ledstripes/source/detail?r=1).  Some via's have been moved to make it easier to manually "plate" the via's.
[Design data of R2.00](http://ledstripes.googlecode.com/files/SquarePCB_R200.PDF)

## LongPCB ##
This PCB measures 15 by 200mm and contains 20 LEDs.

![![](http://ledstripes.googlecode.com/files/LongPCB_R000_small.png)](http://ledstripes.googlecode.com/files/LongPCB_R000.png)

  * [Revision 0](https://code.google.com/p/ledstripes/source/detail?r=0)
[Design data of R0.00](http://ledstripes.googlecode.com/files/LongPCB_R0.00.PDF)

## ControlPCB ##
This PCB is an Arduino shield that interfaces three LED strings.  Control is done using a TV remote control.  In the image you can see the ControlPCB plugged onto an Arduino Mega.

![![](http://ledstripes.googlecode.com/files/ControlPCB_R000_small.png)](http://ledstripes.googlecode.com/files/ControlPCB_R000.png)

[Design data of R0.00](http://ledstripes.googlecode.com/files/ControlPCB_R000.PDF)