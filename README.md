HaloFree / HaloFreeLoader2

Simple port of HaloFreeLoader to C++ (but I didn't add in any mapload stuff yet trololo)

Does:
- Patch StopGame call when no login given (useless since it crashes later on)
- Patch StopGame func itself to not stop the game when countdown is up etc (why do I still have the other patch?)
- Patch russian -> english
- Patch some debug log stuff to write to a log file (one of these is just a simple sprintf wrapper tho im sure)
- Patch MP mode stuff so you can load maps in MP (lets you load s3d_reactor)
- Patch session ID text to whatever
- Hooks D3DDevice->EndScene() to draw AntTweakBar (still need to hook the games WndProc so we can give ATB input though)

This is for educational purposes only, probably won't be working on this more since there's better things now

Usage:
- Copy AntTweakBar folder to same dir as solution, folder structure should be
- \HaloFree.sln
- \AntTweakBar\
- \AntTweakBar\include\

- edit post-build so it copies DLL to your HO folder
- compile
- run the game! (might crash on launch sometimes, only happened after I added AntTweakBar, if you run it a few times it'll work)