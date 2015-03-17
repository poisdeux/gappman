

# Introduction #

Gappman creates and manages the main User Interface (UI).
It runs as a daemon listening to requests from applets/widgets and
starts applications.

The idea is that gappman calculates the best setup for the UI and
draws and places the buttons on the screen to provide an evenly spaced
layout. It monitors started applications and provides applets with details
about the UI layout and started applications.

# Screenshot #

![http://wiki.gappman.googlecode.com/git/GAppMan.jpg](http://wiki.gappman.googlecode.com/git/GAppMan.jpg)

# Configuration #

The screenshot above was created using the following configuration file for GAppMan.

```
<appmanager>
  <actions width="40%" height="15%" align="top,left">
    <action>
      <name>Shutdown</name>
      <exec>./applets/shutdown/gmshutdown</exec>
      <arg>-c</arg>
      <arg>applets/shutdown/xml-config-files/shutdown.xml</arg>
      <arg>-r</arg>
      <arg>gtk-config/gtkrc</arg>
      <logo>./applets/shutdown/logos/shutdown.png</logo>
    </action>
    <action>
      <name>Processmanager</name>
      <exec>./applets/processmanager/gmprocessmanager</exec>
      <arg>-c</arg>
      <arg>xml-config-files/conf.xml</arg>
      <arg>-r</arg>
      <arg>gtk-config/gtkrc</arg>
      <logo>./applets/processmanager/logos/clanbomber.png</logo>
    </action>
    <action>
      <name>Change resolution</name>
      <exec>./applets/changeresolution/gmchangeresolution</exec>
      <arg>-c</arg>
      <arg>xml-config-files/conf.xml</arg>
      <arg>-r</arg>
      <arg>gtk-config/gtkrc</arg>
      <logo>./applets/changeresolution/logos/randr.png</logo>
    </action>
  </actions>
  <programs max_elts="3" width="100%" height="50%" align="bottom,center">
    <arrowleft>./logos/leftarrow.png</arrowleft>
    <arrowright>./logos/rightarrow.png</arrowright>
    <program>
      <name>NX Desktop</name>
      <resolution>1280x1024</resolution>
      <exec>/bin/sleep</exec>
      <arg>15</arg>
      <logo>./logos/nxclient-icon.png</logo>
      <autostart>0</autostart>
      <printlabel>0</printlabel>
    </program>
    <program>
      <name>MythTV</name>
      <resolution>800x600</resolution>
      <exec>/bin/sleep</exec>
      <arg>50</arg>
      <logo>./logos/mythtv.png</logo>
      <autostart>0</autostart>
      <printlabel>1</printlabel>
    </program>
    <program>
      <name>Stepmania</name>
      <resolution>640x480</resolution>
      <exec>/bin/sleep</exec>
      <arg>50</arg>
      <logo>./logos/stepmania.png</logo>
      <autostart>0</autostart>
      <printlabel>1</printlabel>
    </program>
  </programs>
  <panel width="40%" height="15%" align="top,right">
    <applet>
      <name>Network manager</name>
      <objectfile>./applets/netman/.libs/gm_netman.so</objectfile>
      <conffile>./applets/netman/xml-config-files/netman.xml</conffile>
    </applet>
    <applet>
      <name>Digital Clock</name>
      <objectfile>./applets/digitalclock/.libs/gm_digitalclock.so</objectfile>
    </applet>
  </panel>
</appmanager>
```

# TODO's #

  * Be able to popup main menu when other apps have focus. (keybindings?)
  * Implement support for keybindings. For instance to start specific applets.
  * Startup indicator when starting programs. We could do this through an animated mouse-pointer or animate the application-button.
  * Have Gappman redraw the main window when the screen resolution changes
  * Add support to bring already started programs to the front when their button is pressed again.
  * Add support for library preloading
  * Make it possible to start gappman as init proces.
    * This probably requires gappman to start and monitor services as init does. Mimick systemd or upstart.
    * Add monitoring facilities for daemons. This might be implemented using an applet with no button.