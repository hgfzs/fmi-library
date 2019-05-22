/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef FMI3_TYPES_H_
#define FMI3_TYPES_H_
/** \file fmi3_types.h
	Transformation of the standard FMI type names into fmi3_ prefixed.
*/
/**
	\addtogroup jm_utils
	@{
		\addtogroup fmi3_utils
	@}
*/

/**	\addtogroup fmi3_utils Functions and types supporting FMI 2.0 processing.
	@{
*/
/** \name Renaming of typedefs 
@{*/
#define fmi3Component fmi3_component_t
#define fmi3ComponentEnvironment fmi3_component_environment_t
#define fmi3FMUstate fmi3_FMU_state_t
#define fmi3ValueReference fmi3_value_reference_t
#define fmi3Real fmi3_real_t
#define fmi3Integer fmi3_integer_t
#define fmi3Boolean fmi3_boolean_t
#define fmi3Char fmi3_char_t
#define fmi3String fmi3_string_t
#define fmi3Byte fmi3_byte_t

/** @}*/
/* Standard FMI 2.0 types */
#ifdef fmi3TypesPlatform_h
#undef fmi3TypesPlatform_h
#endif
#include <FMI3/fmi3TypesPlatform.h>
#undef fmi3TypesPlatform_h

/** FMI platform name constant string.*/
static const char * fmi3_get_types_platform(void) {
	return fmi3TypesPlatform;
}

#undef fmi3TypesPlatform

/** FMI boolean constants.*/
typedef enum {
	fmi3_true=fmi3True,
	fmi3_false=fmi3False
} fmi3_boolean_enu_t;

#undef fmi3True
#undef fmi3False

/**	
	@}
*/

#undef fmi3Component
#undef fmi3ValueReference
#undef fmi3Real
#undef fmi3Integer
#undef fmi3Boolean
#undef fmi3String
#undef fmi3UndefinedValueReference

#endif /* End of header file FMI3_TYPES_H_ */