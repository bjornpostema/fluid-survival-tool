#####################
# DFPN model file   #
#####################
#
#### Number of places
3
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
0 pumpOn 1 0 0
0 demandOn 1 0 0
1 reservoir 0 0 10
#
#### Number of transitions
4
#
# List of transitions
# Syntax: type id discFiringTime weight priority fluidFlowRate genDistribution(EXP rate)
#
# Types:
#
# DETERMINISTIC   0
# IMMEDIATE       1
# FLUID           2
# GENERAL         3
#
2 pump 0 0 0 2.0 na
2 demand 0 0 0 1.0 na
3 pumpBreaks 3 1.0 0 0 7.5
0 demandStops 5 1.0 0 0 na
#
#### Number of arcs
6
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
3 pipe1 pump       reservoir   1 1 0
2 pipe2 reservoir  demand      1 1 0
5 test1 pumpOn     pump        1 1 0
5 test2 demandOn   demand      1 1 0
0 da1   pumpOn     pumpBreaks  1 1 0
0 da2   demandOn   demandStops 1 1 0
#
