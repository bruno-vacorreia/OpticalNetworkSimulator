#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/Data/Data.o \
	${OBJECTDIR}/src/Data/Options.o \
	${OBJECTDIR}/src/Data/Parameters.o \
	${OBJECTDIR}/src/Kernel.o \
	${OBJECTDIR}/src/SimulationType/MultiLoadSimulation.o \
	${OBJECTDIR}/src/SimulationType/SimulationType.o \
	${OBJECTDIR}/src/Structure/Link.o \
	${OBJECTDIR}/src/Structure/Node.o \
	${OBJECTDIR}/src/Structure/Topology.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/opticalnetworksimulator_backup

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/opticalnetworksimulator_backup: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/opticalnetworksimulator_backup ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/Data/Data.o: src/Data/Data.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Data
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Data/Data.o src/Data/Data.cpp

${OBJECTDIR}/src/Data/Options.o: src/Data/Options.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Data
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Data/Options.o src/Data/Options.cpp

${OBJECTDIR}/src/Data/Parameters.o: src/Data/Parameters.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Data
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Data/Parameters.o src/Data/Parameters.cpp

${OBJECTDIR}/src/Kernel.o: src/Kernel.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Kernel.o src/Kernel.cpp

${OBJECTDIR}/src/SimulationType/MultiLoadSimulation.o: src/SimulationType/MultiLoadSimulation.cpp
	${MKDIR} -p ${OBJECTDIR}/src/SimulationType
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SimulationType/MultiLoadSimulation.o src/SimulationType/MultiLoadSimulation.cpp

${OBJECTDIR}/src/SimulationType/SimulationType.o: src/SimulationType/SimulationType.cpp
	${MKDIR} -p ${OBJECTDIR}/src/SimulationType
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/SimulationType/SimulationType.o src/SimulationType/SimulationType.cpp

${OBJECTDIR}/src/Structure/Link.o: src/Structure/Link.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Structure
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Structure/Link.o src/Structure/Link.cpp

${OBJECTDIR}/src/Structure/Node.o: src/Structure/Node.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Structure
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Structure/Node.o src/Structure/Node.cpp

${OBJECTDIR}/src/Structure/Topology.o: src/Structure/Topology.cpp
	${MKDIR} -p ${OBJECTDIR}/src/Structure
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Structure/Topology.o src/Structure/Topology.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc