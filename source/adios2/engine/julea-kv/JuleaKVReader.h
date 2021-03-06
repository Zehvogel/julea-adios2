/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * JULEA engine using the JULEA storage framework to handle lower I/O.
 *
 *  Created on: Nov 14, 2018
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */

#ifndef ADIOS2_ENGINE_JULEAREADER_H_
#define ADIOS2_ENGINE_JULEAREADER_H_

#include "JuleaMetadata.h"

#include "adios2/core/ADIOS.h"
#include "adios2/core/Engine.h"
#include "adios2/helper/adiosFunctions.h"
#include "adios2/toolkit/format/bp/BPBase.h"

#include "adios2/toolkit/format/bp/bp3/BP3Serializer.h"
#include "adios2/toolkit/format/buffer/heap/BufferSTL.h"
#include "adios2/toolkit/transportman/TransportMan.h" //transport::TransportsMan

#include <complex.h>
#include <glib.h>
#include <julea.h>

namespace adios2
{
namespace core
{
namespace engine
{

/** used for Variables and Attributes, name, type, type-index */
using DataMap =
    std::unordered_map<std::string, std::pair<std::string, unsigned int>>;

class JuleaKVReader : public Engine
{
public:
    /**
     * Constructor for single BP capsule engine, writes in BP format into a
     * single
     * heap capsule
     * @param name unique name given to the engine
     * @param accessMode
     * @param mpiComm
     * @param method
     * @param debugMode
     * @param hostLanguage
     */
    JuleaKVReader(IO &adios, const std::string &name, const Mode mode,
                  helper::Comm comm);

    virtual ~JuleaKVReader();
    // StepStatus BeginStep(StepMode mode = StepMode::NextAvailable,
    //                      const float timeoutSeconds = -1.0) final;
    StepStatus BeginStep(StepMode mode = StepMode::Read,
                         const float timeoutSeconds = -1.0) final;
    size_t CurrentStep() const final;
    void EndStep() final;
    void PerformGets() final;

    /** Step related metadata for a variable broken down to put into JULEA  */
    struct CStepMetadata
    {
        bool isConstantDims;
        size_t typeSize = 0;
        char *type = nullptr;
        size_t shapeSize = 0;
        size_t *shape = nullptr;
        size_t startSize = 0;
        size_t *start = nullptr;
        size_t countSize = 0;
        size_t *count = nullptr;

        size_t numberSteps = 0;
        size_t *blocks = nullptr;
        // int type;
    };

    /** Step related metadata for a variable:  */
    struct StepMetadata
    {
        Dims shape;
        Dims start;
        Dims count;
        std::string type;
        size_t *blocks = nullptr;
        size_t numberSteps = 0;
        // int type = 0;
        bool isConstantDims = true;
    };

    /** Operators metadata info */
    struct Operation
    {
        /** reference to object derived from Operator class,
         *  needs a pointer to enable assignment operator (C++ class) */
        core::Operator *Op;
        /** Variable specific parameters */
        Params Parameters;
        /** resulting information from executing Operation (e.g. buffer size) */
        Params Info;
    };

    template <class T>
    struct Metadata
    {
        // TODO: needed?
        // std::map<size_t, std::vector<helper::SubStreamBoxInfo>>
        // StepBlockSubStreamsInfo;
        // struct helper::BlockDivisionInfo SubBlockInfo;
        // SelectionType Selection = SelectionType::BoundingBox;

        Dims Shape;
        Dims Start;
        Dims Count;
        Dims MemoryStart;
        Dims MemoryCount;

        // std::vector<core::IO::Operation> Operations;
        std::vector<core::VariableBase::Operation> Operations;
        // std::vector<T> Values;
        // std::vector<T> MinMaxs; // sub-block level min-max

        // size_t Step = 0; // TODO: currentStep? variable has no Step itself
        size_t StepsStart = 0;
        size_t StepsCount = 0;
        size_t BlockID = 0;

        size_t CurrentStep = 0; // Julea Engine
        size_t BlockNumber = 0; // Julea Engine

        // T *Data = nullptr;
        T Min = T();
        T Max = T();
        // T Value = T();   //TODO: not set in variable?!

        // int WriterID = 0; //TODO: what exactly is this and when used?

        /** Global array was written as Joined array, so read accordingly */
        bool IsReadAsJoined = false;

        /** Global array was written as Local value, so read accordingly */
        bool IsReadAsLocalValue = false;

        /** For read mode, false: streaming */
        bool IsRandomAccess = true;

        /** is single value or array */
        bool IsValue = false;
        /** if reader and writer have different ordering (column vs row major)
         */
        bool IsReverseDimensions = false;
    };

private:
    // JuleaInfo *m_JuleaInfo;
    JSemantics *m_JuleaSemantics;
    int m_Verbosity = 5; // TODO: changed to 5 for debugging
    int m_ReaderRank;    // my rank in the readers' comm

    // EndStep must call PerformGets if necessary
    bool m_NeedPerformGets = false;

    bool m_CollectiveMetadata = true;

    // step info should be received from the writer side in BeginStep()
    size_t m_CurrentStep = 0; // starts at 0

    size_t m_CurrentBlockID = 0; // starts at 0

    bool m_FirstStep = true;

    /** Parameter to flush transports at every number of steps, to be used at
     * EndStep */
    size_t m_FlushStepsCount = 1;

    /** manages all communication tasks in aggregation */
    aggregator::MPIChain m_Aggregator;

    /** tracks Put and Get variables in deferred mode */
    std::set<std::string> m_DeferredVariables;

    /** tracks all variables written (not BP, new for JULEA) */
    std::set<std::string> m_WrittenVariableNames;

    /** tracks the overall size of deferred variables */
    size_t m_DeferredVariablesDataSize = 0;

    /** statistics verbosity, only 0 is supported */
    unsigned int m_StatsLevel = 0;

    /** contains data buffer for this rank */
    format::BufferSTL m_Data;

    /** contains collective metadata buffer, only used by rank 0 */
    format::BufferSTL m_Metadata;

    // HELP! how do I actually get the compiler to accept MetadataSet as a type?
    /** contains bp1 format metadata indices*/ // DESIGN: needed?
    format::BP3Base::MetadataSet m_MetadataSet;

    // format::BP3Deserializer m_BP3Deserializer;  //HELP! is this really a good
    // idea?

    // DESIGN
    /** Manage BP data files Transports from IO AddTransport */
    // transportman::TransportMan m_FileDataManager; //FIXME: compiler?!

    /** Manages the optional collective metadata files */
    // transportman::TransportMan m_FileMetadataManager; FIXME: compiler?!

    void Init() final; ///< called from constructor, gets the selected Skeleton
                       /// transport method from settings

    /** Parses parameters from IO SetParameters */
    void InitParameters() final;
    /** Parses transports and parameters from IO AddTransport */
    void InitTransports() final;

    // template <class T>
    void InitVariables(); // needs to be final? HELP
    void InitAttributes();

// #define declare_type(T)                                                        \
//     void DoGetSync(Variable<T> &, T *) final;                                  \
//     void DoGetDeferred(Variable<T> &, T *) final;
//     ADIOS2_FOREACH_TYPE_1ARG(declare_type)
// #undef declare_type
#define declare_type(T)                                                        \
    void DoGetSync(Variable<T> &, T *) final;                                  \
    void DoGetDeferred(Variable<T> &, T *) final;
    ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type

    template <class T>
    void GetSyncCommon(Variable<T> &variable, T *data);

    template <class T>
    void GetDeferredCommon(Variable<T> &variable, T *data);

    // template <class T>
    // void ReadVariableBlocks(Variable<T> &variable); //TODO:needed?

    void DoClose(const int transportIndex = -1);

    // void ReadData();

    // void AggregateReadData();

    /**
     * DESIGN: is this function needed here? is there something one would want
     * to do different with a variable coming from JULEA?
     *
     * Sets read block information from the available metadata information
     * @param variable
     * @param blockInfo
     */
    template <class T>
    void SetVariableBlockInfo(core::Variable<T> &variable,
                              typename core::Variable<T>::Info &blockInfo);

    template <class T>
    std::map<size_t, std::vector<typename core::Variable<T>::Info>>
    DoAllStepsBlocksInfo(const core::Variable<T> &variable) const;

    template <class T>
    std::vector<std::vector<typename core::Variable<T>::Info>>
    DoAllRelativeStepsBlocksInfo(const core::Variable<T> &variable) const;

    template <class T>
    std::vector<typename core::Variable<T>::Info>
    DoBlocksInfo(const core::Variable<T> &variable, const size_t step) const;

#define declare_type(T)                                                        \
    std::map<size_t, std::vector<typename Variable<T>::Info>>                  \
    DoAllStepsBlocksInfo(const Variable<T> &variable) const final;             \
                                                                               \
    std::vector<std::vector<typename Variable<T>::Info>>                       \
    DoAllRelativeStepsBlocksInfo(const Variable<T> &) const final;             \
                                                                               \
    std::vector<typename Variable<T>::Info> DoBlocksInfo(                      \
        const Variable<T> &variable, const size_t step) const final;

    ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type
    // std::vector<typename Variable<T>::Info> DoBlocksInfo(                      \
    //     const Variable<T> &variable, const size_t step)  final;

    //     #define declare_type(T)                                                        \
//     std::map<size_t, std::vector<typename Variable<T>::Info>>                  \
//     DoAllStepsBlocksInfo(const Variable<T> &variable) const final;             \
//                                                                                \
//     std::vector<typename Variable<T>::Info> DoBlocksInfo(                      \
//         const Variable<T> &variable, const size_t step) const final;

    //     ADIOS2_FOREACH_TYPE_1ARG(declare_type)
    // #undef declare_type
};

} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif /* ADIOS2_ENGINE_JULEAREADER_H_ */
