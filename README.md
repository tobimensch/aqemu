###Latest news:
2020: 0.9.6 is now the current development version (in master for now),
use TBK fork if you want to use a stable checkout now, but use this repository
if you want to contribute to AQEMU's future.
Building using meson/ninja is recommended now.
See the changelog for all the new features that will work in the next stable
release, I'm most exciting about AQEMU being turned into a manager for VM services.
This means you can close the AQEMU UI and your VMs will keep running, also you
can restart the UI and manage running VMs you previously started with AQEMU.
And even better, you're able to start/stop VMs from the command line without
the need for a UI. Giving you a lot more flexibility, and hopefully increasing
the audience for AQEMU. As you may have guessed there are still many issues with this
new set of features, therefore a stable release can't be provided currently.
I was working on those features years ago, when I had to stop due various reasons,
but now I plan to bring the work to an end, hopefully with the help of the community.

Example how to build using meson/ninja:
```
meson builddir
cd builddir
ninja
./aqemu
```

I set up a [crowdfunding page for AQEMU](https://salt.bountysource.com/teams/aqemu), it will enable users to donate for the whole
project and/or for individual issues. Please consider setting up a monthly donation.

I want AQEMU to pick up steam again with your help, with the goal of making the best
virtual machine manager GUI.

Bountysource crowdfunding page for AQEMU: https://salt.bountysource.com/teams/aqemu

Patreon crowdfunding page: https://www.patreon.com/tobimensch

Donate via PayPal: donate2aqemu@emailn.de


![ScreenShot](https://i.imgur.com/PkvFUEk.png)

Current stable release: https://github.com/tobimensch/aqemu/releases/tag/v0.9.2
See the CHANGELOG for details.

Upgrading from 0.8.2 is highly recommended.

---

Port of AQEMU 0.8.2 from Qt4 to Qt5.

---

Use cmake to build.

Dependencies: 
 - Qt5Core
 - Qt5Widgets 
 - Qt5Network
 - Qt5Test
 - Qt5PrintSupport
 - Qt5DBus
 - LibVNCServer


---

As an alternative to cmake the meson build system is also supported:
https://github.com/mesonbuild/meson

