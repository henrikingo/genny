#ifndef HEADER_338F3734_B052_443C_A358_60215BA9D5A0_INCLUDED
#define HEADER_338F3734_B052_443C_A358_60215BA9D5A0_INCLUDED

#include <string_view>

#include <mongocxx/database.hpp>
#include <mongocxx/pool.hpp>

#include <gennylib/Actor.hpp>
#include <gennylib/ExecutionStrategy.hpp>
#include <gennylib/PhaseLoop.hpp>
#include <gennylib/context.hpp>
#include <value_generators/DefaultRandom.hpp>
#include <value_generators/value_generators.hpp>

#include <cast_core/config/RunCommandConfig.hpp>

namespace genny::actor {

/**
 * CrudActor is an actor that performs CRUD operations on a collection and has the ability to start
 * and commit transactions. This actor aims to support the operations in the mongocxx-driver
 * collections API. The actor supports the usage of a list of operations for a single phase.
 *
 * Example:
 *
 * ```yaml
 * Actors:
 * - Name: BulkWriteInTransaction
 *   Type: CrudActor
 *   Database:
 *   Type: CrudActor
 *   Database: testdb
 *   ExecutionStrategy:
 *     ThrowOnFailure: true
 *   Phases:
 *   - Repeat: 1
 *     Collection: test
 *     Operations:
 *     - OperationName: startTransaction
 *       OperationCommand:
 *         Options:
 *           WriteConcern:
 *             Level: majority
 *             Journal: true
 *           ReadConcern:
 *             Level: snapshot
 *           ReadPreference:
 *             ReadMode: primaryPreferred
 *             MaxStalenessSeconds: 1000
 *     - OperationName: bulkWrite
 *       OperationCommand:
 *         WriteOperations:
 *         - WriteCommand: insertOne
 *           Document: { a: 1 }
 *         - WriteCommand: updateOne
 *           Filter: { a: 1 }
 *           Update: { $set: { a: 5 } }
 *         Options:
 *           Ordered: true
 *           WriteConcern:
 *             Level: majority
 *         OnSession: true
 *     - OperationName: commitTransaction
 *   - Repeat: 1
 *     Collection: test
 *     Operation:
 *       OperationName: drop
 */
class CrudActor : public Actor {

public:
    class Operation;

public:
    explicit CrudActor(ActorContext& context);
    ~CrudActor() = default;

    static std::string_view defaultName() {
        return "CrudActor";
    }

    void run() override;

private:
    mongocxx::pool::entry _client;
    genny::DefaultRandom _rng;

    struct PhaseConfig;
    PhaseLoop<PhaseConfig> _loop;
};

}  // namespace genny::actor

#endif  // HEADER_338F3734_B052_443C_A358_60215BA9D5A0_INCLUDED
