

# Introduction #

The shutdown applet provides the user with several (configurable) options to shutdown the system.

# Screenshot #

![http://wiki.gappman.googlecode.com/git/ShutDown.jpg](http://wiki.gappman.googlecode.com/git/ShutDown.jpg)

# Configuration #

This is the configuration file that was used to create the above screenshot:

```
<shutdown>
  <cachelocation>/tmp/</cachelocation>
  <actions width="30%" height="10%" align="center">
    <action>
      <name>Shutdown</name>
      <printlabel>1</printlabel>
      <exec>ngc</exec>
      <arg>-0</arg>
      <logo>./logos/system-shutdown.png</logo>
    </action>
    <action>
      <name>Reboot</name>
      <printlabel>1</printlabel>
      <exec>ngc</exec>
      <arg>-6</arg>
      <logo>./logos/system-restart.png</logo>
    </action>
    <action>
      <name>Suspend</name>
      <printlabel>1</printlabel>
      <exec></exec>
      <arg></arg>
      <logo>./logos/system-suspend.png</logo>
    </action>
  </actions>
</shutdown>
```

# TODO's #