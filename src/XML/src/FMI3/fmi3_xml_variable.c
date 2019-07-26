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

#include <string.h>
#include <stdio.h>

#include <JM/jm_vector.h>

#include "fmi3_xml_parser.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_model_description_impl.h"

#include "fmi3_xml_variable_impl.h"

static const char* module = "FMI3XML";

const char* fmi3_xml_get_variable_name(fmi3_xml_variable_t* v) {
    return v->name;
}

const char* fmi3_xml_get_variable_description(fmi3_xml_variable_t* v) {
    return v->description;
}

size_t fmi3_xml_get_variable_original_order(fmi3_xml_variable_t* v) {
    return v->originalIndex;
}

fmi3_value_reference_t fmi3_xml_get_variable_vr(fmi3_xml_variable_t* v) {
    return v->vr;
}

fmi3_variable_alias_kind_enu_t fmi3_xml_get_variable_alias_kind(fmi3_xml_variable_t* v) {
    return (fmi3_variable_alias_kind_enu_t)v->aliasKind;
}

fmi3_xml_variable_t* fmi3_xml_get_variable_alias_base(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v) {
    fmi3_xml_variable_t key;
    fmi3_xml_variable_t *pkey = &key, *base;
    void ** found;
    if(!md->variablesByVR) return 0;
    if(v->aliasKind == fmi3_variable_is_not_alias) return v;
    key = *v;
    key.aliasKind = fmi3_variable_is_not_alias;

    found = jm_vector_bsearch(jm_voidp)(md->variablesByVR,(void**)&pkey, fmi3_xml_compare_vr);
    assert(found);
    base = *found;
    return base;
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases.
*/
jm_status_enu_t fmi3_xml_get_variable_aliases(fmi3_xml_model_description_t* md,fmi3_xml_variable_t* v, jm_vector(jm_voidp)* list) {
    fmi3_xml_variable_t key, *cur;
    fmi3_value_reference_t vr = fmi3_xml_get_variable_vr(v);
    size_t baseIndex, i, num = jm_vector_get_size(jm_voidp)(md->variablesByVR);
    key = *v;
    key.aliasKind = 0;
    cur = &key;
    baseIndex = jm_vector_bsearch_index(jm_voidp)(md->variablesByVR,(void**)&cur, fmi3_xml_compare_vr);
    cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, baseIndex);
    assert(cur);
    i = baseIndex + 1;
    while(fmi3_xml_get_variable_vr(cur) == vr) {
        if(!jm_vector_push_back(jm_voidp)(list, cur)) {
            jm_log_fatal(md->callbacks,module,"Could not allocate memory");
            return jm_status_error;
        };
        if(i >= num) break;
        cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        assert(cur);
        i++;
    }
    if(baseIndex) {
        i = baseIndex - 1;
        cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        while(fmi3_xml_get_variable_vr(cur) == vr) {
            if(!jm_vector_push_back(jm_voidp)(list, cur)) {
                jm_log_fatal(md->callbacks,module,"Could not allocate memory");
                return jm_status_error;
            };
            i--;
            if(!i) break;
            cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i - 1);
            assert(cur);
        }
    }
    return jm_status_success;
}


fmi3_xml_variable_typedef_t* fmi3_xml_get_variable_declared_type(fmi3_xml_variable_t* v) {
    return (fmi3_xml_variable_typedef_t*)(fmi3_xml_find_type_struct(v->typeBase, fmi3_xml_type_struct_enu_typedef));
}

fmi3_base_type_enu_t fmi3_xml_get_variable_base_type(fmi3_xml_variable_t* v) {
    fmi3_xml_variable_type_base_t* type = v->typeBase;
    return (type->baseType);
}

int fmi3_xml_get_variable_has_start(fmi3_xml_variable_t* v) {
    return (v->typeBase->structKind == fmi3_xml_type_struct_enu_start);
}

fmi3_variability_enu_t fmi3_xml_get_variability(fmi3_xml_variable_t* v) {
    return (fmi3_variability_enu_t)v->variability;
}

fmi3_causality_enu_t fmi3_xml_get_causality(fmi3_xml_variable_t* v) {
    return (fmi3_causality_enu_t)v->causality;
}

fmi3_initial_enu_t fmi3_xml_get_initial(fmi3_xml_variable_t* v) {
    return (fmi3_initial_enu_t)v->initial;
}

fmi3_xml_variable_t* fmi3_xml_get_previous(fmi3_xml_variable_t* v) {
    return v->previous;
}

fmi3_boolean_t fmi3_xml_get_canHandleMultipleSetPerTimeInstant(fmi3_xml_variable_t* v) {
    return (fmi3_boolean_t)v->canHandleMultipleSetPerTimeInstant;
}


double fmi3_xml_get_real_variable_start(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_variable_start_real_t* start = (fmi3_xml_variable_start_real_t*)(vv->typeBase);
        return start->start;
    }
        return fmi3_xml_get_real_variable_nominal(v);
}

fmi3_xml_real_variable_t* fmi3_xml_get_real_variable_derivative_of(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;

    return (fmi3_xml_real_variable_t *)vv->derivativeOf;
}

fmi3_boolean_t fmi3_xml_get_real_variable_reinit(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_boolean_t)vv->reinit;
}

fmi3_xml_unit_t* fmi3_xml_get_real_variable_unit(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_real_type_props_t* props = (fmi3_xml_real_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));
    if(!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi3_xml_display_unit_t* fmi3_xml_get_real_variable_display_unit(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_real_type_props_t* props = (fmi3_xml_real_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

double fmi3_xml_get_real_variable_max(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_real_type_props_t* props = (fmi3_xml_real_type_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

double fmi3_xml_get_real_variable_min(fmi3_xml_real_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_real_type_props_t* props = (fmi3_xml_real_type_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMin;
}

double fmi3_xml_get_real_variable_nominal(fmi3_xml_real_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_real_type_props_t* props = (fmi3_xml_real_type_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeNominal;
}

int fmi3_xml_get_integer_variable_start(fmi3_xml_integer_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_variable_start_integer_t* start = (fmi3_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

int fmi3_xml_get_integer_variable_min(fmi3_xml_integer_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_integer_type_props_t* props = (fmi3_xml_integer_type_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMin;
}

int fmi3_xml_get_integer_variable_max(fmi3_xml_integer_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_integer_type_props_t* props = (fmi3_xml_integer_type_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

int fmi3_xml_get_enum_variable_min(fmi3_xml_enum_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_variable_type_base_t* props = fmi3_xml_find_type_props(vv->typeBase);
    return ((fmi3_xml_enum_variable_props_t*)props)->typeMin;
}

int fmi3_xml_get_enum_variable_max(fmi3_xml_enum_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_enum_variable_props_t* props =
        (fmi3_xml_enum_variable_props_t*)(fmi3_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

const char* fmi3_xml_get_string_variable_start(fmi3_xml_string_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_variable_start_string_t* start = (fmi3_xml_variable_start_string_t*)(vv->typeBase);
        return start->start;
    }
    return 0;
}

int fmi3_xml_get_enum_variable_start(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_variable_start_integer_t* start = (fmi3_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

fmi3_boolean_t fmi3_xml_get_boolean_variable_start(fmi3_xml_bool_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_variable_start_integer_t* start = (fmi3_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

fmi3_xml_real_variable_t* fmi3_xml_get_variable_as_real(fmi3_xml_variable_t* v) {
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_real)  return (void*)v;
    return 0;
}

fmi3_xml_integer_variable_t* fmi3_xml_get_variable_as_integer(fmi3_xml_variable_t*v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_int)  return (void*)v;
    return 0;
}
fmi3_xml_enum_variable_t* fmi3_xml_get_variable_as_enum(fmi3_xml_variable_t* v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_enum)  return (void*)v;
    return 0;
}
fmi3_xml_string_variable_t* fmi3_xml_get_variable_as_string(fmi3_xml_variable_t* v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_str)  return (void*)v;
    return 0;
}
fmi3_xml_bool_variable_t* fmi3_xml_get_variable_as_boolean(fmi3_xml_variable_t* v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_bool)  return (void*)v;
    return 0;
}

int fmi3_xml_handle_Variable(fmi3_xml_parser_context_t *context, const char* data) {
    const int called_from_start_tag = !data;

     /* The real ID of the variable, such as 'RealVariable' */
    fmi3_xml_elm_enu_t elm_id = called_from_start_tag ? context->currentElemIdStartTag : context->currentElmID;

    if (called_from_start_tag) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_t* variable;
        fmi3_xml_variable_t dummyV;
        const char* description = 0;
        jm_named_ptr named, *pnamed;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context,2,100);
        unsigned int vr;

        if(!bufName || !bufDescr) return -1;

        /*   <xs:attribute name="valueReference" type="xs:unsignedInt" use="optional but required for FMI"> */
        if(fmi3_xml_set_attr_uint(context, elm_id, fmi_attr_id_valueReference, 1, &vr, 0)) return -1;

        if(
        /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
            fmi3_xml_set_attr_string(context, fmi3_xml_elmID_Variable, fmi_attr_id_name, 1, bufName) ||
        /* <xs:attribute name="description" type="xs:string"/> */
            fmi3_xml_set_attr_string(context, elm_id, fmi_attr_id_description, 0, bufDescr)
        ) return -1;

        if(context->skipOneVariableFlag) {
            jm_log_error(context->callbacks,module, "Ignoring variable with undefined vr '%s'", jm_vector_get_itemp(char)(bufName,0));
            return 0;
        }
        if(jm_vector_get_size(char)(bufDescr)) {
            description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr,0));
        }

        named.ptr = 0;
        named.name = 0;
        pnamed = jm_vector_push_back(jm_named_ptr)(&md->variablesByName, named);

        if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi3_xml_variable_t), dummyV.name - (char*)&dummyV, context->callbacks);
        variable = named.ptr;
        if( !pnamed || !variable ) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        variable->vr = vr;
        variable->description = description;
        variable->typeBase = 0;
        variable->originalIndex = jm_vector_get_size(jm_named_ptr)(&md->variablesByName) - 1;
        variable->derivativeOf = 0;
        variable->previous = 0;
        variable->aliasKind = fmi3_variable_is_not_alias;
        variable->reinit = 0;
        variable->canHandleMultipleSetPerTimeInstant = 1;

        {
            jm_name_ID_map_t causalityConventionMap[] = {
                {"local",fmi3_causality_enu_local},
                {"input",fmi3_causality_enu_input},
                {"output",fmi3_causality_enu_output},
                {"parameter",fmi3_causality_enu_parameter},
                {"calculatedParameter",fmi3_causality_enu_calculated_parameter},
                {"independent",fmi3_causality_enu_independent},
                {0,0}
            };
            jm_name_ID_map_t variabilityConventionMap[] = {
                {"continuous",fmi3_variability_enu_continuous},
                {"constant", fmi3_variability_enu_constant},
                {"fixed", fmi3_variability_enu_fixed},
                {"tunable", fmi3_variability_enu_tunable},
                {"discrete", fmi3_variability_enu_discrete},
                {0,0}
            };
            jm_name_ID_map_t initialConventionMap[] = {
                {"approx",fmi3_initial_enu_approx},
                {"calculated",fmi3_initial_enu_calculated},
                {"exact",fmi3_initial_enu_exact},
                {0,0}
            };
            unsigned int causality, variability, initial;
            fmi3_initial_enu_t defaultInitial;
            /* <xs:attribute name="causality" default="local"> */
            if(fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_causality,0,&causality,fmi3_causality_enu_local,causalityConventionMap))
                causality = fmi3_causality_enu_local;
            variable->causality = causality;
            /*  <xs:attribute name="variability" default="continuous"> */
            if(fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_variability,0,&variability,fmi3_variability_enu_continuous,variabilityConventionMap))
                variability = fmi3_variability_enu_continuous;

            if (!fmi3_is_valid_variability_causality(variability, causality)) {
                fmi3_variability_enu_t bad_variability = variability;
                variability = fmi3_get_default_valid_variability(causality);
                fmi3_xml_parse_error(context,
                    "Invalid combination of variability %s and causality %s for"
                    " variable '%s'. Setting variability to '%s'",
                    fmi3_variability_to_string(bad_variability),
                    fmi3_causality_to_string(causality),
                    variable->name,
                    fmi3_variability_to_string(variability));
            }
            variable->variability = variability;

            defaultInitial = fmi3_get_default_initial(variability, causality);

            /* <xs:attribute name="initial"> */
            if(fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_initial,0,&initial,defaultInitial,initialConventionMap))
                initial = defaultInitial;
            defaultInitial = fmi3_get_valid_initial(variability, causality, initial);
            if(defaultInitial != initial) {
                fmi3_xml_parse_error(context,
                    "Initial '%s' is not allowed for variability '%s' and "
                    "causality '%s'. Setting initial to '%s' for variable '%s'",
                    fmi3_initial_to_string(initial),
                    fmi3_variability_to_string(variability),
                    fmi3_causality_to_string(causality),
                    fmi3_initial_to_string(defaultInitial),
                    variable->name);
                initial = defaultInitial;
            }
            variable->initial = initial;
        }
        {
            unsigned int previous, multipleSet;
            if (
            /*   <xs:attribute name="previous" type="xs:unsignedInt"> */
                fmi3_xml_set_attr_uint(context, elm_id, fmi_attr_id_previous, 0, &previous, 0) ||
            /*   <xs:attribute name="canHandleMultipleSetPerTimeInstant" type="xs:boolean"> */
                fmi3_xml_set_attr_boolean(context, elm_id, fmi_attr_id_canHandleMultipleSetPerTimeInstant, 0, &multipleSet, 1)
            ) return -1;

                /* Store the index as a pointer since we cannot access the variables list yet (we are constructing it). */
            variable->previous = (void*)((char *)NULL + previous);
            variable->canHandleMultipleSetPerTimeInstant = (char)multipleSet;

            if (variable->variability != fmi3_causality_enu_input && !multipleSet) {
                fmi3_xml_parse_error(context, "Only variables with causality='input' can have canHandleMultipleSetPerTimeInstant=false");
                return -1;
            }
        }
    }
    else {
        if(context->skipOneVariableFlag) {
            context->skipOneVariableFlag = 0;
        }
        else {
            /* check that the type for the variable is set */
            fmi3_xml_model_description_t* md = context->modelDescription;
            fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
            if(!variable->typeBase) {
                jm_log_error(context->callbacks, module, "No variable type element for variable %s. Assuming Real.", variable->name);

                return fmi3_xml_handle_RealVariable(context, NULL);
            }
        }
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi3_xml_get_has_start(fmi3_xml_parser_context_t *context, fmi3_xml_variable_t* variable) {
    int hasStart = fmi3_xml_is_attr_defined(context, fmi_attr_id_start);
    if(!hasStart)  {
        if (variable->initial != (char)fmi3_initial_enu_calculated) {
            fmi3_xml_parse_error(context,
                    "Start attribute is required for this causality, variability and initial combination");
            hasStart = 1;
        }
    } else {
        /* If initial = calculated, it is not allowed to provide a start value. */
        if(variable->initial == (char)fmi3_initial_enu_calculated) {
            fmi3_xml_parse_error(context, "Start attribute is not allowed for variables with initial='calculated'");
            hasStart = 0;
        }
    }
    return hasStart;
}

static void fmi3_log_error_if_start_required(
    fmi3_xml_parser_context_t *context,
    fmi3_xml_variable_t *variable)
{
    if (variable->causality == fmi3_causality_enu_input) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for input variables",
                       variable->name);
    } else if (variable->causality == fmi3_causality_enu_parameter) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for parameter variables",
                       variable->name);
    } else if (variable->variability == fmi3_variability_enu_constant) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with constant variability",
                       variable->name);
    } else if (variable->initial == fmi3_initial_enu_exact) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with initial == \"exact\"",
                       variable->name);
    } else if (variable->initial == fmi3_initial_enu_approx) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with initial == \"approx\"",
                       variable->name);
    }
}

int fmi3_xml_handle_RealVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    fmi3_xml_handle_Variable(context, data);

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_type_base_t * declaredType = 0;
        fmi3_xml_real_type_props_t * type = 0;
        int hasStart;

        assert(!variable->typeBase);

        declaredType = fmi3_get_declared_type(context, fmi3_xml_elmID_Real, &td->defaultRealType.typeBase);

        if(!declaredType) return -1;

        {
            int hasUnit = fmi3_xml_is_attr_defined(context, fmi_attr_id_unit) ||
                    fmi3_xml_is_attr_defined(context, fmi_attr_id_displayUnit);
            int hasMin =  fmi3_xml_is_attr_defined(context, fmi_attr_id_min);
            int hasMax = fmi3_xml_is_attr_defined(context, fmi_attr_id_max);
            int hasNom = fmi3_xml_is_attr_defined(context, fmi_attr_id_nominal);
            int hasQuan = fmi3_xml_is_attr_defined(context, fmi_attr_id_quantity);
            int hasRelQ = fmi3_xml_is_attr_defined(context, fmi_attr_id_relativeQuantity);
            int hasUnb = fmi3_xml_is_attr_defined(context, fmi_attr_id_unbounded);


            if(hasUnit || hasMin || hasMax || hasNom || hasQuan || hasRelQ ||hasUnb) {
                fmi3_xml_real_type_props_t* props = 0;

                if(declaredType->structKind == fmi3_xml_type_struct_enu_typedef)
                    props = (fmi3_xml_real_type_props_t*)(declaredType->baseTypeStruct);
                else
                    props = (fmi3_xml_real_type_props_t* )declaredType;

                fmi3_xml_reserve_parse_buffer(context, 1, 0);
                fmi3_xml_reserve_parse_buffer(context, 2, 0);

                type = fmi3_xml_parse_real_type_properties(context, fmi3_xml_elmID_Real);

                if(!type) return -1;
                type->typeBase.baseTypeStruct = declaredType;
                if( !hasUnit) type->displayUnit = props->displayUnit;
                if( !hasMin)  type->typeMin = props->typeMin;
                if( !hasMax) type->typeMax = props->typeMax;
                if( !hasNom) type->typeNominal = props->typeNominal;
                if( !hasQuan) type->quantity = props->quantity;
                if( !hasRelQ) type->typeBase.isRelativeQuantity = type->typeBase.isRelativeQuantity;
                if( !hasUnb) type->typeBase.isUnbounded = type->typeBase.isUnbounded;
            }
            else
                type = (fmi3_xml_real_type_props_t*)declaredType;
        }
        variable->typeBase = &type->typeBase;

        hasStart = fmi3_xml_get_has_start(context, variable);

        if(hasStart) {
            fmi3_xml_variable_start_real_t * start = (fmi3_xml_variable_start_real_t*)fmi3_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi3_xml_variable_start_real_t));
            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:double"/> */
                    fmi3_xml_set_attr_double(context, fmi3_xml_elmID_Real, fmi_attr_id_start, 0, &start->start, 0)
                )
                    return -1;
            variable->typeBase = &start->typeBase;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }

        {
            /*   <xs:attribute name="derivative" type="xs:unsignedInt"> */
            unsigned int derivativeOf;
            unsigned int reinit;

            if(fmi3_xml_set_attr_uint(context, fmi3_xml_elmID_Real,
                fmi_attr_id_derivative, 0, &derivativeOf, 0)) return -1;
            /* TODO: consider: is it ok to read in an unsigned int to store in a size_t? */
            /* Store the index as a pointer since we cannot access the variables list yet (we are constructing it). */
            variable->derivativeOf = (void *)((char *)NULL + derivativeOf);

            /*   <xs:attribute name="reinit" type="xs:boolean" use="optional" default="false"> */
            if(fmi3_xml_set_attr_boolean(context, fmi3_xml_elmID_Real,
                fmi_attr_id_reinit, 0, &reinit, 0)) return -1;
            variable->reinit = (char)reinit;

            if (variable->variability != fmi3_variability_enu_continuous && reinit) {
                /* If reinit is true, this variable must be continuous. */
                fmi3_xml_parse_error(context, "The reinit attribute may only be set on continuous-time states.");
                return -1;
            }
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_IntegerVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    fmi3_xml_handle_Variable(context, data);

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi3_xml_variable_type_base_t * declaredType = 0;
        fmi3_xml_integer_type_props_t * type = 0;
        int hasStart;

        declaredType = fmi3_get_declared_type(context, fmi3_xml_elmID_Integer,&td->defaultIntegerType.typeBase) ;

        if(!declaredType) return -1;
        {
            int hasMin = fmi3_xml_is_attr_defined(context,fmi_attr_id_min);
            int hasMax = fmi3_xml_is_attr_defined(context,fmi_attr_id_max);
            int hasQuan =  fmi3_xml_is_attr_defined(context,fmi_attr_id_quantity);
        if( hasMin || hasMax || hasQuan) {
            fmi3_xml_integer_type_props_t* props = 0;

            if(declaredType->structKind != fmi3_xml_type_struct_enu_typedef)
                props = (fmi3_xml_integer_type_props_t*)declaredType;
            else
                props = (fmi3_xml_integer_type_props_t*)(declaredType->baseTypeStruct);
            assert(props->typeBase.structKind == fmi3_xml_type_struct_enu_props);
            fmi3_xml_reserve_parse_buffer(context, 1, 0);
            fmi3_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi3_xml_parse_integer_type_properties(context, fmi3_xml_elmID_Integer);
            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
            if(!hasMin) type->typeMin = props->typeMin;
            if(!hasMax) type->typeMax = props->typeMax;
            if(!hasQuan) type->quantity = props->quantity;
        }
        else
            type = (fmi3_xml_integer_type_props_t*)declaredType;
        }
        variable->typeBase = &type->typeBase;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            fmi3_xml_variable_start_integer_t * start = (fmi3_xml_variable_start_integer_t*)fmi3_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi3_xml_variable_start_integer_t));
            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:integer"/> */
                    fmi3_xml_set_attr_int(context, fmi3_xml_elmID_Integer, fmi_attr_id_start, 0, &start->start, 0)
                ) {
                    /* not sure how to peek a default here (and start is probably required attriute)*/
                    jm_log_error(context->callbacks, module, "Start value zero will be assumed.");
                    start->start = 0;
            }
            variable->typeBase = &start->typeBase;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_BooleanVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    fmi3_xml_handle_Variable(context, data);

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

        assert(!variable->typeBase);

        variable->typeBase = fmi3_get_declared_type(context, fmi3_xml_elmID_Boolean, &td->defaultBooleanType) ;

        if(!variable->typeBase) return -1;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            fmi3_xml_variable_start_integer_t * start = (fmi3_xml_variable_start_integer_t*)fmi3_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi3_xml_variable_start_integer_t ));
            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                  /*  <xs:attribute name="start" type="xs:boolean"/> */
                    fmi3_xml_set_attr_boolean(context, fmi3_xml_elmID_Boolean, fmi_attr_id_start, 0, (unsigned*)&start->start, 0)
                )
                    return -1;
            variable->typeBase = &start->typeBase;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_StringVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    fmi3_xml_handle_Variable(context, data);

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

        assert(!variable->typeBase);

        variable->typeBase = fmi3_get_declared_type(context, fmi3_xml_elmID_String,&td->defaultStringType) ;

        if(!variable->typeBase) return -1;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            jm_vector(char)* bufStartStr = fmi3_xml_reserve_parse_buffer(context,1, 100);
            size_t strlen;
            fmi3_xml_variable_start_string_t * start;
            if(
                 /*   <xs:attribute name="start" type="xs:string"/> */
                    fmi3_xml_set_attr_string(context, fmi3_xml_elmID_String, fmi_attr_id_start, 0, bufStartStr)
                )
                    return -1;
            strlen = jm_vector_get_size_char(bufStartStr);

            start = (fmi3_xml_variable_start_string_t*)fmi3_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi3_xml_variable_start_string_t) + strlen);

            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (strlen != 0) { /* No need to memcpy empty strings (gives assetion error) */
                memcpy(start->start, jm_vector_get_itemp_char(bufStartStr,0), strlen);
            }
            start->start[strlen] = 0;
            variable->typeBase = &start->typeBase;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi3_xml_enum_variable_props_t * fmi3_xml_parse_enum_properties(fmi3_xml_parser_context_t* context, fmi3_xml_enum_variable_props_t* declaredType) {

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_enum_variable_props_t * props = 0;
    fmi3_xml_elm_enu_t elmID = fmi3_xml_elmID_Enumeration;
    const char* quantity = 0;

    /*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
            jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context,3,100);

    props = (fmi3_xml_enum_variable_props_t*)fmi3_xml_alloc_variable_type_props(&md->typeDefinitions,
                    &md->typeDefinitions.defaultEnumType.base.typeBase, sizeof(fmi3_xml_enum_variable_props_t));

    if(!bufQuantity || !props ||
            /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity)
            )
        return 0;
    if(jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = (quantity == 0) ? declaredType->quantity: quantity;

    if(     /* <xs:attribute name="min" type="xs:int"/> */
            fmi3_xml_set_attr_int(context, elmID, fmi_attr_id_min, 0, &props->typeMin, declaredType->typeMin) ||
            /* <xs:attribute name="max" type="xs:int"/> */
            fmi3_xml_set_attr_int(context, elmID, fmi_attr_id_max, 0, &props->typeMax, declaredType->typeMax)
            ) return 0;
    return props;
}

int fmi3_xml_handle_EnumerationVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    fmi3_xml_handle_Variable(context, data);

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi3_xml_variable_type_base_t * declaredType = 0;
        fmi3_xml_enum_variable_props_t * type = 0;
        int hasStart;

        assert(!variable->typeBase);

        declaredType = fmi3_get_declared_type(context, fmi3_xml_elmID_Enumeration,&td->defaultEnumType.base.typeBase);

        if(!declaredType) return -1;

        if(
                fmi3_xml_is_attr_defined(context,fmi_attr_id_min) ||
                fmi3_xml_is_attr_defined(context,fmi_attr_id_max) ||
                fmi3_xml_is_attr_defined(context,fmi_attr_id_quantity)
                ) {
            fmi3_xml_enum_variable_props_t* props = 0;

            if(declaredType->structKind != fmi3_xml_type_struct_enu_typedef)
                props = (fmi3_xml_enum_variable_props_t*)declaredType;
            else
                props = (fmi3_xml_enum_variable_props_t*)declaredType->baseTypeStruct;
            assert(props->typeBase.structKind == fmi3_xml_type_struct_enu_props);
            fmi3_xml_reserve_parse_buffer(context, 1, 0);
            fmi3_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi3_xml_parse_enum_properties(context, props);
            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
        }
        else
            type = (fmi3_xml_enum_variable_props_t*)declaredType;

        variable->typeBase = &type->typeBase;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            fmi3_xml_variable_start_integer_t * start = (fmi3_xml_variable_start_integer_t*)fmi3_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi3_xml_variable_start_integer_t ));
            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:integer"/> */
                    fmi3_xml_set_attr_int(context, fmi3_xml_elmID_Enumeration, fmi_attr_id_start, 0, &start->start, 0)
                )
                start->start = type->typeMin;
            variable->typeBase = &start->typeBase;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

static int fmi3_xml_compare_variable_original_index (const void* first, const void* second) {
    size_t a = (*(fmi3_xml_variable_t**)first)->originalIndex;
    size_t b = (*(fmi3_xml_variable_t**)second)->originalIndex;
    if(a < b) return -1;
    if(a > b) return 1;
    return 0;
}

void fmi3_xml_eliminate_bad_alias(fmi3_xml_parser_context_t *context, size_t indexVR) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    jm_vector(jm_voidp)* varByVR = md->variablesByVR;
    fmi3_xml_variable_t* v = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);
    fmi3_value_reference_t vr = v->vr;
    fmi3_base_type_enu_t vt = fmi3_xml_get_variable_base_type(v);
    size_t i, n = jm_vector_get_size(jm_voidp)(varByVR);
    for(i = 0; i< n; i++) {
        jm_named_ptr key;
        size_t index;
        v = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
        if((v->vr != vr)||(vt != fmi3_xml_get_variable_base_type(v))) continue;
        jm_vector_remove_item_jm_voidp(varByVR,i);
        n--; i--;
        key.name = v->name;
        index = jm_vector_bsearch_index(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
        assert(index <= n);
        jm_vector_remove_item(jm_named_ptr)(&md->variablesByName,index);

        index = jm_vector_bsearch_index(jm_voidp)(md->variablesOrigOrder, (jm_voidp*)&v, fmi3_xml_compare_variable_original_index);
        assert(index <= n);

        jm_vector_remove_item(jm_voidp)(md->variablesOrigOrder,index);

        jm_log_error(context->callbacks, module,"Removing incorrect alias variable '%s'", v->name);
        md->callbacks->free(v);
    }
}

static int fmi3_xml_compare_vr_and_original_index (const void* first, const void* second) {
    int ret = fmi3_xml_compare_vr(first, second);
    if(ret != 0) return ret;

    {
        fmi3_xml_variable_t* a = *(fmi3_xml_variable_t**)first;
        fmi3_xml_variable_t* b = *(fmi3_xml_variable_t**)second;
        ret = a->causality - b->causality;
        if(ret != 0 ) return ret;
        ret = a->variability - b->variability;
        if(ret != 0) return ret;
        {
            size_t ai = a->originalIndex;
            size_t bi = b->originalIndex;
            if(ai > bi) return 1;
            if(ai < bi) return -1;
        }
    }

    return 0;
}

int fmi3_xml_handle_ModelVariables(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_log_verbose(context->callbacks, module,"Parsing XML element ModelVariables");
        /*  reset handles for the elements that are specific under ModelVariables */
        fmi3_xml_set_element_handle(context, "Real", FMI3_XML_ELM_ID(RealVariable));
        fmi3_xml_set_element_handle(context, "Integer", FMI3_XML_ELM_ID(IntegerVariable));
        fmi3_xml_set_element_handle(context, "Enumeration", FMI3_XML_ELM_ID(EnumerationVariable));
        fmi3_xml_set_element_handle(context, "String", FMI3_XML_ELM_ID(StringVariable));
        fmi3_xml_set_element_handle(context, "Boolean", FMI3_XML_ELM_ID(BooleanVariable));
        fmi3_xml_set_element_handle(context, "Tool", FMI3_XML_ELM_ID(VariableTool));
    }
    else {
         /* postprocess variable list */

        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(jm_voidp)* varByVR;
        size_t i, numvar;

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);

        /* store the list of vars in original order */
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesOrigOrder = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesOrigOrder) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesOrigOrder, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                }
            }
        }

        /* look up actual pointers for the derivativeOf and previous fields in variablesOrigOrder */
        {
            size_t size = jm_vector_get_size(jm_voidp)(md->variablesOrigOrder);
            size_t k;
            for (k=0; k < size; k++) {
                fmi3_xml_variable_t *variable = jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, k);

                if (variable->derivativeOf) {
                    /* Retrieve index that was stored as a pointer */
                    size_t index = (char*)variable->derivativeOf - (char *)NULL;
                    /* Convert from one- to zero-based indexing */
                    index--;
                    /* Ok to just check upper bound since index is unsigned. */
                    if (index >= size) {
                        fmi3_xml_parse_error(context, "The 'derivative' attribute must have a value between 1 and the number of model variables.");
                        /* todo: free allocated memory? */
                        return -1;
                    }
                    variable->derivativeOf = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, index);
                }
                if (variable->previous) {
                    /* retrieve index that was stored as a pointer */
                    size_t index = (char*)variable->previous - (char *)NULL;
                    /* Convert from one- to zero-based indexing */
                    index--;
                    /* Ok to just check upper bound since index is unsigned. */
                    if (index >= size) {
                        fmi3_xml_parse_error(context, "The 'previous' attribute must have a value between 1 and the number of model variables.");
                        /* todo: free allocated memory? */
                        return -1;
                    }
                    variable->previous = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, index);
                }
            }
        }

        /* sort the variables by names */
        jm_vector_qsort(jm_named_ptr)(&md->variablesByName,jm_compare_named);

        /* create VR index */
        md->status = fmi3_xml_model_description_enu_ok;
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesByVR = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesByVR) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesByVR, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                }
            }
        }

        md->status = fmi3_xml_model_description_enu_empty;
        if(!md->variablesByVR || !md->variablesOrigOrder) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        varByVR = md->variablesByVR;
        jm_vector_qsort(jm_voidp)(varByVR, fmi3_xml_compare_vr_and_original_index);

        numvar = jm_vector_get_size(jm_voidp)(varByVR);

        if(numvar > 1){
            int foundBadAlias;

            jm_log_verbose(context->callbacks, module,"Building alias index");
            do {
                fmi3_xml_variable_t* a = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, 0);
                int startPresent = fmi3_xml_get_variable_has_start(a);
                int isConstant = (fmi3_xml_get_variability(a) == fmi3_variability_enu_constant);
                a->aliasKind = fmi3_variable_is_not_alias;

                foundBadAlias = 0;

                for(i = 1; i< numvar; i++) {
                    fmi3_xml_variable_t* b = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
                    int b_startPresent = fmi3_xml_get_variable_has_start(b);
                    int b_isConstant = (fmi3_xml_get_variability(b) == fmi3_variability_enu_constant);
                    if((fmi3_xml_get_variable_base_type(a) == fmi3_xml_get_variable_base_type(b))
                            && (a->vr == b->vr)) {
                            /* an alias */
                            jm_log_verbose(context->callbacks,module,"Variables %s and %s reference the same vr %u. Marking '%s' as alias.",
                                                  a->name, b->name, b->vr, b->name);
                            b->aliasKind = fmi3_variable_is_alias;

                            if(!isConstant != !b_isConstant) {
                                jm_log_error(context->callbacks,module,
                                "Only constants can be aliases with constants (variables: %s and %s)",
                                    a->name, b->name);
                                fmi3_xml_eliminate_bad_alias(context,i);
                                numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                foundBadAlias = 1;
                                break;
                            } else if (isConstant) {
                                if (!startPresent  || !b_startPresent) {
                                    jm_log_error(context->callbacks,module,
                                        "Constants in alias set must all have start attributes (variables: %s and %s)",
                                        a->name, b->name);
                                    fmi3_xml_eliminate_bad_alias(context,i);
                                    numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                    foundBadAlias = 1;
                                    break;
                                }
                                /* TODO: Check that both start values are the same */
                            } else if(startPresent && b_startPresent) {
                                jm_log_error(context->callbacks,module,
                                    "Only one variable among non constant aliases is allowed to have start attribute (variables: %s and %s) %d, %d, const enum value: %d",
                                        a->name, b->name, fmi3_xml_get_variability(a), fmi3_xml_get_variability(b), fmi3_variability_enu_constant);
                                fmi3_xml_eliminate_bad_alias(context,i);
                                numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                foundBadAlias = 1;
                                break;
                            }
                            if(b_startPresent) {
                                startPresent = 1;
                                a = b;
                            }
                    }
                    else {
                        b->aliasKind = fmi3_variable_is_not_alias;
                        startPresent = b_startPresent;
                        isConstant = b_isConstant;
                        a = b;
                    }
                }
            } while(foundBadAlias);
        }

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);

        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}
