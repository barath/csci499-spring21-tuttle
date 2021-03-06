// Exercise 1 for USC CS499, March 2021
// Author: Christopher Tuttle
//
// Implementation of BackendManager.

#include "backend.h"

#include<stdlib.h>
#include<iostream>
#include<set>
#include<string>
#include<vector>

#include "shard-assignment.h"

BackendManager::BackendManager(const ShardAssignment* assignment,
                               ReplicationPolicy policy)
  : assignment_(assignment),
    policy_(policy) {
  const int num_tasks = assignment->num_tasks();
  stubs_.resize(num_tasks);
  for (int i = 0; i < num_tasks; ++i) {
    stubs_[i] = new Stub(i);
  }
}

Stub* BackendManager::GetStubForShard(int shard) {
  // BEGIN CODE TO BE FILLED IN BY STUDENTS //

  // END CODE TO BE FILLED IN BY STUDENTS //
  return nullptr;
}
