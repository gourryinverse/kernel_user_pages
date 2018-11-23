struct shared_state
{
  char data[4096];
};

struct map_memory_args
{
  void* user_ptr;
  bool mapped;
};

enum driver_args
{
  UKM_MAP_MEMORY,
  UKM_CHECK_BUFFER,
  UKM_MUTATE,
  UKM_UNMAP_MEMORY
};
