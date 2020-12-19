# RayGun
This is "Part 2" of [my experiment with Ray Casting](https://github.com/stefanos-86/Ray-Cast-Exercise): turn it into a Game.

![Sample screeshot 1](https://github.com/stefanos-86/RayGun/blob/main/screenshot_for_readme.png "The least happy cardboard targets ever.")

## Is this a real game?
I will let you decide from the feature list. We have:
* a maze you have to escape from;
* a gun (photo-edited spray bottle, you say?);
* (immobile) targets to hit;
* music and sound effect (singular, there's only one);
* Binary Space Partition trees (can't have a "classic FPS" without them).

## What are we giving up?
As with many hobby projects, it is impossible to apply all the cutting edge technology of professional game studios. We don't have:
* any real game engine;
* stereo sound, network play;
* anything touching the GPU;
* fonts;
* vector math;
* double precision floating point variables;
* ...colors.

## A few remarks on quality and speed.
To keep things as small as possible, the only libraries linked in the project are whatever the compiler-issued runtime is and SDL.

All in all, it still runs at 60 FPS, completes the video rendering in less than 5 milliseconds, takes 4% of my CPU.
How it ended up taking 25 MB of RAM is beyond me.

The code should be organized in a decent way. There are some test to cover the most difficult areas. This work was done "on the fly"
with no plan to maintain the code in the long term, more as a joke than anything else.
Shortcuts were taken, corners were cut, mistakes were _intentionally_ made and duly annotated with TODOs.

The only reliability target was: "it runs, most of the time".

If, after all this, you still want to compile and play this game, refer to the README of the original project. It works in the same way.
Additional commands: space bar to shoot, P to pause, ESC to quit.

### Music Score By Nora Kant
Special thanks to Alessio Castorrini of [Nora Kant](https://soundcloud.com/nora-kant) for the 4 musical tracks that form the background music.
The bad in-game on-the-fly mixing... is entirely my fault.
