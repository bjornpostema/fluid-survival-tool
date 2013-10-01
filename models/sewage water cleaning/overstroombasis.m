#####################
# DFPN model file   #
#####################
#
#### Number of places
12
# List of places
# Syntax: type id discMarking fluidLevel fluidBound
#
# Types:
#
# DISCRETE   0
# FLUID      1
#
1 overstroom 0 0 50
1 extradroog 0 0 0
1 extraregen 0 0 0
0 droog 1 0 0 
0 regen 0 0 0
1 gemeente 0 0 15
0 energieOn 1 0 0
0 energieOff 0 0 0
0 failOnce 1 0 0
1 afvoer 0 0 100
0 pompenOn 4 0 0
0 pompenOff 0 0 0
#
#### Number of transitions
12
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
2 overstroomdroog 0 0 0 0.40 na
2 overstroomregen 0 0 0 2.00 na
0 regenOn 20 1 0 0 na
0 droogOn 10 1 0 0 na
2 alsDroog 0 0 0 0.40 na
2 alsDroog1 0 0 0 0.40 na
2 alsRegen 0 0 0 2.00 na
2 alsRegen1 0 0 0 2.00 na
2 aanvoer 0 0 0 1.20 na
0 storingOn 40 1 0 0 na
0 reparatie 11 1 0 0 na
3 storingOff 1 1 0 0 exp{0.1}
#
#### Number of arcs
26
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
5 testRegen regen alsRegen 1 1 0
5 testRegen1 regen alsRegen1 1 1 0
5 testDroog droog alsDroog 1 1 0
5 testDroog1 droog alsDroog1 1 1 0
0 regenUit regen droogOn 1 1 0 
1 droogIn droogOn droog 1 1 0
0 droogUit droog regenOn 1 1 0
1 regenin regenOn regen 1 1 0
2 extra1 extraregen alsRegen1 1 1 0
2 extra3 extradroog alsDroog1 1 1 0
2 extra5 extraregen overstroomregen 1 1 1
2 extra6 extradroog overstroomdroog 1 1 1
3 regenOn alsRegen extraregen 1 1 0
3 extra2 alsRegen1 gemeente 1 1 0
3 droogOn alsDroog extradroog 1 1 0
3 extra4 alsDroog1 gemeente 1 1 0
3 extra7 overstroomregen overstroom 1 1 0
3 extra8 overstroomdroog overstroom 1 1 0
2 aanvoer1 gemeente aanvoer 1 1 0
3 afvoer1 aanvoer afvoer 1 1 0
0 fail failOnce storingOn 1 1 0
0 energie1 energieOn storingOn 1 1 0
1 energie2 storingOn energieOff 1 1 0
0 energie3 energieOff storingOff 1 1 0
1 energie4 storingOff energieOn 1 1 0
5 storing1 energieOn aanvoer 1 1 0
#
