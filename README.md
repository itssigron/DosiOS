<a name="readme-top"></a>

<!-- TABLE OF CONTENTS -->
### Table of Contents
<li>
    <a href="#about-the-project">About The Project</a>
</li>
<li>
    <a href="#getting-started">Getting Started</a>
    <ul>
    <li><a href="#prerequisites">Prerequisites</a></li>
    <li><a href="#installation">Installation</a></li>
    </ul>
</li>
<li><a href="#usage">Usage</a></li>
<li><a href="#contact">Contact</a></li>

<!-- ABOUT THE PROJECT -->
## About The Project

This project is a 32 bit operating system developed as part of Magshimim's final project (israel's national cyber education program)

The project's features are pretty basic:
* Keyboard and screen(terminal) driver
* File system
* Memory system

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

#### Note: This project can only be built in a linux environment.

### Prerequisites
In order to build this project, you need to have **git**, **make**, **nasm**, **i686-elf-tools** and **qemu**.

* git, make, nasm, qemu
  ```sh
  sudo apt install git make nasm qemu-system-x86
  ```
* i686-elf-tools
 
    In order to install **i686-elf-tools** you need to follow the instructions in their [github](https://github.com/lordmilko/i686-elf-tools) and install GCC for linux and put the contents of the **bin** folder inside any directory which is in the PATH, or alternatively just add that **bin** folder to PATH.
### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/itssigron/DosiOS.git
   ```
2. Move to the installed directory.
   ```sh
   cd DosiOS
   ```
3. Build the project
   ```sh
   sudo make
   ```
   By default, "make" will automatically run the project after building it. You may delete lines 8-9 inside [Makefile](Makefile#L8) if you simply want it only to build.

* Running the project
  ```sh
  sudo ./run.sh
  ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->
## Usage

Start by running the **help** command.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTACT -->
## Contact

* Harel Sigron - arel1234123@gmail.com
* Dolev Shlomo - [github](https://github.com/dolev2766)

Project Link: [https://github.com/itssigron/DosiOS](https://github.com/itssigron/DosiOS)

<p align="right">(<a href="#readme-top">back to top</a>)</p>