// Exercise 1 for USC CS499, March 2021
// Author: Christopher Tuttle
// 
// This file contains a few classes to show the interactions between RPC
// clients, various data layouts, and physical servers (server
// tasks/instances). These are the components:
//   - There is a service of N tasks, numbered 0..N-1, with each task running
//     on a different node/physical machine/vm. Perhaps it is a key-value
//     store, backed by many physical data files.
//   - The data is sharded -- partitioned -- into S shards, and these shards
//     are distributed over the service. For better reliability and/or
//     performance, each shard is assigned to R (=3) different server tasks.
//     Each replica of a shard has exactly the same data.
//   - The server tasks may crash or restart, but they report their health
//     to clients. When a server is healthy, all its data shards are ready.
//   - On the client side, a BackendManager class is responsible for
//     determining which server task to send an RPC to.
//

#ifndef DAY1_BACKEND_H__
#define DAY1_BACKEND_H__

#include<stdlib.h>
#include<iostream>
#include<set>
#include<string>
#include<vector>

#include "shard-assignment.h"

// Represents one backend connection of an imaginary N-way sharded service.
//
// In a full example, this class might also include the RPC channels, stats,
// and the generated methods to invoke RPCs.
class Stub {
 public:
  explicit Stub(int index) : index_(index), healthy_(true) {}

  bool is_healthy() const { return healthy_; }
  void set_healthy(bool h) { healthy_ = h; }

  // In [0..N).
  int index() const { return index_; }

 private:
  Stub(const Stub&) = delete;
  Stub& operator=(const Stub&) = delete;

  int index_;
  bool healthy_;
};

// Manages Stubs, representing backend connections, and gives them out
// according to the Stub's health and the assignment of shards to servers.
class BackendManager {
 public:
  // These express what algorithm (policy) the BackendManager should use
  // when selecting a stub. These represent the different reasons shards
  // might be replicated in a cluster.
  enum class ReplicationPolicy {
    // When shards are replicated kForAvailability, a Stub should be
    // returned as long as any of the backends hosting the shard are
    // up.
    //
    // Notably: We assume that *any* of the replicas can handle the
    // additional request load if a replica goes away.
    // 
    // This policy would be used for serving data off of disk on several
    // hosts, so if any of them went down (due to crashes, upgrades, etc.),
    // the data would still be available.
    kForAvailability,

    // When shards are replicated kForLoad, requests should be dropped
    // proportionally to the number of unhealthy backends.
    //
    // For example, consider a workload that is CPU-bound and uses up
    // the full CPU allocation on every task that's running (e.g. running
    // image recognition). If there were 2 replicas per shard, they would
    // each be running at 100% capacity. If one were to crash, the
    // kForAvailability policy would push all of the load to the healthy
    // replica, which would overload it. Instead, the kForLoad policy
    // returns 50% errors (a nullptr instead of a Stub to the healthy
    // replica), thus avoiding sending extra RPCs to the healthy replica.
    //
    // [Note: There's a tradeoff here: One could also implement
    // load-shedding at the replicas so the healthy one could reject
    // requests once it is full. As with all things, there are + and -.]
    kForLoad,
  };

  // Creates a new BackendManager with the given assignment and policy.
  // REQUIRES: assignment is non-NULL and outlives BackendManager.
  BackendManager(const ShardAssignment* assignment,
                 ReplicationPolicy policy);

  // Returns a Stub pointing to a server that hosts |shard|, or nullptr.
  //
  // Ownership of the returned Stub remains with BackendManager.
  //
  // IMPLEMENTING THIS FUNCTION IS THE PROJECT FOR THE DAY.
  Stub* GetStubForShard(int shard);

  // Provides accessors to the health of each Stub.
  // 
  // These are for the main/Driver class.
  bool GetStubHealth(int stub) const { return stubs_[stub]->is_healthy(); }
  void SetStubHealth(int stub, bool healthy) {
    stubs_[stub]->set_healthy(healthy);
  }

 private:
  BackendManager(const BackendManager&) = delete;
  BackendManager& operator=(const BackendManager&) = delete;

  const ShardAssignment* assignment_;
  ReplicationPolicy policy_;

  // Created and owned by this class. In a real situation, these would come
  // from the RPC system.
  std::vector<Stub*> stubs_;
};

#endif  // DAY1_BACKEND_H__
