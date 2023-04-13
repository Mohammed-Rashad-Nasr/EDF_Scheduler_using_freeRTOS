
# EDF scheduler using freeRTOS

this project is an application on RTOS using freeRTOS we used freeRTOS to implement EDF scheduler. we made changes in freeRTOS files in order to change schedueling algorithm.
project is done during egFWD advanced embedded systems nanodegree and tested usin kiel simulator.

## Acknowledgement

 - this project is done using steps stated in the following thesis [here](http://beru.univ-brest.fr/cheddar/contribs/examples_of_use/carraro16.pdf) + some other changes  


## Documentation

video is uploaded for demonstration and testing



## test tasks



| task | periodicity     | exec time                |
| :-------- | :------- | :------------------------- |
| button1 | 50 | 0.013 |
| button2 | 50 | 0.013 |
| periodic transmitter | 100 | 0.15 |
| reciever | 20 | 0.15 |
| load1 | 10 | 5 |
| load2 | 100 | 12 |

#### description

buttons tasks are used to send special messages for each button to the receiver task when they are pressed and they should be checked periodically if theay are pressed or not , periodic transmitter transmits constant string to the receiver too while the receiver prints the messages received and loads are just for loops added to add cpu load and test schedualability.
all calculations provided in pdf in verification folder


## screenshots

![tasks schedueling](https://github.com/Mohammed-Rashad-Nasr/EDF_Scheduler/blob/main/4-%20verification/screenshots.png)

video provided for demonstration and test [watch here](https://drive.google.com/file/d/15d81R3g0S8eLT8F9I88962b0HLTSpbXO/view?usp=share_link)
