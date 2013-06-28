/*
 * DFPN.cpp
 *
 *  Created on: 10 nov. 2011
 *      Author: GhasemiehH and PostemaBF
 */

#include "DFPN2.h"
#include <QString>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern "C" {
#include <matheval.h>
}

namespace model {

//#include <QLocale>

const char *transTypeName[] = { "Det. ", "Imm. ", "Fluid", "Gen. " };
const char *arcsTypeName[] = { "D.In ", "D.Out", "F.In ", "F.Out", "Inhib", "Test " };

/*********** Read Model (support function) *********************/
#define READ_BUF_DIM 16384

double ReadData(FILE *fp) {
	static char Buff[READ_BUF_DIM];
	double Val;

    if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;
	while (!feof(fp) && (Buff[0] == '#'))
        if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;
	sscanf(Buff, "%lf", &Val);

	return Val;
}

char *ReadString(FILE *fp) {
	static char Buff[READ_BUF_DIM];
	static char Val[READ_BUF_DIM];

    if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;
	while (!feof(fp) && (Buff[0] == '#'))
        if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;
	sscanf(Buff, "%s", Val);

	return strdup(Val);
}

char *ReadLine(FILE *fp) {
	static char Buff[READ_BUF_DIM];

    if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;
	while (!feof(fp) && (Buff[0] == '#'))
        if (!fgets(Buff, READ_BUF_DIM, fp)) return 0;

	return strdup(Buff);
}

/************ Support functions ***************/

int findPlace(Model *M, char *id) {
	int i;

	for (i = 0; i < M->N_places; i++) {
		if (strcmp(M->places[i].id, id) == 0) {
			return i;
		}
	}
	printf("Error: place %s not found\n\n", id);
    return -1;
}

int findTransition(Model *M, char *id) {
	int i;

	for (i = 0; i < M->N_transitions; i++) {
		if (strcmp(M->transitions[i].id, id) == 0) {
			return i;
		}
	}
	printf("Error: transition %s not found\n\n", id);
    return -1;
}

/*********** Read Model *********************/

#define MAX_ID_LEN 2048

bool validateModel(Model *model, Logger *guic) {
    bool res = true;
    /*
     * Place checks
     */
    for (int i = 0; i < model->N_places; i++) {
        /* Guarentee unique place names */
        for (int j = 0; j < i; j++) {
            if (strcmp(model->places[i].id, model->places[j].id) == 0) {
                guic->addError(QString("Place #%1 : %2 has the same name as place #%3 : %4.").arg(i+1).arg(model->places[i].id).arg(j+1).arg(model->places[j].id).toStdString());
                res = false;
            }
        }
        if (model->places[i].type == PT_DISCRETE) {
            /* A deterministic place should not have a negative discrete marking. */
            if (model->places[i].d_mark < 0) {
                guic->addError(QString("Place #%1 : %2 has a negative discrete marking.").arg(i+1).arg(model->places[i].id).toStdString());
                res = false;
            }
            /* A deterministic place should only have zeros for fluid values */
            if (model->places[i].f_bound != 0 || model->places[i].f_level != 0) {
                guic->addError(QString("Place #%1 : %2 can only contain zeros for the fluid level and fluid bound.").arg(i+1).arg(model->places[i].id).toStdString());
                res = false;
            }
        } else if (model->places[i].type == PT_FLUID) {
            /* A fluid place should not have a negative fluid level */
            if (model->places[i].f_level < 0) {
                guic->addError(QString("Place #%1 : %2 has a negative fluid level.").arg(i+1).arg(model->places[i].id).toStdString());
                res = false;
            }
            /* A fluid place should not have a negative fluid bound */
            if (model->places[i].f_bound < 0) {
                guic->addError(QString("Place #%1 : %2 has a negative fluid bound").arg(i+1).arg(model->places[i].id).toStdString());
                res = false;
            }
            /* A fluid place should only have zeros for discrete values */
            if (model->places[i].d_mark != 0) {
                guic->addError(QString("Place #%1 : %2 can only contain zeros for the discrete marking.").arg(i+1).arg(model->places[i].id).toStdString());
                res = false;
            }
        }
    }

    /*
     * Transition checks
     */
    int gentrans = 0;

    for (int i = 0; i < model->N_transitions; i++) {
        /* Guarentee unique transition names */
        for (int j = 0; j < i; j++) {
            if (strcmp(model->transitions[i].id, model->transitions[j].id) == 0) {
                guic->addError(QString("Transition #%1 : %2 has the same name as transition #%3 : %4.").arg(i+1).arg(model->transitions[i].id).arg(j+1).arg(model->transitions[j].id).toStdString());
                res = false;
            }
        }
        if (model->transitions[i].type == TT_GENERAL) gentrans++;
    }

    /* The maximum amount of general transitions is restricted. */
    if (gentrans != 1) {
        guic->addError(QString("There are %1 general transition(s), while there should be 1.").arg(gentrans).toStdString());
        res = false;
    }

    /*
     * Arc checks
     */
    for (int i = 0; i < model->N_arcs; i++) {
        /* Guarentee unique transition names */
        for (int j = 0; j < i; j++) {
            if (strcmp(model->arcs[i].id, model->arcs[j].id) == 0) {
                guic->addError(QString("Arc #%1 : %2 has the same name as arc #%3 : %4.").arg(i).arg(model->arcs[i].id).arg(j).arg(model->arcs[j].id).toStdString());
                res = false;
            }
        }
    }

    return res;
}

Model *ReadModel(const char *FileName, Logger *guic) {
    /*
     * 'locale' should be set in order for european machine's to work with dots in the files.
     * Using comma's instead of dots would be a major confusion, as was I when I discovered this tiny little bug. -Bjorn
     */
    bool res = true;
    std::locale::global( std::locale( "C" ) );

    FILE *fp;
	Model *M;
	int i;
	char *buffLine;
	char idBuff[MAX_ID_LEN], distrBuff[MAX_ID_LEN];
	char sBuff[MAX_ID_LEN], dBuff[MAX_ID_LEN];
    int sId, dId;
	printf("Reading model: %s\n", FileName);
	fp = fopen(FileName, "r");
	if (fp == NULL) {
		printf("\n\n Error: cannot find model file %s\n\n", FileName);
        guic->addError(QString("Error: cannot find model file %1").arg(FileName).toStdString());
        return NULL;
	}

    M = (Model *) malloc(sizeof(Model));

	M->N_discretePlaces = 0;
	M->N_fluidPlaces = 0;
	M->N_determTransitions = 0;
    M->N_fluidTransitions = 0;
	M->N_generalTransitions = 0;

	M->N_places = (int) ReadData(fp);
	printf("Number of places:                                        : %d\n\n",
			M->N_places);
	if (M->N_places != 0) {
        M->places = (Place *) calloc(M->N_places, sizeof(Place));

        for (i = 0; i < M->N_places; i++) {
			buffLine = ReadLine(fp);

            if (sscanf(buffLine, "%d %s %d %lg %lg", &M->places[i].type, idBuff,
					&M->places[i].d_mark, &M->places[i].f_level,
                    &M->places[i].f_bound) != 5) {
                printf("Error in place %s \n", idBuff);
                guic->addError(QString("Place #%1 has the wrong amount of arguments.").arg(i+1).toStdString());
                return NULL;
            }

			M->places[i].id = strdup(idBuff);
			free(buffLine);

			printf("Place %d: %s %s %d %g %g\n", i,
					(M->places[i].type == 0 ? "disc." : "fluid"),
					M->places[i].id, M->places[i].d_mark, M->places[i].f_level,
					M->places[i].f_bound);

			if (M->places[i].type == PT_DISCRETE) {
				M->places[i].idInMarking = M->N_discretePlaces;
				M->N_discretePlaces += 1;
			} else {
				M->places[i].idInMarking = M->N_fluidPlaces;
				M->N_fluidPlaces += 1;
			}
		}
	}
	printf("\n");

	M->N_transitions = (int) ReadData(fp);
	printf("Number of transitions:                                   : %d\n",
			M->N_transitions);
	if (M->N_transitions != 0) {
        M->transitions = (Transition *) calloc(M->N_transitions, sizeof(Transition));

		for (i = 0; i < M->N_transitions; i++) {
			buffLine = ReadLine(fp);

            //guic->addText(buffLine);
            if(sscanf(buffLine, "%d %s %lg %lg %d %lg %s",
					&M->transitions[i].type, idBuff, &M->transitions[i].time,
					&M->transitions[i].weight, &M->transitions[i].priority,
                    &M->transitions[i].flowRate, distrBuff) != 7) {
                guic->addError(QString("Transition #%1 has the wrong amount of arguments.").arg(i+1).toStdString());
                return NULL;
            }

            printf("%s",buffLine);
			M->transitions[i].id = strdup(idBuff);

            char *distrFunc = strdup(distrBuff);
            char delims[] = "{}";
            char *distrFinder = strtok(distrFunc,delims);

            if (distrFinder != NULL && strcmp ("exp",distrFinder) == 0) {
                M->transitions[i].df_distr = Exp;
            } else if (strcmp ("uni",distrFinder) == 0) {
                M->transitions[i].df_distr = Uni;
            } else if (strcmp ("gen",distrFinder) == 0) {
                M->transitions[i].df_distr = Gen;
            } else if (strcmp ("gamma",distrFinder) == 0) {
                M->transitions[i].df_distr = Gamma;
            } else if (strcmp ("norm",distrFinder) == 0) {
                M->transitions[i].df_distr = Norm;
            } else if (strcmp ("foldednorm",distrFinder) == 0) {
                M->transitions[i].df_distr = FoldedNorm;
            } else if (strcmp ("dtrm",distrFinder) == 0) {
                M->transitions[i].df_distr = Dtrm;
            } else if (M->transitions[i].type == TT_GENERAL) {
                guic->addError(QString("Transition #%1 : %2 has an incorrect cumulative distribution function (cdf). The supported cdfs are: exp{[lambda]}, uni{[a],[b]}, gen{[f(s)]}, norm{[mu],[sigma]}, foldednorm{[mu],[sigma]}, gamma{[K], [lambda]}. For example: exp{10} and gen{(1-exp(-s/10))}.").arg(i+1).arg(M->transitions[i].id).toStdString());
                res = false;
            }
            distrFinder = strtok(NULL,delims);

            M->transitions[i].df_argument = distrFinder;

			free(buffLine);

			printf("Transition %d: %s %s %lg %lg %d %lg %s\n", i,
					transTypeName[M->transitions[i].type],
					M->transitions[i].id, M->transitions[i].time,
					M->transitions[i].weight, M->transitions[i].priority,
					M->transitions[i].flowRate, distrFunc);

			if (M->transitions[i].type == TT_FLUID) {
				M->transitions[i].idInMarking = M->N_fluidTransitions;
				M->N_fluidTransitions += 1;
			} else if (M->transitions[i].type == TT_DETERMINISTIC) {
				M->transitions[i].idInMarking = M->N_determTransitions;
				M->N_determTransitions += 1;
			} else if (M->transitions[i].type == TT_GENERAL) {
				M->transitions[i].idInMarking = M->N_generalTransitions;
				M->N_generalTransitions += 1;
			}

        }
	}
	printf("\n");

	M->N_arcs = (int) ReadData(fp);
	printf("Number of arcs:                                          : %d\n",
			M->N_arcs);
	if (M->N_arcs != 0) {
        M->arcs = (Arc *) calloc(M->N_arcs, sizeof(Arc));

		for (i = 0; i < M->N_arcs; i++) {
			buffLine = ReadLine(fp);

            if (sscanf(buffLine, "%d %s %s %s %lg %lg %d", &M->arcs[i].type,
					idBuff, sBuff, dBuff, &M->arcs[i].weight,
                    &M->arcs[i].share, &M->arcs[i].priority) != 7) {
                guic->addError(QString("Arc #%1 has the wrong amount of arguments.").arg(i+1).toStdString());
                res = false;
            }
			M->arcs[i].id = strdup(idBuff);
			free(buffLine);

			switch (M->arcs[i].type) {
			case AT_DISCRETE_OUTPUT:
			case AT_FLUID_OUTPUT:
				sId = findTransition(M, sBuff);
				dId = findPlace(M, dBuff);
                if (sId == -1) {
                    guic->addError(QString("Arc #%1 : %2 has an invalid transition name %3").arg(i+1).arg(M->arcs[i].id).arg(sBuff).toStdString());
                    res = false;
                } else if (dId == -1) {
                    guic->addError(QString("Arc #%1 : %2 has an invalid place name %3").arg(i+1).arg(M->arcs[i].id).arg(dBuff).toStdString());
                    res = false;
                }
                M->arcs[i].fromId = sId;
				M->arcs[i].toId = dId;
				M->arcs[i].transId = sId;
				M->arcs[i].placeId = dId;
				break;
			case AT_DISCRETE_INPUT:
			case AT_FLUID_INPUT:
			case AT_INHIBITOR:
			case AT_TEST:
				sId = findPlace(M, sBuff);
				dId = findTransition(M, dBuff);
                if (sId == -1) {
                    guic->addError(QString("Arc #%1 : %2 has an invalid transition named %3").arg(i+1).arg(M->arcs[i].id).arg(sBuff).toStdString());
                    res = false;
                } else if (dId == -1) {
                    guic->addError(QString("Arc #%1 : %2 has an invalid place named %3").arg(i+1).arg(M->arcs[i].id).arg(dBuff).toStdString());
                    res = false;
                }
                M->arcs[i].fromId = sId;
				M->arcs[i].toId = dId;
				M->arcs[i].transId = dId;
				M->arcs[i].placeId = sId;
				break;
			default:
				sId = dId = -1;
				break;
			}

			printf("Arc %d: %s %s %d(%s) %d(%s) %lg %lg %d\n", i,
					arcsTypeName[M->arcs[i].type], M->arcs[i].id,
					M->arcs[i].fromId, sBuff, M->arcs[i].toId, dBuff,
					M->arcs[i].weight, M->arcs[i].share, M->arcs[i].priority);

		}
	}
	printf("\n");

    /* Validate the model and found results */
    return (res && validateModel(M, guic)) ? M : NULL;
}

void InitializeModel(Model *M) {
	int i, j, k;
	int tmp;
	for (i = 0; i < M->N_transitions; i++) {
		M->transitions[i].inputListSize = 0;
		M->transitions[i].inhibListSize = 0;
		M->transitions[i].testListSize = 0;
		M->transitions[i].outputListSize = 0;
	}
	for (i = 0; i < M->N_places; i++) {
		M->places[i].inputListSize = 0;
		M->places[i].inhibListSize = 0;
		M->places[i].testListSize = 0;
		M->places[i].outputListSize = 0;
	}

	for (i = 0; i < M->N_arcs; i++) {
		// Depending on the arc type, it increases the counter (of the corresponding transition)
		switch (M->arcs[i].type) {
		case AT_DISCRETE_OUTPUT:
		case AT_FLUID_OUTPUT:
			// check which transtition it is
			M->transitions[M->arcs[i].fromId].outputListSize += 1;
			M->places[M->arcs[i].toId].inputListSize += 1;
			break;
		case AT_DISCRETE_INPUT:
		case AT_FLUID_INPUT:
			M->transitions[M->arcs[i].toId].inputListSize += 1;
			M->places[M->arcs[i].fromId].outputListSize += 1;
			break;
		case AT_INHIBITOR:
			M->transitions[M->arcs[i].toId].inhibListSize += 1;
			M->places[M->arcs[i].fromId].inhibListSize += 1;
			break;
		case AT_TEST:
			M->transitions[M->arcs[i].toId].testListSize += 1;
			M->places[M->arcs[i].fromId].testListSize += 1;
			break;
		default:
			printf("error");
			break;

		}
	}

	for (i = 0; i < M->N_transitions; i++) {
		// Now that we know the sizes, we can allocate the memory for the lists
        M->transitions[i].inputList = (int *) calloc(M->transitions[i].inputListSize, sizeof(int));
        M->transitions[i].inhibList = (int *) calloc(M->transitions[i].inhibListSize, sizeof(int));
        M->transitions[i].testList = (int *) calloc(M->transitions[i].testListSize, sizeof(int));
        M->transitions[i].outputList = (int *) calloc(M->transitions[i].outputListSize, sizeof(int));

		//TODO: horrible variable change!!!!
		M->transitions[i].inputListSize = 0; // now this variable becomes the index of the next element of the list
		M->transitions[i].outputListSize = 0;
		M->transitions[i].inhibListSize = 0;
		M->transitions[i].testListSize = 0;
	}

	for (i = 0; i < M->N_places; i++) {
        M->places[i].inputList = (int *) calloc(M->places[i].inputListSize, sizeof(int));
        M->places[i].inhibList = (int *) calloc(M->places[i].inhibListSize, sizeof(int));
        M->places[i].testList = (int *) calloc(M->places[i].testListSize, sizeof(int));
        M->places[i].outputList = (int *) calloc(M->places[i].outputListSize, sizeof(int));
		M->places[i].inputListSize = 0;
		M->places[i].inhibListSize = 0;
		M->places[i].testListSize = 0;
		M->places[i].outputListSize = 0;
	}

	for (i = 0; i < M->N_arcs; i++) {
		// Depending on the arc type, it puts the elements in the array

		switch (M->arcs[i].type) {
		case AT_DISCRETE_OUTPUT:
		case AT_FLUID_OUTPUT:
			// check which transtition it is
			M->transitions[M->arcs[i].fromId].outputList[M->transitions[M->arcs[i].fromId].outputListSize]
					= i;
			M->transitions[M->arcs[i].fromId].outputListSize += 1;
			M->places[M->arcs[i].toId].inputList[M->places[M->arcs[i].toId].inputListSize]
					= i;
			M->places[M->arcs[i].toId].inputListSize += 1;
			break;
		case AT_DISCRETE_INPUT:
		case AT_FLUID_INPUT:
			M->transitions[M->arcs[i].toId].inputList[M->transitions[M->arcs[i].toId].inputListSize]
					= i;
            M->transitions[M->arcs[i].toId].inputListSize += 1;
			M->places[M->arcs[i].fromId].outputList[M->places[M->arcs[i].fromId].outputListSize]
					= i;
			M->places[M->arcs[i].fromId].outputListSize += 1;
			break;
		case AT_INHIBITOR:
			M->transitions[M->arcs[i].toId].inhibList[M->transitions[M->arcs[i].toId].inhibListSize]
					= i;
			M->transitions[M->arcs[i].toId].inhibListSize += 1;
			M->places[M->arcs[i].fromId].inhibList[M->places[M->arcs[i].fromId].inhibListSize]
					= i;
			M->places[M->arcs[i].fromId].inhibListSize += 1;
			break;
		case AT_TEST:
			M->transitions[M->arcs[i].toId].testList[M->transitions[M->arcs[i].toId].testListSize]
					= i;
			M->transitions[M->arcs[i].toId].testListSize += 1;
			M->places[M->arcs[i].fromId].testList[M->places[M->arcs[i].fromId].testListSize]
					= i;
			M->places[M->arcs[i].fromId].testListSize += 1;
			break;
		default:
			printf("error");
			break;
		}
	}

	///// Sorts arcs by priority and virtual saturation time
	for (i = 0; i < M->N_places; i++) {
		if (M->places[i].type == PT_FLUID) {
			// Only for fluid places

			// input bag:
			for (j = 0; j < M->places[i].inputListSize - 1; j++) {
				for (k = j + 1; k < M->places[i].inputListSize; k++) {
					if ((M->arcs[M->places[i].inputList[k]].priority
							< M->arcs[M->places[i].inputList[j]].priority)
							|| ((M->arcs[M->places[i].inputList[k]].priority
									== M->arcs[M->places[i].inputList[j]].priority)
									&& (M->arcs[M->places[i].inputList[k]].share
											> M->arcs[M->places[i].inputList[j]].share))) {
						//printf("Is");
						tmp = M->places[i].inputList[k];
						M->places[i].inputList[k] = M->places[i].inputList[j];
						M->places[i].inputList[j] = tmp;
					}
				}
			}

			// output bag:
			for (j = 0; j < M->places[i].outputListSize - 1; j++) {
				for (k = j + 1; k < M->places[i].outputListSize; k++) {
					if ((M->arcs[M->places[i].outputList[k]].priority
							< M->arcs[M->places[i].outputList[j]].priority)
							|| ((M->arcs[M->places[i].outputList[k]].priority
									== M->arcs[M->places[i].outputList[j]].priority)
									&& (M->arcs[M->places[i].outputList[k]].share
											> M->arcs[M->places[i].outputList[j]].share))) {
						//printf("Os [%d]%d [%d]%d,", j, M->places[i].outputList[j], k, M->places[i].outputList[k]);
						tmp = M->places[i].outputList[k];
						M->places[i].outputList[k] = M->places[i].outputList[j];
						M->places[i].outputList[j] = tmp;
					}
				}
			}

			printf("\n Place %s: \n input  -> ", M->places[i].id);
			for (j = 0; j < M->places[i].inputListSize; j++) {
				printf(
						"%d=P:%d %s (%lg), ",
						M->places[i].inputList[j],
						M->arcs[M->places[i].inputList[j]].priority,
						M->transitions[M->arcs[M->places[i].inputList[j]].fromId].id,
						M->arcs[M->places[i].inputList[j]].share);
			}
			printf("\n output -> ");
			for (j = 0; j < M->places[i].outputListSize; j++) {
				printf(
						"%d=P:%d %s (%lg), ",
						M->places[i].outputList[j],
						M->arcs[M->places[i].outputList[j]].priority,
						M->transitions[M->arcs[M->places[i].outputList[j]].toId].id,
						M->arcs[M->places[i].outputList[j]].share);
			}
			printf("\n\n");
		}
	}
	/*
	 // read out the transition bags for every transition
	 for(i = 0; i < M->N_transitions; i++) {
	 printf("Transition %d: inputListSize %d, arcs: ",
	 i,
	 M->transitions[i].inputListSize);

	 for(j =0; j < M->transitions[i].inputListSize; j++){

	 printf(" %d ",
	 M->transitions[i].inputList[j]);
	 }
	 printf("\n");
	 }
	 printf("\n");
	 for(i = 0; i < M->N_transitions; i++) {
	 printf("Transition %d: outputListSize %d, arcs: ",
	 i,
	 M->transitions[i].outputListSize);

	 for(j =0; j < M->transitions[i].outputListSize; j++){

	 printf(" %d ",
	 M->transitions[i].outputList[j]);
	 }
	 printf("\n");
	 }
	 printf("\n");
	 for(i = 0; i < M->N_transitions; i++) {
	 printf("Transition %d: inhibListSize %d, arcs: ",
	 i,
	 M->transitions[i].inhibListSize);

	 for(j =0; j < M->transitions[i].inhibListSize; j++){

	 printf(" %d ",
	 M->transitions[i].inhibList[j]);
	 }
	 printf("\n");
	 }
	 printf("\n");
	 for(i = 0; i < M->N_transitions; i++) {
	 printf("Transition %d: testListSize %d, arcs: ",
	 i,
	 M->transitions[i].testListSize);

	 for(j =0; j < M->transitions[i].testListSize; j++){

	 printf(" %d ",
	 M->transitions[i].testList[j]);
	 }
	 printf("\n");
	 }
	 */

}

Marking *allocMarking(Model *M) {
	Marking *out;
	out = (Marking *) malloc(sizeof(Marking));
    out->tokens = (int *) calloc(M->N_discretePlaces, sizeof(int));
    out->fluid0 = (double *) calloc(M->N_fluidPlaces, sizeof(double));
    out->fluid1 = (double *) calloc(M->N_fluidPlaces, sizeof(double));
    out->enabling = (int *) calloc(M->N_transitions, sizeof(int));
    out->actFluidRate
            = (double *) calloc(M->N_fluidTransitions, sizeof(double));
    out->clock0 = (double *) calloc(M->N_determTransitions, sizeof(double));
    out->clock1 = (double *) calloc(M->N_determTransitions, sizeof(double));
    out->generalHasFired = (int *) calloc(M->N_generalTransitions, sizeof(int));
    out->generalDisabled = (double *) calloc(M->N_generalTransitions, sizeof(double));

    out->fluidPlaceDeriv = (double *) calloc(M->N_fluidPlaces, sizeof(double));
    out->N_generalFired = 0;

	return out;
}

void freeMarking(Marking *K) {
	free(K->tokens);
	free(K->fluid0);
	free(K->fluid1);
	free(K->enabling);
	free(K->actFluidRate);
	free(K->clock0);
	free(K->clock1);
	free(K->generalHasFired);
	free(K->generalDisabled);

	free(K->fluidPlaceDeriv);

	free(K);
}

Marking *copyMarking(Model *M, Marking *Src) {
	Marking *out;
	int i;

	out = allocMarking(M);

	for (i = 0; i < M->N_discretePlaces; i++)
		out->tokens[i] = Src->tokens[i];
	for (i = 0; i < M->N_fluidPlaces; i++)
		out->fluid0[i] = Src->fluid0[i];
	for (i = 0; i < M->N_fluidPlaces; i++)
		out->fluid1[i] = Src->fluid1[i];
	for (i = 0; i < M->N_fluidPlaces; i++)
		out->fluidPlaceDeriv[i] = Src->fluidPlaceDeriv[i];

	for (i = 0; i < M->N_transitions; i++)
		out->enabling[i] = Src->enabling[i];
	for (i = 0; i < M->N_determTransitions; i++)
		out->clock0[i] = Src->clock0[i];
	for (i = 0; i < M->N_determTransitions; i++)
		out->clock1[i] = Src->clock1[i];
	//for(i = 0; i < M->N_fluidTransitions; i++) out->actFluidRate[i] = Src->actFluidRate[i];
	for (i = 0; i < M->N_transitions; i++) {
		if (M->transitions[i].type == TT_FLUID) {
			out->actFluidRate[M->transitions[i].idInMarking]
					= M->transitions[i].flowRate;
		}
	}
	for (i = 0; i < M->N_generalTransitions; i++)
		out->generalHasFired[i] = Src->generalHasFired[i];
	for (i = 0; i < M->N_generalTransitions; i++)
		out->generalDisabled[i] = Src->generalDisabled[i];

	//for(i = 0; i < M->N_fluidPlaces; i++) out->fluidPlaceDeriv[i] = Src->fluidPlaceDeriv[i];
	out->N_generalFired = Src->N_generalFired;

	return out;
}

Marking *createInitialMarking(Model *M) {
	int i;
    Marking *Mrk;

    Mrk = allocMarking(M); // allocate the initial marking
    // for all discrete places
    for (i = 0; i < M->N_places; i++) {
        if (M->places[i].type == 0) //discrete place
        {
            Mrk->tokens[M->places[i].idInMarking] = M->places[i].d_mark;
        } else //fluid place
        {
            Mrk->fluid0[M->places[i].idInMarking] = M->places[i].f_level;
            Mrk->fluid1[M->places[i].idInMarking] = 0.0;
        }
    }

    // set the enabling for all transitions to zero
    // for fluid transitions set actFluidRate to fluid rate
    for (i = 0; i < M->N_transitions; i++) {
        Mrk->enabling[i] = 0;
        if (M->transitions[i].type == TT_FLUID) {
            Mrk->actFluidRate[M->transitions[i].idInMarking]
                    = M->transitions[i].flowRate;
        }
        if (M->transitions[i].type == TT_DETERMINISTIC) {
            Mrk->clock0[M->transitions[i].idInMarking] = 0.0;
            Mrk->clock1[M->transitions[i].idInMarking] = 0.0;
        }
        if (M->transitions[i].type == TT_GENERAL) {
            Mrk->generalHasFired[M->transitions[i].idInMarking] = 0;
        }
    }

	return Mrk;
}

void checkEnabled(Model *M, Marking *K) {
	int i, j;
	int enable;

	// fills the K->enabling field for all the transitions
	K->N_generalFired = 0.0;

	for (i = 0; i < M->N_transitions; i++) {
		enable = 1;
		// deterministic transition------Hamed: Anything except Fluid !?
		if (M->transitions[i].type != TT_FLUID) {
			// check all input places
			for (j = 0; j < M->transitions[i].inputListSize; j++) {
				// disable if the input place has at least as many tokens in the current marking as the weight of the corresponding arc
				// Hamed: If at least exists an arc with weight greater than input place marking.
				if (K->tokens[M->places[M->arcs[M->transitions[i].inputList[j]].fromId].idInMarking]
						< M->arcs[M->transitions[i].inputList[j]].weight) {
					enable = 0;
					//printf(
					//		"disable transition %d due to inputarc from place %d \n",
					//		i, M->arcs[M->transitions[i].inputList[j]].fromId);
					break;

				}
			}
		}
		// check all test arcs
		if (enable == 1) {
			for (j = 0; j < M->transitions[i].testListSize; j++) {
				// disable if one test condition is not valid
				// if the place is discrete
				if (M->places[M->arcs[M->transitions[i].testList[j]].fromId].type
						== PT_DISCRETE) {
					if (K->tokens[M->places[M->arcs[M->transitions[i].testList[j]].fromId].idInMarking]
							< M->arcs[M->transitions[i].testList[j]].weight) {
						enable = 0;
						//printf(
						//		"disable transition %d due to testarc from place %d \n",
						//		i,
						//		M->arcs[M->transitions[i].testList[j]].fromId);
						break;
					}
				} else {
					if (K->fluid0[M->places[M->arcs[M->transitions[i].testList[j]].fromId].idInMarking]
							< M->arcs[M->transitions[i].testList[j]].weight) {
						enable = 0;
						//printf(
						//		"disable transition %d due to testarc from place %d \n",
						//		i,
						//		M->arcs[M->transitions[i].testList[j]].fromId);
						break;
					}
				}
			}
		}

		// check all inhib arcs

		if (enable == 1) {
			for (j = 0; j < M->transitions[i].inhibListSize; j++) {
				// disable if one inhibitor condition is not valid
				// if the place is discrete
				if (M->places[M->arcs[M->transitions[i].testList[j]].fromId].type
						== PT_DISCRETE) {
					if (K->tokens[M->places[M->arcs[M->transitions[i].inhibList[j]].fromId].idInMarking]
							>= M->arcs[M->transitions[i].inhibList[j]].weight) {
						enable = 0;
						//printf(
						//		"disable transition %d due to inhibarc from place %d \n",
						//		i,
						//		M->arcs[M->transitions[i].inhibList[j]].fromId);
						break;
					}
				} else {
					if (K->fluid0[M->places[M->arcs[M->transitions[i].testList[j]].fromId].idInMarking]
							> M->arcs[M->transitions[i].testList[j]].weight) {
						enable = 0;
						//printf(
						//		"disable transition %d due to testarc from place %d \n",
						//		i,
						//		M->arcs[M->transitions[i].testList[j]].fromId);
						break;
					}
				}
			}
		}
		K->enabling[i] = enable;

		if (M->transitions[i].type == TT_GENERAL) {
			if (K->generalHasFired[M->transitions[i].idInMarking]) {
				K->N_generalFired++;
			}
		}

		// printf("transition %d enabled %d \n",i,enable);
	}

}

void ShareFlow(Model *M, Marking *K, double Flux, int *_arcs, int N_arcs) {
	int i, j;
	double Dsum;
	int currentPri;
	double fluxReq, sharedFlux;

	i = 0;
	while (i < N_arcs) {
		//printf("Entering share at %d (Flux = %lg)\n", i, Flux);
		while ((K->enabling[M->arcs[_arcs[i]].transId] == 0) && (i < N_arcs)) {
			i++;// skip all the transitions that are not enabled
		}
		//printf("Skipped disabled until %d\n", i);

		// first arc at this priority
		currentPri = M->arcs[_arcs[i]].priority;

		fluxReq = M->transitions[M->arcs[_arcs[i]].transId].flowRate;
		Dsum = M->arcs[_arcs[i]].share
				* M->transitions[M->arcs[_arcs[i]].transId].flowRate;
		j = i + 1;
		while ((j < N_arcs) && (M->arcs[_arcs[j]].priority == currentPri) ) { // look for other enabled transitions at the same priority level
			//TODO: (very IMPORTANT) the "i" here should be "j".
			//commented by HAMED.
			//if (K->enabling[M->arcs[_arcs[i]].transId] == 1) { // consider only the enabled transitions
			if (K->enabling[M->arcs[_arcs[j]].transId] == 1) { // consider only the enabled transitions
				fluxReq += M->transitions[M->arcs[_arcs[j]].transId].flowRate;
				Dsum += M->arcs[_arcs[j]].share
						* M->transitions[M->arcs[_arcs[j]].transId].flowRate;
			}
			j++;
		}
		//printf("Found transitions with same priority until %d\n", j);

		if (Flux > fluxReq) { // Here we have enough for this priority level
			//printf("Enough flux to satisfy priority level\n");
			Flux -= fluxReq; // We give every transition what it requires
			i = j; // j is the first element with the next lower priority
		} else {
			if (j == i + 1) { // there is only one transition in this priority level
				//printf("Only one transition gets the whole share %g to %d\n", Flux, M->transitions[M->arcs[_arcs[i]].transId].idInMarking);
				// We give what we have
				K->actFluidRate[M->transitions[M->arcs[_arcs[i]].transId].idInMarking] = Flux;
				i = j;
				break;
			} else { // There are more than one transition in this priority level
				//printf("Starting priority sharing algorithm\n");
				j = i;
				while ((j< N_arcs) && (M->arcs[_arcs[j]].priority == currentPri)) { // look for other enabled transitions at the same priority level
					//TODO: (very IMPORTANT) the "i" here should be "j".
					//commented by HAMED
					//if (K->enabling[M->arcs[_arcs[i]].transId] == 1) { // consider only the enabled transitions
					if (K->enabling[M->arcs[_arcs[j]].transId] == 1) { // consider only the enabled transitions
						sharedFlux
								= Flux * M->arcs[_arcs[j]].share
										* M->transitions[M->arcs[_arcs[j]].transId].flowRate
										/ Dsum;
						if (sharedFlux > M->transitions[M->arcs[_arcs[j]].transId].flowRate) {
							Flux -= M->transitions[M->arcs[_arcs[j]].transId].flowRate;
							Dsum -= M->transitions[M->arcs[_arcs[j]].transId].flowRate * M->arcs[_arcs[j]].share;
						} else {
							K->actFluidRate[M->transitions[M->arcs[_arcs[j]].transId].idInMarking]
									= sharedFlux;
						}
					}
					j++;
				}
				i = j;
				break;
			}
		}
	}
	while (i < N_arcs) { // Put to 0 all the remaining transitions
		//TODO: I think we should do this only to enabled transitions. However, I don't know if there is a significant difference.
		K->actFluidRate[M->transitions[M->arcs[_arcs[i]].transId].idInMarking]
				= 0.0;
		i++;
	}
}

#define MAX_F_CONF_SOL_ITER  500
/**
 * Hamed: In this function I see a significant difference between implementation and what is described in the paper!!!
 **/
void setActFluidRate(Model *M, Marking *K, double s0) {
	//	static double *lastIterVal = NULL;
	int i, j, trId;
	double inFlux, outFlux;
	// for all places
	int noChange = 0;
	int nIter = 0;

	while (noChange == 0) {
		noChange = 1;
		nIter++;

		for (i = 0; i < M->N_places; i++) {
			// check all arcs whether they are fluid input / ouput arcs to that place
			if (M->places[i].type == PT_FLUID) {
				inFlux = outFlux = 0.0;

				for (j = 0; j < M->places[i].inputListSize; j++) {
					trId = M->arcs[M->places[i].inputList[j]].fromId;
					if (K->enabling[trId] > 0) {
						inFlux
								+= K->actFluidRate[M->transitions[trId].idInMarking];
					}
				}

				for (j = 0; j < M->places[i].outputListSize; j++) {
					trId = M->arcs[M->places[i].outputList[j]].toId;
					if (K->enabling[trId] > 0) {
						outFlux
								+= K->actFluidRate[M->transitions[trId].idInMarking];
					}
				}

				K->fluidPlaceDeriv[M->places[i].idInMarking] = inFlux - outFlux;
				//printf("%d %d %g\n", nIter, i, inFlux - outFlux);
				if ((inFlux - outFlux > ZERO_PREC)
						&& (IS_ZERO(K->fluid0[M->places[i].idInMarking] + s0* K->fluid1[M->places[i].idInMarking] - M->places[i].f_bound)) 
						&& (K->fluid1[M->places[i].idInMarking] >= 0.0)) {
					// Here, since inFlux > outFlux, we know that some transition will not receive its full amount of fluid
					//					printf("\n\n Problem at the upper bound at place %s \n\n", M->places[i].id);
					ShareFlow(M, K, outFlux, M->places[i].inputList, M->places[i].inputListSize);
					K->fluidPlaceDeriv[M->places[i].idInMarking] = 0;
					noChange = 0;
				}

				if ((inFlux - outFlux < -ZERO_PREC)
						&& (IS_ZERO(K->fluid0[M->places[i].idInMarking] + s0 * K->fluid1[M->places[i].idInMarking]))
						&& (K->fluid1[M->places[i].idInMarking] <= 0.0)) {
					//					printf("\n\n Problem at the lower bound at place %s \n\n", M->places[i].id);
					ShareFlow(M, K, inFlux, M->places[i].outputList,
							M->places[i].outputListSize);
					K->fluidPlaceDeriv[M->places[i].idInMarking] = 0;
					noChange = 0;
				}

			}
		}

		if (nIter > MAX_F_CONF_SOL_ITER) {
			printf("\n ERROR: Loop in conflict resolution\n\n");
			exit(0);
		}
	}

}

StateTimeAlt *makeTimeAlt(double left, double right, StateTimeAlt *next) {
	StateTimeAlt *A;
    A = (StateTimeAlt *) malloc(sizeof(StateTimeAlt));
	A->leftInt = left;
	A->rightInt = right;
	A->next = next;
	return A;
}

StateProbAlt *makeProbAlt(State *S, double P) {
	StateProbAlt *A;

	A = (StateProbAlt *) malloc(sizeof(StateProbAlt));
	A->p = P;
	A->next = NULL;
	A->S = S;
	return A;
}

#define POS_INFINITY 1.0e100


Marking *advanceMarking(Model *M, Marking *m, double T1, double T0, double *drift, int *enabled) {
	Marking *NewM;
	int i;

	NewM = copyMarking(M, m);

	// Advance fluid
	for (i = 0; i < M->N_places; i++) {
		if (M->places[i].type == PT_FLUID) {
			//printf("Updating fluid level f1:%g, f0:%g, T1:%g, T0:%g\n",
			//		NewM->fluid1[M->places[i].idInMarking],
			//		NewM->fluid0[M->places[i].idInMarking], T1, T0);
			NewM->fluid0[M->places[i].idInMarking]
					+= drift[M->places[i].idInMarking] * T0;
			NewM->fluid1[M->places[i].idInMarking]
					+= drift[M->places[i].idInMarking] * T1;
			//printf("%d => %g s + %g\n", i,
			//		NewM->fluid1[M->places[i].idInMarking],
			//		NewM->fluid0[M->places[i].idInMarking]);
		}
	}
	// Advance clocks
	for (i = 0; i < M->N_transitions; i++) {
		if ((M->transitions[i].type == TT_DETERMINISTIC) && (enabled[i] == 1)) {
//			printf("Updating clock with fluid Dep.\n");
			NewM->clock0[M->transitions[i].idInMarking] += T0;
			NewM->clock1[M->transitions[i].idInMarking] += T1;
			//printf("%d => %g s + %g\n", i,
			//		NewM->clock1[M->transitions[i].idInMarking],
			//		NewM->clock0[M->transitions[i].idInMarking]);
		}
		if ((M->transitions[i].type == TT_GENERAL) && (enabled[i] == 0)
				&& (NewM->generalHasFired[M->transitions[i].idInMarking] == 0)) {
			NewM->generalDisabled[M->transitions[i].idInMarking] += T0;
			//printf(
			//		"AAAAAARRRRRRGGGGHHHHHHHHHH! This should not happen now!\n\n\n\n\n\n");
		}
	}

	return NewM;
}


void fireTransition(Model *M, Marking *NewM, int Tr) {
	int j;
	for (j = 0; j < M->transitions[Tr].inputListSize; j++) {
		NewM->tokens[M->places[M->arcs[M->transitions[Tr].inputList[j]].fromId].idInMarking]
				-= M->arcs[M->transitions[Tr].inputList[j]].weight;
		//printf("remove one token from place %s \n",  M->places[M->arcs[Tr].inputList[j]].fromId].id);
	} // add tokens to each output place according to outputarc.weight  !!!!! is this correct

	for (j = 0; j < M->transitions[Tr].outputListSize; j++) {
		NewM->tokens[M->places[M->arcs[M->transitions[Tr].outputList[j]].toId].idInMarking]
				+= M->arcs[M->transitions[Tr].outputList[j]].weight;
		//printf("add one token to place %s \n",  M->places[M->arcs[M->transitions[Tr].outputList[j]].toId].id);
	}
	if (M->transitions[Tr].type == TT_DETERMINISTIC) {
		NewM->clock0[M->transitions[Tr].idInMarking] = 0.0;
		NewM->clock1[M->transitions[Tr].idInMarking] = 0.0;
	}
}

bool isGTransitionEnabled(Model* model, Marking* marking){
	checkEnabled(model, marking);

	int gtId;
	for (gtId = 0; gtId < model->N_transitions; gtId++){
		if (model->transitions[gtId].type == TT_GENERAL)
			break;
	}

	return marking->enabling[gtId];


}

void fireGeneralTransition(Model *M, Marking *NewM) {
	int gtId;
	for (gtId = 0; gtId < M->N_transitions; gtId++){
		if (M->transitions[gtId].type == TT_GENERAL)
			break;
	}

	fireTransition(M, NewM, gtId);
}

int gTransitionId(Model* M){
	int gtId;
	for (gtId = 0; gtId < M->N_transitions; gtId++){
		if (M->transitions[gtId].type == TT_GENERAL)
			return gtId;
	}
	return -1;
}

}
