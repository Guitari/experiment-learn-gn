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

## Steps

Put GN in PATH, e.g.

    export PATH=/c/dev/tp/skia/bin:$PATH

May also need ninja on PATH:

    export PATH=$PATH:/c/dev/tp/cmake-3.12.1-win64-x64/bin

Ensure Python is ready to go:

    $ python -V
    Python 2.7.15

The quick start:

- https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/quick_start.md
- and: https://gn.googlesource.com/gn/+/master/docs/
- and read: https://github.com/timniederhausen/gn-build
- and this example from the above: https://github.com/timniederhausen/gn-build/tree/testsrc
- about Ninja (not Gyp or GN): https://ninja-build.org/manual.html#_using_ninja_for_your_project
- about Gyp (not Ninja or GN): https://gyp.gsrc.io/docs/UserDocumentation.md
- Gyp to GN cookbook: https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/cookbook.md
- Good GN installation notes: https://github.com/mmotorny/build#instructions-for-windows
- Interesting MSVC reference: https://chromium.googlesource.com/chromium/src/build/config/+/master/win/BUILD.gn#231


The `gn` binary does not ship with toolchain config. This is a clone of a standalone version (`gn-build`) above:

    git@github.com:aellerton/gn-build.git

To get that, I did:

    git init  # if it hasn't been done already
    git submodule add git@home-github.com:aellerton/gn-build.git build
    git submodule update --init --recursive

(The `home-github` this is for my ssh config)

I needed to change studio version:

```
$ gn args out/vs2017

(and in the editor write - )

visual_studio_version = "2017"
```

Then generate project files in the `out/Default`:

    gn gen out/Default


Then to build:

```
$ cd out/Default

$ ninja
[1/3] CC obj/src/demo/main.obj
[2/3] LINK demo.exe demo.exe.pdb
[3/3] STAMP obj/root.stamp

$ ./demo.exe
Hello, 1 args

```

Or, alternatively:

```
$ ninja -C out/Default
```

Clean is like this:

```
$ ninja -C out/Default -t clean
```


Per [GN docs][1], run in verbose mode for more info (`-v`):

    gn gen -v out/Default

or:

    gn desc out/Default


## Building release

Configure like this:

```
$ gn gen out/Release --args='visual_studio_version="2017" is_official_build=true'
Done. Made 3 targets from 17 files in 2731ms
```

Note that this could be `is_debug=false` -- "is_official_build" is explained better 
in `gn args --list out/Default`:

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


build:

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

