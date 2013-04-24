#-------------------------------------------------
#
# Project configuration file created by B.F. Postema
# Use the model-view-controller principle
#
#-------------------------------------------------

TEMPLATE = subdirs
SUBDIRS =   model \
#            view

# ordered takes care of the compilation order of the directories.
CONFIG += ordered
# build must be last:
SUBDIRS += controller
