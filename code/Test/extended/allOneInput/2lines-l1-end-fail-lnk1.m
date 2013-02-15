#####################
# DFPN model file   #
#####################
#
#### Number of places
18
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
0 InputOn   1 0 0
0 Filt1_l1on  1 0 0
0 Filt2_l1on  1 0 0
0 Stor_l1On  1 0 0
1 soft_l1      0 7 7
1 filt1_l1      0 2 2
1 filt2_l1      0 2 2
0 Filt1_l2on  1 0 0
0 Filt2_l2on  1 0 0
0 Stor_l2On  1 0 0
1 soft_l2      0 5 5
1 filt1_l2      0 2 2
1 filt2_l2      0 2 2
0 Off      0 0 0 
0 Dtime1   0 0 0
0 Dtime2   1 0 0
1 stor       0 20 20
0 failonce   1 0 0
#
#### Number of transitions
14
#
# List of transitions
# Syntax: type id discFiringTime weight priority fluidFlowRate genDistribution
#
# Types:
#
# DETERMINISTIC   0
# IMMEDIATE       1
# FLUID           2
# GENERAL         3
#
2 input  0 0 0 2.0 na
2 spump_l1  0 0 0 10.0 na
2 f1pump_11  0 0 0 1.7 na
2 f2pump_11  0 0 0 1.7 na
2 spump_l2  0 0 0 10.0 na
2 f1pump_12  0 0 0 1.7 na
2 f2pump_12  0 0 0 1.7 na
2 demand1  0 0 0 2.0 na
2 demand2  0 0 0 4.0 na
0 failure  3 1 0 na
3 repair   1 1 0 0 tba
0 at1  9 1 0 0 na
0 at2 15 1 0 0 na
2 lnkpump  0 0 0 1.7 na
#
#### Number of arcs
38
#
# List of arcs
# Syntax: type id fromId toId weight share priority
#
# Types:
# DISCRETE_INPUT  0  from a place to a transition
# DISCRETE_OUTPUT 1  from a transition to a place
# FLUID_INPUT     2
# FLUID_OUTPUT    3   from a transition to a fluid place
# INHIBITOR       4
# TEST            5   from place to transition
#
# Input,test,inhibitor: Place -> Transition,  Output: Transition -> Place
#
3 pipe_l1a input soft_l1  1 1 0
2 pipe_l1b soft_l1  spump_l1 1 1 0 
3 pipe_l1c spump_l1 filt1_l1 1 1 0
2 pipe_l1d filt1_l1 f1pump_11 1 1 0
3 pipe_l1e f1pump_11 filt2_l1   1 1 0
2 pipe_l1f filt2_l1 f2pump_11   1 1 0
3 pipe_l1g f2pump_11 stor   1 1 0
5 teIn_l1  InputOn input 1 1 0
5 teSt_l1  Stor_l1On  spump_l1 1 1 0
5 teFi1_l1 Filt1_l1on  f1pump_11 1 1 0
5 teFi2_l1 Filt2_l1on  f2pump_11 1 1 0
3 pipe_l2a input soft_l2  1 1 0
2 pipe_l2b soft_l2  spump_l2 1 1 0 
3 pipe_l2c spump_l2 filt1_l2 1 1 0
2 pipe_l2d filt1_l2 f1pump_12 1 1 0
3 pipe_l2e f1pump_12 filt2_l2   1 1 0
2 pipe_l2f filt2_l2 f2pump_12   1 1 0
3 pipe_l2g f2pump_12 stor   1 1 0
5 teIn_l2  InputOn input 1 1 0
5 teSt_l2  Stor_l2On  spump_l2 1 1 0
5 teFi1_l2 Filt1_l2on  f1pump_12 1 1 0
5 teFi2_l2 Filt2_l2on  f2pump_12 1 1 0
2 out1  stor demand1 1 1 0
2 out2  stor demand2 1 1 0
0 din1   Dtime1 at1 1 1 0
0 din2   Dtime2 at2 1 1 0
1 dout1 at1 Dtime2 1 1 0
1 dout2  at2  Dtime1  1 1 0
5 test1 Dtime1 demand1 1 1 0
5 test2 Dtime2 demand2 1 1 0
0 inBreak  Filt2_l1on failure 1 1 0
1 outBreak failure Off 1 1 0
0 inRepair Off repair  1 1 0
1 outRepair repair Filt2_l1on 1 1 0
0 failoncetest failonce failure 1 1 0
2 lnkpipe1 filt2_l2 lnkpump 1 1 0
3 lnkpipe2 lnkpump stor   1 1 0
5 teLnk Off  lnkpump 1 1 0
#
