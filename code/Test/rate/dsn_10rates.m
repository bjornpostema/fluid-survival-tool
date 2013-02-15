#####################
# DFPN model file   #
#####################
#
#### Number of places
18
# List of places
# Syntax type id discMarking fluidLevel fluidBound
#
# Types
#
# DISCRETE   0
# FLUID      1
#
0 Input1On   1 0 0
0 Filt1On  1 0 0
0 Stor1On  1 0 0
0 Off      0 0 0 
0 time1   1 0 0
0 time2  0 0 0
0 time3  0 0 0
0 time4  0 0 0
0 time5  0 0 0
0 time6  0 0 0
0 time7  0 0 0
0 time8  0 0 0
0 time9  0 0 0
0 time10  0 0 0
1 soft1      0 20 20
1 filt1      0 2 2
1 stor       0 8 8
0 failonce   1 0 0
#
#### Number of transitions
25
#
# List of transitions
# Syntax type id discFiringTime weight priority fluidFlowRate genDistribution
#
# Types
#
# DETERMINISTIC   0
# IMMEDIATE       1
# FLUID           2
# GENERAL         3
#
2 input1  0 0 0 2.0 na
2 pump1a  0 0 0 10.0 na
2 pump1b  0 0 0 1.7 na
2 demand1  0 0 0 .5 na
2 demand2  0 0 0 1.0 na
2 demand3  0 0 0 1.5 na
2 demand4  0 0 0 2.0 na
2 demand5  0 0 0 2.5 na
2 demand6  0 0 0 3.0 na
2 demand7  0 0 0 3.5 na
2 demand8  0 0 0 4.0 na
2 demand9  0 0 0 4.5 na
2 demand10  0 0 0 5.0 na
0 failure  3 1 0 na
3 repair   1 1 0 0 tba
0 at1  2 1 0 0 na
0 at2  2 1 0 0 na
0 at3  2 1 0 0 na
0 at4  3 1 0 0 na
0 at5  3 1 0 0 na
0 at6  3 1 0 0 na
0 at7  3 1 0 0 na
0 at8  2 1 0 0 na
0 at9  2 1 0 0 na
0 at10 2 1 0 0 na
#
#### Number of arcs
53
#
# List of arcs
# Syntax type id fromId toId weight share priority
#
# Types
# DISCRETE_INPUT  0  from a place to a transition
# DISCRETE_OUTPUT 1  from a transition to a place
# FLUID_INPUT     2
# FLUID_OUTPUT    3   from a transition to a fluid place
# INHIBITOR       4
# TEST            5   from place to transition
#
# Input,test,inhibitor Place - Transition,  Output Transition - Place
#
3 pipe1a input1 soft1  1 1 0
2 pipe1b soft1  pump1a 1 1 0 
3 pipe1c pump1a filt1  1 1 0
2 pipe1d filt1  pump1b 1 1 0
3 pipe1e pump1b stor   1 1 0
2 out1   stor demand1 1 1 0
2 out2   stor demand2 1 1 0
2 out3   stor demand3 1 1 0
2 out4   stor demand4 1 1 0
2 out5   stor demand5 1 1 0
2 out6   stor demand6 1 1 0
2 out7   stor demand7 1 1 0
2 out8   stor demand8 1 1 0
2 out9   stor demand9 1 1 0
2 out10   stor demand10 1 1 0
5 teIn1  Input1On input1 1 1 0
5 teFi1  Filt1On  pump1a 1 1 0
5 teSt1  Stor1On  pump1b 1 1 0
0 in12    time1 at2 1 1 0
0 in23    time2 at3 1 1 0
0 in34    time3 at4 1 1 0
0 in45    time4 at5 1 1 0
0 in56    time5 at6 1 1 0
0 in67    time6 at7 1 1 0
0 in78    time7 at8 1 1 0
0 in89    time8 at9 1 1 0
0 in910   time9 at10 1 1 0
0 in101   time10 at1 1 1 0
1 out2  at2 time2 1 1 0
1 out3  at3 time3 1 1 0
1 out4  at4 time4 1 1 0
1 out5  at5 time5 1 1 0
1 out6  at6 time6 1 1 0
1 out7  at7 time7 1 1 0
1 out8  at8 time8 1 1 0
1 out9  at9 time9 1 1 0
1 out10  at10 time10 1 1 0
1 out1  at1 time1 1 1 0
5 test1 time1 demand1 1 1 0
5 test2 time2 demand2 1 1 0
5 test3 time3 demand3 1 1 0
5 test4 time4 demand4 1 1 0
5 test5 time5 demand5 1 1 0
5 test6 time6 demand6 1 1 0
5 test7 time7 demand7 1 1 0
5 test8 time8 demand8 1 1 0
5 test9 time9 demand9 1 1 0
5 test10 time10 demand10 1 1 0
0 inBreak  Input1On failure 1 1 0
1 outBreak failure Off 1 1 0
0 inRepair Off repair  1 1 0
1 outRepair repair Input1On 1 1 0
0 failoncetest failonce failure 1 1 0
#
