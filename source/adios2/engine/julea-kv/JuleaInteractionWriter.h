/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * JULEA engine using the JULEA storage framework to handle lower I/O.
 *
 *  Created on: Aug 01, 2019
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */

#ifndef ADIOS2_ENGINE_JULEAINTERACTION_H_
#define ADIOS2_ENGINE_JULEAINTERACTION_H_

#include "JuleaKVWriter.h"
#include "JuleaMetadata.h"

namespace adios2
{
namespace core
{
namespace engine
{

/* Variable Functions */
template <class T>
void PutVariableDataToJulea(Variable<T> &variable, const T *data,
                            const std::string nameSpace, size_t currentStep);

template <class T>
void PutVariableMetadataBSONToJulea(Variable<T> &variable, bson_t *bsonMetadata,
                                    const std::string nameSpace,
                                    size_t currentStep, size_t blockID,
                                    bool isNameWritten);

// template <class T>
// void PutVariableMetadataToJulea(Variable<T> &variable,
//                                 JuleaKVWriter::Metadata<T> &md,
//                                 const std::string nameSpace, size_t currentStep,
//                                 size_t blockID, bool isNameWritten);
template <class T>
void PutVariableMetadataToJulea(Variable<T> &variable,
                                gpointer &md, guint32 valueLen,
                                const std::string nameSpace, size_t currentStep,
                                size_t blockID, bool isNameWritten);

// template <class T>
// void WriteVarMetadataToJuleaKV(Variable<T> &variable, const std::string
// nameSpace, size_t currStep);

/* Attribute Functions */
template <class T>
void PutAttributeDataToJulea(Attribute<T> &attribute,
                             const std::string nameSpace);

template <class T>
void PutAttributeMetadataToJulea(Attribute<T> &attribute, bson_t *bsonMetadata,
                                 const std::string nameSpace);

template <class T>
void PutAttributeDataToJuleaSmall(Attribute<T> &attribute, const T *data,
                                  const std::string nameSpace);
template <class T>
void PutAttributeMetadataToJuleaSmall(Attribute<T> &attribute,
                                      bson_t *bsonMetadata,
                                      const std::string nameSpace);
// extern template void WriteVarMetadataToJuleaKV(Variable<T> &variable, const std::string nameSpace, size_t currStep);\

#define declare_template_instantiation(T)                                      \
    extern template void PutVariableDataToJulea(                               \
        Variable<T> &variable, const T *data, const std::string nameSpace,     \
        size_t currentStep);                                                   \
    extern template void PutVariableMetadataBSONToJulea(                       \
        Variable<T> &variable, bson_t *bsonMetadata,                           \
        const std::string nameSpace, size_t currentStep, size_t blockID,       \
        bool isNameWritten);                                                   \
    extern template void PutVariableMetadataToJulea(                           \
        Variable<T> &variable,gpointer &md, guint32 valueLen,                 \
        const std::string nameSpace, size_t currentStep, size_t blockID,       \
        bool isNameWritten);                                                   \
                                                                               \
    extern template void PutAttributeDataToJulea(Attribute<T> &attribute,      \
                                                 const std::string nameSpace); \
    extern template void PutAttributeDataToJuleaSmall(                         \
        Attribute<T> &attribute, const T *data, const std::string nameSpace);  \
    extern template void PutAttributeMetadataToJulea(                          \
        Attribute<T> &attribute, bson_t *bsonMetadata,                         \
        const std::string nameSpace);                                          \
    extern template void PutAttributeMetadataToJuleaSmall(                     \
        Attribute<T> &attribute, bson_t *bsonMetadata,                         \
        const std::string nameSpace);                                          \
    ADIOS2_FOREACH_STDTYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif /* ADIOS2_ENGINE_JULEAINTERACTION_H_ */
