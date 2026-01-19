################################################################################
# CONFIGURE PROJECT MAKEFILE (optional)
#   This file is where we make project specific configurations.
################################################################################

################################################################################
# OF ROOT
#   The location of your root openFrameworks installation
#       (default) OF_ROOT = ../../..
################################################################################
# OF_ROOT = ../../..

################################################################################
# PROJECT ROOT
#   The location of the project - aass this file is in the root of the project
#   this means it is located at ./
################################################################################
# PROJECT_ROOT = .

################################################################################
# PROJECT SPECIFIC CHECKS
#   This is a project specific flag useful to determine if the addon should be
#   included based on certain criteria. Can be any valid bash command like
#   - check specific libraries is installed
################################################################################
#PROJECT_SPECIFIC_CHECKS =

################################################################################
# PROJECT EXTERNAL SOURCE PATHS
#   These are fully qualified paths that are not within the PROJECT_ROOT folder.
#   Like source paths in the PROJECT_ROOT, these paths are subject to
#   exlclusion via the PROJECT_EXLCUSIONS list.
#
#     (default) PROJECT_EXTERNAL_SOURCE_PATHS = (blank)
#
#   Note: Leave a trailing space when adding list items with the += operator
################################################################################
# PROJECT_EXTERNAL_SOURCE_PATHS =

################################################################################
# PROJECT EXCLUSIONS
#   These makefiles assume that all folders in your current project directory
#   and target specific ADDONS folders contain code that should be compiled.
#   To prevent specific folders from being compiled, add them to the
#   PROJECT_EXCLUSIONS list.
################################################################################
#PROJECT_EXCLUSIONS =

################################################################################
# PROJECT LINKER FLAGS
#	These flags will be sent to the linker when compiling the executable.
#
#		(default) PROJECT_LDFLAGS = -Wl,-rpath=./libs
#
#   Note: Leave a trailing space when adding list items with the += operator
################################################################################
# PROJECT_LDFLAGS =

################################################################################
# PROJECT DEFINES
#   Preprocessor definitions to include when compiling this project
################################################################################
# PROJECT_DEFINES =

################################################################################
# PROJECT CFLAGS
#   This is a list of fully qualified CFLAGS required when compiling for this
#   project.  These CFLAGS will be used IN ADDITION TO the PLATFORM_CFLAGS
#   defined in your platform specific core configuration files. These settings
#   are not necessarily related to addon settings.
#
#      (default) PROJECT_CFLAGS = (blank)
#
#   Note: Leave a trailing space when adding list items with the += operator
################################################################################
# PROJECT_CFLAGS =
