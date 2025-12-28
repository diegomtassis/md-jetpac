# JetPac MD - a port of Jetpac to Sega Megadrive

### JetPac

[JetPac](https://en.wikipedia.org/wiki/Jetpac) was created originally in 1983 by Tim and Chris Stamper for the ZX Spectrum and published by [Ultimate Play the Game](https://en.wikipedia.org/wiki/Ultimate_Play_the_Game).

It is a property of [RARE](https://en.wikipedia.org/wiki/Rare_(company)).


### Mega Drive port

Sega Mega Drive / Genesis port written by diegomtassis using SGDK.

It's currently work in progress.

This port includes 2 modes:
* ZX mode, trying to be as faith as possible to the original ZX Sinclair game.
* MD mode, maintaining the same visual aspect and playability while adding new level layouts.
* SANDBOX mode, allowing to play in a customized planet.

Jetpac and the jetman are a RARE property. The same applies to all the artwork in this port, which has been extracted directly from the original game. This port is free and is been built without RARE permission.

https://github.com/diegomtassis/md-jetpac

#### Improvements and new playable features
* 2 player mode available both in ZX and MD modes.
* Enhanced graphics in MD mode.
* Different platforms layouts in MD mode.
* Limited ammo in MD mode.
* Enemies behavior change according to difficulty level in MD mode.
* Gravity differs for each planet in MD mode.

#### Planetary systems
In MD mode each planet is part of a planetary system.
All the planets in a planetary system share the same background.
The availabe planetary systems are:
* Proxima Centauri, with graphics created by [Ansimuz](https://ansimuz.itch.io).
* Ran, with graphics created by [Ansimuz](https://ansimuz.itch.io).
* Lalande, with graphics extracted from [Assault Suit Leynos](https://segaretro.org/Target_Earth).

#### To be implemented
* Sound.
* Enemies IAs:
    * meteorite - done
    * alien - partially done
    * bubble - done
    * fighter - done
    * saucer - todo
    * falcon - todo
    * cross - todo
    * owl - todo
* More levels in MD mode.
* Smart drop of spaceship parts after dying.
* Polishing and optimization.

#### Cover

![JetPac MD](res/cover/Jetpac_MD.png?raw=true "Cover")

### SGDK

SGDK is a development kit for Sega Mega Drive created by Stephane Dallongeville
https://stephane-d.github.io/SGDK/

GitHub page: https://github.com/Stephane-D/SGDK

Wiki page: https://github.com/Stephane-D/SGDK/wiki

JetPac MD is built using SGDK 2.11
