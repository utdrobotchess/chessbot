Chessbot Arduino Code
========

This repository contains the source code for the Chessbots robots developed at
the University of Texas at Dallas. The microcontroller for which this code was
developed is the Arduino ATMega 2560. The MCU's specs are found here
(http://arduino.cc/en/Main/arduinoBoardMega2560) and more information about
Arduino in general can be found here (http://arduino.cc).

In the following README, we assume the user knows how to use the Arduino IDE to
compile and upload .ino files to Arduino-based MCU's.

For more information about the UTDallas Robotchess project, visit our website
(site currently unavailable, but URL will be http://www.utdallas.edu/robotchess
and we will update this README as soon as the site is up).

Contents
--------

src/chessbot-main/ : Contains all source code that is necessary to run a chess
game with the Chessbots. This must be uploaded to all 32 Chessbots in order to
play a game. Note that once the Chessbots are running this code, the user
must be running our Chessgame App (https://github.com/utdrobotchess/chess-game)
to communicate with the robots and get them to move about the board. On master,
the user should be able to compile chessbot-main.ino once they have cloned this
repo.

src/test/ : Contains several directories, each containing a test for a specific
sensor. These are mainly used for debugging/learning purposes. Please read the
following section about using the shell scripts in the root directory before
trying to compile these tests.

Using the update.sh and clean.sh
--------

Since the Arduino IDE does not conveniently allow inclusion of 3rd party
libraries, we've decided to store our libraries in the same directory as every
.ino test file that needs to include those libraries. Because every .ino file
needs to be stored in its own directory for the Arduino IDE to compile and
upload it, there would be duplication in these library files if multiple .ino
files require them. Thus, a simple shell script, update.sh, was written to copy the
libraries from src/chess-test/ to src/test/[]-test/ directories. To clean these
duplicated files, use the clean.sh.

Issues
--------
We track issues using the github issue tracker. If you feel that something is
missing or broken, please visit our issues site: (https://github.com/utdrobotchess/chessbot/issues).
Reading the open issues here may help you understand why something isn't
working the way you expect it to.


Documentation
-------------
The documentation for our project will be stored in manual.tex within doc/ of
this repo. Those who are familiar with LaTeX and have it installed can either
compile it however they know how, or run 'make manual' within this directory.

Those unfamiliar with LaTeX may want to visit this site (http://latex-project.org)
and learn more about it. For this project, it is enough to install LaTeX, cd
into doc/ and run 'make manual'


