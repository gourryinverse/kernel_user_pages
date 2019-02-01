struct shared_state
{
  char data[4096];
};

enum driver_args
{
  UKM_MAP_MEMORY = 0x80000001,
  UKM_CHECK_BUFFER = 0x80000002,
  UKM_MUTATE = 0x80000004,
  UKM_UNMAP_MEMORY = 0x80000008,
};
