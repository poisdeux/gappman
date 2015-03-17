&lt;wiki:gadget url="http://www.ohloh.net/p/319892/widgets/project\_partner\_badge.xml" height="53" border="0"/&gt;

Gappman stands for _Graphical Application Manager_ and should become an application management utility that uses a XML configuration file to
specify the applications it should manage.
The idea is that Gappman provides a framework in which it is easy to start and manage apps for custom build systems, e.g. mediaplayers, gamestations, desktopclients.


The menu is automatically created by scaling and placing the buttons to provide an evenly spaced layout.
This is very convenient when developing applications for heterogeneous platforms
where the display size may vary considerably, e.g. mobile devices, HD
television screens, computer screens. Gappman will give you a flexible graphical application launcher that looks good on almost all resolutions.

For now Gappman provides basic functionality to launch applications. Additional app/system management functionality is being added as addons (applets or widgets).

The currently available programs are (some still very beta):
  * [GAppMan](GAppMan.md): the main program
  * [ShutDown](ShutDown.md): a simple button applet
  * [ChangeResolution](ChangeResolution.md): a dynamic screen resolution changer (using XRandR)
  * [NetMan](NetMan.md): network status and restart applet
  * [ProcessManager](ProcessManager.md): a simple graphical tool to send signals to applications started by [GAppMan](GAppMan.md)
  * [DigitalClock](DigitalClock.md): a simple clock showing the hours and minutes and a second indicator

### Features: ###

  * Automatic scaling and placement of buttons
  * Supports three menu-groups
    * Actions. This menu should hold management applications that allow a user to manage the system. Like configuring the X server, restarting the system, setting up network, etc..
    * Programs. This menu should hold the applications which fullfill the actual purpose of the system. E.g. mediaplayer, remote desktop, webbrowsing, etc. etc..
    * Panel. This menu is meant for applets like a [DigitalClock](DigitalClock.md) or a Network Manager (i.e. [NetMan](NetMan.md)).
  * Change screen resolution when a program is started and change it back to the original resolution when the program quits.
  * Fully configurable using oner or more XML configuration files.
  * Image file caching to minimize scaling of images when GUI is started.

### Roadmap ###

There is no road. See [GenericTodos](GenericTodos.md) for a list of todo's that are not specific to one of the above listed programs. The todo's for a specific program are listed on its wikipage.

### Members ###

Just me (martijn brekhof m.brekhof...gmail.com) for now.