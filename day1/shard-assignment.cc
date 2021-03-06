// Exercise 1 for USC CS499, March 2021
// Author: Christopher Tuttle
// 
// Implemention of ShardAssignment.

#include "shard-assignment.h"

#include<stdlib.h>
#include<iostream>
#include<set>
#include<string>
#include<vector>

ShardAssignment::ShardAssignment(int num_tasks, int num_shards, int num_replicas,
                                 Algorithm algorithm)
  : num_tasks_(num_tasks),
    num_shards_(num_shards),
    num_replicas_(num_replicas) {
  server_to_shard_.resize(num_tasks_);
  shard_to_server_.resize(num_shards_);
  switch (algorithm) {
    case Algorithm::kSequential: AssignSequentially(); break;
    case Algorithm::kRoundRobin: AssignRoundRobin(); break;
  }
}

void ShardAssignment::AssignSequentially() {
  for (int shard = 0; shard < num_shards_; ++shard) {
    // Assign the primary replica for shard i on task i, and the
    // additional replicas on subsequent tasks.
    for (int replica = 0; replica < num_replicas_; ++replica) {
      const int task = (shard + replica) % num_tasks_;
      Assign(shard, task);
    }
  }
}

void ShardAssignment::AssignRoundRobin() {
  int target_task = 0;  // This is what gets round-robined.
  for (int shard = 0; shard < num_shards_; ++shard) {
    for (int replica = 0; replica < num_replicas_; ++replica) {
      Assign(shard, target_task);
      target_task = (target_task + 1) % num_tasks_;
    }
  }
}

void ShardAssignment::Assign(int shard, int task) {
  server_to_shard_[task].insert(shard);
  shard_to_server_[shard].insert(task);
}
