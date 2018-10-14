# experiment: learn GN

CMake showed potential but became frustrating quickly.

I liked the idea of Gyp and I haven't ruled it out for future work.
However, Skia/Chromium use GN and docs say it is better, faster than Gyp,
with the downside being that it only works with Ninja, not studio/xcode etc
that Gyp can do.

If different build backends (cmake, studio, xcode, etc) become critical,
I may have to revisit this choice. I think the only reason they would be
needed is for debugging. Maybe there's a way to do that in studio/xcode
without using the native project, dunno.

## Initial setup steps

I initially used `gn` from the Skia build I had. That worked fine, but below is how to set
it up from scratch.

Install old school Python 2.7 and ensure it is ready to go. On Windows, use either CMD or Cygwin/bash-here etc:

    $ python -V
    Python 2.7.15

Get GN: https://gn.googlesource.com/gn/

e.g.

    cd /c/dev/tp
    git clone https://gn.googlesource.com/gn
    cd gn

The following is from `gn/README.md`, which also states that (on Windows) C++ (i.e. Visual Studio) needs to be in the PATH, so run from a Studio CMD window:

    python build/gen.py
    ninja -C out
    # To run tests:
    out/gn_unittests

Put GN in PATH, e.g.:

    export PATH=/c/dev/tp/gn/out:$PATH

(May also need ninja on PATH ... though not sure this still applies):

    export PATH=$PATH:/c/dev/tp/cmake-3.12.1-win64-x64/bin

Interestingly, in the Studio-CMD window, Ninja was in the PATH under `CommonExtensions/Microsoft/CMake/Ninja`. I can't remember if that was me or not.

On Cygwin/git-bash I needed to add it explicitly, but it could detect Studio.

Read these to get your brain into GN and Ninja:

- https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/quick_start.md
- and: https://gn.googlesource.com/gn/+/master/docs/
- and read: https://github.com/timniederhausen/gn-build
- and this example from the above: https://github.com/timniederhausen/gn-build/tree/testsrc
- about Ninja (not Gyp or GN): https://ninja-build.org/manual.html#_using_ninja_for_your_project
- about Gyp (not Ninja or GN): https://gyp.gsrc.io/docs/UserDocumentation.md
- Gyp to GN cookbook: https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/cookbook.md
- Good GN installation notes: https://github.com/mmotorny/build#instructions-for-windows
- Interesting MSVC reference: https://chromium.googlesource.com/chromium/src/build/config/+/master/win/BUILD.gn#231


The `gn` binary does not ship with toolchain config. Fortunately, kind souls have built and
shared standalone versions, like (as mentioned above) https://github.com/timniederhausen/gn-build

My clone of that:

    git@github.com:aellerton/gn-build.git

To get that, I did:

    git init  # if it hasn't been done already
    git submodule add git@home-github.com:aellerton/gn-build.git build
    git submodule update --init --recursive

(The `home-github` this is for my ssh config. Make it `github` for yours, probably.)

By default that uses Visual Studio 2013. I have 2017 at the time of writing.
To change that config, you need to set the args.

One way is this:

    gn gen out/Debug --args='visual_studio_version="2017" is_debug=true'

In other words:

- run `gn`
- build everything under the dir `out/Debug`
- override the arg for Visual Studio version to be "2017" (the double quotes mean string)
- set debug mode to true.

Another way is to use `gn args`:

```
$ gn args out/Debug
```

An editor will pop up. Add the below:

visual_studio_version = "2017"
```

Then project files are built in `out/Debug`:

    gn gen out/Debug

Build in one line:

```
$ ninja -C out/Debug
```

Or build in two steps:

```
$ cd out/Default

$ ninja
[1/3] CC obj/src/demo/main.obj
[2/3] LINK demo.exe demo.exe.pdb
[3/3] STAMP obj/root.stamp

$ ./demo.exe
Hello, 1 args

```

Clean is like this:

```
$ ninja -C out/Default -t clean
```


Per [GN docs][1], run in verbose mode for more info (`-v`):

    gn gen -v out/Debug

or:

    gn desc out/Debug


## Building release

Configure like this:

```
$ gn gen out/Release --args='visual_studio_version="2017" is_debug=false'
Done. Made 3 targets from 17 files in 2731ms

$ gn gen out/Official --args='visual_studio_version="2017" is_official_build=true'
Done. Made 3 targets from 17 files in 2731ms
```

The difference between `is_debug=false` and `is_official_build=true` is explained better 
in `gn args --list <dir>`:

```
is_official_build
    Current value (from the default) = false
      From //build/config/BUILDCONFIG.gn:131

    Set to enable the official build level of optimization. This has nothing
    to do with branding, but enables an additional level of optimization above
    release (!is_debug). This might be better expressed as a tri-state
    (debug, release, official) but for historical reasons there are two
    separate flags.
```

Executable size differences:

```
$ ninja -C out/Release
```

Results with debug and release:

```
$ find out/ -name "*.exe" | xargs ls -lh
-rwxr-xr-x 1 user 1049089 1000K Oct 12 21:10 out/Default/demo.exe
-rwxr-xr-x 1 user 1049089  1.7M Oct 12 21:10 out/Default/hello_win.exe
-rwxr-xr-x 1 user 1049089  127K Oct 12 21:14 out/Release/demo.exe
-rwxr-xr-x 1 user 1049089  213K Oct 12 21:14 out/Release/hello_win.exe
```

One line (ish) to reconfigure the lot:

```
$ rm -rf out && \
  gn gen out/Debug --args='visual_studio_version="2017" is_debug=true' && \
  gn gen out/Release --args='visual_studio_version="2017" is_debug=false' && \
  gn gen out/Official --args='visual_studio_version="2017" is_official_build=true'
```

Then building:

```
$ ninja -C out/Debug && ninja -C out/Release && ninja -C out/Official && find out -name "*.exe" | xargs ls -lh
ninja: Entering directory `out/Debug'
[1/5] CC obj/src/hello_console/hello_console/main.obj
[2/5] CXX obj/src/hello_win/hello_win/main.obj
[3/5] LINK hello_console.exe hello_console.exe.pdb
[4/5] LINK hello_win.exe hello_win.exe.pdb
[5/5] STAMP obj/root.stamp
ninja: Entering directory `out/Release'
[1/5] CC obj/src/hello_console/hello_console/main.obj
[2/5] LINK hello_console.exe hello_console.exe.pdb
[3/5] CXX obj/src/hello_win/hello_win/main.obj
[4/5] LINK hello_win.exe hello_win.exe.pdb
[5/5] STAMP obj/root.stamp
ninja: Entering directory `out/Official'
[1/5] CC obj/src/hello_console/hello_console/main.obj
[2/5] CXX obj/src/hello_win/hello_win/main.obj
[3/5] LINK hello_console.exe hello_console.exe.pdb
Generating code
Finished generating code
[4/5] LINK hello_win.exe hello_win.exe.pdb
Generating code
Finished generating code
[5/5] STAMP obj/root.stamp
-rwxr-xr-x 1 andrew.ellerton 1049089 1000K Oct 13 10:21 out/Debug/hello_console.exe
-rwxr-xr-x 1 andrew.ellerton 1049089  1.7M Oct 13 10:21 out/Debug/hello_win.exe
-rwxr-xr-x 1 andrew.ellerton 1049089  127K Oct 13 10:21 out/Official/hello_console.exe
-rwxr-xr-x 1 andrew.ellerton 1049089  213K Oct 13 10:21 out/Official/hello_win.exe
-rwxr-xr-x 1 andrew.ellerton 1049089  127K Oct 13 10:21 out/Release/hello_console.exe
-rwxr-xr-x 1 andrew.ellerton 1049089  213K Oct 13 10:21 out/Release/hello_win.exe
```

## Debugging with Visual Studio

GN can build studio `sln` files, as noted on the [Skia build docs][2] at "Visual Studio Solutions":

> If you use Visual Studio, you may want to pass --ide=vs to bin/gn gen to generate all.sln

You can also debug an exe in Visual Studio by doing File > Open then choosing the exe directly.
Read the MSDN docs on ["how to debug an executable"][3].

## Using CMake and CLion

The [Skia build docs][2] note that CMake files can be generated "mainly for use with IDEs that
like CMake project descriptions" and that this "is not meant for any purpose beyond development":

    bin/gn gen out/config --ide=json --json-ide-script=../../gn/gn_to_cmake.py

That should do the job for opening the project in CLion.

The `gn_to_cmake.py` script is particular to Skia it seems (it was mentioned in the Skia docs, after all),
so in my particular case the command was;

     gn gen out/config \
       --ide=json \
       --json-ide-script=/c/dev/tp/skia/gn/gn_to_cmake.py \
       --args='visual_studio_version="2017"'

It loaded in CLion, built and ran, but was a little quirky. All the code is there, so perhaps
its a matter of refining that conversion script.

## Other notes

If you don't do `gn args <builddir>` it will (on my current system) fail to find
Visual Studio 2017. You can modify the toolchain, but that seems silly. If you
want to, it looks like this:

```
$ cd build

$ git diff
diff --git a/toolchain/win/settings.gni b/toolchain/win/settings.gni
index cf6d1ed..05fa8f1 100644
--- a/toolchain/win/settings.gni
+++ b/toolchain/win/settings.gni
@@ -5,7 +5,7 @@ if (is_clang) {
 declare_args() {
   # Version of Visual Studio pointed to by the visual_studio_path.
   # Use "2013" for Visual Studio 2013.
-  visual_studio_version = "2013"
+  visual_studio_version = "2017"
```


[1]: https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/quick_start.md
[2]: https://skia.org/user/build
[3]: https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-debug-an-executable-not-part-of-a-visual-studio-solution?view=vs-2017

