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



/** \file fmi3_import_type.h
*  \brief Public interface to the FMI XML C-library: variable types handling.
*/

#ifndef FMI3_IMPORT_TYPE_H_
#define FMI3_IMPORT_TYPE_H_

#include "fmi3_import_unit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
	\addtogroup fmi3_import
	@{
	\addtogroup fmi3_import_typedef Support for processing variable types
	@}
	\addtogroup fmi3_import_typedef Support for processing variable types
  @{
*/
/**@name   Type definitions supporting structures*/
/**@{ */
/** \brief Opaque float definition object. */
typedef struct fmi3_xml_float_typedef_t fmi3_import_float_typedef_t;
/** \brief Opaque integer type definition object. */
typedef struct fmi3_xml_integer_typedef_t fmi3_import_integer_typedef_t; /* TODO: remove */
/** \brief Opaque integer type definition object. */
typedef struct fmi3_xml_int_typedef_t fmi3_import_int_typedef_t;
/** \brief Opaque enumeration type definition object. */
typedef struct fmi3_xml_enumeration_typedef_t fmi3_import_enumeration_typedef_t;
/** \brief Opaque general variable type definition object. */
typedef struct fmi3_xml_variable_typedef_t fmi3_import_variable_typedef_t;
/** \brief Opaque list of the type definitions in the model */
typedef struct fmi3_xml_type_definitions_t fmi3_import_type_definitions_t;
/**@} */

/** \brief Get the number of available type definitions */
FMILIB_EXPORT unsigned int fmi3_import_get_type_definition_number(fmi3_import_type_definitions_t* td);

/** \brief Get a type definition specified by the index 
	@param td the type definition list object
	@param index the index of type definition. Must be less than the number returned by #fmi3_import_get_type_definition_number
	@return A type definition object or NULL if index is out of range.
*/
FMILIB_EXPORT fmi3_import_variable_typedef_t* fmi3_import_get_typedef(fmi3_import_type_definitions_t* td, unsigned int  index);

/** \brief Get the type name*/
FMILIB_EXPORT const char* fmi3_import_get_type_name(fmi3_import_variable_typedef_t*);

/**\brief Get type description.

   Note that an empty string is returned if the attribute is not present in the XML.*/
FMILIB_EXPORT const char* fmi3_import_get_type_description(fmi3_import_variable_typedef_t*);

/** \brief Get base type used for the type definition */
FMILIB_EXPORT fmi3_base_type_enu_t fmi3_import_get_base_type(fmi3_import_variable_typedef_t*);

/* Boolean and String has no extra attributes -> not needed*/

/** \brief Cast the general type definition object to an object with a specific base type 
	@return Pointer to the specific type object or NULL if base type does not match.
*/
FMILIB_EXPORT fmi3_import_float_typedef_t* fmi3_import_get_type_as_float(fmi3_import_variable_typedef_t*);

/** \brief Cast the general type definition object to an object with a specific base type 
	@return Pointer to the specific type object or NULL if base type does not match.
*/
FMILIB_EXPORT fmi3_import_integer_typedef_t* fmi3_import_get_type_as_int(fmi3_import_variable_typedef_t*);

/** \brief Cast the general type definition object to an object with a specific base type 
	@return Pointer to the specific type object or NULL if base type does not match.
*/
FMILIB_EXPORT fmi3_import_integer_typedef_t* fmi3_import_get_type_as_intXX(fmi3_import_variable_typedef_t*); /* TODO: rename function after removing old get_type_as_int */

/** \brief Cast the general type definition object to an object with a specific base type 
	@return Pointer to the specific type object or NULL if base type does not match.
*/
FMILIB_EXPORT fmi3_import_enumeration_typedef_t* fmi3_import_get_type_as_enum(fmi3_import_variable_typedef_t*);

/** \brief Get the quantity associated with the type definition.

	@return NULL-pointer is always returned for strings and booleans.
	Empty string is returned if attribute is not present for other types.
*/
FMILIB_EXPORT const char* fmi3_import_get_type_quantity(fmi3_import_variable_typedef_t*);

/** \brief Get minimal value for the type.

	@return Either the value specified in the XML file or negated DBL_MAX as defined in <float.h>
*/
FMILIB_EXPORT fmi3_float64_t fmi3_import_get_float64_type_min(fmi3_import_float_typedef_t*);

/** \brief Get maximum value for the type

	@return Either the value specified in the XML file or DBL_MAX as defined in <float.h>
*/
FMILIB_EXPORT fmi3_float64_t fmi3_import_get_float64_type_max(fmi3_import_float_typedef_t*);

/** \brief Get the nominal value associated with the type definition */
FMILIB_EXPORT fmi3_float64_t fmi3_import_get_float64_type_nominal(fmi3_import_float_typedef_t*);

/** \brief Get the unit object associated with the type definition if any*/
FMILIB_EXPORT fmi3_import_unit_t* fmi3_import_get_float64_type_unit(fmi3_import_float_typedef_t*);

/** \brief Get the 'relativeQuantity' flag */
FMILIB_EXPORT int fmi3_import_get_float64_type_is_relative_quantity(fmi3_import_float_typedef_t*);

/** \brief Get the 'unbounded' flag */
FMILIB_EXPORT int fmi3_import_get_float64_type_is_unbounded(fmi3_import_float_typedef_t*);

/**
	\brief Get display unit associated with a type definition.
	@return Display unit object of NULL if none was given.
*/
FMILIB_EXPORT fmi3_import_display_unit_t* fmi3_import_get_float64_type_display_unit(fmi3_import_float_typedef_t*);

/** \brief Get minimal value for the type.

	@return Either the value specified in the XML file or negated FLT_MAX as defined in <float.h>
*/
FMILIB_EXPORT fmi3_float32_t fmi3_import_get_float32_type_min(fmi3_import_float_typedef_t*);

/** \brief Get maximum value for the type

	@return Either the value specified in the XML file or FLT_MAX as defined in <float.h>
*/
FMILIB_EXPORT fmi3_float32_t fmi3_import_get_float32_type_max(fmi3_import_float_typedef_t*);

/** \brief Get the nominal value associated with the type definition */
FMILIB_EXPORT fmi3_float32_t fmi3_import_get_float32_type_nominal(fmi3_import_float_typedef_t*);

/** \brief Get the unit object associated with the type definition if any*/
FMILIB_EXPORT fmi3_import_unit_t* fmi3_import_get_float32_type_unit(fmi3_import_float_typedef_t*);

/** \brief Get the 'relativeQuantity' flag */
FMILIB_EXPORT int fmi3_import_get_float32_type_is_relative_quantity(fmi3_import_float_typedef_t*);

/** \brief Get the 'unbounded' flag */
FMILIB_EXPORT int fmi3_import_get_float32_type_is_unbounded(fmi3_import_float_typedef_t*);

/**
	\brief Get display unit associated with a type definition.
	@return Display unit object of NULL if none was given.
*/
FMILIB_EXPORT fmi3_import_display_unit_t* fmi3_import_get_float32_type_display_unit(fmi3_import_float_typedef_t*);

/** \brief Get minimal value for the type.
	
	@return Either the value specified in the XML file or INT_MIN as defined in <limits.h>
*/
FMILIB_EXPORT int fmi3_import_get_integer_type_min(fmi3_import_integer_typedef_t*);

/** \brief Get maximum value for the type
	
	@return Either the value specified in the XML file or INT_MAX as defined in <limits.h>
*/
FMILIB_EXPORT fmi3_int8_t fmi3_import_get_int8_type_max(fmi3_import_int_typedef_t*);

/** \brief Get minimal value for the type.
	
	@return Either the value specified in the XML file or INT_MIN as defined in <limits.h>
*/
FMILIB_EXPORT fmi3_int8_t fmi3_import_get_int8_type_min(fmi3_import_int_typedef_t*);

/** \brief Get maximum value for the type
	
	@return Either the value specified in the XML file or INT_MAX as defined in <limits.h>
*/
FMILIB_EXPORT int fmi3_import_get_integer_type_max(fmi3_import_integer_typedef_t*);

/** \brief Get minimal value for the type.
	
	@return Either the value specified in the XML file or 0
*/
FMILIB_EXPORT unsigned int fmi3_import_get_enum_type_min(fmi3_import_enumeration_typedef_t*);

/** \brief Get maximum value for the type.
	
	@return Either the value specified in the XML file or INT_MAX as defined in <limits.h>
*/
FMILIB_EXPORT unsigned int fmi3_import_get_enum_type_max(fmi3_import_enumeration_typedef_t*);

/** \brief Get the number of elements in the enum */
FMILIB_EXPORT unsigned int  fmi3_import_get_enum_type_size(fmi3_import_enumeration_typedef_t*);

/** \brief Get an enumeration item name by index */
FMILIB_EXPORT const char* fmi3_import_get_enum_type_item_name(fmi3_import_enumeration_typedef_t*, unsigned int  item);

/** \brief Get an enumeration item value by index */
FMILIB_EXPORT int fmi3_import_get_enum_type_item_value(fmi3_import_enumeration_typedef_t*, unsigned int  item);

/** \brief Get an enumeration item description by index */
FMILIB_EXPORT const char* fmi3_import_get_enum_type_item_description(fmi3_import_enumeration_typedef_t*, unsigned int  item);

/** \brief Get an enumeration item name for the given value */
FMILIB_EXPORT const char* fmi3_import_get_enum_type_value_name(fmi3_import_enumeration_typedef_t* t, int value);

/**
*  @}
*/
#ifdef __cplusplus
}
#endif
#endif

