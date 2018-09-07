# PokemonCpp
A C++17 implementation of a Pokémon game.

# Project dependencies 

In order to build the project you will require the following 
dependencies:

- SFML 2.5.0 
- Lua 5.3.4 or newer
- gcc/g++ 8.2.0 or newer

**On Arch Linux:**

All dependencies are available on the distribution's repositories,
and may be installed with Pacman. The first step is optional, but recommended,
in order to update your repositories and packages: 

	# pacman -Syu
    # pacman -S sfml lua gcc

**On Ubuntu 18.04:**

Some dependencies are available on the distribution's repositories and may be installed
with APT. The first step is optional, but recommended, in order to update your repositories:

	# apt update
	# apt install lua5.3 build-essential
	
However, the version of SFML available on the repositories is outdated and won't work with this project.
Building from source is, therefore, required. Instructions for that are available at https://www.sfml-dev.org/tutorials/2.5/start-linux.php.

**On Fedora:**

Some dependencies are available on the distribution's repositories and may be installed
with DNF. The first step is optional, but recommended, in order to update your repositories and
packages:

	# dnf update
	# dnf install lua gcc-c++
	
However, the version of SFML available on the repositories is outdated and won't work with this project.
Building from source is, therefore, required. Instructions for that are available at https://www.sfml-dev.org/tutorials/2.5/start-linux.php.

# Build instructions

To clean the project's **binaries and dependencies**, run

	$ make distclean

To clean **only the project's binary files**, run

	$ make clean

And to build the project, run 

	$ make

The executable file will be available as bin/pokemon.

# Enjoy!

