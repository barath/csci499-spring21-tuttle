// Exercise 1 for USC CS499, March 2021
// Author: Christopher Tuttle
// 
// This file contains ShardAssignment, representing the mapping of shards
// to tasks (servers) in the example system.

#ifndef DAY1_SHARD_ASSIGNMENT_H__
#define DAY1_SHARD_ASSIGNMENT_H__

#include<set>
#include<vector>

// Represents the assignment of data shards to server tasks.
class ShardAssignment {
 public:
  // These are a few different ways that shards can be assigned to tasks.
  enum class Algorithm {
    // Assigns the primary replica for shard i to task i,
    // and its subsequent replicas on tasks i+1, i+2, ... (mod num_tasks).
    kSequential,

    // Assigns each shard and replica in a round-robin fashion.
    // Similar to kSequential, but can produce worse layouts (included here
    // as an illustration for the curious).
    kRoundRobin,
  };

  // Creates a new shard assignment with num_tasks hosts (e.g physical
  // machines), num_shards shards, each with num_replicas replicas.
  // 
  // For example, num_tasks=1024, num_shards=8000, num_replicas=3 might
  // be a good choice for serving disk-hosted data that needs replication
  // for redudancy.
  //
  // For a cluster that loaded all the data in memory (and filled all the
  // hosts), num_tasks=1024, num_shards=1024, num_replicas=1 would be
  // appropriate (or perhaps num_shards=1000 to leave some "holes" for
  // rolling swaps).
  ShardAssignment(int num_tasks, int num_shards, int num_replicas,
                  Algorithm algorithm);

  // Fills |out| with the shard numbers that are hosted by task |index|.
  void GetShardsForServer(int index, std::set<int>* out) const {
    *out = server_to_shard_[index];
  }

  // Fills |out| with the tasks that host shard |shard|.
  void GetServersForShard(int shard, std::set<int>* out) const {
    *out = shard_to_server_[shard];
  }

  int num_tasks() const { return num_tasks_; }
  int num_shards() const { return num_shards_; }
  int num_replicas() const { return num_replicas_; }

 private:
  ShardAssignment(const ShardAssignment&) = delete;
  ShardAssignment& operator=(const ShardAssignment&) = delete;

  void AssignSequentially();
  void AssignRoundRobin();

  // Internal helper for assigning |shard| to |server|.
  void Assign(int shard, int server);

  int num_tasks_;
  int num_shards_;
  int num_replicas_;

  // Computed by AssignSequentially(). See public accessors.
  std::vector<std::set<int>> server_to_shard_;
  std::vector<std::set<int>> shard_to_server_;
};

#endif  // DAY1_SHARD_ASSIGNMENT_H__
