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
0 Input1On   1 0 0
0 Filt1On  1 0 0
0 Stor1On  1 0 0
0 Input2On   1 0 0
0 Filt2On  1 0 0
0 Stor2On  1 0 0
0 Off    0 0 0 
0 time21to6  0 0 0
0 time6to9   1 0 0
0 time9to12  0 0 0
0 time12to14 0 0 0
0 time14to17 0 0 0
0 time17to21 0 0 0
1 soft1      0 0 6
1 soft2      0 0 6
1 filt1      0 0 6
1 filt2      0 0 10
1 stor       0 0 10 
#
#### Number of transitions
20
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
2 input1 0 0 0 3.0 na
2 input2 0 0 0 3.0 na
2 pump1a  0 0 0 3.0 na
2 pump1b 0 0 0 3.0 na
2 pump2a 0 0 0 3.0 na
2 pump2b 0 0 0 3.0 na
2 demand21to6  0 0 0 2.0 na
2 demand6to9   0 0 0 2.0 na
2 demand9to12  0 0 0 3.0 na
2 demand12to14 0 0 0 3.0 na
2 demand14to17 0 0 0 3.0 na
2 demand17to21 0 0 0 6.0 na
0 failure 3 1 0 0 na
3 repair  3 0 0 0 tba
0 at6  9 1 0 0 na
0 at9  3 1 0 0 na
0 at12 3 1 0 0 na
0 at14 2 1 0 0 na
0 at17 3 1 0 0 na
0 at21 4 1 0 0 na
#
#### Number of arcs
44
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
3 pipe2a input2 soft2  1 1 0
2 pipe2b soft2  pump2a 1 1 0 
3 pipe2c pump2a filt2  1 1 0
2 pipe2d filt2  pump2b 1 1 0
3 pipe2e pump2b stor   1 1 0
2 out21  stor   demand21to6  1 1 0
2 out6   stor   demand6to9   1 1 0
2 out9   stor   demand9to12  1 1 0
2 out12  stor   demand12to14 1 1 0
2 out14  stor   demand14to17 1 1 0
2 out17  stor   demand17to21 1 1 0
5 teto6    time21to6  demand21to6 1 1 0
5 teto9    time6to9   demand6to9  1 1 0
5 teto12   time9to12  demand9to12 1 1 0
5 teto14   time12to14 demand12to14 1 1 0
5 teto17   time14to17 demand14to17 1 1 0
5 teto21   time17to21 demand17to21 1 1 0
5 teIn1  Input1On input1 1 1 0
5 teIn2  Input2On input2 1 1 0
5 teFi1  Filt1On  pump1a 1 1 0
5 teFi2  Filt2On  pump2a 1 1 0
5 teSt1  Stor1On  pump1b 1 1 0
5 teSt1  Stor2On  pump2b 1 1 0
0 in6    time21to6   at6 1 1 0
0 in9    time6to9    at9 1 1 0
0 in12   time9to12  at12 1 1 0
0 in14   time12to14 at14 1 1 0
0 in17   time14to17 at17 1 1 0
0 in21   time17to21 at21 1 1 0
1 out21   at21 time21to6 1 1 0
1 out6    at6  time6to9  1 1 0
1 out9    at9  time9to12 1 1 0
1 out12   at12 time12to14 1 1 0
1 out14   at14 time14to17 1 1 0
1 out17   at17 time17to21 1 1 0
0 inBreak  Stor2On failure 1 1 0
1 outBreak failure Off 1 1 0
0 inRepair Off repair  1 1 0
1 outRepair repair Stor2On 1 1 0
#
