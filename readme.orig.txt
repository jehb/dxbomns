DirectX Bomns Readme
====================
by: Toxite (Charles Lindsay), the creator of Bomns and DXBomns
on: March 16, 2002
updated: March 29, 2002

Current DXBomns version: v1.1.8.0


Sections that have changed since the previous readme.txt:
 - Section 1.4: What's New In This Version?



Table of Contents
-----------------
 0: Random Thoughts
  0.1: Copyright/License
  0.2: An Important Note to the Beta Testers!

 1: Introduction
  1.1: What is Bomns?
  1.2: And DXBomns?
  1.3: Features
  1.4: What's New in This Version?  Any Plans for Future Versions?
  1.5: System Requirements
  1.6: Installation/Uninstallation
  1.7: Where can I Get DXBomns or Other Bomns Games?
  1.8: And Where Should I Report Bugs or Send Feedback?
  1.9: Known Issues

 2: The Editor Interface
  2.1: What Are These .dat and .cfg Files?
  2.2: The Editor Dialog
  2.3: Level Manipulation
  2.4: Editing the Level

 3: The Game
  3.1: The Object?
  3.2: And to Accomplish it?
  3.3: A Look at the Game Screen
  3.4: The Different Level Tiles

 4: Advanced Topics
  4.1: Editing the .dat and .cfg Files Yourself
  4.2: The Level File Format
  4.3: Editing the Graphics and Sounds


 Section 0: Random Thoughts
 ===============================================================================

  0.1: Copyright/License
  ----------------------
     DirectX Bomns is copyright (c) 1998-2002 Charles Lindsay (that's me), one
might say for Allied Computers, Jasón's and my "business."  But that's not
really important.  What IS important is that DXBomns is open source and
freeware.  This means that I do not mind whatsoever if you copy, modify, and/or
distribute the contents of this .zip file however you wish.  I do request that
you not separate the contents of this .zip if you distribute it in its current
form (if you modify it, though, you may modify the .zip however you like).
Also, I request that if you use this game as a base for anything else you give
me credit and let me know what you're doing (I like to know if someone else
thinks what I'm doing is important enough to use as a starting point for
something else).  Please don't charge money for this game or its source.

  0.2: An Important Note to the Beta Testers!
  -------------------------------------------
     First of all, thanks to everyone who tested this game for me.
     Now!  If you're unzipping this release over the beta release, delete all of
the .cfg and .dat files created by the beta before playing this release.  I've
slightly changed the format of gamopts.dat, and if you don't delete it, DXBomns
won't work!  I make note of this in the installation section (1.6) as well, but
I thought it needed special mention here, too.


 Section 1: Introduction
 ===============================================================================

  1.1: What is Bomns?
  -------------------
     Well, Bomns is a little hard to describe.  I created it one summer day
(actually, more like over the course of three days) in 1998, and since then, my
friends and I have been hooked.  If not hooked on PLAYING it, we've certainly
been hooked on programming it.  How else could I explain the multitude of
different versions of Bomns that have been "programmed" (read: started, but
usually not finished) to date?  I'll list a few: Bomns Squared (completed!),
Bomns Cubed (Jasón?), Bomns for Basic (again, Jasón?), Bomns for Linux (on its
way from Keith), and of course, DirectX Bomns, the reason for this readme.
     So what's the big deal?  It's a really simple action/arcade game that lends
itself well enough to being played in text mode, and doesn't put too much of a
strain on things in graphics mode.  So what, you say?  Well, programming a
variation of Bomns is a learning experience.  It's customary, now, between my
friends and me, to start programming a version of Bomns whenever an API, OS, or
language comes along that we want to learn something about.  This probably
accounts for the large number of unfinished Bomns projects--they were started
when one of us was motivated to learn something about a new programming paradigm
and once we felt mildly comfortable with it, we lost motivation to continue work
on the project.

  1.2: And DXBomns?
  -----------------
     DirectX Bomns, as its full name reads, is simply the newest, most ambitious
Bomns project to date.  At first, my goal in programming it was to learn
something about Windows interface and DirectX coding, but soon enough after
starting it (and giving up on it several times), I realized that it was my place
as creator of the original Bomns to carry the Bomns torch a little further.  So,
I set out (several MORE times) to turn Bomns into the most fun, graphically- and
audially-immersing, customizable game on the planet.  I didn't succeed, but I
DID manage to make the most fun and best looking and sounding (in MY opinion)
Bomns game to date, and inarguably the most customizable as well: DXBomns.  Read
on to see its biggest features!

  1.3: Features
  -------------
     Lightning-fast, DirectX enhanced Bomns engine (60+ fps with WinAmp on in
the background on my P233 w/ 64MB RAM)
     New Bomns features including new powerups, dynamic level spawning, and more
     Full-fledged, DirectX enhanced level editor, allowing you to create any
style of level you want, with any powerups you desire
     Robust option editor in the most standard of Windows interfaces, letting
you easily change everything from starting bomn radius to the game's time limit
     The least intrusive Win32 program EVER, showing no advertisements, staying
completely out of your registry and system .ini files, not using a bitmapped GDI
interface, and ALWAYS allowing alt-tabbing
     And as if the game wasn't customizable enough already: fully customizable
artwork and sounds, allowing for whatever look in the game you happen to want

  1.4: What's New in This Version?  Any Plans for Future Versions?
  ----------------------------------------------------------------
     I found a bug in version 1.0.8.0, and fixed that bug.  That's the only
change.  The bug manifested when players tried to use <enter> as a key in the
game... the engine wouldn't register that it had been pressed.  Anyway, look to
history.txt for more information.
     Future versions?  Yes.  I'd like to add network support, eventually.  And
possibly AI.  That's about it, really.  Network and AI support.  Don't expect
either too soon, though.  Also, check section 1.9: Known Issues.  I might
eventually fix any known issues, if there are any.

  1.5: System Requirements
  ------------------------
     DirectX Bomns goes as light as possible on your computer.  I'm not EXACTLY
sure what it "requires," but it runs BEAUTIFULLY on my P1 233 w/MMX + 64MB RAM +
3dfx Voodoo 3 2000 w/ 16MB on board RAM + Creative Sound Blaster 64, running
Windows XP Professional and DirectX 8.1.  That in mind, I'll tell you what I
KNOW it requires, and estimate the rest:
     Microsoft Windows (any 32 bit version that supports DirectX 8.1)
     Microsoft DirectX 8.1 (or later)
     A desktop resolution of 640x480 or higher, with at least 4bpp (16 colors)
     A monitor (and video card) capable of entering 1024x768 resolution at 16bpp
     A DirectDraw 7.0a compatible video card (preferably one that has at least
4MB of on-board RAM, as DXBomns is a little greedy with video memory, and also
preferably one that supports hardware blitting at 1024x768x16)
     A DirectSound 8.1 compatible sound card (not actually required if you don't
care about sound)
     A keyboard
     Probably around the equivalent of a Pentium 1 100MHz or faster
     Something like 8MB of RAM
     A good 4MB or so of hard drive space

  1.6: Installation/Uninstallation
  --------------------------------
     To install: simply unzip the entire contents of this .zip file somewhere
(preferably in its own directory), preserving the .zip directory structure so
that all the sounds end up in \sfx\ and the bitmaps are in \gfx\.
     To uninstall: delete the entire directory into which you unzipped DXBomns,
or, if for some reason there were OTHER things in the directory, you can delete
all the files and subdirectories that this .zip created, and any .dat and .cfg
files that DXBomns created.  That's it!
     Important note!  It is recommended that you delete ALL of the .dat and .cfg
files from previous installations, if you're unzipping this over an old
installation, as I don't guarantee that the file structure will be the same.

  1.7: Where can I Get DXBomns or Other Bomns Games?
  --------------------------------------------------
     http://bomns.sourceforge.net/ is the official Bomns webpage.  There, you
can find the newest release of DXBomns (and source), many releases of other
Bomns games, and a myriad of other random applications that I've programmed over
the years.  Check it out.  Also, if you're lookin' for Bomns for Linux, go to
http://sourceforge.net/projects/greenridge/ (that's Keith's page).

  1.8: And Where Should I Report Bugs or Send Feedback?
  -----------------------------------------------------
     My email address is Toxite@aol.com, and that's where.  Don't hesitate.

  1.9: Known Issues
  -----------------
     The only thing that comes close to being an "issue" as of yet is the way
overlapping explosions will kind of shimmer back and forth between explosion
sprites.  This is because they only draw themselves every 10th of a second, and
when they do so, they overlap any other explosion that was there.  No biggie,
more of a little peeve, but it's ALMOST annoying enough that I might try to fix
it in some future version.  We'll see.


 2: The Editor Interface
 ===============================================================================

  2.1: What Are These .dat and .cfg Files?
  ----------------------------------------
     To store your options and levels, DXBomns creates (in its working
directory) several .dat files and a .cfg file.  If you delete them, DXBomns will
simply reset all the editor values to defaults (and recreate them).  The .cfg
stores your key configuration, and is human-readable if you want to edit it
manually (though there's no reason to, since the editor dialog does it for you).
These files are covered in greater detail in section 4.1.

  2.2: The Editor Dialog
  ----------------------
     After clicking "Play..." on the startup dialog, you're immediately
confronted with the most intimidating-looking part of DXBomns: the editor.  It's
really not as bad as it looks, though.  If you want to skip it and get right to
the play, just hit "Play."  If you want to stick around and get to the
CONFIGURING, though, stick with me, and I'll explain the options in detail.
     Actually, I won't explain the options in detail, as that's what the
"Help..." button does.  Really, the options should be really self-explanatory,
especially after playing the game a few times.  If you're confused about some
powerups or something, check section 3.4.  It'll help you out.

  2.3: Level Manipulation
  -----------------------
     Also from the editor dialog, you can manipulate the level.  Of course,
clicking the big bitmapped button will take you to the level editor (see section
2.4), but aside from that, you can save and load levels by clicking the
appropriate buttons, generate new levels randomly (see the "Help..." button in
the generate dialog for assistance with that, and see section 3.4 for info on
the different tiles), and clear the current level.

  2.4: Editing the Level
  ----------------------
     As I mentioned above, clicking the big bitmapped button will take you to
the level editor.  This is where you get to design your own custom levels.  You
really should be able to figure out how to do this, especially with the help of
pressing F1 and seeing the help message.  And I really don't feel like
explaining it to you here.  So, I'll just remind you that you won't be able to
save the level unless all four player start points are defined, which means that
four tiles in the level need to be free from walls (normal or indestructible)
and warps (though they may have powerups in them).  These four tiles are where
you put the start points, obviously.  Beyond that restriction, though, any level
is "valid."  If you have questions about the different tiles, see section 3.4.
     You can take a screenshot, which will appear in the game's current 
directory as "sshotxxx.bmp", by pressing <PRINTSCREEN>.


 3: The Game
 ===============================================================================

  3.1: The Object?
  ----------------
     The object, of course, is to kill all the other players.  :)  More
specifically, I guess, it's to be the last player with health greater than 0.
Note that you CAN, in the editor dialog, specify 0 players, in which case the
object becomes to look at the level a lot, or 1 player, in which case the object
is to move around and collect all the powerups and become invulnerable and
destroy as many walls as fast as possible so you can hear crumble.wav over and
over until it bores you.

  3.2: And to Accomplish it?
  --------------------------
     Well, there are two main ways, depending on how you edit the game options.
You can either drop bomns such that when they explodes, other players are in the
radii (and hopefully not you), or you can walk up to other players and smack
into them.  Now, you can edit the options to make no bomns appear in the level
or make bomns do no damage or make them have no radius, effectively disabling
way number 1.  Also, you can edit the options to set punch damage to 0, thereby
disabling way number 2.  Normally, it takes several bomns or several punches to
kill a player, but, by editing the options, you can make one hit kill, as well.
How customizable!

  3.3: A Look at the Game Screen
  ------------------------------
     Everything but the bottom 48 pixels is the play area--where you move and
drop bomns and stuff.  The bottom 48 pixels are special, though.  They are the
"HUD," which, in case you hadn't heard, stands for something lame like "heads up
display."  The HUD shows each player's status, how much health each player has
(the number of colored hearts by the player's status), and how many bomns each
player is carrying (the number after the hearts).  In the middle of the screen,
in the yellow box, is the game's time remaining (if any) or how many seconds
remain before being taken to the winner screen (if someone just won).  And at
the very bottom is a little bomns horn-tooting.  :)
     Note that if you want to take a screenshot, press <PRINTSCREEN> and DXBomns
will create "sshotxxx.bmp" in its current directory.

  3.4: The Different Level Tiles
  ------------------------------
     "But... what are the little different colored things in the game?" you ask?
Well!  I'll tell you!  Let's start with the most simple: blank spots.  They're
just empty blackness (unless you've got a different sprites.bmp!).  Ok, maybe I
shouldn't have even talked about these, since there's really nothing to them.
Moving on... next we come to walls.  There are two varieties: normal, and
indestructible.  Normally, they are big gray blocks, and the indestructible ones
have a yellow border.  They BOTH block your path.  The normal walls can be
destroyed, however, by bomn blasts or by you smacking into them when you're
invulnerable.  The indestructible walls cannot be destroyed, obviously.  Next:
warps.  They look, normally, like cyan spiral thingies.  They simply teleport
you to a random location in the level.  Not too exciting.
     Then we come to the actual powerups (the previous ones are more considered
"tiles" than powerups, since you can't pick them up).  Most importantly is the
health powerup, which is white with a red cross in it.  It simply increases your
current health when you're feelin' down.  Then there's the powerup for which the
game is named: bomns, which look like little bombs with fuses and everything.
They simply give you more bomns that you can drop.  Invulnerabilities, the
yellow circles with dual black exclamation marks inside, make you invulnerable
for a length of time determined by the editor dialog.  Next to last are the
explosion radius-ups (green concentric circles), which increase the size of your
bomn explosions, and explosion radius-downs (red concentric circles), which
decrease the same.  Lastly are the explosion damage-ups (gray hearts being
stabbed by a green dagger), which increase the damage your bomn explosions do,
and explosion damage-downs (gray hearts with a red dagger) which decrease the
same.  That's all!
     Note that you won't be able to pick up a powerup if you've already got the
maximum (or minimum, where applicable) value for that item, as specified in the
editor dialog.


 4: Advanced Topics
 ===============================================================================

  4.1: Editing the .dat and .cfg Files Yourself
  ---------------------------------------------
     Here, I'll attempt to explain the format of each of the .dat and .cfg files
in the interest of allowing others to create any sorts of DXBomns editors they
desire.
     As I'm writing this, I realize what a fool I've been regarding these files.
When I wrote the editor, I knew I'd have to clip things to reasonable values.
But, I made sure to check them only before saving the data (not before loading
it) so that if someone wanted to get hardcore enough, he could hex-edit the
files himself, letting him get unreasonable, engine-pushing values for things if
he really wanted to.  However, I made one grave mistake: it ALWAYS saves the
data (checking for validity) immediately before going to the game (and there's
no way I can think of to avoid this).  Oh well.  If you wanted to, you could
strip the engine code from the rest, and have it use whatever data files you
want, but until then, this section only serves to allow someone to create 3rd-
party editors.  Read on!
     First, keys.cfg, the only .cfg file created by DXBomns, is human readable,
and should be really easy to figure out (look at the help message in the keys
dialog for what key constants are defined).
     Now, on to the REAL editing.  First, I'll discuss gamopts.dat, which stores
the values you enter in the editor dialog under the Game Options section.  It's
not exactly complex, but here's the format, nonetheless (note that all values
are interpreted as unsigned):
     Size Offset Description
     ---- ------ -----------
        1      0 Number of players
        1      1 Starting health
        1      2    "     bomns
        1      3    "     radius
        1      4    "     damage
        1      5 Max health
        1      6  "  bomns
        1      7  "  radius
        1      8  "  damage
        1      9 Min radius
        1     10  "  damage
        2     11 Time limit (0 to disable)
        1     13 Max bomns out at a time
        1     14 Invulnerability time
        1     15 Bomn timer
        1     16 Punch damage
        1     17 Health per powerup
        1     18 Persistent powerups (0 if false, 1 if true)
     ----
       19 bytes

     Next, appopts.dat which is simply two bytes: whether to display the fps (0
if no, 1 if yes), followed by whether audio is enabled (0 if no, 1 if yes).
     Levopts.dat isn't much more complex than gamopts.dat.  Here's the structure
(everything's unsigned again):
     Size Offset Description
     ---- ------ -----------
        2      0 Spawn frequency (0 to disable)
        1      2 Spawn chance (%)
        1      3 Relative chance to spawn bomns
        1      4    "       "    "    "   health
        1      5    "       "    "    "   invulnerabilities
        1      6    "       "    "    "   radius ups
        1      7    "       "    "    "   radius downs
        1      8    "       "    "    "   damage ups
        1      9    "       "    "    "   damage downs
     ----
       10 bytes

     Lev.dat is simply the current level, stored in the level file format, which
I'll discuss in the next section.  Preview.dat is a standard bitmap generated by
the level that gets drawn on the big edit button in the editor dialog.  It's
really no use editing it, since it's automatically generated all the time.
     The last file to discuss is genopts.dat, the level generation options.
Here's the structure of the file (all fields unsigned):
     Size Offset Description
     ---- ------ -----------
        4      0 Seed for random numbers (0 for random seed)
        2      4 Number of normal walls
        2      6   "    "  indestructible walls
        2      8   "    "  warp zones
        2     10   "    "  bomns
        2     12   "    "  healths
        2     14   "    "  invulnerabilities
        2     16   "    "  radius ups
        2     18   "    "  radius downs
        2     20   "    "  damage ups
        2     22   "    "  damage downs
     ----
       24 bytes

     I should note that all these files are just the numbers you type into the
editor dialog... they have no special significance to the game (except lev.dat,
which is the level the game will use, and I'll discuss that format now).

  4.2: The Level File Format
  --------------------------
     Of all the file formats this game involves, the level format is obviously
the most important.  And it's really very simple.  The file is 2888 bytes long.
The first 2880 bytes are simply the level tiles, in order (left to right, then
top to bottom).  I should mention that the level is 64x45 tiles (64*45=2880).
Each of those bytes can have the one of the following values, the significance
of each I've noted here as well:
     Value Significance
     ----- ------------
         0 Blank tile
         1 Normal wall
         2 Indestructible wall
         3 Warp zone
         4 Bomn
         5 Health
         6 Invulnerability
         7 Radius up
         8 Radius down
         9 Damage up
        10 Damage down

     The last 8 bytes are each player's starting position, defined as x, then y
(as game tiles) for each of the 4 players in order.  So, it looks something like
this: (2880 bytes of level),(p1x),(p1y),(p2x),(p2y),(p3x),(p3y),(p4x),(p4y).
Simple, eh?  Go write your own editor!
     I should tell you again that the results are undefined if any player's
starting position is defined as being on a tile that is occupied by a wall
(either form) or a warp (though it may be on any other tile).  Don't do it.

  4.3: Editing the Graphics and Sounds
  ------------------------------------
     The only thing of significance to note here is that you may edit the
graphics and sounds however you wish, with a few small restrictions.  DXBomns
uses 11025Hz, 16 bit, stereo sound in its engine, so if you make any of the
sounds a different format, DirectSound'll have to do some mixing, which may slow
things down.  Also, I would advise you to keep the format of the graphics the
same (most are 24bpp, uncompressed).  The only bitmap I can think of off the top
of my head that NEEDS to have a specific format is preview.bmp, which needs to
be 4bpp, uncompressed to correctly generate a level preview, but I've made the
other graphics their specific format for a reason, whether I had speed or size
in mind.  Make it look good!  And if you want, send me your work and I might
post it on the Bomns webpage as an add-on!

 ===============================================================================

     Thanks for reading this readme.txt!  If you found it useful or have any
questions I didn't address or something, don't hesitate to email me!  My email
address, once again, is Toxite@aol.com.  Also, go to the Bomns webpage at
http://bomns.sourceforge.net/ for the newest release of DXBomns and for lots of
other cool stuff.  Thanks!
  --Toxite
