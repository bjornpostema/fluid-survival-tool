#####################
# DFPN model file   #
#####################
#
#### Number of places
7
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
0 Input1On   1 0 0
0 Filt1On  1 0 0
0 Stor1On  1 0 0
0 Off      0 0 0 
1 soft1      0 0 1
1 filt1      0 0 2
1 stor       0 0 3 
#
#### Number of transitions
6
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
2 input1 0 0 0 4.0 na
2 pump1a  0 0 0 3.0 na
2 pump1b 0 0 0 2.0 na
2 demand  0 0 0 1.0 na
0 failure 5 1 0 0 na
0 repair  5 1 0 0 tba
#
#### Number of arcs
13
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
3 pipe1a input1 soft1  1 1 0
2 pipe1b soft1  pump1a 1 1 0 
3 pipe1c pump1a filt1  1 1 0
2 pipe1d filt1  pump1b 1 1 0
3 pipe1e pump1b stor   1 1 0
2 outarc  stor   demand   1 1 0
5 teIn1  Input1On input1 1 1 0
5 teFi1  Filt1On  pump1a 1 1 0
5 teSt1  Stor1On  pump1b 1 1 0
0 inBreak  Stor1On failure 1 1 0
1 outBreak failure Off 1 1 0
0 inRepair Off repair  1 1 0
1 outRepair repair Stor1On 1 1 0
#
