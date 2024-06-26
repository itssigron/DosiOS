#include "fs.h"

void* fat_buffer  = NULL;
void* root_buffer = NULL;

uint16_t root_entries = 0;

static bool was_fat_read = false;
static bool was_root_read = false;

static inline uint16_t entries_to_sectors(uint16_t entries) {

  size_t sectors = (entries * DIR_ENTRY_SIZE) / SECTOR_SIZE;
  return (!sectors && entries) ? 1 : sectors;
}

static inline size_t size_to_sectors(size_t size) {

  size_t sectors = size / SECTOR_SIZE;
  return (size % SECTOR_SIZE) ? ++sectors : sectors;
}


/* Write FAT data from RAM (buffer) to the disk */
static inline void fat_write(void* buffer) {

  ata_write(FAT_SECTOR_OFFSET, SECTORS_IN_FAT, buffer);
  was_fat_read = false;
}

/* Read FAT from disk to RAM (buffer) */
static inline void fat_read(void* buffer) {

  if (!was_fat_read) { was_fat_read = true; ata_read(FAT_SECTOR_OFFSET, SECTORS_IN_FAT, buffer); }
}

/* Write root data from RAM (buffer) to the disk */
static inline void root_write(void* buffer, size_t sectors) {
 
  ata_write(ROOT_SECTOR_OFFSET, sectors, buffer);
  was_root_read = false;
}

/* Read root from disk to RAM (buffer) */
static inline void root_read(void* buffer, size_t sectors) {
  
  if (!sectors) { return;}
  if (!was_root_read) { was_root_read = true; ata_read(ROOT_SECTOR_OFFSET, sectors, buffer); }
}


/* Initialize fat table and root directory if they weren't initialised yet */
void init_fs() {
  
  fat_buffer = kcalloc(1, FAT_SIZE);
  root_buffer = kcalloc(1, ROOT_SIZE);
  root_entries = 0;
  if (fat_extract_value(0) != FAT_SIGNATURE) { 
    fat_setup_table();
  }
}

/* Setup initial table values, indexes 0 & 1 are reserved */
void fat_setup_table() {
   
  ((uint16_t*)fat_buffer)[ROOT_SECTOR_OFFSET] = EOC;

  ((uint16_t*)fat_buffer)[0] = FAT_SIGNATURE;
  ((uint16_t*)fat_buffer)[1] = FAT_SIGNATURE;

  fat_write(fat_buffer);
}

/* Get the size of a file using it's elf header */
uint32_t get_elf_size(elf32_header_t* elf) {
  return elf->shoff + (elf->shentsize * elf->shnum);
}

/* When the OS first initializes we want to load our shell application */
void load_shell() {

  uint8_t buffer[512];   
  ata_read(390, 1, buffer);
  
  elf32_header_t* elf = (elf32_header_t*)buffer;
  size_t size = get_elf_size(elf);
  
  size_t sectors = size_to_sectors(size);

  void* file_buffer = kmalloc(sectors * SECTOR_SIZE);

  ata_read(390, sectors, file_buffer);

  create_file("sabaka.run", NULL, 0);
  write_file("sabaka.run", file_buffer, size);
  free(file_buffer);
}

/* Returns a value from the fat table at the given location */
uint16_t fat_extract_value(uint16_t index) {
  
  fat_read(fat_buffer);
  return (uint16_t)(((uint16_t*)fat_buffer)[index]);
}

/* Takes a file name and it's extention and creates a full filename string */
char* make_full_filename(char* filename, char* ext) {

  char* filename_buff = NULL;
  size_t size = 0;

  if (!ext || !*ext) { 
    
    size = strl(filename) + 1;
    filename_buff = kmalloc(size);
    memcpy(filename_buff, filename, size);

    return filename_buff; 
  }

  size = strl(filename) + strl(ext) + sizeof(char) + 1;

  filename_buff = kcalloc(1, size);

  memcpy(filename_buff, filename, strl(filename));
  strcat(filename_buff, ".");
  strcat(filename_buff, ext);

  filename_buff[size - 1] = '\0';
 
  return filename_buff;
}

/* Assign a node it's name */
void fat_create_filename(inode_t* inode, char* name) {

  char* it = name;
  while (*it != '.' && *it) { it++; }
  
  size_t name_size = it - name;
  if (*it) { it++; }
 
  memcpy(inode->filename, name, name_size);
  memcpy(inode->ext, it, strl(it));
}

/* Checks if a filename contains an extention (.) */
bool file_has_extention(char* full_filename) 
{
  size_t size = strl(full_filename);
  for (uint8_t i = 0; i < size; i++) { if (full_filename[i] == '.') { return true; } }

  return false;
}

/* Eats the first directory in a given path reference, deletes it from the string and returns it's name */
char* eat_path(char* path) {

  if (!path) { return NULL; }

  char* it = path;

  if (*it && CHECK_SEPERATOR(*it)) { it++; }

  while (*it && !CHECK_SEPERATOR(*it)) {
    it++;
    if (((uint32_t)it - (uint32_t)path) > FULL_FILENAME_SIZE) { return NULL; }
  }
 
  if (*it && CHECK_SEPERATOR(*it)) { it++; }

  size_t size = strl(path) - (strl(path) - strl(it)) + 1;
 
  if (size == 1) { return NULL; }

  char* ret = kmalloc(size);
  memcpy(ret, it, size);

  return ret;
}

/* Returns the first file (specified by path)'s name */
char* get_first_file_from_path(char* path) {
  
  if (path == NULL) { return NULL; }

  char* it = path;

  while (*it && !CHECK_SEPERATOR(*it)) {
    it++;
    if (((uint32_t)it - (uint32_t)path) > FULL_FILENAME_SIZE) { return NULL; }
  }

  size_t size = (uint32_t)it - (uint32_t)path + 1;
  char* filename = kmalloc(size);
  memcpy(filename, path, size);
  filename[size - 1] = '\0';

  return filename;
}

/* Returns the last file (specified by path)'s name */
char* get_last_file_from_path(char* path) {

  if (!path || !*path) { return NULL; }

  size_t size = strl(path);

  char* it = path;
  if (CHECK_SEPERATOR(it[size - 1])) { size--; }
  
  unsigned long i = size - 1;
  for (; i > 0 && !CHECK_SEPERATOR(it[i]); i--) { }
 
  if (CHECK_SEPERATOR(it[i])) { i++; }

  char* ret = kmalloc(size - i + 1);
  memcpy(ret, it + i, size - i + 1);
  ret[size - i] = '\0';
  return ret;
}

/* Eat the last file/directory in a path */
char* eat_path_reverse(char* path) {

  if (!path || !*path) { return NULL; }
  
  size_t psize = strl(path);

  char* it = path;
  if (CHECK_SEPERATOR(it[psize - 1])) { psize--; }
  
  unsigned long i = psize - 1;
  for (; i > 0 && !CHECK_SEPERATOR(it[i]); i--) { }
  
  if (!i) { return NULL; }
  
  char k = it[i];
  it[i] = '\0';


  char* ret = kmalloc(i + 1);
  memcpy(ret, it, i + 1); 
  it[i] = k;

  return ret;
}

/* Takes a file path and returns the last specified directory */
inode_t* navigate_dir(char* path, void** buff_ref) {
 
  char* name = NULL;
  char* navigate_path = path;
  
  if (!root_entries) { return NULL; }
  root_read(root_buffer, entries_to_sectors(root_entries));
 
  inode_t* current_file = NULL;
  
  char* buffer = (char*)root_buffer;
  char* tb = NULL;
  
  while (navigate_path && *navigate_path) {
   
    name = get_first_file_from_path(navigate_path);
    navigate_path = eat_path(navigate_path);
    
    if (file_has_extention(name)) { return current_file; } 

    if (tb != (char*)root_buffer) { free(tb); }
    
    //find file in my dir or in root dir
    current_file = current_file ? find_file(buffer, current_file->size, name) : find_file(buffer, ROOT_CURRENT_SIZE, name);
    free(name);

    if (navigate_path && *navigate_path) { 
      tb = buffer;
      buffer = kread_file(current_file); 
    }

    if (buff_ref) { *buff_ref = buffer; }

  }
  return current_file;
} 

/* Get the last file from the given path
 * If buff_ref is not null, it will be a reference to buffer that will hold the 
 * directory buffer holding the specified file
 * */
inode_t* navigate_file(char* path, void** buff_ref) {

  inode_t* dir = navigate_dir(path, NULL);
  
  char* np = eat_path(path);
  if (dir && !file_has_extention(get_last_file_from_path(np))) { return dir; }
  free(np);

  char* buffer = kread_file(dir);
  
  if (buff_ref) { *buff_ref = buffer; }

  return find_file(buffer, dir ? dir->size : ROOT_CURRENT_SIZE , get_last_file_from_path(path));
}

/* Find a file by it's name from a given data buffer
 * The function searches for the file name in a directory data buffer
 * and returns the file node if it exists, or NULL, if it doesn't
 * */
inode_t* find_file(char* buffer, size_t size, char* filename) {
  
  char* it = NULL; 

  for (uint32_t i = 0; i < size; i += DIR_ENTRY_SIZE, buffer += DIR_ENTRY_SIZE) {

    it = make_full_filename(((inode_t*)buffer)->filename, ((inode_t*)buffer)->ext);
    if (!strcmp(it, filename)) { return (inode_t*)buffer; } 
    free(it);
  }

  return NULL;
}

/* Initialize a file's first cluster, every file, even an empty one, holds at least 1 cluster */
void init_first_cluster(inode_t* inode) {
  
  fat_read(fat_buffer);

  int err = NO_ERROR;
  inode->cluster = fat_find_free_cluster(fat_buffer, &err); 
  if (err) { panic(err); } 

  ((uint16_t*)fat_buffer)[inode->cluster] = EOC; 

  fat_write(fat_buffer);
}

/* Creates a directory, for more information look at the create_file function */
inode_t* create_directory(char* dirname, char* path, uint8_t attributes) 
{
  return create_file(dirname, path, attributes | ATTRIBUTE_DIRECTORY);
}

/* Create a file 
 * Input: Desired filename, [up to 8 bytes of name].[up to 3 bytes of file extention]
 * Input: File path, where the file will be stored, this parameter can be null the directory is the root
 * Input: File attributes, more information in the header file
 * */
inode_t* create_file(char* filename, char* path, uint8_t attributes) {

  inode_t* file = init_file(filename, attributes);

  init_first_cluster(file);

  void* dir_buffer;
  inode_t* dir = navigate_dir(path, &dir_buffer);

  enter_file(file, dir);
 
  if (dir) 
  {
    char* dir_path = eat_path_reverse(path);
    inode_t* dir_dir = navigate_dir(dir_path, NULL);
    edit_file(dir_dir, dir_buffer, (dir_dir ? dir_dir->size : ROOT_CURRENT_SIZE));   /* Edit directory's size */
  }

  return file;
}

/* Initialize a file handler, with a specified name and file attributes */
inode_t* init_file(char* filename, uint8_t attributes) {
  
  inode_t* inode = kcalloc(1, sizeof(inode_t));
  
  fat_create_filename(inode, filename);

  inode->attributes = attributes;
  
  return inode;
}

/* Links the file to the specified directory */
void enter_file(inode_t* file, inode_t* dir) {
  cat_file(dir, file, DIR_ENTRY_SIZE);
}

void ls(char* path) {
  inode_t* dir = navigate_dir(path, NULL);
  if (!dir) {
      printf("ERROR! this directory does not exists\n");
      return;
  }
  
  printf("List of files in %s:\n", path);
  void* dir_buffer = kread_file(dir);
  uint32_t num_entries = dir->size / DIR_ENTRY_SIZE;

  for (uint32_t i = 0; i < num_entries; ++i) {
      inode_t* entry = ((inode_t*)dir_buffer) + i;

      if((entry->attributes & ATTRIBUTE_DIRECTORY) == ATTRIBUTE_DIRECTORY)
      {
        printf("%s/\n", entry->filename);
      }
      else
      {
        printf("%s.%s\n", entry->filename, entry->ext);
      }
  }
}


/* Find first cluster that is marked as free in the fat table */
uint16_t fat_find_free_cluster(void* buffer, int* err) {
  
  for (uint16_t i = DATA_START; i < SYSTEM_SECTORS; i++) {
    if (!(((uint16_t*)buffer)[i])) { return i; }
  }

  *err = ERROR_NOT_ENOUGH_DISK_SPACE;
  
  return 0;
}

/* Resize the clusters holding a file, and it's handler's size */
void fat_resize_file(inode_t* inode, size_t size) {

  uint16_t cluster = inode->cluster;
  uint16_t new_cluster_amount = size / CLUSTER_SIZE;
  if (size % CLUSTER_SIZE) { new_cluster_amount++; }

  uint16_t old_cluster_amount = inode->size / CLUSTER_SIZE;
  if (inode->size % CLUSTER_SIZE) { old_cluster_amount++; }

  inode->size = size;
  if (new_cluster_amount == old_cluster_amount) { return; }

  fat_read(fat_buffer);

  if (inode->size > size) {

    uint16_t it = inode->cluster;

    for (uint16_t i = 0; i < new_cluster_amount; i++) { cluster = ((uint16_t*)fat_buffer)[cluster]; }
    it = ((uint16_t*)fat_buffer)[cluster];
    
    ((uint16_t*)fat_buffer)[cluster] = EOC;
    
    uint16_t it2 = it;
    while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[it2] = 0x0; it2 = it; }

    goto write;
  }

  int err = NO_ERROR;
  
  if (inode->size < size) {
    
    while (((uint16_t*)fat_buffer)[cluster] != EOC) { cluster = ((uint16_t*)fat_buffer)[cluster]; }
    size_t s = new_cluster_amount - old_cluster_amount - 1;
    
    for (uint16_t i = 0; i < s; i++) {
     
      ((uint16_t*)fat_buffer)[cluster] = fat_find_free_cluster(fat_buffer, &err); 
      if (err) { panic(err); }

      cluster = ((uint16_t*)fat_buffer)[cluster];
    }

    ((uint16_t*)fat_buffer)[cluster] = EOC;
  }

write:

  inode->size = size;
  fat_write(fat_buffer);
}

/* Remove a given entry name from a given file */
void remove_dir_entry(char* dir_path, char* entry) {
 
  void* dir_buffer;
  inode_t* dir = navigate_dir(dir_path, &dir_buffer);

  size_t size = dir ? dir->size : ROOT_CURRENT_SIZE;
  if (!size) { return; }

  void* new_dir_buffer = kmalloc(size - DIR_ENTRY_SIZE);
  void* old_dir_buffer = kread_file(dir);

  char* it = NULL;
  for (uint32_t i = 0; i < size; i += DIR_ENTRY_SIZE, old_dir_buffer += DIR_ENTRY_SIZE) {
    it = make_full_filename(((inode_t*)old_dir_buffer)->filename, ((inode_t*)old_dir_buffer)->ext);
    if (strcmp(it, entry)) { memcpy(new_dir_buffer, old_dir_buffer, DIR_ENTRY_SIZE); old_dir_buffer += DIR_ENTRY_SIZE; } 
    free(it);
  }

  edit_file(dir, new_dir_buffer, dir ? (dir->size - DIR_ENTRY_SIZE) : (ROOT_CURRENT_SIZE - DIR_ENTRY_SIZE));

  if (dir) {
    
    dir_path = eat_path_reverse(dir_path);
    inode_t* dir_dir = navigate_dir(dir_path, NULL);

    edit_file(dir_dir, dir_buffer, (dir_dir ? dir_dir->size : ROOT_CURRENT_SIZE));   /* Edit directory's size */
  }
  else { root_entries--; }
}

/* Write a file to the disk 
 * Input: Path of file to write, file has to exist
 * Input: Data buffer to write to file
 * Input: Amount of data to take from the data buffer
 * */
void write_file(char* path, void* buffer, size_t size) {

  inode_t* dir = navigate_dir(path, NULL);
  
  
  void* dir_buffer = kread_file(dir);
  inode_t* file = find_file(dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE, get_last_file_from_path(path));

  write_file_data(file, buffer, size);

  edit_file(dir, dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE); 
  
}

void cat(char* dst, char* src) {
  
  inode_t* dir = navigate_dir(src, NULL);
  
  void* dir_buffer = kread_file(dir);
  inode_t* src_file = find_file(dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE, get_last_file_from_path(src));

  dir = navigate_dir(dst, NULL);
  
  dir_buffer = kread_file(dir);
  inode_t* dst_file = find_file(dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE, get_last_file_from_path(dst));

  cat_file(dst_file, kread_file(src_file), src_file->size);
}

/* Write data to a specified file
 * Input: File node to link to created data
 * Input: Buffer containing the data to save (write)
 * Input: Size of buffer, how many bites to actually write
 * */
void write_file_data(inode_t* inode, void* buffer, size_t size) {

  size_t remainder = size % SECTOR_SIZE;
  size_t sectors = size / SECTOR_SIZE;

  if (sectors > SYSTEM_SECTORS) { panic(ERROR_FILE_TOO_LARGE); }

  fat_resize_file(inode, size);   
  fat_read(fat_buffer);

  uint16_t cluster = inode->cluster;
  
  if (sectors) {

    do {

      ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, buffer);   /* Write one cluster */ 
      
      cluster = ((uint16_t*)fat_buffer)[cluster];
      buffer += CLUSTER_SIZE;
      size -= CLUSTER_SIZE;
    
    } while (cluster != EOC && size >= CLUSTER_SIZE);
  }

  /* Last sector to write is smaller than the default sector size */
  if (remainder) { 
     
    uint8_t* remainder_buffer = kmalloc(CLUSTER_SIZE);
    
    memcpy(remainder_buffer, buffer, remainder);
    memset((void*)((size_t)remainder_buffer + remainder), '\0', SECTOR_SIZE - remainder);

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, (void*)remainder_buffer);
    free(remainder_buffer);
  }
}

/* Concatenate a file's contents with a given buffer and buffer size */
void cat_file(inode_t* inode, void* buffer, size_t size) {

  void* file_data = kread_file(inode);
  size_t prev_size = inode ? inode->size : (root_entries++ * DIR_ENTRY_SIZE);

  file_data = krealloc(file_data, prev_size + size);

  void* appended_buffer = (void*)((uint32_t)file_data + prev_size);
  memcpy(appended_buffer, buffer, size);
  
  edit_file(inode, file_data, prev_size + size);

  free(file_data);  
}

/* Change a file's data to contain what's inside the given buffer, at the specified size */
void edit_file(inode_t* inode, void* buffer, size_t size) {

  /* Normal cases */
  if (inode) {
    write_file_data(inode, buffer, size);
    return;
  }

  /* Only for cases where the file is NULL, we're going to edit the root directory */
  size_t fixed_size = size_to_sectors(size) * SECTOR_SIZE;
  void* root = kcalloc(1, fixed_size);
  memcpy(root, buffer, size);

  root_write(root, entries_to_sectors(root_entries));
  free(root);
}

/* Delete a specified file from the fat table & the fat table ONLY */
void fat_delete_file(inode_t* inode) {
  
  if (!inode || !inode->cluster) { return; }

  fat_read(fat_buffer);
  
  uint16_t cluster = inode->cluster;
  uint16_t it = cluster;

  inode->cluster = 0x0;

  while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[cluster] = 0x0; cluster = it; }

  ((uint16_t*)fat_buffer)[it] = 0x0;
  fat_write(fat_buffer);
}

/* Delete all clusters */
void fat_clear_file(inode_t* inode) {

  if (!inode || !inode->cluster) { return; }

  fat_read(fat_buffer);
  
  uint16_t cluster = inode->cluster;
  uint16_t it = cluster;

  while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[cluster] = 0x0; cluster = it; }

  ((uint16_t*)fat_buffer)[it] = 0x0;
  ((uint16_t*)fat_buffer)[inode->cluster] = EOC;
  fat_write(fat_buffer);
}

void* kread_file(inode_t* inode) {
  return read_file(inode, true);
}

void* uread_file(inode_t* inode) {
  return read_file(inode, false);
}

/* Read and return a file's data */
void* read_file(inode_t* inode, bool kernel) {
  
  uint8_t* buffer = NULL;

  /* Read from root */
  if (!inode) {

    if (!root_entries) { return NULL; }
    root_read(root_buffer, entries_to_sectors(root_entries));
    buffer = kernel ? kmalloc(ROOT_CURRENT_SIZE) : malloc(ROOT_CURRENT_SIZE);
    memcpy(buffer, root_buffer, ROOT_CURRENT_SIZE);
  
    return buffer;
  }

  /* Normal file read */
  size_t buff_sector_size = inode->size / SECTOR_SIZE;
  if (inode->size % SECTOR_SIZE) { buff_sector_size++; }
  if (!buff_sector_size) { return NULL; }

  buffer = kernel ? kmalloc(buff_sector_size * SECTOR_SIZE) : malloc(buff_sector_size * SECTOR_SIZE);
  fat_read(fat_buffer);

  void* it = buffer; 
  for (uint16_t cluster = inode->cluster; cluster != EOC; cluster = ((uint16_t*)fat_buffer)[cluster]) {
    ata_read(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, it);
    it += CLUSTER_SIZE;
  }

  return buffer;
}

/* Create a new file at a specified location with the contents of the old file */
void copy_file(char* old_path, char* new_path) {

  inode_t* file = navigate_file(old_path, NULL);

  char* full_filename = make_full_filename(file->filename, file->ext);
  create_file(full_filename, new_path, file->attributes);

  size_t new_path_size = strl(new_path);
  char* full_path = kcalloc(1, new_path_size + strl(full_filename) + 
      (new_path && !CHECK_SEPERATOR(new_path[new_path_size - 1]) ? 1 : 0) + 1);
  
  if (new_path) {
    memcpy(full_path, new_path, strl(new_path));
    if (!CHECK_SEPERATOR(new_path[new_path_size - 1])) { strcat(full_path, "/"); }
  }

  strcat(full_path, full_filename);

  write_file(full_path, kread_file(file), file->size);
}

/* Move a file from one directory to a new one */
void move_file(char* old_path, char* new_path) {
    
  copy_file(old_path, new_path);
  delete_file(old_path);
}

/* Given file path's last file will be renamed to the new given filename */
void rename_file(char* path, char* new_filename) {
  
  char* filename = get_last_file_from_path(path);
  char* dir_path = eat_path_reverse(path);

  inode_t* dir = navigate_dir(dir_path, NULL);
  void* dir_buffer = kread_file(dir);
  inode_t* file = find_file(dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE, filename);
  fat_create_filename(file, new_filename);
  
  edit_file(dir, dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE);
}

/* Completely delete a file, it's directory entry & it's data link */
void delete_file(char* path) {

  char* filename = get_last_file_from_path(path);
  char* dir_path = eat_path_reverse(path);

  inode_t* dir = navigate_dir(dir_path, NULL);
  void* dir_buffer = kread_file(dir);
  inode_t* file = find_file(dir_buffer, dir ? dir->size : ROOT_CURRENT_SIZE, filename);
  
  fat_delete_file(file);
  remove_dir_entry(dir_path, filename);  
}

