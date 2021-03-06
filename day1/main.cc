// Exercise 1 for USC CS499, March 2021
// Author: Christopher Tuttle
// 
// This file simulates a real application with an RPC system. It uses the
// BackendManager and ShardAssignment to repeatedly request RPC Stubs,
// and it counts basic stats on the number of successful requests and
// how the resulting stubs are distributed over the backend hosts.

#include<stdlib.h>
#include<iostream>
#include<set>
#include<string>
#include<vector>

#include "backend.h"
#include "shard-assignment.h"

namespace {

void RunQueriesAndOutputResults(const ShardAssignment& assignment,
                                BackendManager* manager) {
  const int kQueriesPerShard = 5;

  const int num_tasks = assignment.num_tasks();
  const int num_shards = assignment.num_shards();

  // Run simulation of requesting Stubs for every shard, and tally successes.
  std::vector<int> queries_per_task(num_tasks, 0);
  int attempts = 0;
  int successes = 0;
  for (int i = 0; i < num_shards; ++i) {
    for (int j = 0; j < kQueriesPerShard; ++j) {
      ++attempts;
      Stub* stub = manager->GetStubForShard(i);
      if (stub != nullptr) {
        // assert stub->is_healthy();
        queries_per_task[stub->index()] += 1;
        ++successes;
      }
    }
  }

  // Dump some stats to the terminal.
  std::cout
      << "----------------------------------------------------" << std::endl
      << "Test with tasks=" << num_tasks
      << " shards=" << num_shards
      << " replicas=" << assignment.num_replicas() << ":" << std::endl
      << "attempts=" << attempts << " successes=" << successes << std::endl;

  std::cout << "Per-shard request counts (-X- indicates unhealthy):\n";
  int max_queries = -1;
  int max_index = -1;
  int min_queries = attempts + 1;
  int min_index = -1;
  for (int i = 0; i < queries_per_task.size(); ++i) {
    if (!manager->GetStubHealth(i)) {
      // Don't include unhealthy backends in min/max query lists.
      std::cout << "-X-, ";
      continue;
    }
    const int num = queries_per_task[i];
    std::cout << num << ", ";

    if (num > max_queries) {
      max_queries = num;
      max_index = i;
    }
    if (num < min_queries) {
      min_queries = num;
      min_index = i;
    }
  }
  std::cout << std::endl << std::endl
            << "max task=" << max_index << " with #queries=" << max_queries
            << std::endl
            << "min task=" << min_index << " with #queries=" << min_queries
            << std::endl;
}

// Runs the simulation with the given layout, policy, and task health.
// Prints out the number of successes / failures / etc. and per-task
// counts to the terminal.
void Run(int num_tasks, int num_shards, int num_replicas,
         ShardAssignment::Algorithm algorithm,
         BackendManager::ReplicationPolicy policy,
         const std::set<int>& unhealthy_tasks) {
  ShardAssignment assignment(num_tasks, num_shards, num_replicas, algorithm);
  BackendManager manager(&assignment, policy);

  for (const auto task : unhealthy_tasks) {
    manager.SetStubHealth(task, false);
  }

  RunQueriesAndOutputResults(assignment, &manager);
}

}  // anonymous namespace


int main(int argc, char **argv) {
  using Algorithm = ShardAssignment::Algorithm;
  using ReplicationPolicy = BackendManager::ReplicationPolicy;

  // This is a simple, one shard per machine example.
  Run(10 /* tasks */, 10 /* shards */, 1 /* replicas */,
      Algorithm::kSequential,
      ReplicationPolicy::kForAvailability,
      {} /* no unhealthy tasks */);

  // Same example, with tasks 2 and 4 made unhealthy.
  // Note that there should be failures ("successes" < "attempts").
  Run(10, 10, 1,
      Algorithm::kSequential,
      ReplicationPolicy::kForAvailability,
      {2, 4});

  // Again, this time with shard replicas. All rpcs should be successful now.
  Run(10, 10, 2,
      Algorithm::kSequential,
      ReplicationPolicy::kForAvailability,
      {2, 4});

  // These 10 shards (r=2, n=5) are spread one each over 10 tasks.
  // With one unhealthy task, there should be some failures, even though
  // there are replicas, because the replication is kForLoad.
  Run(10, 5, 2,
      Algorithm::kRoundRobin,
      ReplicationPolicy::kForLoad,
      {2});


  // A more practically-sized example for shards in a cluster.
  Run(1024 /* tasks */, 8000 /* shards */, 3 /* replicas */,
      Algorithm::kSequential,
      ReplicationPolicy::kForAvailability,
      {2, 4});

  // What happens to the query balance when there are multiple failed
  // tasks together?
  Run(1024 /* tasks */, 8000 /* shards */, 3 /* replicas */,
      Algorithm::kSequential,
      ReplicationPolicy::kForAvailability,
      {2, 3, 4});

  // And some bigger examples for replicating for for load.
  // First, one shard per host, r=1.
  Run(1024 /* tasks */, 1024 /* shards */, 1 /* replicas */,
      Algorithm::kRoundRobin,
      ReplicationPolicy::kForLoad,
      {2, 3, 4});

  // And now one shard per host, r=4.
  Run(1024 /* tasks */, 256 /* shards */, 4 /* replicas */,
      Algorithm::kRoundRobin,
      ReplicationPolicy::kForLoad,
      {2, 4, 6});

  return 0;
}
