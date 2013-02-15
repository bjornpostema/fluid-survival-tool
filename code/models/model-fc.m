#####################
# DFPN model file   #
#####################
#
################### TO TEST FLUID CONFLICT RESOLUTION
#
#### Number of places
1
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
1 fp 0 0 10
#
#### Number of transitions
5
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
2 in   0 0 0 19.0 na
2 out1 0 0 0 4.0 na
2 out2 0 0 0 3.0 na
2 out3 0 0 0 5.0 na
2 out4 0 0 0 6.0 na
#
#### Number of arcs
5
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
3 pipe1 in  fp   1 1 0
2 pipe2 fp  out1 1 1 0
2 pipe3 fp  out2 1 1 2
2 pipe4 fp  out3 1 2 1
2 pipe5 fp  out4 1 1 1
#
