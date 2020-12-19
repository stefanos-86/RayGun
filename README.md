# RayGun
This is "Part 2" [my experiment with Ray Casting](https://github.com/stefanos-86/Ray-Cast-Exercise): turn it into a Game.

## Is this a real game?
I will let you decide from the feature list. We have:
* a labirynth you have to escape from;
* a gun;
* (immobile) targets to hit;
* music and sound effect (singular, there's only one);
* Binary Space Partition trees (can't have a "classic FPS" without them).

## What are we giving up?
As with many hobby projects, it is impossible to apply all the cutting edge technology of pro game company studios. We don't have:
* any real game engine;
* anything touching a GPU;
* stereo sound;
* fonts;
* vector math;
* double precision floating point variables;
* ...colors.

## A few remarks on quality and speed.
To keep things as small as possible, the only libraries linked in the project are whatever the compiler-issued runtime is and SDL.

All in all, it still runs at 60 FPS, completes the video rendering in less than 5 milliseconds, takes 4% of my CPU.
How it ended up taking 25 MB of RAM is beyond me.

The code should be organized in a decent way. There are some test to cover the most difficult areas. But all in all, this work was done "on the fly"
with no plan to maintain the code in the long term, more as a joke than anything else.
Shortcuts were taken, mistakes were _intentionally_ made and duly annotated with TODOs.

The only real goal was "it runs, most of the time".

If, after all this, you still want to compile and play this game, refer to the README of the original project. It works in the same way.
Additional commands: space bar to shoot, P to pause, ESC to quit.
