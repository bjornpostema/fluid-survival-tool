#####################
# DFPN model file   #
#####################
#
#### Number of places
16
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
0 time21to6   1 0 0
0 time6to21   0 0 0
1 soft1      0 20 20
1 filt1      0 0 2
1 filt2      0 0 2
1 filt3      0 0 2
1 filt4      0 0 2
1 filt5      0 0 2
1 filt6      0 0 2
1 filt7      0 0 2
1 stor7      0 8 8
0 failonce   1 0 0
#
#### Number of transitions
15
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
2 input1  0 0 0 2.0 na
2 pump1a  0 0 0 10.0 na
2 pump1b  0 0 0 10.0 na
2 pump1c  0 0 0 10.0 na
2 pump1d  0 0 0 10.0 na
2 pump1e  0 0 0 10.0 na
2 pump1f  0 0 0 10.0 na
2 pump1g  0 0 0 10.0 na
2 pump1h  0 0 0 1.7 na
2 demand21to6  0 0 0 1.0 na
2 demand6to21  0 0 0 2.0 na
0 failure  3 1 0 na
3 repair   1 1 0 0 tba
0 at6  9 1 0 0 na
0 at21 15 1 0 0 na
#
#### Number of arcs
33
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
3 pipe1e pump1b filt2  1 1 0
2 pipe1f filt2  pump1c 1 1 0
3 pipe1g pump1c filt3  1 1 0
2 pipe1h filt3  pump1d 1 1 0
3 pipe1i pump1d filt4  1 1 0
2 pipe1j filt4  pump1e 1 1 0
3 pipe1k pump1e filt5  1 1 0
2 pipe1l filt5  pump1f 1 1 0
3 pipe1m pump1f filt6  1 1 0
2 pipe1n filt6  pump1g 1 1 0
3 pipe1o pump1g filt7  1 1 0
2 pipe1p filt7 pump1h 1 1 0
3 pipe1g pump1h stor7   1 1 0
2 out21  stor7 demand21to6 1 1 0
2 out6   stor7 demand6to21 1 1 0
5 teIn1  Input1On input1 1 1 0
5 teFi1  Filt1On  pump1a 1 1 0
5 teSt1  Stor1On  pump1b 1 1 0
0 in6    time21to6   at6 1 1 0
0 in21   time6to21 at21 1 1 0
1 out21   at6 time6to21 1 1 0
1 out6    at21  time21to6  1 1 0
5 test21to6 time21to6 demand21to6 1 1 0
5 test6to21 time6to21 demand6to21 1 1 0
0 inBreak  Input1On failure 1 1 0
1 outBreak failure Off 1 1 0
0 inRepair Off repair  1 1 0
1 outRepair repair Input1On 1 1 0
0 failoncetest failonce failure 1 1 0
#
