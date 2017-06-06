# NND3d (aka FlipSIDE)

This is a port of NND3d2 aka FlipSIDE, a small uncompleted game I wrote in Visual Basic 6 in 2001 to C++.

Ever heard the phrase "dance like no one is watching?" In my youth, I routinely programmed like no one was watching and wrote code that might charitably be called "terrible." It was almost completely unstructured- few functions, few types, `goto` considered awesome, `gosub` considered fancy- and thus was nearly impossible to read or maintain. Back then, my sole concern was getting stuff done fast- consequences be damned!

Fast-forward a decade and a half, and I now write extremely professional code as part of my professional career. I've read Clean Code by Bob Martin and spend hours arguing in pull requests over the finer philosophical points of when something should be used as an "internal" interface instead of a "package" interface, the reasons PEP8 will or won't save mankind and how my own styles on brace placement are inherently correct and a sign of good character.

But one thing I don't do is write games, or anything else that creative. At least, not like before.

So I'm porting this ancient stuff as an academic exercise as well as an attempt to unearth any forgotten virtues of the barbarism I once engaged in. I also hope to show how no code is unrecoverable, as all (working) legacy code, if given some TLC, can be fixed or ported to use modern tools and idioms.


## Notes on the Macaroni Port

I actually [started porting](https://github.com/TimSimpson/FlipSIDE-Macaraoni) this to my own C++ sublanguage, [Macaroni for C++](https://github.com/TimSimpson/Macaroni) (using the older version of the [Lp3-Engine](https://github.com/TimSimpson/Lp3-Engine-Macaroni), which had partial support for the Sega Dreamcast), about a year and a half ago, but I thought it would be interesting to redo the whole process (including the source code conversion) in plain C++ so that it was easier to follow along. So the first dozen or so commits are actually somewhat copy and pasted from the original Macaroni conversion.


## Credits

The old code contained some routines lifted directly from Direct 3D Donuts example application / demo which Microsoft released as part of their SDK. I can't find any information on this demo so my hope is that it was permissively licensed. Additionally since the graphics routines are changing entirely to SDL / OpenGL, none of the Direct3D code applies. Still thought I'd mention it.
