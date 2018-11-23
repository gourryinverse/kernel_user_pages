struct shared_state
{
  char data[4096];
};

enum driver_args
{
  UKM_MAP_MEMORY,
  UKM_CHECK_BUFFER,
  UKM_MUTATE,
  UKM_UNMAP_MEMORY
};
