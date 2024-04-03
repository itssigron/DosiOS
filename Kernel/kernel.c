#include "Tables/gdt.h"
#include "Tables/idt.h"
#include "Memory/heap.h"
#include "Drivers/keyboard.h"
#include "Drivers/ata.h"
#include "fs/fs.h"
#include "System/stdio.h"

char* showPath(char* path[]);

int kmain(void) 
{
  // Initialize all systems
  terminal_initialize();
  setup_gdt();
  setup_idt();
  init_irq();
  init_keyboard();
  init_heap();
  init_fs();

  size_t path_size = 10;
  char* initial_path = "dosi";
  char* path = (char*)malloc(path_size);
  
  char* command_info = NULL;
  strncat(path, initial_path, strl(initial_path));

  create_directory(path, NULL, 0);

  printf("welcome to our terminal!!!\nto see the list of commands, enter \"help\"\n");

  while(true)
  {
    printf("%s/ >", path);
    //check for create-file command
    char* get_command = getline();

    if(checkCommand(get_command, "cf "))
    {
      command_info = getInfoAfterCommand(get_command, "cf ");
      if(command_info)
      {
        create_file(command_info, path, 0x0);
        printf("create file -> %s\n", command_info);
      }
    }
    else if(checkCommand(get_command, "write "))
    {
      command_info = getInfoAfterCommand(get_command, "write ");
      if(command_info)
      {
        //create path to file
        char* all = (char*)malloc(strl(path) + strl(command_info) + 2);
        strncat(all, path, strl(path));
        strncat(all, "/", 1);
        strncat(all, command_info, strl(command_info));
        all[strl(all)] = NULL;          
        if(navigate_file(all, NULL)) //check if file exist
        {
          printf("data -> ");
          char* data = getline();
          write_file(all, data, strl(data));
        }      
        else
        {
          printf("ERROR! this file does not exists\n");
        }
      }
    }
    else if(checkCommand(get_command, "cdir "))
    {
      command_info = getInfoAfterCommand(get_command, "cdir ");
      if(command_info)
      {
        create_directory(command_info, path, 0x0);
        printf("create folder -> %s\n", command_info);
      }
    }
    else if(checkCommand(get_command, "cat "))
    {
      command_info = getInfoAfterCommand(get_command, "cat ");
      if(command_info)
      {
        //create path to file
        char* all = (char*)malloc(strl(path) + strl(command_info) + 2);
        strncat(all, path, strl(path));
        strncat(all, "/", 1);
        strncat(all, command_info, strl(command_info));
        all[strl(all)] = NULL;       
        inode_t* file_node = navigate_file(all, NULL);   
        if(file_node) //check if file exist
        {
          printf("file data -> %s\n", read_file(file_node, 0));
        }
        else
        {
          printf("ERROR! this file does not exist\n");
        }
      }
    }
    //out folder
    else if(checkCommand(get_command, "cd .."))
    {
      size_t pathLen = strl(path);
      int i = 0;
      
      //if it is not the dosi (root) folder 
      if(strcmp(path, "dosi") != 0)
      {
      //loop to find folder
        path[pathLen - 1] = NULL;
        for(i = pathLen; path[i] != '/' && i != 0; i--) {};

        //check if have folder is not the root
        if(i != 0)
        {
          path[i] = NULL;
        }
      }
    }
    //into folder
    else if(checkCommand(get_command, "cd "))
    {
      command_info = getInfoAfterCommand(get_command, "cd ");
      if(command_info)
      {
        char* all = (char*)malloc(strl(path) + strl(command_info) + 2);
        strncat(all, path, strl(path));
        strncat(all, "/", 1);
        strncat(all, command_info, strl(command_info));
        all[strl(all)] = NULL;
        inode_t* dir = navigate_dir(all, NULL);

        if(dir)
        {
          path_size = strl(path) + strl(command_info) + 1;
          path = realloc(path, path_size);
          strncat(path, "/", 1);
          strncat(path, command_info, strl(command_info));
        }
        else
        {
          printf("This directory is not in the os\n");
        }
      }
    }
    else if(checkCommand(get_command, "ls"))
    {
      command_info = getInfoAfterCommand(get_command, "ls ");

      if(command_info)
      {
        char* all = (char*)malloc(strl(path) + strl(command_info) + 2);
        strncat(all, path, strl(path));
        strncat(all, "/", 1);
        strncat(all, command_info, strl(command_info));
        all[strl(all)] = NULL;

        ls(all);
      }
      else
      {
        ls(path);
      }
    }
    else if(checkCommand(get_command, "help"))
    {
        printf("commannds ->\n1: cdir [dir] - creates a directory\n2: cf [file_name] - creates a file\n3: cls - clean the terminal screen\n"
        "4: cd [dir] - get into dir\n"
        "5: write [file] - write contents to a file\n"
        "6: cat [file] - read file contents\n"
        "7: ls [dir?] - list directory\n");
    }
    //clear treminal
    else if(checkCommand(get_command, "cls"))
    {
      terminal_initialize();
    }
    else
    {
      printf("\"%s\" is an illegal command\n", get_command);
    }

    free(get_command);
  }

  return 0;
}