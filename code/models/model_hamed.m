#####################
# DFPN model file   #
#####################
#
#### Number of places
5
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
0 Pd1 1 0 0
0 Pd2 1 0 0
0 Pd3 1 0 0
1 Pf1 0 2 10
1 Pf2 0 1 7
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
2 Tf1 0 0 0 3.0 na
2 Tf2 0 0 0 2.0 na
2 Tf3 0 0 0 1.0 na
3 Tg1 0 1.0 0 0 exp(3)
0 Td1 5.0 1.0 0 na
0 Td2 9.0 1.0 0 0 na
#
#### Number of arcs
10
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
3 pipe1 Tf1  Pf1 1 1 0
2 pipe2 Pf1  Tf2 1 1 0
3 pipe3 Tf2  Pf2 1 1 0
2 pipe4 Pf2  Tf3 1 1 0
5 test1 Pd1  Tf1 1 1 0
5 test2 Pd2  Tf2 1 1 0
5 test3 Pd3  Tf3 1 1 0
0 da1   Pd1  Tg1 1 1 0
0 da2   Pd2  Td1 1 1 0
0 da3   Pd3  Td2 1 1 0
#
