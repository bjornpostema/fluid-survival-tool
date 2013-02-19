#####################
# DFPN model file   #
#####################
#
#### Number of places
6
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
0 pumpOn     1 0 0
0 demandOff  1 0 0
0 demandOn   0 0 0 
1 reservoir1 0 0 6
1 reservoir2 0 0 6
1 reservoir3 0 0 6
#
#### Number of transitions
8
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
2 pump1 0 0 0 1.0 na
2 pump2 0 0 0 1.0 na
2 pump3 0 0 0 1.0 na
2 out1  0 0 0 1.0 na
2 out2  0 0 0 2.0 na
2 out3  0 0 0 3.0 na
3 pumpBreaks 0 0 0 0 exp(3)
0 demandStop 5 1.0 0 0 na
#
#### Number of arcs
15
#
# List of arcs
# Syntax: type id fromId toId weight share priority
#
# Types:
# DISCRETE_INPUT  0
# DISCRETE_OUTPUT 1
# FLUID_INPUT     2
# FLUID_OUTPUT    3
# INHIBITOR       4
# TEST            5
#
# Input,test,inhibitor: Place -> Transition,  Output: Transition -> Place
#
3 pipe1 pump1       reservoir1   1 1 0
3 pipe2 pump2       reservoir2   1 1 0
3 pipe3 pump3       reservoir3   1 1 0
2 pipe4 reservoir1  out1         1 1 0
2 pipe5 reservoir2  out2         1 1 0
2 pipe6 reservoir3  out3         1 1 0
5 test1 pumpOn      pump1        1 1 0
5 test2 pumpOn      pump2        1 1 0
5 test3 pumpOn      pump3        1 1 0
5 test4 demandOn    out1         1 1 0
5 test5 demandOn    out2         1 1 0
5 test6 demandOn    out3         1 1 0
0 da1   pumpOn      pumpBreaks   1 1 0
0 da2   demandOff   demandStop   1 1 0
1 da3   demandStop  demandOn     1 1 0
#
